#include <assert.h>
#define plugInModeRGBColor	   3
#define plugInModeGrayScale    1


//**********************************************************************
//* Send Image To Output
#define PutCol(x,y,r,g,b) Img_R[y][x]=r; Img_G[y][x]=g; Img_B[y][x]=b; Img_A[y][x]=255;
void ImageFilter_LensF::PutSquare(int x,int y)
{
	if( x<1 || x>xres-2 || y<1 || y>yres-2 ) return;
	PutCol(x-1,y-1,255,0,0);PutCol(x,y-1,255,0,0);PutCol(x+1,y-1,255,0,0);
	PutCol(x-1,y,255,0,0);PutCol(x,y,255,0,0);PutCol(x+1,y,255,0,0);
	PutCol(x-1,y+1,255,0,0);PutCol(x,y+1,255,0,0);PutCol(x+1,y+1,255,0,0);
}

int RegistrationCheck;


void ImageFilter_LensF::PutImage(void)
{	
	int a=GetCode();
	if (encr(a)!=encr(encr(Code())))
						{ int cx=xres/2;
						  int cy=yres/2;

						  seed=time(NULL)<<16;
						  srand48(seed);

						  float theta=drand48()*1.57f;
						  float dx=(float)cos(theta);
						  float dy=(float)sin(theta);
						  int xpos,ypos;
						  int size;
						  if( xres>yres ) size=cx;
						  else			  size=cy;

						  for(int r=0;r<size;r++) {
							xpos=(int)(r*dx);ypos=(int)(r*dy);
							PutSquare(cx+xpos,cy+ypos);
							PutSquare(cx-xpos,cy-ypos);
							PutSquare(cx+ypos,cy-xpos);
							PutSquare(cx-ypos,cy+xpos);
						  }
					}

}


//*******************************************************************************************************
//* Very Crafty Pointwise Delauney Triangulator
//* Can't remember for the life of me how this works !
inline float lfsqr(float a) { return a*a; }

#define GetDistance(dx,dy)		if (lfsqr(FromX[y+dy][x+dx]+dx)+lfsqr(FromY[y+dy][x+dx]+dy)<							\
									lfsqr(FromX[y   ][x   ]   )+lfsqr(FromY[y   ][x   ]   ))							\
										{	FromX[y][x]=FromX[y+dy][x+dx]+dx;											\
											FromY[y][x]=FromY[y+dy][x+dx]+dy;											\
										}
				
BOOL ImageFilter_LensF::Delauney(float **FromX,float **FromY)
{			int	x, y;
			
			//**************************************
			//
			//      XX
			//   XX --
			//

			y=0;
			for(x=1;x!=xres;x++)
			{	GetDistance(-1,0);
			}

			for(y=1;y!=yres;y++)
				{	x=0;
					GetDistance(0,-1);

					for(x=1;x!=xres;x++)
						{ GetDistance(-1,0);
						  GetDistance(0,-1);
						}
				}

			//**************************************
			//
			//   -- XX
			//   XX 
			//

			y=yres-1;
			for(x=xres-2;x>=0;x--)
			{	GetDistance(1,0);
			}

			for(y=yres-2;y>=0;y--)
				{	x=xres-1;
					GetDistance(0,1);

					for(x=xres-2;x>=0;x--)
						{ GetDistance(1,0);
						  GetDistance(0,1);
						}
				}

			//**************************************
			//
			//   XX
			//   -- XX
			//

			y=0;
			for(x=xres-2;x!=0;x--)
			{	GetDistance(1,0);
			}

			for(y=1;y!=yres;y++)
				{	x=xres-1;
					GetDistance(0,-1);

					for(x=xres-2;x!=0;x--)
						{ GetDistance(0,-1);
						  GetDistance(1,0);
						}
				}

			//**************************************
			//
			//   XX --
			//      XX
			//

			y=yres-1;
			for(x=1;x!=xres;x++)
			{	GetDistance(-1,0);
			}

			for(y=yres-2;y>=0;y--)
				{	x=0;
					GetDistance(0,1);

					for(x=1;x!=xres;x++)
						{ GetDistance(-1,0);
						  GetDistance(0,1);
						}
				}

			return FALSE;
}			


//*******************************************************************************************************
//* Very Fast 2D Blurring Routines 
//* Time Complexity ~sqrt(n) !!

inline float fsqr (float a) { return a*a; }

// This Bluring function is based on blurring the Distance Transform and recovering angles again
// This is the best algorithm I've come up with so far, and it's the 10th try !
BOOL ImageFilter_LensF::QuickBlur(float Size,float **FromX,float **FromY)
{		if (Size==0.0f) return FALSE;
		int x,y;
		float Size2=fsqr((float)Size)*2.0f;

		float FallOff=1.0f/(float)Size;

		for(y=0;y!=yres;y++)
		for(x=0;x!=xres;x++)
				FromX[y][x]=(float)sqrt(fsqr(FromX[y][x])+fsqr(FromY[y][x]));

		// X Blur
		float *TmpX=(float *)malloc(sizeof(float)*xres);
		for(y=0;y!=yres;y++)
			{ if (rinfo.Abort()) { free(TmpX); return TRUE; }
			  StatusProgress(y,yres*2);
		      for(x=0;x!=xres;x++)
					{ float c=0.0f;
					  float tx=0.0f;

					  int tSize=(int)floor(Size);

					  if (x-tSize<0)		tSize=x;
					  if (x+tSize>=xres)	tSize=xres-x-1;

		              for (int _x=-tSize;_x<=tSize;_x++)
							{ float C=1.0f-fsqr((float)_x*FallOff); c+=C;
					          tx+=C*(float)FromX[y][x+_x];
							}

					  TmpX[x]=tx/c;
					}

		      for(x=0;x!=xres;x++) FromX[y][x]=TmpX[x];
			}
		free(TmpX);

		// Y Blur
		TmpX=(float *)malloc(sizeof(float)*yres);
		for(x=0;x!=xres;x++)
			{ if (rinfo.Abort()) { free(TmpX); return TRUE; }
		      StatusProgress(x+xres,xres*2);
		      for(y=0;y!=yres;y++)
					{ float c=0.0f;
					  float tx=0.0f;

					  int tSize=(int)floor(Size);

					  if (y-tSize<0)		tSize=y;
					  if (y+tSize>=yres)	tSize=yres-y-1;

		              for (int _y=-tSize;_y<=tSize;_y++)
							{ float C=1.0f-fsqr((float)_y*FallOff); c+=C;
					          tx+=C*(float)FromX[y+_y][x];
							}

					  TmpX[y]=tx/c;
					}

		      for(y=0;y!=yres;y++) FromX[y][x]=TmpX[y];
			}

		free(TmpX);

		for(y=0;y!=yres-1;y++)
		for(x=0;x!=xres-1;x++)
				{ float dX=FromX[y][x+1]-FromX[y][x];
		          float dY=FromX[y+1][x]-FromX[y][x];
				  float dst2=(float)sqrt(dX*dX+dY*dY);

				  if (dst2==0.0f) { FromX[y][x]=0.0f; FromY[y][x]=0.0f; }
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
{		if (Size==0.0f) return FALSE;
	    int x,y;

		float FallOff=1.0f/(float)Size;

		// X Blur
		float *TmpX=(float *)malloc(sizeof(float)*xres);
		for(y=0;y!=yres;y++)
			{ StatusProgress(y,yres*2);
		      if (rinfo.Abort()) { free(TmpX); return TRUE; }
			  for(x=0;x!=xres;x++)
					{ float c=0.0f;
					  float tx=0.0f;

					  int tSize=(int)floor(Size);

					  if (x-tSize<0)		tSize=x;
					  if (x+tSize>=xres)	tSize=xres-x-1;

		              for (int _x=-tSize;_x<=tSize;_x++)
							{ float C=1.0f-fsqr((float)_x*FallOff); c+=C;
					          tx+=C*(float)From[y][x+_x];
							}

					  TmpX[x]=tx/c;
					}

		      for(x=0;x!=xres;x++) From[y][x]=(uchar)(TmpX[x]+0.5f);
			}
		free(TmpX);

		// Y Blur
		TmpX=(float *)malloc(sizeof(float)*yres);
		for(x=0;x!=xres;x++)
			{ StatusProgress(x+xres,xres*2);
		      if (rinfo.Abort()) { free(TmpX); return TRUE; }
		      for(y=0;y!=yres;y++)
					{ float c=0.0f;
					  float tx=0.0f;

					  int tSize=(int)floor(Size);

					  if (y-tSize<0)		tSize=y;
					  if (y+tSize>=yres)	tSize=yres-y-1;

		              for (int _y=-tSize;_y<=tSize;_y++)
							{ float C=1.0f-fsqr((float)_y*FallOff); c+=C;
					          tx+=C*(float)From[y+_y][x];
							}

					  TmpX[y]=tx/c;
					}

		      for(y=0;y!=yres;y++) From[y][x]=(uchar)(TmpX[y]+0.5f);
			}
		free(TmpX);

		return FALSE;
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

		// hack 12/4/97 rcw
		rinfo.Depth=0.0f;

		// Get Centroid
		float xpos=0.0f;
		float ypos=0.0f;
		float npos=0.0f;

		for(x=0;x!=xres;x++)
		for(y=0;y!=yres;y++)
		if ((rinfo.FromX[y][x]==0.0f)&&(rinfo.FromY[y][x]==0.0f)) { xpos+=(float)x; ypos+=(float)y; npos++; }

		if (npos==0.0f) return FALSE;
		xpos/=npos; ypos/=npos;

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
					
					rinfo.StatusText("Pre Process #4.1"); if (abort==FALSE) abort=QuickBlur((float)data.ColouriseSmoothness*xres,rinfo.Col_R);
					rinfo.StatusText("Pre Process #4.2"); if (abort==FALSE) abort=QuickBlur((float)data.ColouriseSmoothness*xres,rinfo.Col_G);
					rinfo.StatusText("Pre Process #4.3"); if (abort==FALSE) abort=QuickBlur((float)data.ColouriseSmoothness*xres,rinfo.Col_B);
					rinfo.StatusText("Pre Process #4.4"); if (abort==FALSE) abort=QuickBlur((float)data.ColouriseSmoothness*xres,rinfo.Col_A);
				}

		rinfo.StatusText("Rendering Area"); 
		StatusProgress(0,4);
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

//*******************************************************************
// Non-Recursive(!) FloodFill v2.0
// This version returns  x,y coords of region centre

void ImageFilter_LensF::FloodFillXY(uchar **Flag,signed short x,signed short y,float& xpos,float& ypos)
{	uchar **Fill;
	Alloc2D(Fill,uchar,yres,xres);

	float npos=0.0f;
	int xx,yy;

	for(xx=0;xx<xres;xx++)
	for(yy=0;yy<yres;yy++)
			Fill[yy][xx]=0;

	Fill[y][x]=1;

	xpos=0.0f;
	ypos=0.0f;

	int Finished;
	do	{ Finished=0;

	      for(xx=0;xx<xres;xx++)
		  for(yy=0;yy<yres;yy++)
				{ if (Fill[yy][xx])
						{	Flag[yy][xx]=0;
							xpos+=xx;ypos+=yy;
							npos++;
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

	xpos/=npos;
	ypos/=npos;

	Free2D(Fill,yres);
}

BOOL ImageFilter_LensF::GoMulti(uchar **Flag)
{		
		BOOL abort=FALSE;

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
		else				{ int Finished;
			                  int x,y;
										Finished=1;
										rinfo.StatusText("Scanning Image"); 
										StatusProgress(Count%11,10); Count++;

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
// Fill region and render centroid

BOOL ImageFilter_LensF::FillAndRender(uchar** Flag,signed short x,signed short y)
{
	float xpos,ypos;
	FloodFillXY(Flag,x,y,xpos,ypos);
	rinfo.xpos=xpos;
	rinfo.ypos=ypos;
	rinfo.UseSingle=1;

	return data.LensFlare.RenderHQArea(rinfo);			// render
}

//****************************************************************************************************
// Render Object ID Elements
void ImageFilter_LensF::RenderImage(void)
{		
		uchar **Flag;
		int Count=0;
		
		xres=data.ImageWidth;
		yres=data.ImageHeight;
		SetupRenderInfo(1.0f);

		if( data.SourceMode==0 ) {				// render from single point
			StatusText("Genesis : Render");
			data.LensFlare.RenderHQArea(rinfo);
		}
		else {									// multi source points
		
			Alloc2D(Flag,uchar,yres,xres);		// need flag to hold source points
			MakeSourceMask(Flag,1.0f,xres);

			if( data.CSE==0 ) {					// still rendering from single point
				if( data.Single )	{			// only one source. Easy!
					// Get Centroid
					float xpos=0.0f;
					float ypos=0.0f;
					float npos=0.0f;
					int x,y;

					for(x=0;x!=xres;x++)
						for(y=0;y!=yres;y++)
							{ xpos+=(float)x*Flag[y][x]; ypos+=(float)y*Flag[y][x]; npos+=(float)Flag[y][x]; }

					if (npos==0.0f) return;
					xpos/=npos; ypos/=npos;
					rinfo.xpos=xpos;rinfo.ypos=ypos;
					rinfo.UseSingle=1;

					StatusText("Genesis : Render");
					data.LensFlare.RenderHQArea(rinfo);			// render
				} else {						// multiple regions
					int Finished;
					int abort=FALSE;
			        do	{	int x,y;
										Finished=1;
										rinfo.StatusText("Scanning Image"); 
										StatusProgress(Count%11,10); Count++;

										// Find First Unused Pixel
										int Found=0;
										for(x=0;((Found==0)&&(x!=xres));x++)
										for(y=0;((Found==0)&&(y!=yres));y++)
											if (Flag[y][x]) Found=1;

										if (Found)		{ Finished=0;
										                  abort=FillAndRender(Flag,(signed short)(x-1),(signed short)(y-1));
										                }
						}
					while((!Finished)&&(!abort));
				}
			} else {		
				GoMulti(Flag);			// Full lot
			}
			Free2D(Flag,yres);
		}

		free(rinfo.zbuffer);
		StatusHide();
}

void ImageFilter_LensF::RenderPreviewToArray(uchar** Red, uchar** Green, uchar** Blue, uchar** Alpha,int xsize,int ysize,int xwidth)
{
	uchar** Flag;
	int	Count=0;

	xres=xsize;yres=ysize;

	SetupRenderInfo(data.ImageScale);

	// setup mbuffer
	rinfo.mbuffer=(BYTE *)malloc(xres*yres);

	int x,y;
	for(y=0;y<yres;y++)
		for(x=0;x<xres;x++)
				rinfo.mbuffer[y*xres+x]=255-MaskVal((int)(x/data.ImageScale),(int)(y/data.ImageScale));

	rinfo.Img_R=Red;
	rinfo.Img_G=Green;
	rinfo.Img_B=Blue;
	rinfo.Img_A=Alpha;

	rinfo.xres=xsize;
	rinfo.yres=ysize;
	rinfo.xpos=0.5f*(1.0f+data.PosX)*(float)xwidth;
	rinfo.ypos=0.5f*(1.0f+data.PosY)*(float)yres;

	rinfo.AntiAliasing=1.0f;

		if( data.SourceMode==0 ) {				// render from single point
			StatusText("Genesis : Render");
			data.LensFlare.RenderHQArea(rinfo);
		}
		else {									// multi source points
		
			Alloc2D(Flag,uchar,ysize,xsize);		// need flag to hold source points
			MakeSourceMask(Flag,data.ImageScale,xwidth);

			if( data.CSE==0 ) {					// still rendering from single point
				if( data.Single )	{			// only one source. Easy!
					// Get Centroid
					float xpos=0.0f;
					float ypos=0.0f;
					float npos=0.0f;
					int x,y;

					for(x=0;x!=xres;x++)
						for(y=0;y!=yres;y++)
							{ xpos+=(float)x*Flag[y][x]; ypos+=(float)y*Flag[y][x]; npos+=(float)Flag[y][x]; }

					if (npos==0.0f) return;
					xpos/=npos; ypos/=npos;
					rinfo.xpos=xpos;rinfo.ypos=ypos;
					rinfo.UseSingle=1;

					StatusText("Genesis : Render");
					data.LensFlare.RenderHQArea(rinfo);			// render
				} else {						// multiple regions

					int Finished;
					int abort=FALSE;

			        do	{	int x,y;
										Finished=1;
										rinfo.StatusText("Scanning Image"); 
										StatusProgress(Count%11,10); Count++;

										// Find First Unused Pixel
										int Found=0;
										for(x=0;((Found==0)&&(x!=xres));x++)
										for(y=0;((Found==0)&&(y!=yres));y++)
											if (Flag[y][x]) Found=1;

										if (Found)		{ Finished=0;
										                  abort=FillAndRender(Flag,(signed short)(x-1),(signed short)(y-1));
										                }
						}
					while((!Finished)&&(!abort));
				}
			} else {		
				GoMulti(Flag);			// Full lot
			}
			Free2D(Flag,yres);
		}

	free(rinfo.zbuffer);
	StatusHide();

}
////////////////////////////////////////////////////////////
//
// Setup render variables from photoshop vars

void ImageFilter_LensF::SetupRenderInfo(float scale)
{
	rinfo.Img_R=Img_R;
	rinfo.Img_G=Img_G;
	rinfo.Img_B=Img_B;
	rinfo.Img_A=Img_A;

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

	rinfo.Time=(float)data.AnimationPosition;

	if( data.SourceMode==0 ) {
		rinfo.UseSingle=1;
		data.Single=1;
	}
	else rinfo.UseSingle=0;

	rinfo.ClipMode=data.ClipMode;

	rinfo.cr=rinfo.cg=rinfo.cb=rinfo.ca=1.0f;

	rinfo.CameraDepth=data.CameraDepth;
	rinfo.UseBuffer=FALSE;
	rinfo.obuffer=NULL;
	rinfo.CompZ=0;
	rinfo.StartLine=0;
	rinfo.SkipLine=1;

	rinfo.SizeMultiply=1000.0f;
	rinfo.SizeMultiplyParticle=1.0f;

	if (data.AntiAliasing==0) rinfo.AntiAliasing=1.0f;
	else if (data.AntiAliasing==1) rinfo.AntiAliasing=0.5f;
	else if (data.AntiAliasing==2) rinfo.AntiAliasing=0.334f;

	rinfo.xpos=0.5f*(1.0f+data.PosX)*(float)xres;
	rinfo.ypos=0.5f*(1.0f+data.PosY)*(float)yres;

	// set up z buffer
	// needed for occlusion over clip mask and self occlusion
	// of flare elements
	// also mask for anti-aliasing edges

	if( data.ClipMode<2 )
		rinfo.UseZ=TRUE;

	rinfo.zbuffer=(float *)malloc(xres*yres*sizeof(float));
	rinfo.mbuffer=mbuffer;
	zbuffer=rinfo.zbuffer;

	int x,y;
	for(y=0;y<yres;y++)
		for(x=0;x<xres;x++) {
			switch(data.ClipMode) {
			case 0:
				rinfo.zbuffer[y*xres+x]=-(float)(MaskVal((int)(x/scale),(int)(y/scale))==255);
				break;
			case 1:
				rinfo.zbuffer[y*xres+x]=-(float)(MaskVal((int)(x/scale),(int)(y/scale))==0);
				break;
			case 2:
				rinfo.zbuffer[y*xres+x]=-1.0f;
				break;
			}
	}

}


////////////////////////////////////////////////////////////
//
// Create sources from source mode
unsigned char ImageFilter_LensF::MaskVal(int x,int y)
{
	if(x>=data.ImageWidth || y>=data.ImageHeight) return 255;
	return 255-mbuffer[y*data.maskdata->Width+x];
}

void ImageFilter_LensF::MakeSourceMask(uchar** mask,float scale,int xwidth)
{
	float	basex,basey,basez;
	float	colorx,colory,colorz;
	float	colorlen;

	if( data.SourceMode==3 ) {
		basex=(float)data.Col1.r;
		colorx=(float)data.Col2.r-basex;
		basey=(float)data.Col1.g;
		colory=(float)data.Col2.g-basex;
		basez=(float)data.Col1.b;
		colorz=(float)data.Col2.b-basex;

		colorlen=(float)sqrt(colorx*colorx+colory*colory+colorz*colorz);
		colorx/=colorlen;
		colory/=colorlen;
		colorz/=colorlen;
	}
	
	int x,y;
	for(y=0;y<yres;y++)
		for(x=0;x<xres;x++) {
				switch(data.SourceMode) {
				case 0:							// from point
					if( (int)(0.5f*(data.PosX+1.0f)*(xwidth-1))==x && (int)(0.5f*(data.PosY+1.0f)*(yres-1))==y )
							mask[y][x]=255;
					else	mask[y][x]=0;
					break;
				case 1:							// All in selected
					mask[y][x]=255-MaskVal((int)(x/scale),(int)(y/scale));
					break;
				case 2:							// external of selected
					mask[y][x]=MaskVal((int)(x/scale),(int)(y/scale));
					break;
				case 3:							// by colour
				{
					float x0,y0,z0;
					float x1,y1,z1;
					int xpos=(int)(x/scale);
					int ypos=(int)(y/scale);
					
					if( data.maskdata->ImageMode==plugInModeGrayScale ) {
						x1=(float)Img_R[ypos][xpos]-basex;
						y1=(float)Img_R[ypos][xpos]-basey;
						z1=(float)Img_R[ypos][xpos]-basez;
					} else {
						x1=(float)Img_R[ypos][xpos]-basex;
						y1=(float)Img_G[ypos][xpos]-basey;
						z1=(float)Img_B[ypos][xpos]-basez;
					}

					x0=x1*colorx+y1*colory+z1*colorz;
					if( x0<0 || x0>colorlen ) {
						mask[y][x]=0;
						break;
					}

					x0=colory*z1-colorz*y1;
					y0=colorz*x1-colorx*z1;
					z0=colorx*y1-colory*x1;

					if( sqrt(x0*x0+y0*y0+z0*z0)<data.Tolerance ) mask[y][x]=255;
					else mask[y][x]=0;
					break;
				}
				default: mask[y][x]=0;
				}
		}
}