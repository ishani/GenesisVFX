inline float fsqr (float a) { return a*a; }

//**********************************************************************
//* Get Image From Output
inline void ImageFilter_LensF::GetImage(void)
{		BMM_Color_64	*l64=(BMM_Color_64 *)malloc(srcmap->Width()*sizeof(BMM_Color_64));

		for(int i=0;i!=yres;i++)
			{ srcmap->GetLinearPixels(0,i,srcmap->Width(),l64);

			  for(int j=0;j!=xres;j++)
					{ Img_R[i][j]=l64[j].r>>8; Img_G[i][j]=l64[j].g>>8;
					  Img_B[i][j]=l64[j].b>>8; Img_A[i][j]=l64[j].a>>8;
					}
			}

		if (frgmap) { for(int i=0;i!=yres;i++)
							{ frgmap->GetLinearPixels(0,i,frgmap->Width(),l64);

							  for(int j=0;j!=xres;j++)
									{ Tmg_R[i][j]=l64[j].r>>8; Tmg_G[i][j]=l64[j].g>>8;
									  Tmg_B[i][j]=l64[j].b>>8; Tmg_A[i][j]=l64[j].a>>8;
									}
							}
					}

		free(l64);
}

//**********************************************************************
//* Send Image To Output
uchar localr=255;	// Anti Crack
uchar localb=0;
#define PutCol(x,y) Img_R[x][y]=localr; Img_G[x][y]=Img_B[x][y]=localb; Img_A[x][y]=255;

int RegistrationCheck;

inline void ImageFilter_LensF::PutImage(void)
{		BMM_Color_64	*l64=(BMM_Color_64 *)malloc(srcmap->Width()*sizeof(BMM_Color_64));
	    BMM_Color_64	*l65=(BMM_Color_64 *)malloc(srcmap->Width()*sizeof(BMM_Color_64));
		WORD *mask16 = NULL;

		if (mskmap) {	mask16 = (WORD *)calloc(srcmap->Width(),sizeof(WORD));	}  

		//**********************************************************************
		// If Not Registered Version PUT Red Cross In Centre Of Screen
		#ifndef REGISTERED
						{ int cx=xres/2;
						  int cy=yres/2;
						  int obs=yres/4;

						  for(int i=-obs;i!=obs;i++)
									{ int ay=cx+i,ax=cy+i;
									  if ((ay>=1)&&(ay<xres-2))
									  if ((ax>=1)&&(ax<yres-2))
												{ PutCol(ax-1,ay-1); PutCol(ax  ,ay-1); PutCol(ax+1,ay-1);
												  PutCol(ax-1,ay  ); PutCol(ax  ,ay  ); PutCol(ax+1,ay  );
												  PutCol(ax-1,ay+1); PutCol(ax  ,ay+1); PutCol(ax+1,ay+1);
												}

									  ax=cy+i; ay=cx-i;

									  if ((ay>=1)&&(ay<xres-2))
									  if ((ax>=1)&&(ax<yres-2))
												{ PutCol(ax-1,ay-1); PutCol(ax  ,ay-1); PutCol(ax+1,ay-1);
												  PutCol(ax-1,ay  ); PutCol(ax  ,ay  ); PutCol(ax+1,ay  );
												  PutCol(ax-1,ay+1); PutCol(ax  ,ay+1); PutCol(ax+1,ay+1);
												}

									}
						}
		#endif

		// LOTS OF ANTI CRACK MEASURES IN THIS
		int tmp1=encr(GetCode());		// Don't want to slow it down to much !
		int tmp2=encr(encr(Code()));

		for(int i=StartLine;i<yres;i+=SkipLine)
			{ if (mskmap) 
					{ mskmap->Get16Gray(0,i,srcmap->Width(),mask16);
		              srcmap->GetLinearPixels(0,i,srcmap->Width(),l65);

					  for(int j=0;j!=xres;j++)
							{	float tmp=(float)mask16[j]/65535.0f,itmp=1.0f-tmp;

							    l64[j].r=(float)(0.5f+(255.0f*tmp*(float)Img_R[i][j])+(itmp*(float)l65[j].r));
								l64[j].g=(float)(0.5f+(255.0f*tmp*(float)Img_G[i][j])+(itmp*(float)l65[j].g));
								l64[j].b=(float)(0.5f+(255.0f*tmp*(float)Img_B[i][j])+(itmp*(float)l65[j].b));
								l64[j].a=(float)(0.5f+(255.0f*tmp*(float)Img_A[i][j])+(itmp*(float)l65[j].a));

								l64[j].r=l64[j].r^tmp1^tmp2;
								l64[j].g=l64[j].g^tmp1^tmp2;
								l64[j].b=l64[j].b^tmp1^tmp2;
								l64[j].a=l64[j].a^tmp1^tmp2;
								tmp1++; tmp2++; // hehe work this one out crackerz
							}
					}
			  else	{	for(int j=0;j!=xres;j++)
							{	l64[j].r=Img_R[i][j]<<8;
								l64[j].g=Img_G[i][j]<<8;
								l64[j].b=Img_B[i][j]<<8;
								l64[j].a=Img_A[i][j]<<8;

								l64[j].r=l64[j].r^tmp1^tmp2;
								l64[j].g=l64[j].g^tmp1^tmp2;
								l64[j].b=l64[j].b^tmp1^tmp2;
								l64[j].a=l64[j].a^tmp1^tmp2;
								tmp1++; tmp2++; // hehe work this one out crackerz
							}
					}

			  srcmap->PutPixels(0,i,srcmap->Width()*(1+(tmp1^tmp2)),l64);	// Crafty !
			}

		free(l64);
		free(l65);

		if (mskmap) free(mask16);
}

//*******************************************************************************************************
//* Distance Transform 
				
BOOL ImageFilter_LensF::Delauney(float **FromX,float **FromY)
{			int	x, y;
			
			rinfo.StatusProgress(0,10);

			//**************************************
			//
			//      XX
			//   XX --
			//

			for(x=1;x!=xres;x++)
			{	int x1=FromX[0][x-1]+1;
				if (x1<FromX[0][x]) FromX[0][x]=x1;
			}

			for(y=1;y!=yres;y++)
				{	{	int y1=FromX[y-1][0]+1;
						if (y1<FromX[y][0]) FromX[y][0]=y1;
					}

					for(x=1;x!=xres;x++)
						{ int x1=FromX[y][x-1]+1;
					      int y1=FromX[y-1][x]+1;

						  if (x1<FromX[y][x]) FromX[y][x]=x1;
						  if (y1<FromX[y][x]) FromX[y][x]=y1;
						}
				}

			//**************************************
			//
			//   -- XX
			//   XX 
			//

			for(x=xres-2;x>=0;x--)
			{	int x1=FromX[0][x+1]+1;
				if (x1<FromX[0][x]) FromX[0][x]=x1;
			}

			for(y=yres-2;y>=0;y--)
				{	{	int y1=FromX[y+1][0]+1;
						if (y1<FromX[y][0]) FromX[y][0]=y1;
					}

					for(x=xres-2;x>=0;x--)
						{ int x1=FromX[y][x+1]+1;
					      int y1=FromX[y+1][x]+1;

						  if (x1<FromX[y][x]) FromX[y][x]=x1;
						  if (y1<FromX[y][x]) FromX[y][x]=y1;
						}
				}

			//**************************************
			//
			//   XX
			//   -- XX
			//

			for(x=xres-2;x!=0;x--)
			{	int x1=FromX[0][x+1]+1;
				if (x1<FromX[0][x]) FromX[0][x]=x1;
			}

			for(y=1;y!=yres;y++)
				{	{	int y1=FromX[y-1][0]+1;
						if (y1<FromX[y][0]) FromX[y][0]=y1;
					}

					for(x=xres-2;x!=0;x--)
						{ int x1=FromX[y][x+1]+1;
					      int y1=FromX[y-1][x]+1;

						  if (x1<FromX[y][x]) FromX[y][x]=x1;
						  if (y1<FromX[y][x]) FromX[y][x]=y1;
						}
				}

			//**************************************
			//
			//   XX --
			//      XX
			//

			for(x=1;x!=xres;x++)
			{	int x1=FromX[0][x-1]+1;
				if (x1<FromX[0][x]) FromX[0][x]=x1;
			}

			for(y=yres-2;y>=0;y--)
				{	{	int y1=FromX[y+1][0]+1;
						if (y1<FromX[y][0]) FromX[y][0]=y1;
					}

					for(x=1;x!=xres;x++)
						{ int x1=FromX[y][x-1]+1;
					      int y1=FromX[y+1][x]+1;

						  if (x1<FromX[y][x]) FromX[y][x]=x1;
						  if (y1<FromX[y][x]) FromX[y][x]=y1;
						}
				}

			return FALSE;
}			

//*******************************************************************************************************
//* Even Faster 2D Blurring Routines 

// This Bluring function is based on blurring the Distance Transform and recovering angles again
// This is the best algorithm I've come up with so far, and it's the 10th try !
BOOL ImageFilter_LensF::QuickBlur(float Size,float **FromX,float **FromY)
{		int x,y;

		if (Size>0.0f) 
			{	float	Size2=fsqr((float)Size)*2.0f;
				float	FallOff=1.0f/(float)Size;
				int		ttSize=(int)floor(Size);

				float *Lookup =(float *)malloc(sizeof(float)*(2*ttSize+1));
				float *Lookup2=(float *)malloc(sizeof(float)*(ttSize+1));
				Lookup+=ttSize;

				for(x=-ttSize;x<=ttSize;x++)
					Lookup[x]=1.0f-fsqr((float)x*FallOff);

				for(x=0;x<=ttSize;x++)
					{	Lookup2[x]=0;
						for(y=-x;y<=x;y++)
							Lookup2[x]+=Lookup[y];

						Lookup2[x]=1.0/Lookup2[x];
					}

				float *TmpX=(float *)malloc(sizeof(float)*xres);
				for(y=0;y!=yres;y++)
					{ if (rinfo.Abort()) { free(TmpX); return TRUE; }
					  rinfo.StatusProgress(y,yres*2);
					  for(x=0;x!=xres;x++)
							{ float c=0.0f;
							  float tx=0.0f;

							  int tSize=ttSize;

							  if (x-tSize<0)		tSize=x;
							  if (x+tSize>=xres)	tSize=xres-x-1;

							  for (int _x=-tSize;_x<=tSize;_x++) tx+=Lookup[_x]*(float)FromX[y][x+_x];
							  TmpX[x]=tx*Lookup2[tSize];
							}

					  for(x=0;x!=xres;x++) FromX[y][x]=TmpX[x];
					}
				free(TmpX);

				TmpX=(float *)malloc(sizeof(float)*yres);
				for(x=0;x!=xres;x++)
					{ if (rinfo.Abort()) { free(TmpX); return TRUE; }
					  rinfo.StatusProgress(x+xres,xres*2);
					  for(y=0;y!=yres;y++)
							{ float c=0.0f;
							  float tx=0.0f;

							  int tSize=ttSize;

							  if (y-tSize<0)		tSize=y;
							  if (y+tSize>=yres)	tSize=yres-y-1;

							  for (int _y=-tSize;_y<=tSize;_y++) tx+=Lookup[_y]*(float)FromX[y+_y][x];
							  TmpX[y]=tx*Lookup2[tSize];
							}

					  for(y=0;y!=yres;y++) FromX[y][x]=TmpX[y];
					}

				free(TmpX);

				Lookup-=ttSize; free(Lookup);
				free(Lookup2);
			}

		for(y=0;y!=yres-1;y++)
		for(x=0;x!=xres-1;x++)
				{ float dX=FromX[y][x+1]-FromX[y][x];
		          float dY=FromX[y+1][x]-FromX[y][x];
				  float dst2=sqrt(dX*dX+dY*dY);

				  if (dst2==0.0f) { FromX[y][x]=0; FromY[y][x]=0; }
				  else	{ dst2=1.0f/dst2;
	
						  dX*=dst2; dY*=dst2;

						  float dst=FromX[y][x];
						   
						  FromX[y][x]=dst*dX;
						  FromY[y][x]=dst*dY;
						}
				}

		return FALSE;
}


BOOL ImageFilter_LensF::QuickBlur(float Size,uchar **From)
{		int x,y;

		if (Size>0.0f) 
			{	float	Size2=fsqr((float)Size)*2.0f;
				float	FallOff=1.0f/(float)Size;
				int		ttSize=(int)floor(Size);

				float *Lookup =(float *)malloc(sizeof(float)*(2*ttSize+1));
				float *Lookup2=(float *)malloc(sizeof(float)*(ttSize+1));
				Lookup+=ttSize;

				for(x=-ttSize;x<=ttSize;x++)
					Lookup[x]=1.0f-fsqr((float)x*FallOff);

				for(x=0;x<=ttSize;x++)
					{	Lookup2[x]=0;
						for(y=-x;y<=x;y++)
							Lookup2[x]+=Lookup[y];

						Lookup2[x]=1.0/Lookup2[x];
					}

				float *TmpX=(float *)malloc(sizeof(float)*xres);
				for(y=0;y!=yres;y++)
					{ if (rinfo.Abort()) 
							{	free(TmpX); 
								free(Lookup); 
								free(Lookup2); 
								return TRUE; 
							}

					  rinfo.StatusProgress(y,yres*2);
					  for(x=0;x!=xres;x++)
							{ float c=0.0f;
							  float tx=0.0f;

							  int tSize=ttSize;

							  if (x-tSize<0)		tSize=x;
							  if (x+tSize>=xres)	tSize=xres-x-1;

							  for (int _x=-tSize;_x<=tSize;_x++) tx+=Lookup[_x]*(float)From[y][x+_x];
							  TmpX[x]=tx*Lookup2[tSize];
							}

					  for(x=0;x!=xres;x++) From[y][x]=(uchar)(TmpX[x]+0.5);
					}
				free(TmpX);

				TmpX=(float *)malloc(sizeof(float)*yres);
				for(x=0;x!=xres;x++)
					{ if (rinfo.Abort()) 
							{	free(TmpX); 
								free(Lookup); 
								free(Lookup2); 
								return TRUE; 
							}

					  rinfo.StatusProgress(x+xres,xres*2);
					  for(y=0;y!=yres;y++)
							{ float c=0.0f;
							  float tx=0.0f;

							  int tSize=ttSize;

							  if (y-tSize<0)		tSize=y;
							  if (y+tSize>=yres)	tSize=yres-y-1;

							  for (int _y=-tSize;_y<=tSize;_y++) tx+=Lookup[_y]*(float)From[y+_y][x];
							  TmpX[y]=tx*Lookup2[tSize];
							}

					  for(y=0;y!=yres;y++) From[y][x]=(uchar)(TmpX[y]+0.5);
					}

				free(TmpX);

				Lookup-=ttSize; free(Lookup);
				free(Lookup2);
			}

		return FALSE;
}

BOOL ImageFilter_LensF::QuickBlur(float Size,uchar **FromR,uchar **FromG,uchar **FromB,uchar **FromA)
{		int x,y;

		if (Size>0.0f) 
			{	float	Size2=fsqr((float)Size)*2.0f;
				float	FallOff=1.0f/(float)Size;
				int		ttSize=(int)floor(Size);

				float *Lookup =(float *)malloc(sizeof(float)*(2*ttSize+1));
				float *Lookup2=(float *)malloc(sizeof(float)*(ttSize+1));
				Lookup+=ttSize;

				for(x=-ttSize;x<=ttSize;x++)
					Lookup[x]=1.0f-fsqr((float)x*FallOff);

				for(x=0;x<=ttSize;x++)
					{	Lookup2[x]=0;
						for(y=-x;y<=x;y++)
							Lookup2[x]+=Lookup[y];

						Lookup2[x]=1.0/Lookup2[x];
					}

				float *TmpR=(float *)malloc(sizeof(float)*xres);
				float *TmpG=(float *)malloc(sizeof(float)*xres);
				float *TmpB=(float *)malloc(sizeof(float)*xres);
				float *TmpA=(float *)malloc(sizeof(float)*xres);

				for(y=0;y!=yres;y++)
					{ if (rinfo.Abort()) 
							{	free(TmpR); 
								free(TmpB); 
								free(TmpG); 
								free(TmpA); 
								free(Lookup); 
								free(Lookup2); 
								return TRUE; 
							}

					  rinfo.StatusProgress(y,yres*2);
					  for(x=0;x!=xres;x++)
							{ float c=0.0f;
							  float tr=0.0f,tg=0.0f,tb=0.0f,ta=0.0f;

							  int tSize=ttSize;

							  if (x-tSize<0)		tSize=x;
							  if (x+tSize>=xres)	tSize=xres-x-1;

							  for (int _x=-tSize;_x<=tSize;_x++) 
									{	tr+=Lookup[_x]*(float)FromR[y][x+_x];
										tg+=Lookup[_x]*(float)FromG[y][x+_x];
										tb+=Lookup[_x]*(float)FromB[y][x+_x];
										ta+=Lookup[_x]*(float)FromA[y][x+_x];
									}

							  TmpR[x]=tr*Lookup2[tSize];
							  TmpG[x]=tg*Lookup2[tSize];
							  TmpB[x]=tb*Lookup2[tSize];
							  TmpA[x]=ta*Lookup2[tSize];
							}

					  for(x=0;x!=xres;x++) 
							{	FromR[y][x]=(uchar)(TmpR[x]+0.5);
								FromG[y][x]=(uchar)(TmpG[x]+0.5);
								FromB[y][x]=(uchar)(TmpB[x]+0.5);
								FromA[y][x]=(uchar)(TmpA[x]+0.5);
							}
					}
				free(TmpR);
				free(TmpG);
				free(TmpB);
				free(TmpA);

				TmpR=(float *)malloc(sizeof(float)*yres);
				TmpG=(float *)malloc(sizeof(float)*yres);
				TmpB=(float *)malloc(sizeof(float)*yres);
				TmpA=(float *)malloc(sizeof(float)*yres);

				for(x=0;x!=xres;x++)
					{ if (rinfo.Abort()) 
							{	free(TmpR); 
								free(TmpB); 
								free(TmpG); 
								free(TmpA); 
								free(Lookup); 
								free(Lookup2); 
								return TRUE; 
							}

					  rinfo.StatusProgress(x+xres,xres*2);
					  for(y=0;y!=yres;y++)
							{ float c=0.0f;
							  float tr=0.0f,tg=0.0f,tb=0.0f,ta=0.0f;

							  int tSize=ttSize;

							  if (y-tSize<0)		tSize=y;
							  if (y+tSize>=yres)	tSize=yres-y-1;

							  for (int _y=-tSize;_y<=tSize;_y++) 
									{	tr+=Lookup[_y]*(float)FromR[y+_y][x];
										tg+=Lookup[_y]*(float)FromG[y+_y][x];
										tb+=Lookup[_y]*(float)FromB[y+_y][x];
										ta+=Lookup[_y]*(float)FromA[y+_y][x];
									}

							  TmpR[y]=tr*Lookup2[tSize];
							  TmpG[y]=tg*Lookup2[tSize];
							  TmpB[y]=tb*Lookup2[tSize];
							  TmpA[y]=ta*Lookup2[tSize];
							}

					  for(y=0;y!=yres;y++) 
							{	FromR[y][x]=(uchar)(TmpR[y]+0.5);
								FromG[y][x]=(uchar)(TmpG[y]+0.5);
								FromB[y][x]=(uchar)(TmpB[y]+0.5);
								FromA[y][x]=(uchar)(TmpA[y]+0.5);
							}
					}

				free(TmpR);
				free(TmpG);
				free(TmpB);
				free(TmpA);

				Lookup-=ttSize; free(Lookup);
				free(Lookup2);
			}

		return FALSE;
}

//****************************************************************************************************
// Fill In RenderNFO
void ImageFilter_LensF::SetupRenderInfo(float Time)
{		rinfo.Img_R=Img_R;
		rinfo.Img_G=Img_G;
		rinfo.Img_B=Img_B;
		rinfo.Img_A=Img_A;

		rinfo.Tmg_R=Tmg_R;
		rinfo.Tmg_G=Tmg_G;
		rinfo.Tmg_B=Tmg_B;
		rinfo.Tmg_A=Tmg_A;

		rinfo.Bak_R=NULL;
		rinfo.Bak_G=NULL;
		rinfo.Bak_B=NULL;
		rinfo.Bak_A=NULL;

		rinfo.Col_R=NULL;
		rinfo.Col_G=NULL;
		rinfo.Col_B=NULL;
		rinfo.Col_A=NULL;

		rinfo.xres=xres;
		rinfo.yres=yres;

		rinfo.Time=Time*data.TimeConvert;

		rinfo.UseSingle=0;
		rinfo.cr=rinfo.cg=rinfo.cb=rinfo.ca=1.0f;

		if ((data.OutputOID!=-1)||(data.OutputMID!=-1))
				{ rinfo.UseBuffer=1;
				  rinfo.mbuffer=mbuffer; rinfo.mbufout=data.OutputMID;
				  rinfo.obuffer=obuffer; rinfo.obufout=data.OutputOID;
				}
		else rinfo.UseBuffer=0;

		rinfo.zbuffer=NULL;
		if (data.CompositeZ)	
				{ rinfo.CompZ=1; 
		          rinfo.zbuffer=zbuffer; 
				}
		else rinfo.CompZ=0;

		if (data.ProcessZ)		
				{ rinfo.UseZ=1; 
		          rinfo.zbuffer=zbuffer; 
				}
		else rinfo.UseZ=0;

		rinfo.CameraDepth=data.CameraDepth;

		rinfo.SizeMultiply=1000.0f;
		rinfo.SizeMultiplyParticle=1.0f;

		if (data.AntiAliasing==0) rinfo.AntiAliasing=1.0f;
		else if (data.AntiAliasing==1) rinfo.AntiAliasing=0.5f;
		else if (data.AntiAliasing==2) rinfo.AntiAliasing=0.334f;

		rinfo.StartLine=StartLine;
		rinfo.SkipLine=SkipLine;
}

//****************************************************************************************************
// Render A Region

#define DEFFLAG(x,y) ((rinfo.FromX[y][x]==0.0f)&&(rinfo.FromY[y][x]==0.0f))

BOOL ImageFilter_LensF::Go(void)
{		BOOL abort=FALSE;
		int x,y;

		// Get Min Z Depth ... ughhh this is complicated because of aliasing
		if (rinfo.zbuffer)
				{ for(y=0;y!=yres;y++)
				  for(x=0;x!=xres;x++)
				  if (DEFFLAG(x,y))
				  if ((zbuffer[x+xres*y]<0.0f)&&(zbuffer[x+xres*y]<rinfo.Depth)) rinfo.Depth=zbuffer[x+xres*y]; 
				}

		// Get Centroid
		float xpos=0.0f;
		float ypos=0.0f;
		float npos=0.0f;

		for(x=0;x!=xres;x++)
		for(y=0;y!=yres;y++)
		if ((rinfo.FromX[y][x]==0.0f)&&(rinfo.FromY[y][x]==0.0f)) { xpos+=(float)x; ypos+=(float)y; npos++; }

		if (npos==0.0f) return FALSE;
		xpos/=npos; ypos/=npos;

		if (data.CSE==0)
				{ rinfo.UseSingle=1;
				  rinfo.xpos=xpos;
				  rinfo.ypos=ypos;
				  rinfo.cr=1.0f; rinfo.cg=1.0f;
				  rinfo.cb=1.0f; rinfo.ca=1.0f;
				  if (data.Test) { for(int i=-5;i<=5;i++)
									  { TestPixel(xpos+i,ypos+i); 
								        TestPixel(xpos+i,ypos-i); 
									  }
								 }
				  else { return data.LensFlare.RenderHQArea(rinfo); }
				  return FALSE;
				}

		if (data.MaskInside==2)
			{ for(x=0;x!=xres;x++)
			  for(y=0;y!=yres;y++) rinfo.Filter[y][x]=0;
			}
		else if (data.MaskInside==1)
			{ for(x=0;x!=xres;x++)
			  for(y=0;y!=yres;y++)
					{ if ((rinfo.FromX[y][x]==0.0f)&&(rinfo.FromY[y][x]==0.0f))	rinfo.FromX[y][x]=rinfo.FromY[y][x]=10000.0f;
					  else rinfo.FromX[y][x]=rinfo.FromY[y][x]=0.0f; 

					  rinfo.Filter[y][x]=255-rinfo.Filter[y][x];
					}
			}

		if (data.Test) { for(x=0;x!=xres;x++)
						 for(y=0;y!=yres;y++)
							{ Img_R[y][x]=rinfo.Filter[y][x];
						      if ((rinfo.FromX[y][x]==0.0f)&&(rinfo.FromY[y][x]==0.0f)) Img_G[y][x]=255;
							}

						 return FALSE;
					   }

		rinfo.StatusText("Pre Process #1"); if (abort==FALSE) abort=Delauney(rinfo.FromX,rinfo.FromY);
		rinfo.StatusText("Pre Process #2"); if (abort==FALSE) abort=QuickBlur((float)data.ExternalSmoothness*xres,rinfo.FromX,rinfo.FromY);
		rinfo.StatusText("Pre Process #3"); if (abort==FALSE) abort=QuickBlur((float)data.InternalSmoothness*xres,rinfo.Filter);

		// Colourise If Need Be
		if (data.Colourise)
				{	rinfo.StatusText("Pre Process #4"); 
		            Alloc2D(rinfo.Col_R,uchar,yres,xres); 
					Alloc2D(rinfo.Col_G,uchar,yres,xres);
					Alloc2D(rinfo.Col_B,uchar,yres,xres);
					Alloc2D(rinfo.Col_A,uchar,yres,xres);

					for(x=0;x!=xres;x++)
					for(y=0;y!=yres;y++)
							{ int _x=(int)(x-rinfo.FromX[y][x]+0.5f); 
							  if (_x>=xres) _x=xres-1;
							  else if (_x<0) _x=0;

							  int _y=(int)(y-rinfo.FromY[y][x]+0.5f); 
							  if (_y>=yres) _y=yres-1;
							  else if (_y<0) _y=0;

						      rinfo.Col_R[y][x]=rinfo.Img_R[_y][_x];
					          rinfo.Col_G[y][x]=rinfo.Img_G[_y][_x];
							  rinfo.Col_B[y][x]=rinfo.Img_B[_y][_x];
							  rinfo.Col_A[y][x]=rinfo.Img_A[_y][_x];
							}
					
					rinfo.StatusText("Pre Process #4"); 
					if (abort==FALSE) abort=QuickBlur((float)data.ColouriseSmoothness*xres,rinfo.Col_R,rinfo.Col_G,rinfo.Col_B,rinfo.Col_A);
				}

		rinfo.StatusText("Rendering Area"); 
		rinfo.StatusProgress(0,4);
		data.LensFlare.GetFrame(rinfo.Time);
		if (abort==FALSE) abort=data.LensFlare.RenderHQArea(rinfo);

		if (data.Colourise)
				{	Free2D(rinfo.Col_R,yres);
					Free2D(rinfo.Col_G,yres);
					Free2D(rinfo.Col_B,yres);
					Free2D(rinfo.Col_A,yres);
				}

		return abort;
}

//*******************************************************************
// Non-Recursive(!) FloodFill v2.0
void ImageFilter_LensF::FloodFill(uchar **Flag,signed short x,signed short y)
{	uchar **Fill;
	Alloc2D(Fill,uchar,yres,xres);

	int xx,yy;

	for(xx=0;xx<xres;xx++)
	for(yy=0;yy<yres;yy++)
			Fill[yy][xx]=0;

	Fill[y][x]=1;

	int Finished;
	do	{ Finished=0;

	      for(xx=0;xx<xres;xx++)
		  for(yy=0;yy<yres;yy++)
				{ if (Fill[yy][xx])
						{	rinfo.Filter[yy][xx]=Flag[yy][xx]; Flag[yy][xx]=0;
							rinfo.FromX[yy][xx]=rinfo.FromY[yy][xx]=0.0f;
							Fill[yy][xx]=0;

							if (xx>0)		if (Flag[yy][xx-1]) Fill[yy][xx-1]=1;
							if (xx<xres-1)	if (Flag[yy][xx+1]) Fill[yy][xx+1]=1;

							if (yy>0)		{	if (xx>0)		if (Flag[yy-1][xx-1]) Fill[yy-1][xx-1]=1;
																if (Flag[yy-1][xx  ]) Fill[yy-1][xx  ]=1;
												if (xx<xres-1)	if (Flag[yy-1][xx+1]) Fill[yy-1][xx+1]=1;
											}

							if (yy<yres-1)	{	if (xx>0)		if (Flag[yy+1][xx-1]) Fill[yy+1][xx-1]=1;
																if (Flag[yy+1][xx  ]) Fill[yy+1][xx  ]=1;
												if (xx<xres-1)	if (Flag[yy+1][xx+1]) Fill[yy+1][xx+1]=1;
											}
							Finished=1;
						}
				}
		}
	while(Finished);

	Free2D(Fill,yres);
}

BOOL ImageFilter_LensF::GoMulti(uchar **Flag)
{		float Time=MaxTime/TicksPerFrame;
		BOOL abort=FALSE;

		SetupRenderInfo(Time);
		data.LensFlare.GetFrame(rinfo.Time);

		// Normalise Flag Array
		int y,x;
		/*float min=0.0f;
		for(y=0;y!=yres;y++) 
		for(x=0;x!=xres;x++) 
		if (Flag[y][x]>min) min=Flag[y][x];

		if (min==0.0f) return FALSE;

		float Mult=254.0f/min;

		for(y=0;y!=yres;y++) 
		for(x=0;x!=xres;x++) 
			Flag[y][x]=(unsigned char)((float)((float)Flag[y][x]*Mult+0.5f));*/

		// Crude Flag Anti Aliasing
		/*float **aa;
		Alloc2D(aa,float,yres,xres);

		for(y=0;y<yres;y++) 
		for(x=0;x<xres;x++) 
		{	float tmp=0;
			
			tmp+=1.0f;
			aa[y][x]+=Flag[y][x];

			if (x>0)		{ tmp+=0.2f;  aa[y][x]+=0.20f*Flag[y][x-1]; }
			if (x<xres-1)	{ tmp+=0.2f;  aa[y][x]+=0.20f*Flag[y][x+1]; }

			if (y>0)		{	if (x>0)		{ tmp+=0.1f; aa[y][x]+=0.1f*Flag[y-1][x-1]; }
												{ tmp+=0.2f;  aa[y][x]+=0.20f*Flag[y-1][x]; }
								if (x<xres-1)	{ tmp+=0.1f; aa[y][x]+=0.1f*Flag[y-1][x+1]; }
							}

			if (y<yres-1)	{	if (x>0)		{ tmp+=0.1f; aa[y][x]+=0.1f*Flag[y+1][x-1]; }
												{ tmp+=0.2f;  aa[y][x]+=0.2f*Flag[y+1][x]; }
								if (x<xres-1)	{ tmp+=0.1f; aa[y][x]+=0.1f*Flag[y+1][x+1]; }
							}
			aa[y][x]/=tmp;
		}

		for(y=0;y<yres;y++) 
		for(x=0;x<xres;x++) 
			Flag[y][x]=(unsigned char)aa[y][x];

		Free2D(aa,yres);*/

		// Handle Field Rendering Stuff
		if (FieldRender)	{ for(y=0;y<yres-1;y+=2)
									{ if (StartLine)	for(x=0;x!=xres;x++) 
																{ Flag[y][x] =Flag[y+1][x];
							                                      Img_R[y][x]=Img_R[y+1][x];
																  Img_G[y][x]=Img_G[y+1][x];
																  Img_B[y][x]=Img_B[y+1][x];
																  Img_A[y][x]=Img_A[y+1][x];
																}

							          else				
														for(x=0;x!=xres;x++) 
																{ Flag[y+1][x]=Flag[y][x];
									                              Img_R[y+1][x]=Img_R[y][x];
																  Img_G[y+1][x]=Img_G[y][x];
																  Img_B[y+1][x]=Img_B[y][x];
																  Img_A[y+1][x]=Img_A[y][x];
																}
									}
							}

		Alloc2D(rinfo.FromX ,float,yres,xres);
		Alloc2D(rinfo.FromY ,float,yres,xres);
		Alloc2D(rinfo.Filter,uchar,yres,xres);
		int Count=0;

		if (data.Single)	{ int x,y;
		                      for(x=0;x!=xres;x++)
							  for(y=0;y!=yres;y++)
									{ if (Flag[y][x])	{ rinfo.Filter[y][x]=Flag[y][x]; 
							                              rinfo.FromX[y][x]=rinfo.FromY[y][x]=0.0f; 
														}
							          else				{ rinfo.Filter[y][x]=0; 
									                      rinfo.FromX[y][x]=rinfo.FromY[y][x]=10000.0f; 
														}
									}

							  abort=Go();
							}
		else				{	int Finished;

								int x,y;
								Finished=1;
								rinfo.StatusText("Scanning Image"); 

								// Clear it
								

								// Find First Unused Pixel
								int Found=0;
								for(x=0;((abort==0)&&(x<xres));x++)
								for(y=0;((abort==0)&&(y<yres));y++)
								if (Flag[y][x]) { int _x,_y;

												  for(_x=0;_x!=xres;_x++)
													for(_y=0;_y!=yres;_y++)
														{ rinfo.FromX[_y][_x]=rinfo.FromY[_y][_x]=10000.0f;
														  rinfo.Filter[_y][_x]=0;
														}

												  FloodFill(Flag,(signed short)x,(signed short)y);
												  abort=Go();
												}
							}

		Free2D(rinfo.FromX,yres);
		Free2D(rinfo.FromY,yres);
		Free2D(rinfo.Filter,yres);

		return abort;
}

//****************************************************************************************************
// Render Object ID Elements
BOOL ImageFilter_LensF::RenderObject(void)
{		if (!data.RenderObj) return FALSE;

		// Must Do It Floating Point
		uchar **Flag;
		
		rinfo.StatusText("Pre Process");
		Alloc2D(Flag,uchar,yres,xres);
		
		int x,y;
		for(x=0;x!=xres;x++)
		for(y=0;y!=yres;y++)
				{ if (obuffer[y*xres+x]==data.ObjectID1) Flag[y][x]=Img_A[y][x];
				  else	Flag[y][x]=0;
				}

		BOOL abort=GoMulti(Flag);		
		if ((abort)||(data.ObjectID2==-1)) { Free2D(Flag,yres); return abort; }

		for(x=0;x!=xres;x++)
		for(y=0;y!=yres;y++)
				{ if (obuffer[y*xres+x]==data.ObjectID2) Flag[y][x]=Img_A[y][x];
				  else	Flag[y][x]=0; 
				}

		abort=GoMulti(Flag);		

		Free2D(Flag,yres);

		return abort;
}

//****************************************************************************************************
// Render Object ID Elements
BOOL ImageFilter_LensF::RenderMaterial(void)
{		if (!data.RenderMat) return FALSE;

		// Must Do It Floating Point
		uchar **Flag;
		
		rinfo.StatusText("Pre Process");
		Alloc2D(Flag,uchar,yres,xres);
		
		int x,y;
		for(x=0;x!=xres;x++)
		for(y=0;y!=yres;y++)
				{ if (mbuffer[y*xres+x]==data.MaterialID1) Flag[y][x]=Img_A[y][x];
				  else	Flag[y][x]=0;
				}

		BOOL abort=GoMulti(Flag);		
		if ((abort)||(data.MaterialID2==-1)) { Free2D(Flag,yres); return abort; }

		for(x=0;x!=xres;x++)
		for(y=0;y!=yres;y++)
				{ if (mbuffer[y*xres+x]==data.MaterialID2) Flag[y][x]=Img_A[y][x];
				  else	Flag[y][x]=0;
				}

		abort=GoMulti(Flag);		

		Free2D(Flag,yres);

		return abort;
}

//****************************************************************************************************
// Render Colour Elements
BOOL ImageFilter_LensF::RenderColour(void)
{		if (!data.RenderCol) return FALSE;

		// Must Do It Floating Point
		uchar **Flag;
		
		rinfo.StatusText("Pre Process");
		Alloc2D(Flag,uchar,yres,xres);
		
		int x,y;

		float HS=data.HueStart; HS-=(HS>1.0f)?1.0f:0.0f;
		float HE=data.HueEnd;	HE-=(HE>1.0f)?1.0f:0.0f;
		int Neg=0; if (HE<HS) Neg=1;

		if (data.HueEnd-data.HueStart>1.0f) { HS=0.0f; HE=1.0f; }

		for(x=0;x!=xres;x++)
		for(y=0;y!=yres;y++)
				{ int Use=1;
				  float tmpr,tmpg,tmpb;

				  ExpandRealPixel(rbuffer[y*xres+x],tmpr,tmpg,tmpb);
				  float max=tmpr;
				  if (tmpg>max) max=tmpg;
				  if (tmpb>max) max=tmpb;

				  max*=255.0f; if (max!=0) tmpr/=max; tmpg/=max; tmpb/=max;

				  int h,s,v;
				  RGBtoHSV(RGB((uchar)tmpr,(uchar)tmpg,(uchar)tmpb),&h,&s,&v);
				  float fh=(float)h/255.0f;
				  float fs=(float)s/255.0f;
				  float fv=max/255.0f; if (fv>10.0f) fv=0.0f;

				  if (Neg)	{ if ((fh>HE)&&(fh<HS)) Use=0; }
				  else if ((fh<HS)||(fh>HE)) Use=0;

				  if ((fs<data.SatStart)||(fs>data.SatEnd)) Use=0;
				  if ((fv<data.IntStart)||(fs>data.IntEnd)) Use=0;

				  if ((data.ColObjectID!=-1)&&(data.ColObjectID!=obuffer[y*xres+x])) Use=0;
				  if ((data.ColMatID!=-1)&&(data.ColMatID!=mbuffer[y*xres+x])) Use=0;
				
				  if (Use) Flag[y][x]=Img_A[y][x];
				  else Flag[y][x]=0;
				}

		BOOL abort=GoMulti(Flag);

		Free2D(Flag,yres);
		return abort;
}

//****************************************************************************************************
// Render Colour Elements
BOOL ImageFilter_LensF::RenderFixed(void)
{	  if (!data.RenderFxd) return FALSE;
	  float Time=MaxTime/TicksPerFrame;

	  rinfo.StatusText("Render Fixed");
	  
	  SetupRenderInfo(Time);
	  data.LensFlare.GetFrame(rinfo.Time);
	  rinfo.UseSingle=1;
	  rinfo.xpos=0.5f*(1.0f+data.PosX)*(float)xres;
	  rinfo.ypos=0.5f*(1.0f+data.PosY)*(float)yres;
	  rinfo.cr=1.0f; rinfo.cg=1.0f;
	  rinfo.cb=1.0f; rinfo.ca=1.0f;

	  if (data.Test) { for(int i=-5;i<=5;i++)
							  { TestPixel(rinfo.xpos+i,rinfo.ypos+i); 
								TestPixel(rinfo.xpos+i,rinfo.ypos-i); 
							  }
					   return FALSE;
					 }

	  return data.LensFlare.RenderHQArea(rinfo);		
}

BOOL ImageFilter_LensF::RenderPosn(float x, float y,float z,float r,float g,float b,float a,float Time,float Rad)
{		// Set It Up and Render
	    SetupRenderInfo(Time);
		data.LensFlare.GetFrame(rinfo.Time);
	    rinfo.UseSingle=1;
	    rinfo.xpos=x;
	    rinfo.ypos=y;
	    rinfo.Depth=z;
	    rinfo.cr=r; rinfo.cg=g;
	    rinfo.cb=b; rinfo.ca=a;
		rinfo.SizeMultiplyParticle=Rad;

	    // Get Screen Posn
	    int _x = (int)(x+0.5f);
	    int _y = (int)(y+0.5f);

		if (data.Test) { for(int i=-5;i<=5;i++)
							  { TestPixel(x+i,y+i); 
								TestPixel(x+i,y-i); 
							  }
						 return FALSE;
					   }

	    // Check Image Z Depth
		if ((data.ProcessZ==0)||(data.LinkPoint)) return data.LensFlare.RenderHQArea(rinfo);
	    else if ((zbuffer[_y*xres+_x]>0.0f)||(zbuffer[_y*xres+_x]-data.RenderRadius<z)) return data.LensFlare.RenderHQArea(rinfo); 
		return FALSE;
}

#include <Particle.h>
#include <Simpobj.h>

#include "stdmat.h"

int ImageFilter_LensF::ScanINodes(INode *node,int Start,
								  float *PosnX,float *PosnY,float *PosnZ,float *PosnT,
								  float *PosnR,float *PosnG,float *PosnB,float *PosnA,
								  float *PosnRad)
{	float Time=MaxTime/TicksPerFrame;
	int Ret=Start;
	ObjectState os=node->EvalWorldState(MaxTime);

	if (os.obj->SuperClassID()==LIGHT_CLASS_ID)
			{	/* SPOTlight : compute light value */
				LightObject		*lob = (LightObject *)os.obj;	
				ObjLightDesc	*LD  = lob->CreateLightDesc(node);
				LightState		*ls   = &LD->ls;
				Interval		tmp;
				lob->EvalLightState(MaxTime,tmp,ls);

				// Colour
				float r=ls->color.r*ls->intens; if (r>1.0f) r=1.0f; if (r<0.0f) r=0.0f;
				float g=ls->color.g*ls->intens; if (g>1.0f) g=1.0f; if (g<0.0f) g=0.0f;
				float b=ls->color.b*ls->intens; if (b>1.0f) b=1.0f; if (b<0.0f) b=0.0f;
				float a=1.0f;

				if (!data.Colourise) r=g=b=a=1.0f;

				// Get Object Position
				Matrix3	m=node->GetObjTMAfterWSM(MaxTime);
				Point3	worldPos=m.GetRow(3);

				// Get Camera Posn
				Point3	pCam(0,0,0);
				pCam =Inverse(RendInfo->worldToCam[0])*pCam;

				Point3	camPos =worldPos*RendInfo->worldToCam[0];
				Point2	screenPos =RendInfo->MapCamToScreen(camPos );

				if (camPos.z<=0.0f)
						  {	if (PosnX)
									 { PosnX[Ret]=screenPos.x;
									   PosnY[Ret]=screenPos.y;
									   PosnZ[Ret]=camPos.z;
									   PosnR[Ret]=r; PosnG[Ret]=g;
									   PosnB[Ret]=b; PosnA[Ret]=a;
									   PosnT[Ret]=Time;
									   PosnRad[Ret]=1.0f;
									 }
							Ret++;
						  }
			}
	else if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
			{ Object *obj=os.obj;
			  SimpleParticle *p=(SimpleParticle *)obj->GetInterface(I_PARTICLEOBJ);

			  if (p!=NULL)	{ ParticleSys pa=p->parts;
								
							  // Go Through All Particles !
							  for(int ii=0;ii<pa.Count();ii++)
									{	  // Colour
										  Mtl *Mtl=node->GetMtl();
										  float r,g,b,a;
										  if ((Mtl)&&(Mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)))
													{	StdMat* std = (StdMat *)Mtl;
										                Color col=std->GetDiffuse(MaxTime);
														r=col.r;
														g=col.g;
														b=col.b;
														a=std->GetOpacity(MaxTime);
													}
										  else r=g=b=a=1.0f;

										  if (!data.Colourise) r=g=b=a=1.0f;

										  // Get Object Position
										  Point3	worldPos=pa.points[ii];
										  TimeValue age=pa.ages[ii];

										  if (age>=0)	{ // Get Camera Posn
														  Point3	pCam(0,0,0);
														  pCam=Inverse(RendInfo->worldToCam[0])*pCam;

														  // Peturb Object Position Using Radius
														  Point3	Delta=worldPos-pCam;
														  float		lDelta=Length(Delta);
														  worldPos=pCam+Delta*((lDelta-data.RenderRadius)/lDelta);

														  // Get Size !
														  Point3	pCamX(1,0,0);
														  pCamX=Inverse(RendInfo->worldToCam[0])*pCamX-pCam;
														  if (data.RenderRadius>0.0f) pCamX*=data.RenderRadius/Length(pCamX);
														  pCamX+=worldPos;

														  Point3	camPos =worldPos*RendInfo->worldToCam[0];
														  Point3	camPosX=pCamX*RendInfo->worldToCam[0];
														  Point2	screenPos =RendInfo->MapCamToScreen(camPos );
														  Point2	screenPosX=RendInfo->MapCamToScreen(camPosX);

														  float Radius=sqrt((screenPos.x-screenPosX.x)*(screenPos.x-screenPosX.x)+
															                (screenPos.y-screenPosX.y)*(screenPos.y-screenPosX.y));

														  if (camPos.z<=0.0f)
																  {	if (PosnX)
																			 { PosnX[Ret]=screenPos.x;
																			   PosnY[Ret]=screenPos.y;
																			   PosnZ[Ret]=camPos.z;

																			   if (data.RenderRadius>0.0f) PosnRad[Ret]=Radius/(0.1f*(float)xres);
																			   else PosnRad[Ret]=1.0f;

																			   PosnR[Ret]=r; PosnG[Ret]=g;
																			   PosnB[Ret]=b; PosnA[Ret]=a;
																			   PosnT[Ret]=(float)(age/TicksPerFrame);
																			 }
																	Ret++;
																  }
														}
									}
							}
			  else			{ // Colour
							  Mtl *Mtl=node->GetMtl();
							  float r,g,b,a;

							  if ((Mtl)&&(Mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)))
									{	StdMat* std = (StdMat *)Mtl;
										Color col=std->GetDiffuse(MaxTime);
										r=col.r;
										g=col.g;
										b=col.b;
										a=std->GetOpacity(MaxTime);
									}
							  else r=g=b=a=1.0f;

							  if (!data.Colourise) r=g=b=a=1.0f;

							  // Get Object Position
							  Matrix3	m=node->GetObjTMAfterWSM(MaxTime);
							  Point3	worldPos=m.GetRow(3);

							  // Get Camera Posn
							  Point3	pCam(0,0,0);
							  pCam =Inverse(RendInfo->worldToCam[0])*pCam;

							  // Peturb Object Position Using Radius
							  Point3	Delta=worldPos-pCam;
							  float		lDelta=Length(Delta);
							  worldPos=pCam+Delta*((lDelta-data.RenderRadius)/lDelta);

							  Point3	camPos =worldPos*RendInfo->worldToCam[0];
							  Point2	screenPos =RendInfo->MapCamToScreen(camPos );

							  if (camPos.z<=0.0f)
									  {	if (PosnX)
												 { PosnX[Ret]=screenPos.x;
												   PosnY[Ret]=screenPos.y;
												   PosnZ[Ret]=camPos.z;
												   PosnRad[Ret]=1.0f;
												   PosnR[Ret]=r; PosnG[Ret]=g;
												   PosnB[Ret]=b; PosnA[Ret]=a;
												   PosnT[Ret]=Time;
												 }
										Ret++;
									  }
							}
			}
	else	{ // Colour
			  float r=1.0f,g=1.0f,b=1.0f,a=1.0f;

			  // Get Object Position
			  Matrix3	m=node->GetObjTMAfterWSM(MaxTime);
			  Point3	worldPos=m.GetRow(3);

			  // Get Camera Posn
			  Point3	pCam(0,0,0);
			  pCam =Inverse(RendInfo->worldToCam[0])*pCam;

			  // Peturb Object Position Using Radius
			  Point3	Delta=worldPos-pCam;
			  float		lDelta=Length(Delta);
			  worldPos=pCam+Delta*((lDelta-data.RenderRadius)/lDelta);

			  Point3	camPos =worldPos*RendInfo->worldToCam[0];
			  Point2	screenPos =RendInfo->MapCamToScreen(camPos );

			  if (camPos.z<=0.0f)
					  {	if (PosnX)
								 { PosnX[Ret]=screenPos.x;
								   PosnY[Ret]=screenPos.y;
								   PosnZ[Ret]=camPos.z;
								   PosnRad[Ret]=1.0f;
								   PosnR[Ret]=r; PosnG[Ret]=g;
								   PosnB[Ret]=b; PosnA[Ret]=a;
								   PosnT[Ret]=Time;
								 }
						Ret++;
					  }
			}

	return Ret;
}

BOOL ImageFilter_LensF::RenderLinked(void)
{		if (!data.RenderLink) return FALSE;

		int NoNodes=0;

		for(int i=0;i!=data.NoNames;i++)
			{ INode *node=Max()->GetINodeByName(data.Names[i]);

			  if (node)	{ NoNodes=ScanINodes(node,NoNodes,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL); }
			}

		float *PosnX=(float *)malloc(sizeof(float)*NoNodes);
		float *PosnY=(float *)malloc(sizeof(float)*NoNodes);
		float *PosnZ=(float *)malloc(sizeof(float)*NoNodes);
		float *PosnT=(float *)malloc(sizeof(float)*NoNodes);

		float *PosnR=(float *)malloc(sizeof(float)*NoNodes);
		float *PosnG=(float *)malloc(sizeof(float)*NoNodes);
		float *PosnB=(float *)malloc(sizeof(float)*NoNodes);
		float *PosnA=(float *)malloc(sizeof(float)*NoNodes);
		float *PosnRad=(float *)malloc(sizeof(float)*NoNodes);

		uchar *Flag=(uchar *)malloc(sizeof(uchar)*NoNodes);

		NoNodes=0;
		for(i=0;i!=data.NoNames;i++)
			{ INode *node=Max()->GetINodeByName(data.Names[i]);

			  if (node)	{ NoNodes=ScanINodes(node,NoNodes,PosnX,PosnY,PosnZ,PosnT,PosnR,PosnG,PosnB,PosnA,PosnRad); }
			}

		for(i=0;i!=NoNodes;Flag[i++]=0);

		rinfo.StatusText("Object Link");
		BOOL abort=FALSE;

		for(i=0;((abort==FALSE)&&(i!=NoNodes));i++)
			{	rinfo.StatusProgress(i+1,NoNodes);
				int Draw;
				float Dst=0;

				for(int j=0;j!=NoNodes;j++)
					if ((Flag[j]==0)&&(PosnZ[j]<=Dst)) { Dst=PosnZ[j]; Draw=j; }
				Flag[Draw]=1;

				abort=RenderPosn(PosnX[Draw],PosnY[Draw],PosnZ[Draw],PosnR[Draw],PosnG[Draw],PosnB[Draw],PosnA[Draw],PosnT[Draw],PosnRad[Draw]);
			}

		free(PosnX); free(PosnY); free(PosnZ); free(PosnT);
		free(PosnR); free(PosnG); free(PosnB); free(PosnA);
		free(PosnRad);

		return abort;
}
