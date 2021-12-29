#ifndef _FLCLASS_
#define _FLCLASS_

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include <Max.h>
#include <bmmlib.h>
#include <fltlib.h>

#include "FileBin.h"
#include "utils.h"
#include "FColour.h"
#include "Fractal.h"
#include "VersionInfo.cpp"

#define MINSTREAK				1.0E-6f
#define LF_PI					3.14159265359f
#define FLARERESOLUTION			1024

typedef unsigned char		uchar;
typedef signed short int	sword;


// Pre Declaration
class Flare;

// Render Parameters Class
// FOR A 1024x768 Image :
// MINIMUM : 6.3Mb		(Non Delauney Triangulation)
// MAXIMUM : 15.9Mb		(Delauney Triangulation)
class RenderNFO
{	public :
			uchar **Img_R,**Img_G,**Img_B,**Img_A;	// Foreground Image
			uchar **Tmg_R,**Tmg_G,**Tmg_B,**Tmg_A;	// Transition Image
			uchar **Bak_R,**Bak_G,**Bak_B,**Bak_A;	// Background Image
			uchar **Col_R,**Col_G,**Col_B,**Col_A;	// Colourise Image

			int		xres,yres;						// Image Resolution
			float	CameraDepth;					// Camera Depth
			float	Time;							// duh !

			int		UseSingle;						// use Single
			float	xpos,ypos;						// Single Position
			float	cr,cg,cb,ca;					// Single Colourise
			float	SizeMultiply;					// Single Multiply Size if >100 then ignore

			float	SizeMultiplyParticle;

			float	AntiAliasing;					// AntiAliasing Number

			float **FromX,**FromY;					// Delauney Triangulation Input
			uchar **Filter;							// Filter Input

			float	*zbuffer;						// Z Buffer Pointer
			float	Depth;
			int		CompZ,UseZ;

			BYTE	*mbuffer;						// Material Buffer Pointer
			int		mbufout;

			WORD	*obuffer;						// Object Buffer
			int		obufout;

			uchar	UseBuffer;						// Do We Use The Buffer

			Flare	*RenderMe;

			// Field Rendering Stuff
			int		StartLine,SkipLine;

			HWND			Status;
			void			SetupStatus(HWND hWnd)	{ Status=hWnd;
			                                          SendMessage(Status,BMM_TEXTMSG,0,(LPARAM)_T(LENSFVERSION));
													}

			void			StatusText(char *F)		{ if (Status) { char Tmp[256];
																	sprintf(Tmp,"Genesis : %s\n",F);
																	SendMessage(Status,BMM_TEXTMSG,0,(LPARAM)Tmp);
																  }
													}

			void			StatusTextProgress(char *F,int i,int j)		
													{ if (Status) { char Tmp[256];
																	sprintf(Tmp,"Genesis : %s %d/%d\n",F,i,j);
																	SendMessage(Status,BMM_TEXTMSG,0,(LPARAM)Tmp);
																  }
													}

			void			StatusText(char *F,char *F2)		
													{ if (Status) { char Tmp[256];
													                sprintf(Tmp,"Genesis : %s (%s) \n",F,F2);
				                                                    SendMessage(Status,BMM_TEXTMSG,0,(LPARAM)Tmp);
																  }
													}

			void			StatusProgress(int x1,int x2)
													{ if (Status) SendMessage(Status,FLT_PROGRESS,x1,x2);
													}

			BOOL			Abort(void)				{ if (Status) { BOOL abort;
																	SendMessage(Status,FLT_CHECKABORT,0,(LPARAM)(BOOL *)&abort);
																	return abort;
																  }
												      return FALSE;
													}

			void			Init()	{ Img_R=Img_G=Img_B=Img_A=NULL;
									  Bak_R=Bak_G=Bak_B=Bak_A=NULL;
									  Col_R=Col_G=Col_B=Col_A=NULL;
									  FromX=NULL;
									  FromY=NULL;
									  Filter=NULL;
									  zbuffer=NULL;
									  mbuffer=NULL;
									  obuffer=NULL;
									  AntiAliasing=1.0f;
									  cr=cg=cb=ca=1.0f;
									  Status=NULL;
									  SizeMultiply=1000.0f;
									  SizeMultiplyParticle=1.0f;
									  UseBuffer=0;
									  CompZ=0;
									  UseZ=0;
									  Depth=0.0f;

									  StartLine=0;
									  SkipLine=1;
									}
};

//-----------------------------------------------------------------------------
class FlarePosAnim
	{ public:
		int		Time;
		
		float	Position,Angle,Depth;
		float	FlareSize;
		float	AngleOffSet;
		float	Colourise;
		float	Intensity;
		float	Occlusion;
		
		float	PositionX,PositionY;

		// FlarePosAnim() {}		
		//~FlarePosAnim() {}
	};

//-----------------------------------------------------------------------------
class FlareShpAnim
	{ public:
		int		Time;
		
		cColour Col		[FLARERESOLUTION];
		cColour Streaks	[FLARERESOLUTION];
		WORD	Shape	[FLARERESOLUTION];
		WORD	Shape2	[FLARERESOLUTION];

		// FlareShpAnim() {}	
		//~FlareShpAnim() {}
	};
//-----------------------------------------------------------------------------

class Flare
	{ private:
		char	 Name[256];

		// Radial Colours
		fColour Col[FLARERESOLUTION];
		fColour Streaks[FLARERESOLUTION];
		float	Shape[FLARERESOLUTION];
		float	Shape2[FLARERESOLUTION];
		
		// Lookup Values

		float	 AngleM;
		float 	 RadiusM;
		float	 RenderDepth;
		float	 AngleAdd;

		//***********************************************************************************************************
		//***********************************************************************************************************
		float fsqr(float a) { return a*a; }

		float lerp(float *In,float a) { int pos1=(float)floor(a),pos2=pos1+1;
		                                if (pos2<FLARERESOLUTION) { float _tmp=a-(float)pos1;
										                            return _tmp*In[pos2%FLARERESOLUTION]+(1.0f-_tmp)*In[pos1%FLARERESOLUTION];
																  }
										else return In[pos1%FLARERESOLUTION];
									  }

		void lerp(fColour *In,float a,fColour &fin) 
									  { int pos1=(float)floor(a),pos2=pos1+1;
		                                if (pos2<FLARERESOLUTION) { float _tmp=a-(float)pos1,itmp=1.0f-_tmp;
										                            fin.r=_tmp*In[pos2].r+itmp*In[pos1].r;
																	fin.g=_tmp*In[pos2].g+itmp*In[pos1].g;
																	fin.b=_tmp*In[pos2].b+itmp*In[pos1].b;
																	fin.a=_tmp*In[pos2].a+itmp*In[pos1].a;
																  }
										else fin=In[pos1%FLARERESOLUTION];
									  }

		void lerpwrap(fColour *In,float a,fColour &fin) 
									  { int pos1=(float)floor(a),pos2=pos1+1;
		                                if (pos2>=FLARERESOLUTION) pos2=0;

		                                float _tmp=a-(float)pos1,itmp=1.0f-_tmp;
										fin.r=_tmp*In[pos2].r+itmp*In[pos1].r;
										fin.g=_tmp*In[pos2].g+itmp*In[pos1].g;
										fin.b=_tmp*In[pos2].b+itmp*In[pos1].b;
										fin.a=_tmp*In[pos2].a+itmp*In[pos1].a;
									  }

		void lerp3(fColour *In,float a,fColour &fin) 
									  { int pos1=(float)floor(a),pos2=pos1+1;
		                                if (pos2<FLARERESOLUTION) { float _tmp=a-(float)pos1,itmp=1.0f-_tmp;
										                            fin.r=_tmp*In[pos2].r+itmp*In[pos1].r;
																	fin.g=_tmp*In[pos2].g+itmp*In[pos1].g;
																	fin.b=_tmp*In[pos2].b+itmp*In[pos1].b;
																  }
										else fin=In[pos1%FLARERESOLUTION];
									  }

		void lerpwrap3(fColour *In,float a,fColour &fin) 
									  { int pos1=(float)floor(a),pos2=pos1+1;
		                                if (pos2>=FLARERESOLUTION) pos2=0;

		                                float _tmp=a-(float)pos1,itmp=1.0f-_tmp;
										fin.r=_tmp*In[pos2].r+itmp*In[pos1].r;
										fin.g=_tmp*In[pos2].g+itmp*In[pos1].g;
										fin.b=_tmp*In[pos2].b+itmp*In[pos1].b;
									  }

		float lerpwrap(float *In,float a) { int pos1=(float)floor(a),pos2=pos1+1;
		                                    if (pos2>=FLARERESOLUTION) pos2=0;
		                                    float _tmp=a-(float)pos1;
											return _tmp*In[pos2]+(1.0f-_tmp)*In[pos1];
										  }

		float lerp2d(uchar **In,float x,float y,int xres,int yres)
			{ int x1=(int)x,x2=x1+1; if (x2>=xres) x2=xres-1;
		      int y1=(int)y,y2=y1+1; if (y2>=yres) y2=yres-1;

			  float tx=x-(float)x1,itx=1.0f-tx;
			  float tmp1=i2f(In[y1][x1])*itx+i2f(In[y1][x2])*tx;
			  float tmp2=i2f(In[y2][x1])*itx+i2f(In[y2][x2])*tx;

			  float ty=y-(float)y1;

			  return tmp1*(1.0f-ty)+tmp2*ty;
			}
		//***********************************************************************************************************
		//***********************************************************************************************************

	  public:
		// Positions Etc...
					float	PositionX,PositionY;
					float	Position,Angle;
					float	FlareSize;
					float	AngleOffSet;
					float	Colourise;
					float	Intensity;
					float	Occlusion;
					float	Depth;

					float	RenderCX,RenderCY;
					float	TrueX,TrueY;

					float	RenderSize;
					fColour	RenderIntensity;

					int		OffScreenRender;
					int		Linked;
					int		Active;
					int		Light;
					int		ReAlign;
					float	IR2;
					int		MinX,MaxX,MinY,MaxY;
					int		Polar;

					Fractal	Inferno;

					void ConvertCoordinates(void)
						{ if (!Polar) { TrueX=Position; TrueY=Angle; }
					      else	{ TrueX=Position*(float)cos(Angle*3.14159f);
						          TrueY=Position*(float)sin(Angle*3.14159f);
								}
						}

		//**************************************************************************

		// Key Framing Info
					int NoPosKeys;
					FlarePosAnim *PosKeys;

					int NoShpKeys;
					FlareShpAnim *ShpKeys;

	  private:

		void Swap(int &i1,int &i2) { int t=i2; i2=i1; i1=t; }

		void Swap(char &i1,char &i2) { char t=i2; i2=i1; i1=t; }
		void Swap(unsigned char &i1,unsigned char &i2) { unsigned char t=i2; i2=i1; i1=t; }
		void Swap(WORD &i1,WORD &i2) { WORD t=i2; i2=i1; i1=t; }

		void Swap(float &i1,float &i2) { float t=i2; i2=i1; i1=t; }

		void Swap(fColour &i1,fColour &i2) { Swap(i1.r,i2.r); 
		                                     Swap(i1.g,i2.g); 
											 Swap(i1.b,i2.b);
											 Swap(i1.a,i2.a); 
										   }

		void Swap(cColour &i1,cColour &i2) { Swap(i1.r,i2.r); 
		                                     Swap(i1.g,i2.g); 
											 Swap(i1.b,i2.b);
											 Swap(i1.a,i2.a); 
										   }
	  public:

		//***********************************************************************************************************
		//***********************************************************************************************************
		//***********************************************************************************************************
		//***********************************************************************************************************
		// Routines For Positioanl Key Frames
		void PutPosValues(FlarePosAnim &F,int Time)
				{ F.PositionX=PositionX; F.PositionY=PositionY;
				  F.Position=Position; F.Angle=Angle;
				  F.FlareSize=FlareSize; F.AngleOffSet=AngleOffSet;
				  F.Colourise=Colourise; F.Intensity=Intensity;
				  F.Occlusion=Occlusion; F.Depth=Depth;
				  F.Time=Time;
				}
		
		void PutPosValues(FlarePosAnim &F)
				{ F.PositionX=PositionX; F.PositionY=PositionY;
				  F.Position=Position; F.Angle=Angle;
				  F.FlareSize=FlareSize; F.AngleOffSet=AngleOffSet;
				  F.Colourise=Colourise; F.Intensity=Intensity;
				  F.Occlusion=Occlusion; F.Depth=Depth;
				}

		void PutPosValues(int i) { PutPosValues(PosKeys[i]); }
		
		int GetPosValues(FlarePosAnim &F)
				{ PositionX=F.PositionX;
				  PositionY=F.PositionY;
				  Position=F.Position;
				  Angle=F.Angle;
				  FlareSize=F.FlareSize;
				  AngleOffSet=F.AngleOffSet;
				  Colourise=F.Colourise;
				  Intensity=F.Intensity;
				  Occlusion=F.Occlusion;
				  Depth=F.Depth;

				  return F.Time;
				}

		void GetPosValues(int i) { GetPosValues(PosKeys[i]); }

		void SwapPosKey(FlarePosAnim &F1,FlarePosAnim &F2)
				{ Swap(F1.PositionX,F2.PositionX);
		          Swap(F1.PositionY,F2.PositionY);
				  Swap(F1.Position,F2.Position);
				  Swap(F1.Angle,F2.Angle);
				  Swap(F1.FlareSize,F2.FlareSize);
				  Swap(F1.AngleOffSet,F2.AngleOffSet);
				  Swap(F1.Colourise,F2.Colourise);
				  Swap(F1.Intensity,F2.Intensity);
				  Swap(F1.Occlusion,F2.Occlusion);
				  Swap(F1.Depth,F2.Depth);
				  Swap(F1.Time,F2.Time);
				}

		void CopyPosKey(FlarePosAnim &F1,FlarePosAnim &F2)
				{ F1.PositionX=F2.PositionX;
		          F1.PositionY=F2.PositionY;
				  F1.Position=F2.Position;
				  F1.Angle=F2.Angle;
				  F1.FlareSize=F2.FlareSize;
				  F1.AngleOffSet=F2.AngleOffSet;
				  F1.Colourise=F2.Colourise;
				  F1.Intensity=F2.Intensity;
				  F1.Occlusion=F2.Occlusion;
				  F1.Depth=F2.Depth;
				  F1.Time=F2.Time;
				}

		void SortPosKeys(void)
				{ // Backwards Bubble Sort ... Probably The Best Way for our particular situation
			      for (int To=0;To<NoPosKeys;To++)
						{ int Swapped=0;
						  for(int i=NoPosKeys-1;i>To;i--)
							if (PosKeys[i].Time<PosKeys[i-1].Time) 
										{ SwapPosKey(PosKeys[i],PosKeys[i-1]); 
						                  Swapped=1; 
										}

						  if (Swapped==0) return;
						}
				}

		void AddPosKey(int Time)
				{ NoPosKeys++;
		          PosKeys=(FlarePosAnim *)realloc(PosKeys,sizeof(FlarePosAnim)*NoPosKeys);

				  PutPosValues(PosKeys[NoPosKeys-1],Time);
				  SortPosKeys();
				}

		void DeletePosKey(int Num)
				{ if (Num>=0) { int i,j;	// This is so you can call it using DeletePosKey(IsPosKey(..));

		                        for(i=0,j=0;i!=NoPosKeys;i++)
								if (i!=Num) { if (i!=j) CopyPosKey(PosKeys[j++],PosKeys[i]);
								              else j++;
								            }

								NoPosKeys--;
								PosKeys=(FlarePosAnim *)realloc(PosKeys,sizeof(FlarePosAnim)*NoPosKeys);
							  }
				}

		 int IsPosKey(int Time)
				{ for(int i=0;((i!=NoPosKeys)&&(PosKeys[i].Time<=Time));i++)
					  if (PosKeys[i].Time==Time) return i;
				  return -1;
				}

		 int GetPosKeyFrame(int Time)
				{ for(int i=0;i!=NoPosKeys;i++)
					  if (PosKeys[i].Time>Time) return i-1;

				  return NoPosKeys-1;
				}

#define Tween(Var) Var=t2*(PosKeys[i-1].Var)+t*(PosKeys[i].Var);
		 void GetPosition(float Time)
				{ for(int i=0;i!=NoPosKeys;i++)
				  if ((float)PosKeys[i].Time>Time)
							{ float t=(Time-(float)PosKeys[i-1].Time)/((float)PosKeys[i].Time-(float)PosKeys[i-1].Time);
		                      float t2=1.0f-t;

							  Tween(Position);
							  Tween(Angle);
						      Tween(Depth);
							  Tween(FlareSize);
							  Tween(AngleOffSet);
							  Tween(Colourise);
							  Tween(Intensity);
						      Tween(Occlusion);
							  Tween(PositionX); 
							  Tween(PositionY); 

							  IR2=FlareSize*FlareSize*Intensity;
							
							  return;
							}

				  GetPosValues(PosKeys[NoPosKeys-1]);
				}

		 void GetPosition(int Time)
				{ GetPosition((float)Time);
				}

#undef Tween

		//***********************************************************************************************************
		//***********************************************************************************************************
		//***********************************************************************************************************
		//***********************************************************************************************************
		// Routines For Shape Key Frames
		unsigned char	f2i(float &a) { return (unsigned char)((float)(a*255.9999f)); }
		float			i2f(unsigned char &a) { return ((float)a)*3.92156862745E-3f; }

		WORD			f2w(float &a) { return (WORD)((float)(a*65535.99f)); }
		float			w2f(WORD &a) { return ((float)a)*1.52590218967E-5; }

		void PutShpValues(FlareShpAnim &F,int Time)
				{ int i;
		  
		          for(i=0;i!=FLARERESOLUTION;i++)
					{ F.Col[i].r		=f2i(Col[i].r);
				      F.Col[i].g		=f2i(Col[i].g);
					  F.Col[i].b		=f2i(Col[i].b);
					  F.Col[i].a		=f2i(Col[i].a);

		              F.Streaks[i].r	=f2i(Streaks[i].r);
					  F.Streaks[i].g	=f2i(Streaks[i].g);
					  F.Streaks[i].b	=f2i(Streaks[i].b);
					  F.Streaks[i].a	=f2i(Streaks[i].a);

		              F.Shape[i]	=f2w(Shape[i]);
		              F.Shape2[i]	=f2w(Shape2[i]);
					}

				  F.Time=Time;
				}
		
		void PutShpValues(FlareShpAnim &F)
				{ int i;
		  
		          for(i=0;i!=FLARERESOLUTION;i++)
					{ F.Col[i].r		=f2i(Col[i].r);
				      F.Col[i].g		=f2i(Col[i].g);
					  F.Col[i].b		=f2i(Col[i].b);
					  F.Col[i].a		=f2i(Col[i].a);

		              F.Streaks[i].r	=f2i(Streaks[i].r);
					  F.Streaks[i].g	=f2i(Streaks[i].g);
					  F.Streaks[i].b	=f2i(Streaks[i].b);
					  F.Streaks[i].a	=f2i(Streaks[i].a);

		              F.Shape[i]	=f2w(Shape[i]);
		              F.Shape2[i]	=f2w(Shape2[i]);
					}
				}

		void PutShpValues(int i) { PutShpValues(ShpKeys[i]); }
		
		int GetShpValues(FlareShpAnim &F)
				{ int i;
		  
		          for(i=0;i!=FLARERESOLUTION;i++)
					{ Col[i].r		=i2f(F.Col[i].r);
				      Col[i].g		=i2f(F.Col[i].g);
					  Col[i].b		=i2f(F.Col[i].b);
					  Col[i].a		=i2f(F.Col[i].a);

		              Streaks[i].r	=i2f(F.Streaks[i].r);
					  Streaks[i].g	=i2f(F.Streaks[i].g);
					  Streaks[i].b	=i2f(F.Streaks[i].b);
					  Streaks[i].a	=i2f(F.Streaks[i].a);

		              Shape[i]		=w2f(F.Shape[i]);
		              Shape2[i]		=w2f(F.Shape2[i]);
					}

				  return F.Time;
				}

		int GetShpValues(int i) { return GetShpValues(ShpKeys[i]); }

		void SwapShpKey(FlareShpAnim &F1,FlareShpAnim &F2)
				{ int i;
		  
		          for(i=0;i!=FLARERESOLUTION;i++)
						{ Swap(F1.Col[i],F2.Col[i]);
						  Swap(F1.Streaks[i],F2.Streaks[i]);
						  Swap(F1.Shape[i],F2.Shape[i]);
						  Swap(F1.Shape2[i],F2.Shape2[i]);
						}

				  Swap(F1.Time,F2.Time);
				}

		void CopyShpKey(FlareShpAnim &F1,FlareShpAnim &F2)
				{ int i;
		  
		          for(i=0;i!=FLARERESOLUTION;i++)
						{ F1.Col[i]=F2.Col[i];
						  F1.Streaks[i]=F2.Streaks[i];
						  F1.Shape[i]=F2.Shape[i];
						  F1.Shape2[i]=F2.Shape2[i];
						}

				  F1.Time=F2.Time;
				}

		void SortShpKeys(void)
				{ // Backwards Bubble Sort ... Probably The Best Way for our particular situation
			      for (int To=0;To<NoShpKeys;To++)
						{ int Swapped=0;
						  for(int i=NoShpKeys-1;i>To;i--)
							if (ShpKeys[i].Time<ShpKeys[i-1].Time) 
										{ SwapShpKey(ShpKeys[i],ShpKeys[i-1]); 
						                  Swapped=1; 
										}

						  if (Swapped==0) return;
						}
				}

		void AddShpKey(int Time)
				{ NoShpKeys++;
		          ShpKeys=(FlareShpAnim *)realloc(ShpKeys,sizeof(FlareShpAnim)*NoShpKeys);

				  PutShpValues(ShpKeys[NoShpKeys-1],Time);
				  SortShpKeys();
				}

		void DeleteShpKey(int Num)
				{ if (Num>=0) { int i,j;	// This is so you can call it using DeleteShpKey(IsShpKey(..));

		                        for(i=0,j=0;i!=NoShpKeys;i++)
								if (i!=Num) { if (i!=j) CopyShpKey(ShpKeys[j++],ShpKeys[i]);
								              else j++;
								            }

								NoShpKeys--;
								ShpKeys=(FlareShpAnim *)realloc(ShpKeys,sizeof(FlareShpAnim)*NoShpKeys);
							  }
				}

		 int IsShpKey(int Time)
				{ for(int i=0;((i!=NoShpKeys)&&(ShpKeys[i].Time<=Time));i++)
					  if (ShpKeys[i].Time==Time) return i;
				  return -1;
				}

		 int GetShpKeyFrame(int Time)
				{ for(int i=0;i!=NoShpKeys;i++)
					  if (ShpKeys[i].Time>Time) return i-1;

				  return NoShpKeys-1;
				}

		 void GetShape(float Time)
				{ for(int i=0;i!=NoShpKeys;i++)
				  if ((float)ShpKeys[i].Time>Time)
							{ float t=(Time-(float)ShpKeys[i-1].Time)/((float)ShpKeys[i].Time-(float)ShpKeys[i-1].Time);
		                      float t2=1.0f-t;

							  for(int j=0;j!=FLARERESOLUTION;j++)
									{ Col[j].r=t2*i2f(ShpKeys[i-1].Col[j].r)+t*i2f(ShpKeys[i].Col[j].r);
							          Col[j].g=t2*i2f(ShpKeys[i-1].Col[j].g)+t*i2f(ShpKeys[i].Col[j].g);
									  Col[j].b=t2*i2f(ShpKeys[i-1].Col[j].b)+t*i2f(ShpKeys[i].Col[j].b);
									  Col[j].a=t2*i2f(ShpKeys[i-1].Col[j].a)+t*i2f(ShpKeys[i].Col[j].a);

									  Streaks[j].r=t2*i2f(ShpKeys[i-1].Streaks[j].r)+t*i2f(ShpKeys[i].Streaks[j].r);
									  Streaks[j].g=t2*i2f(ShpKeys[i-1].Streaks[j].g)+t*i2f(ShpKeys[i].Streaks[j].g);
									  Streaks[j].b=t2*i2f(ShpKeys[i-1].Streaks[j].b)+t*i2f(ShpKeys[i].Streaks[j].b);
									  Streaks[j].a=t2*i2f(ShpKeys[i-1].Streaks[j].a)+t*i2f(ShpKeys[i].Streaks[j].a);
									  
									  Shape [j]=t2*w2f(ShpKeys[i-1].Shape [j])+t*w2f(ShpKeys[i].Shape [j]);
									  Shape2[j]=t2*w2f(ShpKeys[i-1].Shape2[j])+t*w2f(ShpKeys[i].Shape2[j]);
									}
							
							  return;
							}

				  GetShpValues(ShpKeys[NoShpKeys-1]);
				}

		 void GetShape(int Time)
				{ GetShape((float)Time);
				}

		//***********************************************************************************************************
		//***********************************************************************************************************
		//***********************************************************************************************************
		//***********************************************************************************************************
	public:
		int				 BuildColour;
		int				 BuildShape;
		float			 BuildWidth;
		float			 BuildTransparency;
		int				 BuildNoStreaks;
		int				 BuildNoSides;
		int				 BuildSeed;
		float			 BuildFactor;
		float			 BuildAspect;
		fColour			 BuildCol;
		
	private:
		// Private Functions
		float	Grid(int x, int y)	{ if ((x%8==0)||(y%8==0)) return 0.4f;
			                          else if (((x/8)+(y/8))%2) return 0.2f;
								      else return 0.1f;
									}

		int		Grid2(int x, int y)	{ if ((x%8==0)||(y%8==0)) return 1;
								      else return 0;
									}

		float	Sqr(float x)				{ return x*x; }
		float	drand48()					{ return (float)rand()/(float)RAND_MAX; }
		void	srand48(unsigned int Seed)	{ srand(Seed); }

		void	InitFlare(float FS, float Pos)	{ for(int i=0;i!=FLARERESOLUTION;i++) Shape[i]=1.0f;
												  for(i=0;i!=FLARERESOLUTION;i++) Shape2[i]=0.0f;
												  for(i=0;i!=FLARERESOLUTION;i++) Streaks[i].SetColourRGBA(1.0f,1.0f,1.0f,1.0f);

												  float t=0.0f, dt=1.0f/(float)(FLARERESOLUTION);
												  for(i=0;i!=FLARERESOLUTION;i++,t+=dt) 
														{ float tmp=(float)exp(-5.545*t*t);
													      Col[i].r=tmp; Col[i].g=tmp; 
														  Col[i].b=tmp; Col[i].a=tmp;
														}

												  Position=Pos; PositionX=0.0f; PositionY=0.0f;
												  FlareSize=FS; Angle=0.0f; AngleOffSet=0.0f; Colourise=1.0f;
												  Active=1; Light=0; ReAlign=0;
												  Intensity=1.0f; Occlusion=0.0f; Depth=0.0f;
												  OffScreenRender=1; Linked=0;
												  Polar=0;
												  IR2=Intensity*FlareSize*FlareSize;

												  // ROOT Positional Key Frame
												  NoPosKeys=1;
												  PosKeys=(FlarePosAnim *)malloc(sizeof(FlarePosAnim)*NoPosKeys);
												  PutPosValues(PosKeys[0],0);

												  NoShpKeys=1;
												  ShpKeys=(FlareShpAnim *)malloc(sizeof(FlareShpAnim)*NoShpKeys);
												  PutShpValues(ShpKeys[0],0);

												  // Initilase Inferno Style Effect
												  Inferno.Initialise();

												  // Setup For New Dialog
												  BuildColour=1;
												  BuildShape=1;
												  BuildWidth=0.1f;
												  BuildTransparency=1.0f;
												  BuildNoStreaks=6;
												  BuildNoSides=6;
												  BuildFactor=0.5f;
												  BuildAspect=0.5f;
												  BuildSeed=0;
												  BuildCol.SetColourRGBA(1.0f,1.0f,1.0f,1.0f);
												}

		void	FreeFlare()					{ if (PosKeys) { free(PosKeys);
		                                                     PosKeys=NULL;
															 NoPosKeys=0;
														   }

		                                      if (ShpKeys) { free(ShpKeys);
		                                                     ShpKeys=NULL;
															 NoShpKeys=0;
														   }

											}

		void Blur(float &,uchar **,float,float,int,int,float);
		void Blur(float &,float &,float &,float &,uchar **,uchar **,uchar **,uchar **,float,float,int,int,float);

	  public:
		// Public Functions
		void	GetRadialColour (fColour &a, int i) { a.r=Col[i].r; a.g=Col[i].g; a.b=Col[i].b; a.a=Col[i].a; }
		void	GetAngularColour(fColour &a, int i) { a.r=Streaks[i].r; a.g=Streaks[i].g; a.b=Streaks[i].b; a.a=Streaks[i].a; }
		void	GetShape        (float &a,   int i) { a=Shape[i]; }	  	
		void	GetShape2       (float &a,   int i) { a=Shape2[i]; }	  	

		void	PutRadialColour (fColour &a, int i) { Col[i].r=a.r; Col[i].g=a.g; Col[i].b=a.b; Col[i].a=a.a; }
		void	PutAngularColour(fColour &a, int i) { Streaks[i].r=a.r; Streaks[i].g=a.g; Streaks[i].b=a.b; Streaks[i].a=a.a; }
		void	PutShape        (float a, int i)	{ Shape[i]=a; }
		void	PutShape2       (float a, int i)	{ Shape2[i]=a; }

		void	PutRadialColourR (float a, int i)	{ Col[i].r=a; }
		void	PutRadialColourG (float a, int i)	{ Col[i].g=a; }
		void	PutRadialColourB (float a, int i)	{ Col[i].b=a; }
		void	PutRadialColourA (float a, int i)	{ Col[i].a=a; }

		void	PutAngularColourR (float a, int i)	{ Streaks[i].r=a; }
		void	PutAngularColourG (float a, int i)	{ Streaks[i].g=a; }
		void	PutAngularColourB (float a, int i)	{ Streaks[i].b=a; }
		void	PutAngularColourA (float a, int i)	{ Streaks[i].a=a; }

		void	ResetRadialRGB(void) { int i; for(i=0;i!=FLARERESOLUTION;i++) Col[i].SetColourRGBA(0.0f,0.0f,0.0f); }
		void	ResetAngularRGB(void) { int i; for(i=0;i!=FLARERESOLUTION;i++) Streaks[i].SetColourRGBA(0.0f,0.0f,0.0f); }
		void	ResetRadialA(void) { int i; for(i=0;i!=FLARERESOLUTION;i++) Col[i].SetColourRGBA(0.0f); }
		void	ResetAngularA(void) { int i; for(i=0;i!=FLARERESOLUTION;i++) Streaks[i].SetColourRGBA(0.0f); }
		void	ResetShape(void) { int i; for(i=0;i!=FLARERESOLUTION;i++) Shape[i]=0.0f; }
		void	ResetShape2(void) { int i; for(i=0;i!=FLARERESOLUTION;i++) Shape2[i]=0.0f; }

		void	DefaultRadialRGB(void)	{ int i; for(i=0;i!=FLARERESOLUTION;i++) 
														{ Col[i].a=1.0f;
		                                                  Streaks[i].r=1.0f;
														  Streaks[i].g=1.0f;
														  Streaks[i].b=1.0f;
														  Streaks[i].a=1.0f;
														  Shape  [i]=1.0f;
														  Shape2 [i]=0.0f;
														}
										}

		void	DefaultRadialA(void)	{ int i; for(i=0;i!=FLARERESOLUTION;i++) 
														{ Col[i].r=Col[i].a;
		                                                  Col[i].g=Col[i].a;
														  Col[i].b=Col[i].a;
														  Col[i].a=1.0f;
		                                                  Streaks[i].r=1.0f;
														  Streaks[i].g=1.0f;
														  Streaks[i].b=1.0f;
														  Streaks[i].a=1.0f;
														  Shape  [i]=1.0f;
														  Shape2 [i]=0.0f;
														}
										}

		void	DefaultAngularRGB(void)	{ int i; for(i=0;i!=FLARERESOLUTION;i++) 
														{ Streaks[i].a=1.0f;
		                                                  Col[i].r=1.0f;
														  Col[i].g=1.0f;
														  Col[i].b=1.0f;
														  Col[i].a=1.0f;
														  Shape  [i]=1.0f;
														  Shape2 [i]=0.0f;
														}
										}

		void	DefaultAngularA(void)	{ int i; for(i=0;i!=FLARERESOLUTION;i++) 
														{ Streaks[i].r=Streaks[i].a;
		                                                  Streaks[i].g=Streaks[i].a;
														  Streaks[i].b=Streaks[i].a;
														  Streaks[i].a=1.0f;
		                                                  Col[i].r=1.0f;
														  Col[i].g=1.0f;
														  Col[i].b=1.0f;
														  Col[i].a=1.0f;
														  Shape  [i]=1.0f;
														  Shape2 [i]=0.0f;
														}
										}

		void	DefaultShape(void)		{ int i; for(i=0;i!=FLARERESOLUTION;i++) 
														{ Streaks[i].r=1.0f;
		                                                  Streaks[i].g=1.0f;
														  Streaks[i].b=1.0f;
														  Streaks[i].a=1.0f;

		                                                  if ((i>>7)&1)  { Col[i].r=1.0f;
																		   Col[i].g=1.0f;
																		   Col[i].b=1.0f;
																		   Col[i].a=1.0f;
																		 }
														  else			 { Col[i].r=0.0f;
																		   Col[i].g=0.0f;
																		   Col[i].b=0.0f;
																		   Col[i].a=0.0f;
																		 }

														  Shape2 [i]=0.0f;
														}
										}

		void	DefaultShape2(void)		{ int i; for(i=0;i!=FLARERESOLUTION;i++) 
														{ Col[i].r=1.0f;
		                                                  Col[i].g=1.0f;
														  Col[i].b=1.0f;
														  Col[i].a=1.0f;

		                                                  if ((i>>6)&1)  { Streaks[i].r=1.0f;
																		   Streaks[i].g=1.0f;
																		   Streaks[i].b=1.0f;
																		   Streaks[i].a=1.0f;
																		 }
														  else			 { Streaks[i].r=0.0f;
																		   Streaks[i].g=0.0f;
																		   Streaks[i].b=0.0f;
																		   Streaks[i].a=0.0f;
																		 }

														  Shape[i]=1.0f;
														}
										}

		BOOL	PreProcess(int,int,float,float,float,float,float,float,float,float,float,float);
	  	BOOL	PreProcess(int xres, int yres, float xpos, float ypos, float CameraDepth,float Time,float Depth)
					{ return PreProcess(xres,yres,xpos,ypos,CameraDepth,1.0f,1.0f,1.0f,1.0f,Time,Depth,1.0f);
					}

		void	SimplePreProcess(int,int,float);
		void	SimplePreProcess(int,int,float,float);

		void	CheckColours();

		uchar	GetColour (float &,float &,float &,float &,uchar **,uchar **,uchar **,uchar **,float,float,int,int);
		uchar	GetColour2(float &,float &,float &,float &,uchar **,uchar **,uchar **,uchar **,float,float,int,int);

		char	*GetName() { return Name; }
		void	SetName(char *a) { strcpy(Name,a); }

		void	GetFlare(Flare *,int);
		void	GetFlareAnim(Flare *);

		int		IsVisible(float CameraDepth)
						{ if (Depth>-CameraDepth+1.0E-6f)  return 1;
						  else return 0;
						}

		//*************************************************
		// This is all Rendering Stuff
		private:
			uchar **Img_R,**Img_G,**Img_B,**Img_A;	// Local Copies For Speed !
			uchar **Bak_R,**Bak_G,**Bak_B,**Bak_A;
			uchar **Col_R,**Col_G,**Col_B,**Col_A;

			void ImageOutput(RenderNFO &,uchar,uchar,uchar,uchar,int,int);

			BOOL RenderCDHQArea_Single(RenderNFO &);
			BOOL RenderCDHQArea_NonSingle(RenderNFO &);

		public: 
			BOOL RenderCDHQArea(RenderNFO &);

			void RenderPreview(char *Bitmap,int xres,int yres,int Checker,float Size,float Time,float Sample)
						{	RenderNFO rinfo; rinfo.Init();
							rinfo.xres=xres; rinfo.yres=yres;
							rinfo.CameraDepth=1.0f;
							rinfo.Time=Time;

							Alloc2D(rinfo.Img_R,uchar,yres,xres);
							Alloc2D(rinfo.Img_G,uchar,yres,xres);
							Alloc2D(rinfo.Img_B,uchar,yres,xres);
							Alloc2D(rinfo.Img_A,uchar,yres,xres);

							rinfo.UseSingle=1;
							rinfo.xpos=xres/2; 
							rinfo.ypos=yres/2;
							rinfo.SizeMultiply=Size;
							rinfo.AntiAliasing=Sample;

							int y,x;

							// Allocate Memory For Preview
							if ((Light>5)&&(Light!=14)&&(Light!=15))
										{ Alloc2D(rinfo.Bak_R,uchar,yres,xres);
										  Alloc2D(rinfo.Bak_G,uchar,yres,xres);
										  Alloc2D(rinfo.Bak_B,uchar,yres,xres);
										  Alloc2D(rinfo.Bak_A,uchar,yres,xres);
										}
							else		{ rinfo.Tmg_R=rinfo.Img_R;
							              rinfo.Tmg_G=rinfo.Img_G;
										  rinfo.Tmg_B=rinfo.Img_B;
										  rinfo.Tmg_A=rinfo.Img_A;
										}

							if ((Light==14)||(Light==15))
										{ Alloc2D(rinfo.Tmg_R,uchar,yres,xres);
										  Alloc2D(rinfo.Tmg_G,uchar,yres,xres);
										  Alloc2D(rinfo.Tmg_B,uchar,yres,xres);
										  Alloc2D(rinfo.Tmg_A,uchar,yres,xres);
										}

							// Get BackGround
							for(y=0;y!=yres;y++)
							 for(x=0;x!=xres;x++)
								{ if (Checker)		{ rinfo.Img_R[y][x]=rinfo.Img_G[y][x]=rinfo.Img_B[y][x]=(uchar)(Grid(x,y)*255); 
													  rinfo.Img_A[y][x]=0; 
													}
								  else rinfo.Img_R[y][x]=rinfo.Img_G[y][x]=rinfo.Img_B[y][x]=rinfo.Img_A[y][x]=0; 
								}

							// If Using Transition
							if ((Light==14)||(Light==15))
								{ for(y=0;y!=yres;y++)
								  for(x=0;x!=xres;x++)
								  if (!Grid2(y,x))
										{ int t=(x/8)&1,tt=(y/8)&1;
										
										  if ((t==1)&&(tt==1)) 
															   { rinfo.Tmg_R[y][x]=255;
											                     rinfo.Tmg_G[y][x]=80;
																 rinfo.Tmg_B[y][x]=80;
															   }
										  else if ((t==0)&&(tt==1)) 
															   { rinfo.Tmg_R[y][x]=80;
											                     rinfo.Tmg_G[y][x]=255;
																 rinfo.Tmg_B[y][x]=80;
															   }
										  else if ((t==1)&&(tt==0)) 
															   { rinfo.Tmg_R[y][x]=80;
											                     rinfo.Tmg_G[y][x]=80;
																 rinfo.Tmg_B[y][x]=255;
															   }
										  else if ((t==0)&&(tt==0)) 
															   { rinfo.Tmg_R[y][x]=255;
											                     rinfo.Tmg_G[y][x]=255;
																 rinfo.Tmg_B[y][x]=255;
															   }

										  rinfo.Tmg_A[y][x]=0;
										}
								  else  rinfo.Tmg_R[y][x]=rinfo.Tmg_G[y][x]=rinfo.Tmg_B[y][x]=rinfo.Tmg_A[y][x]=0;
								}

							// Get Preview
							RenderCDHQArea(rinfo);

							// Free Memory
							if ((Light>5)&&(Light!=14)&&(Light!=15))
										 { Free2D(rinfo.Bak_R,lyres);
										   Free2D(rinfo.Bak_G,lyres);
										   Free2D(rinfo.Bak_B,lyres);
										   Free2D(rinfo.Bak_A,lyres);
										 }

							if ((Light==14)||(Light==15))
										{ Free2D(rinfo.Tmg_R,lyres);
										  Free2D(rinfo.Tmg_G,lyres);
										  Free2D(rinfo.Tmg_B,lyres);
										  Free2D(rinfo.Tmg_A,lyres);
										}

							// Copy It Into Char Memory
							uchar *tmp=(uchar *)Bitmap;
							for(y=1;y<=yres;y++)
							for(x=0;x!=xres;x++)
								{ *(tmp++)=rinfo.Img_B[yres-y][x];
								  *(tmp++)=rinfo.Img_G[yres-y][x];
								  *(tmp++)=rinfo.Img_R[yres-y][x];
								}

							// Free Memory
							Free2D(rinfo.Img_R,yres);
							Free2D(rinfo.Img_G,yres);
							Free2D(rinfo.Img_B,yres);
							Free2D(rinfo.Img_A,yres);
						}

		void RenderPreview(char *Bitmap,int xres,int yres,int Checker,float Size,float Sample)
					{ RenderPreview(Bitmap,xres,yres,Checker,Size,-1.0f,Sample);
					}

		// Constructor and Destructor
				Flare()						{ strcpy(Name,"Default Element"); 
				                              InitFlare(0.1f,1.0f); 
				                            }

				Flare(char *a)				{ strcpy(Name,a); InitFlare(0.1f,1.0f); }
				Flare(Flare *A)				{ strcpy(Name,"Default Element"); 
				                              InitFlare(0.1f,1.0f); 
											  GetFlare(A,1);
											}

				Flare(float FS, float PS)	{ strcpy(Name,"Default Element"); InitFlare(FS,PS); }
				Flare(char *a,float FS, float PS) { strcpy(Name,a); InitFlare(FS,PS); }

	  		   ~Flare()						{ FreeFlare(); }

				void	Load(FILE *);
				void	Save(FILE *);
				void	MemLoad(char **);
				int		MemSave(char **);

				//***********************************************************************************************************		  		
				// Real Position Processing
				void RealPosn(int xres,int yres,float xpos,float ypos,float sx,float sy,float CameraDepth)
						{ float ScreenCX,ScreenCY;

					      if (OffScreenRender&2)
								{ ScreenCX=xpos; xpos-=xres;
								  ScreenCY=ypos;
								}
						  else	{ ScreenCX=(0.5f+PositionX*0.5f)*xres;
								  ScreenCY=(0.5f+PositionY*0.5f)*yres;
								}

						  float dx1=xpos-ScreenCX,dy1=ypos-ScreenCY;
					      float dx2=-dy1,dy2=dx1;

						  RenderCX=sx;	
						  RenderCY=sy;

						  if ((dy2*dx1-dx2*dy1==0.0f)||(dy2*dx1-dx2*dy1==0.0f)) return;
						  
						  Position=(-dx2*RenderCY*CameraDepth-dx2*RenderCY*Depth+
									 dx2*ScreenCY*CameraDepth+dx2*ScreenCY*Depth+
								     dy2*RenderCX*CameraDepth+dy2*RenderCX*Depth-
									 dy2*ScreenCX*CameraDepth-dy2*ScreenCX*Depth)/
									 (CameraDepth*(dy2*dx1-dx2*dy1));


						  Angle   =-(-dx1*RenderCY*CameraDepth-dx1*RenderCY*Depth
									 +dx1*ScreenCY*CameraDepth+dx1*ScreenCY*Depth
									 +dy1*RenderCX*CameraDepth+dy1*RenderCX*Depth
									 -dy1*ScreenCX*CameraDepth-dy1*ScreenCX*Depth)/
									 (CameraDepth*(dy2*dx1-dx2*dy1));

						  // Convert To Polar If Need Be
						  if (Polar)		{ float tmpx=Position;
						                      float tmpy=Angle;

											  Angle=(float)atan2(tmpy,tmpx)/LF_PI;
											  Position=(float)sqrt(tmpx*tmpx+tmpy*tmpy);
											}
						}

				void ToPolar(int Use)
						{ float tmpx=Position;
						  float tmpy=Angle;

			              Angle   =(float)atan2(tmpy,tmpx)/3.14159f;
			              Position=(float)sqrt(tmpx*tmpx+tmpy*tmpy);

						  if (Use)	{ for(int i=0;i!=NoPosKeys;i++)
										{ tmpx=PosKeys[i].Position;
										  tmpy=PosKeys[i].Angle;

										  PosKeys[i].Angle   =(float)atan2(tmpy,tmpx)/3.14159f;
										  PosKeys[i].Position=(float)sqrt(tmpx*tmpx+tmpy*tmpy);
										}
									}
						}

				void FromPolar(int Use)
						{ float tmpx=Position;
						  float tmpy=Angle;

					      Position=tmpx*(float)cos(tmpy*3.14159f);
						  Angle   =tmpx*(float)sin(tmpy*3.14159f); 

						  if (Use)	{ for(int i=0;i!=NoPosKeys;i++)
											{ tmpx=PosKeys[i].Position;
											  tmpy=PosKeys[i].Angle;

											  PosKeys[i].Position=tmpx*(float)cos(tmpy*3.14159f);
											  PosKeys[i].Angle   =tmpx*(float)sin(tmpy*3.14159f); 
											}
									}
						}

				void ToAbsolute(int xres,int yres,float xpos,float ypos,float CameraDepth,float Time,int Use,int Old,int New)
						{ OffScreenRender=(Old&1)?Old:Old+1; 
				          PreProcess(xres,yres,xpos,ypos,CameraDepth,1.0f,1.0f,1.0f,1.0f,Time,1.0f,1.0f);
						  OffScreenRender=New;
				          RealPosn(xres,yres,xpos,ypos,RenderCX,RenderCY,CameraDepth);

						  if (Use)	{ for(int i=0;i!=NoPosKeys;i++)
											{	GetPosValues(i);
												OffScreenRender=(Old&1)?Old:Old+1; 
												PreProcess(xres,yres,xpos,ypos,CameraDepth,1.0f,1.0f,1.0f,1.0f,Time,1.0f,1.0f);
												OffScreenRender=New;
												RealPosn(xres,yres,xpos,ypos,RenderCX,RenderCY,CameraDepth);
												PutPosValues(i);
											}
									  GetPosition(Time);
									}
						}

	};	

#endif