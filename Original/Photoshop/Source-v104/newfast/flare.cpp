#include <Max.h>
#include <bmmlib.h>
#include <fltlib.h>

#include "FColour.h"
#include "Flare.h"

//----------------------------------------------------------------------------------------------------------
// if i==0 We Get Shape Only
// Else We Get All
// Used When Editing Shapes of Flares, but not copying Entire KeyFrames
void Flare::GetFlare(Flare *A,int i)
	{ for(int j=0;j!=FLARERESOLUTION;j++)
			{ fColour tmp;
              float ftmp;

		      A->GetRadialColour(tmp,j);  
			  PutRadialColour(tmp,j);

			  A->GetAngularColour(tmp,j); 
			  PutAngularColour(tmp,j);

			  A->GetShape(ftmp,j);        
			  PutShape(ftmp,j);

			  A->GetShape2(ftmp,j);       
			  PutShape2(ftmp,j);
			}

	  if (i) {		PositionX=A->PositionX;		
					PositionY=A->PositionY;
					Position=A->Position ;		
					Angle=A->Angle;
					FlareSize=A->FlareSize;		
					AngleOffSet=A->AngleOffSet;
					Colourise=A->Colourise;		
					Light=A->Light;			
					ReAlign=A->ReAlign;
					Intensity=A->Intensity;
					Linked=A->Linked;
					Active=A->Active;;
					IR2=A->IR2;
					Occlusion=A->Occlusion;
					Depth=A->Depth;;
					Polar=A->Polar;
					OffScreenRender=A->OffScreenRender;

					BuildColour=A->BuildColour;
					BuildShape=A->BuildShape;
					BuildWidth=A->BuildWidth;
					BuildTransparency=A->BuildTransparency;
					BuildNoStreaks=A->BuildNoStreaks;
					BuildNoSides=A->BuildNoSides;
					BuildFactor=A->BuildFactor;
					BuildAspect=A->BuildAspect;
					BuildCol=A->BuildCol;
					BuildSeed=A->BuildSeed;

					Inferno.GetFractal(A->Inferno);
			}
	}

//----------------------------------------------------------------------------------------------------------
// if i==0 We Get Shape Only
// Else We Get All
void Flare::GetFlareAnim(Flare *A)
	{ for(int j=0;j!=FLARERESOLUTION;j++)
			{ fColour tmp;
              float ftmp;

		      A->GetRadialColour(tmp,j);  
			  PutRadialColour(tmp,j);

			  A->GetAngularColour(tmp,j); 
			  PutAngularColour(tmp,j);

			  A->GetShape(ftmp,j);        
			  PutShape(ftmp,j);

			  A->GetShape2(ftmp,j);       
			  PutShape2(ftmp,j);
			}

	  NoPosKeys	 =A->NoPosKeys;
	  PosKeys=(FlarePosAnim *)realloc(PosKeys,sizeof(FlarePosAnim)*NoPosKeys);

	  for(j=0;j!=A->NoPosKeys;j++) CopyPosKey(PosKeys[j],A->PosKeys[j]);

	  NoShpKeys	 =A->NoShpKeys;
	  ShpKeys=(FlareShpAnim *)realloc(ShpKeys,sizeof(FlareShpAnim)*NoShpKeys);

	  for(j=0;j!=A->NoShpKeys;j++) CopyShpKey(ShpKeys[j],A->ShpKeys[j]);

	  PositionX  =A->PositionX;		
	  PositionY  =A->PositionY;
      Position   =A->Position ;		
      Angle      =A->Angle;
      FlareSize  =A->FlareSize;		
      AngleOffSet=A->AngleOffSet;
      Colourise  =A->Colourise;		
      Light		 =A->Light;			
      ReAlign	 =A->ReAlign;
      Intensity	 =A->Intensity;
      Linked	 =A->Linked;
      Active	 =A->Active;;
      IR2		 =A->IR2;
      Occlusion	 =A->Occlusion;
      Depth		 =A->Depth;
	  Polar		 =A->Polar;
      OffScreenRender=A->OffScreenRender;

	  BuildColour=A->BuildColour;
	  BuildShape=A->BuildShape;
	  BuildWidth=A->BuildWidth;
	  BuildTransparency=A->BuildTransparency;
	  BuildNoStreaks=A->BuildNoStreaks;
	  BuildNoSides=A->BuildNoSides;
	  BuildFactor=A->BuildFactor;
	  BuildAspect=A->BuildAspect;
	  BuildCol=A->BuildCol;
	  BuildSeed=A->BuildSeed;

	  Inferno.GetFractal(A->Inferno);
	}

#define LimitRange(a) a=(a>1.0f)?1.0f:a; a=(a<0.0f)?0.0f:a;
void Flare::CheckColours()
	{ for(int i=0;i!=FLARERESOLUTION;i++)
		{ LimitRange(Col[i].r);
		  LimitRange(Col[i].g);
		  LimitRange(Col[i].b);
		  LimitRange(Col[i].a);
          
		  LimitRange(Streaks[i].r);
		  LimitRange(Streaks[i].g);
		  LimitRange(Streaks[i].b);
		  LimitRange(Streaks[i].a);

		  LimitRange(Shape[i]);
		  LimitRange(Shape2[i]);
		}
	}

//***********************************************************************************************************		  		
// Pre Processing
BOOL Flare::PreProcess(int xres, int yres, float xpos, float ypos, float CameraDepth,float ir,float ig,float ib,float ia,float Time,float RDepth,float Size)
	  		{ // SOurce Off the Screen
					if ((OffScreenRender&1)==0)
						if ((xpos<0)||(xpos>=xres)||(ypos<0)||(ypos>=yres)) return TRUE;

			  // Depth Values
					RenderDepth=RDepth*(1.0f+Depth);

			  // Perspective Size & Intensity
					RenderSize=Size*FlareSize*CameraDepth/(CameraDepth+Depth);

					float _RenderIntensity;

					if (Linked) _RenderIntensity=IR2/(RenderSize*RenderSize);
					else _RenderIntensity=Intensity;

					RenderIntensity.r=((1.0f-Colourise)+Colourise*ir)*_RenderIntensity;
					RenderIntensity.g=((1.0f-Colourise)+Colourise*ig)*_RenderIntensity;
					RenderIntensity.b=((1.0f-Colourise)+Colourise*ib)*_RenderIntensity;
					RenderIntensity.a=((1.0f-Colourise)+Colourise*ia)*_RenderIntensity;

			  // Constants For Rendering
					RadiusM =(float)FLARERESOLUTION/(RenderSize*(float)xres);
	  				AngleM  =(float)FLARERESOLUTION*0.159154943092f;		// Divide by 2Pi

			  float ScreenCX,ScreenCY;

			  if (OffScreenRender&2)
				   {	ScreenCX=xpos; xpos-=xres;
				        ScreenCY=ypos;
			       }
			  else {	// Screen Centre
						ScreenCX=(0.5f+PositionX*0.5f)*xres;
						ScreenCY=(0.5f+PositionY*0.5f)*yres;
				   }

			  // Axis
					float dx1=xpos-ScreenCX,dy1=ypos-ScreenCY;
					float dx2=-dy1,dy2=dx1;

			  // Convert From Polar If Necessary
					ConvertCoordinates();

			  // Get Centre And Render Position
					RenderCX=TrueX*dx1+TrueY*dx2;
					RenderCY=TrueX*dy1+TrueY*dy2;

			  // Project Onto Screen
					RenderCX*=CameraDepth/(CameraDepth+Depth); RenderCX+=ScreenCX;
					RenderCY*=CameraDepth/(CameraDepth+Depth); RenderCY+=ScreenCY;

			  // Angle Offsets
					if (OffScreenRender&2)
						{ ScreenCX=(0.5f+PositionX*0.5f)*xres;
						  ScreenCY=(0.5f+PositionY*0.5f)*yres;
						}

					AngleAdd=(int)((float)(-0.5f*AngleOffSet*FLARERESOLUTION));
					if (ReAlign) AngleAdd-=(float)FLARERESOLUTION*(float)atan2(RenderCY-ScreenCY,RenderCX-ScreenCX)*0.159154943092f;
					while(AngleAdd<0) AngleAdd+=FLARERESOLUTION;

			  // Compute Bounding Box
					MaxX=RenderCX+(float)(RenderSize*xres)+1;
					MinX=RenderCX-(float)(RenderSize*xres)-1;
					MaxY=RenderCY+(float)(RenderSize*xres)+1;
					MinY=RenderCY-(float)(RenderSize*xres)-1;

					if (Inferno.IsUsed())
							{ if (Inferno.UseSimpleNoise)
									{ MaxX+=(float)(2.0f*RenderSize*xres*Inferno.SimpleXSize);
					                  MinX-=(float)(2.0f*RenderSize*xres*Inferno.SimpleXSize);
					                  MaxY+=(float)(2.0f*RenderSize*xres*Inferno.SimpleYSize);
									  MinY-=(float)(2.0f*RenderSize*xres*Inferno.SimpleYSize);
									}

					          if (Inferno.UseFracNoise)
									{ MaxX+=(float)(2.0f*RenderSize*xres*Inferno.FracXSize);
							          MinX-=(float)(2.0f*RenderSize*xres*Inferno.FracXSize);
					                  MaxY+=(float)(2.0f*RenderSize*xres*Inferno.FracYSize);
									  MinY-=(float)(2.0f*RenderSize*xres*Inferno.FracYSize);
									}
							}

			  // FractalNoise Stuff
					Inferno.Setup(RenderSize*xres,RenderCX,RenderCY,Time);

					return FALSE;
	  		}

//***********************************************************************************************************		  		

void Flare::SimplePreProcess(int xres, int yres, float Size)
	  		{ RenderSize=0.45*Size;
              RenderIntensity.r=Intensity;
			  RenderIntensity.g=Intensity;
			  RenderIntensity.b=Intensity;
			  RenderIntensity.a=Intensity;

			  RenderDepth=0.0f;

              RadiusM =(float)FLARERESOLUTION/(RenderSize*(float)xres);
	  		  AngleM  =(float)FLARERESOLUTION/(LF_PI*2.0f);

			  AngleAdd=(int)((float)(-0.5f*AngleOffSet*FLARERESOLUTION));
			  while(AngleAdd<0) AngleAdd+=FLARERESOLUTION;

			  float ScreenCX=0.5f*xres;
			  float ScreenCY=0.5f*yres;

			  float dx1=xres/2-ScreenCX,dy1=yres/2-ScreenCY;
			  float dx2=-dy1,dy2=dx1;

			  if (Polar)
					{ float SA=Position*(float)cos(Angle*3.14159f);
					  float CA=Position*(float)sin(Angle*3.14159f);

					  RenderCX=SA*dx1+CA*dx2+ScreenCX;
					  RenderCY=SA*dy1+CA*dy2+ScreenCY;
					}
			  else	{ RenderCX=Position*dx1+Angle*dx2+ScreenCX;
			          RenderCY=Position*dy1+Angle*dy2+ScreenCY;
					}

			  MaxX=RenderCX+(float)(RenderSize*xres)+1;
			  MinX=RenderCX-(float)(RenderSize*xres)-1;
			  MaxY=RenderCY+(float)(RenderSize*xres)+1;
			  MinY=RenderCY-(float)(RenderSize*xres)-1;

			  Inferno.NoNoise();
	  		}

void Flare::SimplePreProcess(int xres, int yres, float Size,float Time)
			{ // Pre Process The Standard Stuff
	                SimplePreProcess(xres,yres,Size);

	          // Compute Bounding Box
					MaxX=RenderCX+(float)(RenderSize*xres)+1;
					MinX=RenderCX-(float)(RenderSize*xres)-1;
					MaxY=RenderCY+(float)(RenderSize*xres)+1;
					MinY=RenderCY-(float)(RenderSize*xres)-1;
					
					if (Inferno.IsUsed())
							{ if (Inferno.UseSimpleNoise)
									{ MaxX+=(float)(2.0f*RenderSize*xres*Inferno.SimpleXSize);
					                  MinX-=(float)(2.0f*RenderSize*xres*Inferno.SimpleXSize);
					                  MaxY+=(float)(2.0f*RenderSize*xres*Inferno.SimpleYSize);
									  MinY-=(float)(2.0f*RenderSize*xres*Inferno.SimpleYSize);
									}

					          if (Inferno.UseFracNoise)
									{ MaxX+=(float)(2.0f*RenderSize*xres*Inferno.FracXSize);
							          MinX-=(float)(2.0f*RenderSize*xres*Inferno.FracXSize);
					                  MaxY+=(float)(2.0f*RenderSize*xres*Inferno.FracYSize);
									  MinY-=(float)(2.0f*RenderSize*xres*Inferno.FracYSize);
									}
							}

			  // FractalNoise Stuff
					Inferno.Setup(RenderSize*xres,RenderCX,RenderCY,Time);
			}

#define RenderPrecision 0.00001f	  		

void Flare::Blur(float &Screen,uchar **Back,float _x,float _y,int xres,int yres,float Width)
	{	float	Sigma=(float)-2.302/(Width*Width);					// Sigma Squared is more useful
		int		Range=(int)(sqrt(-5.54517744448f/Sigma)+0.5f);		// Range Of Drawing
		int		x,y,xx,yy;

		if (Range<1) { Screen=i2f(Back[(int)(_y+0.5f)][(int)(_x+0.5f)]); 
		               return; 
		             }

		float *Tmp=(float *)malloc((Range+1)*(Range+1)*sizeof(float));

		Tmp[0]=(float)1.0;

		for(x=1;x<=Range;x++)
			{ Tmp[x*Range]=Tmp[x]=(float)exp(Sigma*x*x);
			  Tmp[x*Range+x]=(float)exp(2*Sigma*x*x);
			}

		for(y=1;y<Range;y++)
		for(x=y+1;x<=Range;x++)
			Tmp[x*Range+y]=Tmp[y*Range+x]=(float)exp(Sigma*(x*x+y*y));

		int lxmax=_x+Range; if (lxmax>=xres) lxmax=xres-1;
		int lymax=_y+Range; if (lymax>=yres) lymax=yres-1;
		int lxmin=_x-Range; if (lxmin<0)     lxmin=0;
		int lymin=_y-Range; if (lymin<0)     lymin=0;

		float Normalise=0.0f;

		// Get Normalisation Constant
		Screen=0.0f;

		for(yy=_y,y=0;yy<=lymax;yy++,y++)
		for(xx=_x,x=0;xx<=lxmax;xx++,x++)
				{ Screen+=i2f(Back[yy][xx])*Tmp[y*Range+x];
				  Normalise+=Tmp[y*Range+x];
				}
		
		if (x>0)
			for(yy=_y  ,y=0;yy<=lymax;yy++,y++)
			for(xx=_x-1,x=1;xx>=lxmin;xx--,x++)
					{ Screen+=i2f(Back[yy][xx])*Tmp[y*Range+x];
					  Normalise+=Tmp[y*Range+x];
					}

		if (y>0)
			for(yy=_y-1,y=1;yy>=lymin;yy-=1,y+=1)
			for(xx=_x  ,x=0;xx<=lxmax;xx++,x++)
					{ Screen+=i2f(Back[yy][xx])*Tmp[y*Range+x];
					  Normalise+=Tmp[y*Range+x];
					}

		if ((x>0)&&(y>0))
			for(yy=_y-1,y=1;yy>=lymin;yy--,y++)
			for(xx=_x-1,x=1;xx>=lxmin;xx--,x++)
					{ Screen+=i2f(Back[yy][xx])*Tmp[y*Range+x];
					  Normalise+=Tmp[y*Range+x];
					}

		if (Normalise!=0.0f) Screen/=Normalise;
		else	Screen=0.0f;

		free(Tmp);
	}

void Flare::Blur(float &ScreenR,float &ScreenG,float &ScreenB,float &ScreenA,
				 uchar **BackR,uchar **BackG,uchar **BackB,uchar **BackA,
				 float _x,float _y,int xres,int yres,float Width)
	{	float	Sigma=(float)-2.302/(Width*Width);					// Sigma Squared is more useful
		int		Range=(int)(sqrt(-5.54517744448f/Sigma)+0.5f);		// Range Of Drawing
		int		x,y,xx,yy;

		if (Range<1) { ScreenR=i2f(BackR[(int)(_y+0.5f)][(int)(_x+0.5f)]); 
		               ScreenG=i2f(BackG[(int)(_y+0.5f)][(int)(_x+0.5f)]); 
					   ScreenB=i2f(BackB[(int)(_y+0.5f)][(int)(_x+0.5f)]); 
					   ScreenA=i2f(BackA[(int)(_y+0.5f)][(int)(_x+0.5f)]); 
		               return; 
		             }

		float *Tmp=(float *)malloc((Range+1)*(Range+1)*sizeof(float));

		Tmp[0]=(float)1.0;

		for(x=1;x<=Range;x++)
			{ Tmp[x*Range]=Tmp[x]=(float)exp(Sigma*x*x);
			  Tmp[x*Range+x]=(float)exp(2*Sigma*x*x);
			}

		for(y=1;y<Range;y++)
		for(x=y+1;x<=Range;x++)
			Tmp[x*Range+y]=Tmp[y*Range+x]=(float)exp(Sigma*(x*x+y*y));

		int lxmax=_x+Range; if (lxmax>=xres) lxmax=xres-1;
		int lymax=_y+Range; if (lymax>=yres) lymax=yres-1;
		int lxmin=_x-Range; if (lxmin<0)     lxmin=0;
		int lymin=_y-Range; if (lymin<0)     lymin=0;

		float Normalise=0.0f;

		// Get Normalisation Constant
		ScreenR=0.0f;
		ScreenG=0.0f;
		ScreenB=0.0f;
		ScreenA=0.0f;

		for(yy=_y,y=0;yy<=lymax;yy++,y++)
		for(xx=_x,x=0;xx<=lxmax;xx++,x++)
				{ ScreenR+=i2f(BackR[yy][xx])*Tmp[y*Range+x];
		          ScreenG+=i2f(BackG[yy][xx])*Tmp[y*Range+x];
				  ScreenB+=i2f(BackB[yy][xx])*Tmp[y*Range+x];
				  ScreenA+=i2f(BackA[yy][xx])*Tmp[y*Range+x];
				  Normalise+=Tmp[y*Range+x];
				}
		
		if (x>0)
			for(yy=_y  ,y=0;yy<=lymax;yy++,y++)
			for(xx=_x-1,x=1;xx>=lxmin;xx--,x++)
					{ ScreenR+=i2f(BackR[yy][xx])*Tmp[y*Range+x];
		              ScreenG+=i2f(BackG[yy][xx])*Tmp[y*Range+x];
					  ScreenB+=i2f(BackB[yy][xx])*Tmp[y*Range+x];
					  ScreenA+=i2f(BackA[yy][xx])*Tmp[y*Range+x];
					  Normalise+=Tmp[y*Range+x];
					}

		if (y>0)
			for(yy=_y-1,y=1;yy>=lymin;yy-=1,y+=1)
			for(xx=_x  ,x=0;xx<=lxmax;xx++,x++)
					{ ScreenR+=i2f(BackR[yy][xx])*Tmp[y*Range+x];
		              ScreenG+=i2f(BackG[yy][xx])*Tmp[y*Range+x];
					  ScreenB+=i2f(BackB[yy][xx])*Tmp[y*Range+x];
					  ScreenA+=i2f(BackA[yy][xx])*Tmp[y*Range+x];
					  Normalise+=Tmp[y*Range+x];
					}

		if ((x>0)&&(y>0))
			for(yy=_y-1,y=1;yy>=lymin;yy--,y++)
			for(xx=_x-1,x=1;xx>=lxmin;xx--,x++)
					{ ScreenR+=i2f(BackR[yy][xx])*Tmp[y*Range+x];
		              ScreenG+=i2f(BackG[yy][xx])*Tmp[y*Range+x];
					  ScreenB+=i2f(BackB[yy][xx])*Tmp[y*Range+x];
					  ScreenA+=i2f(BackA[yy][xx])*Tmp[y*Range+x];
					  Normalise+=Tmp[y*Range+x];
					}

		if (Normalise!=0.0f) 
				{ ScreenR/=Normalise;
		          ScreenG/=Normalise;
				  ScreenB/=Normalise;
				  ScreenA/=Normalise;
				}
		else	{ ScreenR=0.0f;
		          ScreenG=0.0f;
				  ScreenB=0.0f;
				  ScreenA=0.0f;
				}

		free(Tmp);
	}

uchar Flare::GetColour(float &r,float &g,float &b, float &a,					// Current Pixel
					    uchar **rb,uchar **gb,uchar **bb,uchar **ab,			// Current Background
					    float _x,float _y,int xres,int yres)					// Current X,Y Coords : float for AntiAliasing
	{ float x,y;
      Inferno.Distort(x,y,_x,_y);

	  float tmpcos=x-RenderCX;
      float tmpsin=y-RenderCY;			  
				  
	  double Ang		=atan2(tmpsin,tmpcos)+6.2831853072f;
      double Dst		=sqrt(tmpcos*tmpcos+tmpsin*tmpsin);

	  float n=fmod((AngleM*Ang)+AngleAdd,FLARERESOLUTION); 
	  if (n>=(FLARERESOLUTION-1.0E-3f)) n=0.0f;
	  float ShapeN=lerpwrap(Shape,n);
      if (ShapeN<1.0E-6f) return 0;
	
	  float Dist=RadiusM*Dst/ShapeN;

	  if (Dist<FLARERESOLUTION)
	  {	  // N : Radial Andle
		  // Dist : Distance From Centre
		  n+=lerp(Shape2,Dist)*FLARERESOLUTION;
		  n=fmod(n,FLARERESOLUTION);
		  
		  // Get Colour Of Position In Element
		  if (Light==1)			
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  Tmp.FilterOver(r,g,b,a);

				  return 1;
				}
		  else if (Light==0)
				{ fColour Tmp;
			      fColour _Col;		lerp3(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap3(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  
				  r=1.0f-(1.0f-r)*(1.0f-Tmp.r);
				  g=1.0f-(1.0f-g)*(1.0f-Tmp.g);
				  b=1.0f-(1.0f-b)*(1.0f-Tmp.b);
				  a+=0.3333f*(Tmp.r+Tmp.g+Tmp.b); if (a>1.0f) a=1.0f;

				  return 1;
				}
		  else if (Light==6)
				{ fColour Tmp;					

		          Tmp.r=(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(Col[(int)Dist].r+Col[(int)Dist].g+Col[(int)Dist].b))*
						(0.333333f*(Streaks[(int)n].r+Streaks[(int)n].g+Streaks[(int)n].b)); 

				  if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.r*=xres*FlareSize*0.1f;
		   
				  Blur(r,g,b,a,rb,gb,bb,ab,_x,_y,xres,yres,Tmp.r);

				  return 1;
				}
		  else if (Light==7)
				{ fColour Tmp;					
				  Tmp.r=RenderIntensity.r*Col[(int)Dist].r*Streaks[(int)n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.r*=xres*FlareSize*0.1f;
				  Blur(r,rb,_x,_y,xres,yres,Tmp.r);

				  Tmp.g=RenderIntensity.g*Col[(int)Dist].g*Streaks[(int)n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.g*=xres*FlareSize*0.1f;
				  Blur(g,gb,_x,_y,xres,yres,Tmp.g);

				  Tmp.b=RenderIntensity.b*Col[(int)Dist].b*Streaks[(int)n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.b*=xres*FlareSize*0.1f;
				  Blur(b,bb,_x,_y,xres,yres,Tmp.b);

				  Tmp.a=RenderIntensity.a*Col[(int)Dist].a*Streaks[(int)n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  Tmp.a*=xres*FlareSize*0.1f;
				  Blur(a,ab,_x,_y,xres,yres,Tmp.a);

				  return 1;
				}
		  else if (Light==8)
				{ fColour Tmp;
			      fColour _Col;		lerp3(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap3(Streaks,n,_Streaks);

				  if (Inferno.RenderNoise)
						{ tmpcos=_x-RenderCX; tmpsin=_y-RenderCY;			  
						  Ang=atan2(tmpsin,tmpcos)+6.2831853072f;
						  Dst=sqrt(tmpcos*tmpcos+tmpsin*tmpsin);
						}
						
				  Tmp.r=3.14159265359f*(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(_Col.r+_Col.g+_Col.b))*(0.333333f*(_Streaks.r+_Streaks.g+_Streaks.b)); 
				  
				  if (Tmp.r>3.14159265359f) Tmp.r=3.14159265359f;
	
				  float PosY=RenderCY+Dst*sin(Ang+Tmp.r); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=RenderCX+Dst*cos(Ang+Tmp.r); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  g=lerp2d(gb,PosX,PosY,xres,yres);
				  b=lerp2d(bb,PosX,PosY,xres,yres);
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==9)
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

		          if (Inferno.RenderNoise)
						{ tmpcos=_x-RenderCX; tmpsin=_y-RenderCY;			  
						  Ang=atan2(tmpsin,tmpcos)+6.2831853072f;
						  Dst=sqrt(tmpcos*tmpcos+tmpsin*tmpsin);
						}
						
				  Tmp.r=3.14159265359f*RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>3.14159265359f) Tmp.r=3.14159265359f;
				  Tmp.g=3.14159265359f*RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>3.14159265359f) Tmp.g=3.14159265359f;
				  Tmp.b=3.14159265359f*RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>3.14159265359f) Tmp.b=3.14159265359f;
				  Tmp.a=3.14159265359f*RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>3.14159265359f) Tmp.a=3.14159265359f;
	
				  float PosY=RenderCY+Dst*sin(Ang+Tmp.r); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=RenderCX+Dst*cos(Ang+Tmp.r); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);

				  PosY=RenderCY+Dst*sin(Ang+Tmp.g); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=RenderCX+Dst*cos(Ang+Tmp.g); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  g=lerp2d(gb,PosX,PosY,xres,yres);

				  PosY=RenderCY+Dst*sin(Ang+Tmp.b); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=RenderCX+Dst*cos(Ang+Tmp.b); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  b=lerp2d(bb,PosX,PosY,xres,yres);

				  PosY=RenderCY+Dst*sin(Ang+Tmp.a); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=RenderCX+Dst*cos(Ang+Tmp.a); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==10)
				{ fColour Tmp;
			      fColour _Col;		lerp3(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap3(Streaks,n,_Streaks);

				  tmpcos=_x-RenderCX; 
				  tmpsin=_y-RenderCY;			  

				  Tmp.r=1.0f-(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(_Col.r+_Col.g+_Col.b))*(0.333333f*(_Streaks.r+_Streaks.g+_Streaks.b)); 
				  if (Tmp.r<0.0f) Tmp.r=0.0f;

				  float PosY=(RenderCY+Tmp.r*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(RenderCX+Tmp.r*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;

				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  g=lerp2d(gb,PosX,PosY,xres,yres);
				  b=lerp2d(bb,PosX,PosY,xres,yres);
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==11)
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

		          tmpcos=_x-RenderCX; 
				  tmpsin=_y-RenderCY;			  

				  Tmp.r=1.0f-RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r<0.0f) Tmp.r=0.0f;
				  Tmp.g=1.0f-RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g<0.0f) Tmp.g=0.0f;
				  Tmp.b=1.0f-RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b<0.0f) Tmp.b=0.0f;
				  Tmp.a=1.0f-RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a<0.0f) Tmp.a=0.0f;

				  float PosY=(RenderCY+Tmp.r*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(RenderCX+Tmp.r*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  
				  PosY=(RenderCY+Tmp.g*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(RenderCX+Tmp.g*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  g=lerp2d(gb,PosX,PosY,xres,yres);

				  PosY=(RenderCY+Tmp.b*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(RenderCX+Tmp.b*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  b=lerp2d(bb,PosX,PosY,xres,yres);

				  PosY=(RenderCY+Tmp.a*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(RenderCX+Tmp.a*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==5)
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r=(1.0f-r)*Tmp.r+r*(1.0f-Tmp.r);
				  g=(1.0f-g)*Tmp.g+g*(1.0f-Tmp.g);
				  b=(1.0f-b)*Tmp.b+b*(1.0f-Tmp.b);
				  a=(1.0f-a)*Tmp.a+a*(1.0f-Tmp.a);

				  return 1;
				}
		  else if (Light==2)
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r=r*Tmp.r;
				  g=g*Tmp.g;
				  b=b*Tmp.b;
				  a=a*Tmp.a;

				  return 1;
				}
		  else if (Light==3)
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r+=Tmp.r; if (r>1.0f) r=1.0f;
				  g+=Tmp.g; if (g>1.0f) g=1.0f;
				  b+=Tmp.b; if (b>1.0f) b=1.0f;
				  a+=Tmp.a; if (a>1.0f) a=1.0f;

				  return 1;
				}
		  else if (Light==4)
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r-=Tmp.r; if (r<0.0f) r=0.0f;
				  g-=Tmp.g; if (g<0.0f) g=0.0f;
				  b-=Tmp.b; if (b<0.0f) b=0.0f;
				  a-=Tmp.a; if (a<0.0f) a=0.0f;

				  return 1;
				}
		  else if (Light==12)
				{ fColour Tmp;
			      fColour _Col;		lerp3(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap3(Streaks,n,_Streaks);

				  Tmp.r=(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(_Col.r+_Col.g+_Col.b))*(0.333333f*(_Streaks.r+_Streaks.g+_Streaks.b)); 
				  if (Tmp.r<0.0f) Tmp.r=0.0f;
				  float iTmp=1.0f-Tmp.r;

				  float PosY=(float)(_y*iTmp+Tmp.r*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(float)(_x*iTmp+Tmp.r*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;

				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  g=lerp2d(gb,PosX,PosY,xres,yres);
				  b=lerp2d(bb,PosX,PosY,xres,yres);
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==13)
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>1.0f) Tmp.a=1.0f;

				  float PosY=(float)(_y*(1.0f-Tmp.r)+Tmp.r*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(float)(_x*(1.0f-Tmp.r)+Tmp.r*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);

				  PosY=(float)(_y*(1.0f-Tmp.g)+Tmp.g*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(float)(_x*(1.0f-Tmp.g)+Tmp.g*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  g=lerp2d(gb,PosX,PosY,xres,yres);

				  PosY=(float)(_y*(1.0f-Tmp.b)+Tmp.b*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(float)(_x*(1.0f-Tmp.b)+Tmp.b*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  b=lerp2d(bb,PosX,PosY,xres,yres);

				  PosY=(float)(_y*(1.0f-Tmp.a)+Tmp.a*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(float)(_x*(1.0f-Tmp.a)+Tmp.a*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==14)			// Transition I
				{ fColour Tmp;
			      fColour _Col;		lerp3(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap3(Streaks,n,_Streaks);

				  Tmp.r=(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(_Col.r+_Col.g+_Col.b))*(0.333333f*(_Streaks.r+_Streaks.g+_Streaks.b)); 
				  if (Tmp.r>1.0f) Tmp.r=1.0f;
				  float iTmp=1.0f-Tmp.r;

			      r=r*iTmp+Tmp.r*i2f(rb[(int)_y][(int)_x]); 
				  g=g*iTmp+Tmp.r*i2f(gb[(int)_y][(int)_x]); 
				  b=b*iTmp+Tmp.r*i2f(bb[(int)_y][(int)_x]); 
				  a=a*iTmp+Tmp.r*i2f(ab[(int)_y][(int)_x]); 

				  return 1;
				}
		  else if (Light==15)			// Transition RGB
				{ fColour Tmp;
			      fColour _Col;		lerp(Col,Dist,_Col);
				  fColour _Streaks; lerpwrap(Streaks,n,_Streaks);

				  Tmp.r=RenderIntensity.r*_Col.r*_Streaks.r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*_Col.g*_Streaks.g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*_Col.b*_Streaks.b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*_Col.a*_Streaks.a; if (Tmp.a>1.0f) Tmp.a=1.0f;

			      r=r*(1.0f-Tmp.r)+Tmp.r*i2f(rb[(int)_y][(int)_x]); 
				  g=g*(1.0f-Tmp.g)+Tmp.g*i2f(gb[(int)_y][(int)_x]); 
				  b=b*(1.0f-Tmp.b)+Tmp.b*i2f(bb[(int)_y][(int)_x]); 
				  a=a*(1.0f-Tmp.a)+Tmp.a*i2f(ab[(int)_y][(int)_x]); 

				  return 1;
				}
	  }

	  return 0;
	}

uchar Flare::GetColour2(float &r,float &g,float &b, float &a,				// Current Pixel
					  uchar **rb,uchar **gb,uchar **bb,uchar **ab,			// Current Background
					  float _x,float _y,int xres,int yres)					// Current X,Y Coords : float for AntiAliasing
	{ float x,y;
      Inferno.Distort(x,y,_x,_y);

	  float tmpcos=x-RenderCX;
      float tmpsin=y-RenderCY;			  
				  
	  double Ang		=atan2(tmpsin,tmpcos)+6.2831853072f;
      double Dst		=sqrt(tmpcos*tmpcos+tmpsin*tmpsin);
	  
	  int n=((int)(AngleM*Ang+AngleAdd))%FLARERESOLUTION;

      if (Shape[n]<1.0E-6f) return 0;
		
	  int Dist=(int)(RadiusM*Dst/Shape[n]);

	  if (Dist<FLARERESOLUTION)
	  {	  // N : Radial Andle
		  // Dist : Distance From Centre
		  n+=(int)(Shape2[Dist]*FLARERESOLUTION);
		  n=n%FLARERESOLUTION;
		  
		  // Get Colour Of Position In Element
		  if (Light==1)			
				{ fColour Tmp;					
				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;

			      Tmp.FilterOver(r,g,b,a);

				  return 1;
				}
		  else if (Light==0)
				{ fColour Tmp;					
				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  
				  r=1.0f-(1.0f-r)*(1.0f-Tmp.r);
				  g=1.0f-(1.0f-g)*(1.0f-Tmp.g);
				  b=1.0f-(1.0f-b)*(1.0f-Tmp.b);
				  a+=0.3333f*(Tmp.r+Tmp.g+Tmp.b); if (a>1.0f) a=1.0f;

				  return 1;
				}
		  else if (Light==6)
				{ fColour Tmp;					

		          Tmp.r=(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(Col[Dist].r+Col[Dist].g+Col[Dist].b))*
						(0.333333f*(Streaks[n].r+Streaks[n].g+Streaks[n].b)); 

				  if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.r*=xres*FlareSize*0.1f;
		   
				  Blur(r,g,b,a,rb,gb,bb,ab,_x,_y,xres,yres,Tmp.r);

				  return 1;
				}
		  else if (Light==7)
				{ fColour Tmp;					
				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.r*=xres*FlareSize*0.1f;
				  Blur(r,rb,_x,_y,xres,yres,Tmp.r);

				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.g*=xres*FlareSize*0.1f;
				  Blur(g,gb,_x,_y,xres,yres,Tmp.g);

				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.b*=xres*FlareSize*0.1f;
				  Blur(b,bb,_x,_y,xres,yres,Tmp.b);

				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  Tmp.a*=xres*FlareSize*0.1f;
				  Blur(a,ab,_x,_y,xres,yres,Tmp.a);

				  return 1;
				}
		  else if (Light==8)
				{ fColour Tmp;

				  if (Inferno.RenderNoise)
						{ tmpcos=_x-RenderCX; tmpsin=_y-RenderCY;			  
						  Ang=atan2(tmpsin,tmpcos)+6.2831853072f;
						  Dst=sqrt(tmpcos*tmpcos+tmpsin*tmpsin);
						}
						
				  Tmp.r=3.14159265359f*(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(Col[Dist].r+Col[Dist].g+Col[Dist].b))*(0.333333f*(Streaks[n].r+Streaks[n].g+Streaks[n].b)); 
				  
				  if (Tmp.r>3.14159265359f) Tmp.r=3.14159265359f;
	
				  float PosY=RenderCY+Dst*sin(Ang+Tmp.r); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=RenderCX+Dst*cos(Ang+Tmp.r); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  g=lerp2d(gb,PosX,PosY,xres,yres);
				  b=lerp2d(bb,PosX,PosY,xres,yres);
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==9)
				{ fColour Tmp;

		          if (Inferno.RenderNoise)
						{ tmpcos=_x-RenderCX; tmpsin=_y-RenderCY;			  
						  Ang=atan2(tmpsin,tmpcos)+6.2831853072f;
						  Dst=sqrt(tmpcos*tmpcos+tmpsin*tmpsin);
						}
						
				  Tmp.r=3.14159265359f*RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>3.14159265359f) Tmp.r=3.14159265359f;
				  Tmp.g=3.14159265359f*RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>3.14159265359f) Tmp.g=3.14159265359f;
				  Tmp.b=3.14159265359f*RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>3.14159265359f) Tmp.b=3.14159265359f;
				  Tmp.a=3.14159265359f*RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>3.14159265359f) Tmp.a=3.14159265359f;
	
				  float PosY=RenderCY+Dst*sin(Ang+Tmp.r); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=RenderCX+Dst*cos(Ang+Tmp.r); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);

				  PosY=RenderCY+Dst*sin(Ang+Tmp.g); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=RenderCX+Dst*cos(Ang+Tmp.g); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  g=lerp2d(gb,PosX,PosY,xres,yres);

				  PosY=RenderCY+Dst*sin(Ang+Tmp.b); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=RenderCX+Dst*cos(Ang+Tmp.b); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  b=lerp2d(bb,PosX,PosY,xres,yres);

				  PosY=RenderCY+Dst*sin(Ang+Tmp.a); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=RenderCX+Dst*cos(Ang+Tmp.a); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==10)
				{ fColour Tmp;
				  tmpcos=_x-RenderCX; 
				  tmpsin=_y-RenderCY;			  

				  Tmp.r=1.0f-(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(Col[Dist].r+Col[Dist].g+Col[Dist].b))*(0.333333f*(Streaks[n].r+Streaks[n].g+Streaks[n].b)); 
				  if (Tmp.r<0.0f) Tmp.r=0.0f;

				  float PosY=(RenderCY+Tmp.r*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(RenderCX+Tmp.r*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;

				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  g=lerp2d(gb,PosX,PosY,xres,yres);
				  b=lerp2d(bb,PosX,PosY,xres,yres);
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==11)
				{ fColour Tmp;

		          tmpcos=_x-RenderCX; 
				  tmpsin=_y-RenderCY;			  

				  Tmp.r=1.0f-RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r<0.0f) Tmp.r=0.0f;
				  Tmp.g=1.0f-RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g<0.0f) Tmp.g=0.0f;
				  Tmp.b=1.0f-RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b<0.0f) Tmp.b=0.0f;
				  Tmp.a=1.0f-RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a<0.0f) Tmp.a=0.0f;

				  float PosY=(RenderCY+Tmp.r*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(RenderCX+Tmp.r*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  
				  PosY=(RenderCY+Tmp.g*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(RenderCX+Tmp.g*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  g=lerp2d(gb,PosX,PosY,xres,yres);

				  PosY=(RenderCY+Tmp.b*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(RenderCX+Tmp.b*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  b=lerp2d(bb,PosX,PosY,xres,yres);

				  PosY=(RenderCY+Tmp.a*tmpsin); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(RenderCX+Tmp.a*tmpcos); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==5)
				{ fColour Tmp;					

				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r=(1.0f-r)*Tmp.r+r*(1.0f-Tmp.r);
				  g=(1.0f-g)*Tmp.g+g*(1.0f-Tmp.g);
				  b=(1.0f-b)*Tmp.b+b*(1.0f-Tmp.b);
				  a=(1.0f-a)*Tmp.a+a*(1.0f-Tmp.a);

				  return 1;
				}
		  else if (Light==2)
				{ fColour Tmp;					

				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r=r*Tmp.r;
				  g=g*Tmp.g;
				  b=b*Tmp.b;
				  a=a*Tmp.a;

				  return 1;
				}
		  else if (Light==3)
				{ fColour Tmp;					

				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r+=Tmp.r; if (r>1.0f) r=1.0f;
				  g+=Tmp.g; if (g>1.0f) g=1.0f;
				  b+=Tmp.b; if (b>1.0f) b=1.0f;
				  a+=Tmp.a; if (a>1.0f) a=1.0f;

				  return 1;
				}
		  else if (Light==4)
				{ fColour Tmp;					

				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;
				  
				  r-=Tmp.r; if (r<0.0f) r=0.0f;
				  g-=Tmp.g; if (g<0.0f) g=0.0f;
				  b-=Tmp.b; if (b<0.0f) b=0.0f;
				  a-=Tmp.a; if (a<0.0f) a=0.0f;

				  return 1;
				}
		  else if (Light==12)
				{ fColour Tmp;
				  
				  Tmp.r=(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(Col[Dist].r+Col[Dist].g+Col[Dist].b))*(0.333333f*(Streaks[n].r+Streaks[n].g+Streaks[n].b)); 
				  if (Tmp.r<0.0f) Tmp.r=0.0f;
				  float iTmp=1.0f-Tmp.r;

				  float PosY=(float)(_y*iTmp+Tmp.r*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(float)(_x*iTmp+Tmp.r*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;

				  r=lerp2d(rb,PosX,PosY,xres,yres);
				  g=lerp2d(gb,PosX,PosY,xres,yres);
				  b=lerp2d(bb,PosX,PosY,xres,yres);
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==13)
				{ fColour Tmp;
				  
				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;

				  float PosY=(float)(_y*(1.0f-Tmp.r)+Tmp.r*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  float PosX=(float)(_x*(1.0f-Tmp.r)+Tmp.r*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  r=lerp2d(rb,PosX,PosY,xres,yres);

				  PosY=(float)(_y*(1.0f-Tmp.g)+Tmp.g*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(float)(_x*(1.0f-Tmp.g)+Tmp.g*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  g=lerp2d(gb,PosX,PosY,xres,yres);

				  PosY=(float)(_y*(1.0f-Tmp.b)+Tmp.b*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(float)(_x*(1.0f-Tmp.b)+Tmp.b*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  b=lerp2d(bb,PosX,PosY,xres,yres);

				  PosY=(float)(_y*(1.0f-Tmp.a)+Tmp.a*y+0.5f); if (PosY<0) PosY=0; if (PosY>=yres) PosY=yres-1;
				  PosX=(float)(_x*(1.0f-Tmp.a)+Tmp.a*x+0.5f); if (PosX<0) PosX=0; if (PosX>=xres) PosX=xres-1;
				  a=lerp2d(ab,PosX,PosY,xres,yres);

				  return 1;
				}
		  else if (Light==14)			// Transition I
				{ fColour Tmp;					
				  Tmp.r=(0.333333f*(RenderIntensity.r+RenderIntensity.g+RenderIntensity.b))*
					    (0.333333f*(Col[Dist].r+Col[Dist].g+Col[Dist].b))*(0.333333f*(Streaks[n].r+Streaks[n].g+Streaks[n].b)); 
				  if (Tmp.r>1.0f) Tmp.r=1.0f;
				  float iTmp=1.0f-Tmp.r;

			      r=r*iTmp+Tmp.r*i2f(rb[(int)_y][(int)_x]); 
				  g=g*iTmp+Tmp.r*i2f(gb[(int)_y][(int)_x]); 
				  b=b*iTmp+Tmp.r*i2f(bb[(int)_y][(int)_x]); 
				  a=a*iTmp+Tmp.r*i2f(ab[(int)_y][(int)_x]); 

				  return 1;
				}
		  else if (Light==15)			// Transition RGB
				{ fColour Tmp;					
				  Tmp.r=RenderIntensity.r*Col[Dist].r*Streaks[n].r; if (Tmp.r>1.0f) Tmp.r=1.0f;
				  Tmp.g=RenderIntensity.g*Col[Dist].g*Streaks[n].g; if (Tmp.g>1.0f) Tmp.g=1.0f;
				  Tmp.b=RenderIntensity.b*Col[Dist].b*Streaks[n].b; if (Tmp.b>1.0f) Tmp.b=1.0f;
				  Tmp.a=RenderIntensity.a*Col[Dist].a*Streaks[n].a; if (Tmp.a>1.0f) Tmp.a=1.0f;

			      r=r*(1.0f-Tmp.r)+Tmp.r*i2f(rb[(int)_y][(int)_x]); 
				  g=g*(1.0f-Tmp.g)+Tmp.g*i2f(gb[(int)_y][(int)_x]); 
				  b=b*(1.0f-Tmp.b)+Tmp.b*i2f(bb[(int)_y][(int)_x]); 
				  a=a*(1.0f-Tmp.a)+Tmp.a*i2f(ab[(int)_y][(int)_x]); 

				  return 1;
				}
	  }

	  return 0;
	}


//***********************************************************************************************************
//* RENDERING ROUTINES
void Flare::ImageOutput(RenderNFO &rinfo,uchar r,uchar g,uchar b,uchar a,int x,int y)
{	  int Posn=y*rinfo.xres+x;

	  if (rinfo.UseZ)
			{ if ((rinfo.zbuffer[Posn]>0.0f)||(RenderDepth>rinfo.zbuffer[Posn])) 
						{ Img_R[y][x]=(uchar)r; Img_G[y][x]=(uchar)g;
						  Img_B[y][x]=(uchar)b; Img_A[y][x]=(uchar)a;

		                  if (rinfo.CompZ) rinfo.zbuffer[Posn]=RenderDepth;

						  if (rinfo.UseBuffer)	{ if (rinfo.mbufout!=-1) rinfo.mbuffer[Posn]=rinfo.mbufout;
												  if (rinfo.obufout!=-1) rinfo.obuffer[Posn]=rinfo.obufout;
												}
						}
	          else		{ float _tmp=(1.0f-Occlusion)*i2f(Img_A[y][x]);		// Better Aliasing
			              float _itmp=1.0f-_tmp;

				          Img_R[y][x]=(uchar)((float)r*_itmp+(float)Img_R[y][x]*_tmp);
			              Img_G[y][x]=(uchar)((float)g*_itmp+(float)Img_G[y][x]*_tmp);
						  Img_B[y][x]=(uchar)((float)b*_itmp+(float)Img_B[y][x]*_tmp); 
						  Img_A[y][x]=(uchar)((float)a*_itmp+(float)Img_A[y][x]*_tmp);
						}
			}
	  else	{ Img_R[y][x]=(uchar)r; Img_G[y][x]=(uchar)g;
			  Img_B[y][x]=(uchar)b; Img_A[y][x]=(uchar)a;

			  if ((rinfo.CompZ)&&((rinfo.zbuffer[Posn]>0.0f)||(RenderDepth>rinfo.zbuffer[Posn]))) rinfo.zbuffer[Posn]=RenderDepth;

			  if (rinfo.UseBuffer)	{ if (rinfo.mbufout!=-1) rinfo.mbuffer[Posn]=rinfo.mbufout;
									  if (rinfo.obufout!=-1) rinfo.obuffer[Posn]=rinfo.obufout;
									}
			}
}

//***********************************************************************************************************
//* RENDERING ROUTINES : MAIN ROUTINES
BOOL Flare::RenderCDHQArea_Single(RenderNFO &rinfo)
{ int _t_=(Light!=14)&&(Light!=15);
	
  if (rinfo.SizeMultiply>100.0f) 
				{ BOOL tmp=PreProcess(rinfo.xres,rinfo.yres,rinfo.xpos,rinfo.ypos,rinfo.CameraDepth,rinfo.cr,rinfo.cg,rinfo.cb,rinfo.ca,rinfo.Time,rinfo.Depth,rinfo.SizeMultiplyParticle);
                  if (tmp) return FALSE;
				}
  else SimplePreProcess(rinfo.xres,rinfo.yres,rinfo.SizeMultiply,rinfo.Time);

  if (!((MinX<rinfo.xres)||(MaxX>=0))&&((MinY<rinfo.yres)||(MaxY>=0))) return FALSE; // Nothing Visible

  int x,y;

  int StartX=MinX; if (StartX<0)			StartX=0;
  int EndX  =MaxX; if (EndX>=rinfo.xres)	EndX=rinfo.xres-1;

  int StartY=MinY; if (StartY<0)			StartY=0;
  int EndY  =MaxY; if (EndY>=rinfo.yres)	EndY=rinfo.yres-1;

  if ((Light>5)&&(Light!=14)&&(Light!=15))
				{ for(y=StartY;y<=EndY;y++) 
					for(x=StartX;x<=EndX;x++)
						{ Bak_R[y][x]=Img_R[y][x]; 
                          Bak_G[y][x]=Img_G[y][x]; 
						  Bak_B[y][x]=Img_B[y][x]; 
						  Bak_A[y][x]=Img_A[y][x]; 
						}
				}

  for(y=(StartY&0xfffffffe)+rinfo.StartLine;y<=EndY;y+=rinfo.SkipLine)
		{	//****************************************************************************************
			// RENDER LINE
			if (rinfo.Abort()) return TRUE;

			for(x=StartX;x<=EndX;x++)
			  if (rinfo.AntiAliasing==1.0f)
							{ float _r=i2f(Img_R[y][x]);
							  float _g=i2f(Img_G[y][x]);
							  float _b=i2f(Img_B[y][x]);
							  float _a=i2f(Img_A[y][x]);

							  int tmp;
							  
							  if (_t_) tmp=GetColour2(_r,_g,_b,_a,Bak_R,Bak_G,Bak_B,Bak_A,(float)x,(float)y,rinfo.xres,rinfo.yres);
							  else tmp=GetColour2(_r,_g,_b,_a,rinfo.Tmg_R,rinfo.Tmg_G,rinfo.Tmg_B,rinfo.Tmg_A,(float)x,(float)y,rinfo.xres,rinfo.yres);

							  if (tmp) ImageOutput(rinfo,_r*255.9999,_g*255.9999,_b*255.9999,_a*255.9999,x,y);
							}
			  else			{ float _ar=0.0f;
							  float _ag=0.0f;
							  float _ab=0.0f;
							  float _aa=0.0f;
							  int n=0,tmp=0;

							  for(float sx=(float)x;sx<(float)x+1.0f;sx+=rinfo.AntiAliasing)
							  for(float sy=(float)y;sy<(float)y+1.0f;sy+=rinfo.AntiAliasing,n++)
									{ float _r=i2f(Img_R[y][x]);
									  float _g=i2f(Img_G[y][x]);
									  float _b=i2f(Img_B[y][x]);
									  float _a=i2f(Img_A[y][x]);

									  if (_t_) tmp|=GetColour(_r,_g,_b,_a,Bak_R,Bak_G,Bak_B,Bak_A,(float)x,(float)y,rinfo.xres,rinfo.yres);
									  else tmp|=GetColour(_r,_g,_b,_a,rinfo.Tmg_R,rinfo.Tmg_G,rinfo.Tmg_B,rinfo.Tmg_A,(float)x,(float)y,rinfo.xres,rinfo.yres);

									  _ar+=_r;_ag+=_g;_ab+=_b;_aa+=_a;
									}

							  
							  if (tmp)	{	float _tmp=255.9999f/n; 
											ImageOutput(rinfo,_ar*_tmp,_ag*_tmp,_ab*_tmp,_aa*_tmp,x,y);
										}
							}
			  //****************************************************************************************
		}

  return FALSE;
}

BOOL Flare::RenderCDHQArea_NonSingle(RenderNFO &rinfo)
{ int x,y;
  int _t_=(Light!=14)&&(Light!=15);

  // Used To Compute MaxRad
  PreProcess(rinfo.xres, rinfo.yres, rinfo.xres/2, rinfo.yres/2, rinfo.CameraDepth, 1.0, 1.0, 1.0, 1.0,rinfo.Time,rinfo.Depth,1.0);
  float xmax=(MaxX-MinX)*0.5;
  float ymax=(MaxY-MinY)*0.5;

  if ((Light>5)&&(Light!=14)&&(Light!=15))
				{ for(y=0;y<rinfo.yres;y++)
				   for(x=0;x<rinfo.xres;x++)
						{ Bak_R[y][x]=Img_R[y][x]; Bak_G[y][x]=Img_G[y][x];
						  Bak_B[y][x]=Img_B[y][x]; Bak_A[y][x]=Img_A[y][x];
						}
				}

  // Use Distance Map !
  for(y=rinfo.StartLine;y<rinfo.yres;y+=rinfo.SkipLine)
	{ if (rinfo.Abort()) return TRUE;
	  for(x=0;x!=rinfo.xres;x++)
	  if ((fabs(rinfo.FromX[y][x])<=xmax)||(fabs(rinfo.FromY[y][x])<=ymax))
			{ float mix=i2f(rinfo.Filter[y][x]),imix=1.0f-mix;

			  if (rinfo.Col_A) PreProcess(rinfo.xres,rinfo.yres,(float)x-rinfo.FromX[y][x],
										  (float)y-rinfo.FromY[y][x],rinfo.CameraDepth,
										  i2f(rinfo.Col_R[y][x]),i2f(rinfo.Col_G[y][x]),i2f(rinfo.Col_B[y][x]),i2f(rinfo.Col_A[y][x]),rinfo.Time,rinfo.Depth,1.0f);

			  else PreProcess(rinfo.xres,rinfo.yres,(float)x-rinfo.FromX[y][x],
							  (float)y-rinfo.FromY[y][x],rinfo.CameraDepth,1.0f,1.0f,1.0f,1.0f,rinfo.Time,rinfo.Depth,1.0f);

			  if (rinfo.AntiAliasing==1.0f)
					{ float _r=i2f(Img_R[y][x]),__r=_r;
					  float _g=i2f(Img_G[y][x]),__g=_g;
					  float _b=i2f(Img_B[y][x]),__b=_b;
					  float _a=i2f(Img_A[y][x]),__a=_a;

					  int tmp;
					  if (_t_) tmp=GetColour2(_r,_g,_b,_a,Bak_R,Bak_G,Bak_B,Bak_A,(float)x,(float)y,rinfo.xres,rinfo.yres);
					  else tmp=GetColour2(_r,_g,_b,_a,rinfo.Tmg_R,rinfo.Tmg_G,rinfo.Tmg_B,rinfo.Tmg_A,(float)x,(float)y,rinfo.xres,rinfo.yres);

					  if (tmp) ImageOutput(rinfo,((uchar)((float)((imix*_r+mix*__r)*255.9999f))),
												 ((uchar)((float)((imix*_g+mix*__g)*255.9999f))),
												 ((uchar)((float)((imix*_b+mix*__b)*255.9999f))),
												 ((uchar)((float)((imix*_a+mix*__a)*255.9999f))),x,y);
					}
			  else	{ float _ar=0.0f;
					  float _ag=0.0f;
					  float _ab=0.0f;
					  float _aa=0.0f;

					  float __r=i2f(Img_R[y][x]);
					  float __g=i2f(Img_G[y][x]);
					  float __b=i2f(Img_B[y][x]);
					  float __a=i2f(Img_A[y][x]);

					  int n=0,tmp=0;

					  for(float sx=(float)x;sx<(float)x+1.0f;sx+=rinfo.AntiAliasing)
					  for(float sy=(float)y;sy<(float)y+1.0f;sy+=rinfo.AntiAliasing,n++)
							{ float _r=__r,_g=__g,_b=__b,_a=__a;

							  if (_t_) tmp|=GetColour(_r,_g,_b,_a,Bak_R,Bak_G,Bak_B,Bak_A,(float)x,(float)y,rinfo.xres,rinfo.yres);
							  else tmp|=GetColour(_r,_g,_b,_a,rinfo.Tmg_R,rinfo.Tmg_G,rinfo.Tmg_B,rinfo.Tmg_A,(float)x,(float)y,rinfo.xres,rinfo.yres);

							  _ar+=_r;_ag+=_g;_ab+=_b;_aa+=_a;
							}

					  if (tmp) { float _tmp=1.0f/(float)n;
								 ImageOutput(rinfo,((uchar)((float)((imix*_ar*_tmp+mix*__r)*255.9999f))),
												   ((uchar)((float)((imix*_ag*_tmp+mix*__g)*255.9999f))),
												   ((uchar)((float)((imix*_ab*_tmp+mix*__b)*255.9999f))),
												   ((uchar)((float)((imix*_aa*_tmp+mix*__a)*255.9999f))),x,y);
							   }
					}
			}
	}

  return FALSE;
}
					  
BOOL Flare::RenderCDHQArea(RenderNFO &rinfo)
{	  Img_R=rinfo.Img_R; Img_G=rinfo.Img_G; Img_B=rinfo.Img_B; Img_A=rinfo.Img_A;
	  Bak_R=rinfo.Bak_R; Bak_G=rinfo.Bak_G; Bak_B=rinfo.Bak_B; Bak_A=rinfo.Bak_A;

	  if (rinfo.UseSingle) return RenderCDHQArea_Single(rinfo);
	  else return RenderCDHQArea_NonSingle(rinfo);
}

//*************************************************************************************************
// DISK Saving Routines
void Flare::Load(FILE *FP){ int i;
						  _FileReadS(FP,Name,256);
						  _FileReadI(FP,&Active);
						  _FileReadI(FP,&Light);
						  _FileReadI(FP,&ReAlign);
						  _FileReadI(FP,&OffScreenRender);
						  _FileReadI(FP,&Linked);
						  _FileReadI(FP,&Active);
						  _FileReadF(FP,&IR2);
						  _FileReadI(FP,&Polar);

						  free(PosKeys);
						  _FileReadI(FP,&NoPosKeys);
						  PosKeys=(FlarePosAnim *)malloc(sizeof(FlarePosAnim)*NoPosKeys);

						  for(i=0;i!=NoPosKeys;i++)
							{	_FileReadI(FP,&PosKeys[i].Time);
								_FileReadF(FP,&PosKeys[i].PositionX);
								_FileReadF(FP,&PosKeys[i].PositionY);
								_FileReadF(FP,&PosKeys[i].Position);
								_FileReadF(FP,&PosKeys[i].Angle);
								_FileReadF(FP,&PosKeys[i].FlareSize);
								_FileReadF(FP,&PosKeys[i].AngleOffSet);
								_FileReadF(FP,&PosKeys[i].Colourise);
								_FileReadF(FP,&PosKeys[i].Intensity);
								_FileReadF(FP,&PosKeys[i].Occlusion);
								_FileReadF(FP,&PosKeys[i].Depth);
							}

						  free(ShpKeys);
						  _FileReadI(FP,&NoShpKeys);
						  ShpKeys=(FlareShpAnim *)malloc(sizeof(FlareShpAnim)*NoShpKeys);

						  for(int j=0;j!=NoShpKeys;j++)
							{	_FileReadI(FP,&ShpKeys[j].Time);
								for(i=0;i!=FLARERESOLUTION;i++) _FileReadW(FP,&ShpKeys[j].Shape [i]); 
								for(i=0;i!=FLARERESOLUTION;i++) _FileReadW(FP,&ShpKeys[j].Shape2[i]);
								for(i=0;i!=FLARERESOLUTION;i++) ShpKeys[j].Streaks[i].Load(FP); 
								for(i=0;i!=FLARERESOLUTION;i++) ShpKeys[j].Col[i].Load(FP);
							}

						    Inferno.Load(FP);

							_FileReadI(FP,&BuildColour);
							_FileReadI(FP,&BuildShape);
							_FileReadF(FP,&BuildWidth);
							_FileReadF(FP,&BuildTransparency);
							_FileReadI(FP,&BuildNoStreaks);
							_FileReadI(FP,&BuildNoSides);
							_FileReadF(FP,&BuildFactor);
							_FileReadF(FP,&BuildAspect);
							_FileReadI(FP,&BuildSeed);
							BuildCol.Load(FP);
						}

void Flare::Save(FILE *FP){ int i;
			              _FileWriteS(FP,Name,256);
						  _FileWriteI(FP,Active);
						  _FileWriteI(FP,Light);
						  _FileWriteI(FP,ReAlign);
						  _FileWriteI(FP,OffScreenRender);
						  _FileWriteI(FP,Linked);
						  _FileWriteI(FP,Active);
						  _FileWriteF(FP,IR2);
						  _FileWriteI(FP,Polar);
						  
						  _FileWriteI(FP,NoPosKeys);
						  for(i=0;i!=NoPosKeys;i++)
							{	_FileWriteI(FP,PosKeys[i].Time);
								_FileWriteF(FP,PosKeys[i].PositionX);
								_FileWriteF(FP,PosKeys[i].PositionY);
								_FileWriteF(FP,PosKeys[i].Position);
								_FileWriteF(FP,PosKeys[i].Angle);
								_FileWriteF(FP,PosKeys[i].FlareSize);
								_FileWriteF(FP,PosKeys[i].AngleOffSet);
								_FileWriteF(FP,PosKeys[i].Colourise);
								_FileWriteF(FP,PosKeys[i].Intensity);
								_FileWriteF(FP,PosKeys[i].Occlusion);
								_FileWriteF(FP,PosKeys[i].Depth);
							}

						  _FileWriteI(FP,NoShpKeys);
						  for(int j=0;j!=NoShpKeys;j++)
							{	_FileWriteI(FP,ShpKeys[j].Time);
								for(i=0;i!=FLARERESOLUTION;i++) _FileWriteW(FP,ShpKeys[j].Shape [i]); 
								for(i=0;i!=FLARERESOLUTION;i++) _FileWriteW(FP,ShpKeys[j].Shape2[i]);
								for(i=0;i!=FLARERESOLUTION;i++) ShpKeys[j].Streaks[i].Save(FP); 
								for(i=0;i!=FLARERESOLUTION;i++) ShpKeys[j].Col[i].Save(FP);
							}

							Inferno.Save(FP);

							_FileWriteI(FP,BuildColour);
							_FileWriteI(FP,BuildShape);
							_FileWriteF(FP,BuildWidth);
							_FileWriteF(FP,BuildTransparency);
							_FileWriteI(FP,BuildNoStreaks);
							_FileWriteI(FP,BuildNoSides);
							_FileWriteF(FP,BuildFactor);
							_FileWriteF(FP,BuildAspect);
							_FileWriteI(FP,BuildSeed);
							BuildCol.Save(FP);
						}

//*************************************************************************************************
// MEMORY Saving Routines
void Flare::MemLoad(char **FP){	  int i;
								  _MemReadS(FP,Name,256);
								  _MemReadI(FP,&Active);
								  _MemReadI(FP,&Light);
								  _MemReadI(FP,&ReAlign);
								  _MemReadI(FP,&OffScreenRender);
								  _MemReadI(FP,&Linked);
								  _MemReadI(FP,&Active);
								  _MemReadF(FP,&IR2);
								  _MemReadI(FP,&Polar);

								  free(PosKeys);
								  _MemReadI(FP,&NoPosKeys);
								  PosKeys=(FlarePosAnim *)malloc(sizeof(FlarePosAnim)*NoPosKeys);

								  for(i=0;i!=NoPosKeys;i++)
									{	_MemReadI(FP,&PosKeys[i].Time);

										_MemReadF(FP,&PosKeys[i].PositionX);
										_MemReadF(FP,&PosKeys[i].PositionY);
										_MemReadF(FP,&PosKeys[i].Position);
										_MemReadF(FP,&PosKeys[i].Angle);
										_MemReadF(FP,&PosKeys[i].FlareSize);
										_MemReadF(FP,&PosKeys[i].AngleOffSet);
										_MemReadF(FP,&PosKeys[i].Colourise);
										_MemReadF(FP,&PosKeys[i].Intensity);
										_MemReadF(FP,&PosKeys[i].Occlusion);
										_MemReadF(FP,&PosKeys[i].Depth);
									}

								  free(ShpKeys);
								  _MemReadI(FP,&NoShpKeys);
								  ShpKeys=(FlareShpAnim *)malloc(sizeof(FlareShpAnim)*NoShpKeys);

								  for(int j=0;j!=NoShpKeys;j++)
									{	_MemReadI(FP,&ShpKeys[j].Time);
										for(i=0;i!=FLARERESOLUTION;i++) _MemReadW(FP,&ShpKeys[j].Shape [i]); 
										for(i=0;i!=FLARERESOLUTION;i++) _MemReadW(FP,&ShpKeys[j].Shape2[i]);
										for(i=0;i!=FLARERESOLUTION;i++) ShpKeys[j].Streaks[i].MemLoad(FP); 
										for(i=0;i!=FLARERESOLUTION;i++) ShpKeys[j].Col[i].MemLoad(FP);
									}

								 Inferno.MemLoad(FP);

								 _MemReadI(FP,&BuildColour);
								 _MemReadI(FP,&BuildShape);
								 _MemReadF(FP,&BuildWidth);
								 _MemReadF(FP,&BuildTransparency);
								 _MemReadI(FP,&BuildNoStreaks);
								 _MemReadI(FP,&BuildNoSides);
								 _MemReadF(FP,&BuildFactor);
								 _MemReadF(FP,&BuildAspect);
								 _MemReadI(FP,&BuildSeed);
								 BuildCol.MemLoad(FP);
						}

int Flare::MemSave(char **FP){ int Ret=0;
						       int i;
							   Ret+=_MemWriteS(FP,Name,256);
							   Ret+=_MemWriteI(FP,Active);
							   Ret+=_MemWriteI(FP,Light);
							   Ret+=_MemWriteI(FP,ReAlign);
							   Ret+=_MemWriteI(FP,OffScreenRender);
							   Ret+=_MemWriteI(FP,Linked);
							   Ret+=_MemWriteI(FP,Active);
							   Ret+=_MemWriteF(FP,IR2);
							   Ret+=_MemWriteI(FP,Polar);
							   
							   Ret+=_MemWriteI(FP,NoPosKeys);
							   for(i=0;i!=NoPosKeys;i++)
								 {	Ret+=_MemWriteI(FP,PosKeys[i].Time);
									Ret+=_MemWriteF(FP,PosKeys[i].PositionX);
									Ret+=_MemWriteF(FP,PosKeys[i].PositionY);
									Ret+=_MemWriteF(FP,PosKeys[i].Position);
									Ret+=_MemWriteF(FP,PosKeys[i].Angle);
									Ret+=_MemWriteF(FP,PosKeys[i].FlareSize);
									Ret+=_MemWriteF(FP,PosKeys[i].AngleOffSet);
									Ret+=_MemWriteF(FP,PosKeys[i].Colourise);
									Ret+=_MemWriteF(FP,PosKeys[i].Intensity);
									Ret+=_MemWriteF(FP,PosKeys[i].Occlusion);
									Ret+=_MemWriteF(FP,PosKeys[i].Depth);
								}

							   Ret+=_MemWriteI(FP,NoShpKeys);
							   for(int j=0;j!=NoShpKeys;j++)
								 {	Ret+=_MemWriteI(FP,ShpKeys[j].Time);
									for(i=0;i!=FLARERESOLUTION;i++) Ret+=_MemWriteW(FP,ShpKeys[j].Shape [i]); 
									for(i=0;i!=FLARERESOLUTION;i++) Ret+=_MemWriteW(FP,ShpKeys[j].Shape2[i]);
									for(i=0;i!=FLARERESOLUTION;i++) Ret+=ShpKeys[j].Streaks[i].MemSave(FP); 
									for(i=0;i!=FLARERESOLUTION;i++) Ret+=ShpKeys[j].Col[i].MemSave(FP);
								 }

								Ret+=Inferno.MemSave(FP);

								Ret+=_MemWriteI(FP,BuildColour);
								Ret+=_MemWriteI(FP,BuildShape);
								Ret+=_MemWriteF(FP,BuildWidth);
								Ret+=_MemWriteF(FP,BuildTransparency);
								Ret+=_MemWriteI(FP,BuildNoStreaks);
								Ret+=_MemWriteI(FP,BuildNoSides);
								Ret+=_MemWriteF(FP,BuildFactor);
								Ret+=_MemWriteF(FP,BuildAspect);
								Ret+=_MemWriteI(FP,BuildSeed);
								Ret+=BuildCol.MemSave(FP);

							   return Ret;
							 }
