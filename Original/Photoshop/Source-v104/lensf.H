#ifndef _LFCLASS_
#define _LFCLASS_

// Anti Cracking #define ! :)
#define PutImage CleanupMemory

//###################################################################################
// Are We Using Registered Version
#define _REGISTERED TRUE
//###################################################################################

//-----------------------------------------------------------------------------
//-- Lens Flare Definitions ---------------------------------------------------
//

#include "version.h"
#include <time.h>

extern HINSTANCE hInst;

extern char InstallLoc[MAX_PATH+10];
#define InstallDir (InstallLoc+10)

extern char Serialise[32];
extern char genesisDir[1024];



void SetBit(int &V,int n,int bit);

class UInterface { 
		public:
			float MinY,MaxY;
			float Falloff, NFalloff;
			float Falloff2, NFalloff2;
			float Intensity,NIntensity;
			float Location,NLocation;
			int FunctionCurve;
			int EditMode;
			int Repetitions;
			int FalloffLock;

			BMM_Color_64 Colour1;
			BMM_Color_64 Colour2;

			float Value1,Value2;

			float    *Curve;
			fColour  *fCurve;

			int EditColour;
			int Seed;
			int Wrap;

		private:

			float drand48() { return (float)rand()*(1.0f/(float)RAND_MAX); }
			float sqr(float x) { return x*x; }

			float Gaussian   (float x, float FO, float FO2) { return (float)exp(-sqr((x>0.0f)?FO:FO2)*x*x); }
			float Exponential(float x, float FO, float FO2) { FO=(float)fabs(FO); FO2=(float)fabs(FO2); return (float)exp(-((x>0.0f)?FO:FO2)*fabs(x)); }
			float TanH       (float x, float FO, float FO2) { return 0.5f+0.5f*(float)tanh(((x>0.0f)?FO:FO2)*x); }
			float Linear	 (float x, float FO, float FO2) { float tmp=1.0f-((x>0.0f)?FO:FO2)*(float)fabs(x); return (tmp<0.0f)?0.0f:tmp; }
			float Cosine	 (float x, float FO, float FO2) { return 0.5f*(1.0f+(float)cos(((x>0.0f)?FO:FO2)*x*3.14159265359f)); }

			void FractalNoiseRecurse(float *Data,int Start,int End,float Add,float Mult)
				{ int Pos=(Start+End)>>1;
			      
			      Data[Pos]=0.5f*(Data[Start]+Data[End])+(1.0f-2.0f*drand48())*Add;

				  if (Pos-Start>1) FractalNoiseRecurse(Data,Start,Pos,Add*Mult,Mult);
				  if (End-Pos>1)   FractalNoiseRecurse(Data,Pos,End,Add*Mult,Mult);
				}

			void FractalNoiseNoLoop(float *Data,float Mult)
				{ Data[0]=drand48();
				  Data[FLARERESOLUTION-1]=drand48();

				  FractalNoiseRecurse(Data,0,FLARERESOLUTION-1,Mult,Mult);

				  float Min=Data[0];
				  float Max=Data[0];

				  for(int i=1;i!=FLARERESOLUTION;i++)
							{      if (Data[i]>Max) Max=Data[i];
				              else if (Data[i]<Min) Min=Data[i];
							}

				  float tmp=1.0f/(Max-Min);
				  for(i=0;i!=FLARERESOLUTION;i++) Data[i]=tmp*(Data[i]-Min);
				}

			void FractalNoiseLoop(float *Data,float Mult)
				{ Data[FLARERESOLUTION-1]=Data[0]=drand48();

				  FractalNoiseRecurse(Data,0,FLARERESOLUTION-1,Mult,Mult);

				  float Min=Data[0];
				  float Max=Data[0];

				  for(int i=1;i!=FLARERESOLUTION;i++)
							{      if (Data[i]>Max) Max=Data[i];
				              else if (Data[i]<Min) Min=Data[i];
							}

				  float tmp=1.0f/(Max-Min);
				  for(i=0;i!=FLARERESOLUTION;i++) Data[i]=tmp*(Data[i]-Min);
				}

			void MakeCurve(float *MakeCurve,float Inten,float FO,float FO2,float Move)
				{ int i;

				  float X=-Move,dX=1.0f/(float)FLARERESOLUTION;

				  switch(FunctionCurve)
				    { case 1: { for(i=0;i!=FLARERESOLUTION;i++,X+=dX) 
									MakeCurve[i]=Gaussian(X,FO,FO2)*Inten;
								break;
							  }

                      case 2: { for(i=0;i!=FLARERESOLUTION;i++,X+=dX) 
				                      MakeCurve[i]=Exponential(X,FO,FO2)*Inten;
								break;
							  }

					  case 3: { for(i=0;i!=FLARERESOLUTION;i++,X+=dX) 
				                      MakeCurve[i]=TanH(X,FO,FO2)*Inten;
								break;
							  }

					  case 4: { for(i=0;i!=FLARERESOLUTION;i++,X+=dX) 
				                      MakeCurve[i]=Linear(X,FO,FO2)*Inten;
								break;
				              }

					  case 5: { for(i=0;i!=FLARERESOLUTION;i++,X+=dX) 
				                      MakeCurve[i]=Cosine(X,FO,FO2)*Inten;
								break;
				              }

					  case 6: { float TmpNoiseData[FLARERESOLUTION];
						        if (Wrap) FractalNoiseLoop(TmpNoiseData,FO*0.01f);
								else FractalNoiseNoLoop(TmpNoiseData,FO*0.01f);
						  
						        for(i=0;i!=FLARERESOLUTION;i++,X+=dX) 
				                      MakeCurve[i]=TmpNoiseData[i]*Inten;
								break;
							  }
				    }

				  //Normalise
				  float tmpmult=1.0f/(MaxY-MinY);
				  for(i=0;i!=FLARERESOLUTION;i++) 
						{ MakeCurve[i]=tmpmult*(MakeCurve[i]-MinY);
				          if (MakeCurve[i]<0.0f) MakeCurve[i]=0.0f;
						  else if (MakeCurve[i]>1.0f) MakeCurve[i]=1.0f;
						}
				  }

			void SingleCurve(float Move, float Inten, float FO, float FO2)
				{ float TempCurve[FLARERESOLUTION];
				  MakeCurve(TempCurve,Inten,FO,FO2,(Wrap)?0.5f:Move);

				  if (!Wrap)
						  for(int i=0;i!=FLARERESOLUTION;i++)
							Curve[i]=(1.0f-TempCurve[i])*Curve[i]+TempCurve[i];

				  else	  for(int i=0;i!=FLARERESOLUTION;i++)
							{ int MI=(int)((float)(-(Move-0.5)*(float)FLARERESOLUTION));
				              int ti=i+MI;
				              
				              while(ti<0) ti+=FLARERESOLUTION;
							  ti=ti%FLARERESOLUTION;

							  Curve[i]=(1.0f-TempCurve[ti])*Curve[i]+TempCurve[ti];
							}
				}

			void SingleCurve(float Move, float Inten, float FO, float FO2, float r, float g, float b)
				{ float TempCurve[FLARERESOLUTION];
			      MakeCurve(TempCurve,Inten,FO,FO2,(Wrap)?0.5f:Move);
				  
				  if (!Wrap)
						  for(int i=0;i!=FLARERESOLUTION;i++)
							{ fCurve[i].r=(1.0f-TempCurve[i])*fCurve[i].r+TempCurve[i]*r;
							  fCurve[i].g=(1.0f-TempCurve[i])*fCurve[i].g+TempCurve[i]*g;
							  fCurve[i].b=(1.0f-TempCurve[i])*fCurve[i].b+TempCurve[i]*b;	
							  Curve[i]=(1.0f-TempCurve[i])*Curve[i]+TempCurve[i];
							}
				  else	  for(int i=0;i!=FLARERESOLUTION;i++)
							{ int MI=(int)((float)(-(Move-0.5)*(float)FLARERESOLUTION));
							  int ti=i+MI;
				              
				              while(ti<0) ti+=FLARERESOLUTION;
							  ti=ti%FLARERESOLUTION;

							  fCurve[i].r=(1.0f-TempCurve[ti])*fCurve[i].r+TempCurve[ti]*r;
							  fCurve[i].g=(1.0f-TempCurve[ti])*fCurve[i].g+TempCurve[ti]*g;
							  fCurve[i].b=(1.0f-TempCurve[ti])*fCurve[i].b+TempCurve[ti]*b;	
							  Curve[i]=(1.0f-TempCurve[ti])*Curve[i]+TempCurve[ti];
							}
				}
         
		public:

			 void Reset() { MinY=0.0f; MaxY=1.0f;
							Falloff=6.0f; NFalloff=0.0f; Falloff2=6.0f; NFalloff2=0.0f;
							FalloffLock=1;Intensity=1.0f; NIntensity=0.0f; Location=0.5f;  NLocation=0.0f;
							Repetitions=1;	EditMode=0; FunctionCurve=1; 
							Seed=0; Colour1.r=65535; Colour1.g=65535; Colour1.b=65535;
							Colour2.r=0; Colour2.g=0; Colour2.b=0;
							Value1=1.0f;
							Value2=0.0f;

							// Wrap Filled in By Holder
							// EditColour Filled in By Holder

							for(int i=0;i!=FLARERESOLUTION;i++)
									{ Curve[i]=0.0f;
							          fCurve[i].SetColourRGBA(0.0f,0.0f,0.0f,0.0f);
									}
						  }

			 UInterface() { Curve		=new float[FLARERESOLUTION];
							fCurve		=new fColour[FLARERESOLUTION];

							Reset();
						  }
			~UInterface() { delete Curve;
							delete fCurve;
			              }

			void ComputeCurve(void)
				{ int i;

			      srand(Seed);

				  for(i=0;i!=FLARERESOLUTION;i++) Curve[i]=0.0f;
				  for(i=0;i!=FLARERESOLUTION;i++) fCurve[i].r=fCurve[i].g=fCurve[i].b=0.0f;
						
				  float L=Location,dL=1.0f/(float)Repetitions;
				  for(i=0;i!=Repetitions;i++)
						{ float TmpL=L; L+=dL;
					      if (i!=0) { TmpL+=NLocation*(2.0f*drand48()-1.0f); 
						              
						              //while(TmpL>1.0f) TmpL-=1.0f;
									  //while(TmpL<0.0f) TmpL+=1.0f;
						            }

						  float TmpI=Intensity; 
					      if (i!=0) TmpI+=NIntensity*(2.0f*drand48()-1.0f);
						  
						  float TmpF=Falloff; 
					      if (i!=0) TmpF+=NFalloff*(2.0f*drand48()-1.0f);
						  
						  float TmpF2=Falloff2; 
					      if (i!=0) TmpF2+=NFalloff2*(2.0f*drand48()-1.0f);

						  float tmp=(i==0)?0.0f:drand48();
						  if (EditColour) { float r=((float)Colour1.r*(1.0f-tmp)+(float)Colour2.r*tmp)/65535.0f;
											float g=((float)Colour1.g*(1.0f-tmp)+(float)Colour2.g*tmp)/65535.0f;
											float b=((float)Colour1.b*(1.0f-tmp)+(float)Colour2.b*tmp)/65535.0f;
											SingleCurve(TmpL,TmpI,TmpF,TmpF2,r,g,b);
										  }
						  else			  { float r=Value1*(1.0f-tmp)+Value2*tmp;
											float g=Value1*(1.0f-tmp)+Value2*tmp;
											float b=Value1*(1.0f-tmp)+Value2*tmp;
											SingleCurve(TmpL,TmpI,TmpF,TmpF2,r,g,b);
										  }
						}
				}

			void ComputeCurve(Flare *In,int Section)
				{ int i;

			      srand(Seed);

				  for(i=0;i!=FLARERESOLUTION;i++) 
						{ Curve[i]=0.0f;

					           if (Section==0) { In->GetRadialColour (fCurve[i],i);
							                     if (!EditColour) fCurve[i].r=fCurve[i].a;
											   }
				          else if (Section==1) { In->GetAngularColour(fCurve[i],i);
						                         if (!EditColour) fCurve[i].r=fCurve[i].a;
											   }
						  else if (Section==2) { float tmp;
						                         In->GetShape(tmp,i);
												 fCurve[i].r=tmp;
												 fCurve[i].g=tmp;
												 fCurve[i].b=tmp;
						                       }
						  else if (Section==3) { float tmp;
						                         In->GetShape2(tmp,i);
												 fCurve[i].r=tmp;
												 fCurve[i].g=tmp;
												 fCurve[i].b=tmp;
						                       }
						}
						
				  float L=Location,dL=1.0f/(float)Repetitions;
				  for(i=0;i!=Repetitions;i++)
						{ float TmpL=L; L+=dL;
					      if (i!=0) { TmpL+=NLocation*(2.0f*drand48()-1.0f); 
						              
						              //while(TmpL>1.0f) TmpL-=1.0f;
									  //while(TmpL<0.0f) TmpL+=1.0f;
						            }

						  float TmpI=Intensity; 
					      if (i!=0) TmpI+=NIntensity*(2.0f*drand48()-1.0f);
						  
						  float TmpF=Falloff; 
					      if (i!=0) TmpF+=NFalloff*(2.0f*drand48()-1.0f);
						  
						  float TmpF2=Falloff2; 
					      if (i!=0) TmpF2+=NFalloff2*(2.0f*drand48()-1.0f);

						  float tmp=(i==0)?0.0f:drand48();
						  if (EditColour) { float r=((float)Colour1.r*(1.0f-tmp)+(float)Colour2.r*tmp)/65535.0f;
											float g=((float)Colour1.g*(1.0f-tmp)+(float)Colour2.g*tmp)/65535.0f;
											float b=((float)Colour1.b*(1.0f-tmp)+(float)Colour2.b*tmp)/65535.0f;
											SingleCurve(TmpL,TmpI,TmpF,TmpF2,r,g,b);
										  }
						  else			  { float r=Value1*(1.0f-tmp)+Value2*tmp;
											float g=Value1*(1.0f-tmp)+Value2*tmp;
											float b=Value1*(1.0f-tmp)+Value2*tmp;
							                SingleCurve(TmpL,TmpI,TmpF,TmpF2,r,g,b);
										  }
						}

				  for(i=0;i!=FLARERESOLUTION;i++) 
						{	   if (Section==0) { if (EditColour) { In->PutRadialColourR(fCurve[i].r,i);
				                                                   In->PutRadialColourG(fCurve[i].g,i);
																   In->PutRadialColourB(fCurve[i].b,i);
				                                                 }
											     else              In->PutRadialColourA(fCurve[i].r,i);
											   }
				          else if (Section==1) { if (EditColour) { In->PutAngularColourR(fCurve[i].r,i);
				                                                   In->PutAngularColourG(fCurve[i].g,i);
																   In->PutAngularColourB(fCurve[i].b,i);
				                                                 }
											     else              In->PutAngularColourA(fCurve[i].r,i);
											   }
						  else if (Section==2) In->PutShape(fCurve[i].r,i);
						  else if (Section==3) In->PutShape2(fCurve[i].r,i);
						}
				}
};

typedef struct tmp_LensF {  DWORD			 version;

							int				 EditPage;
							UInterface		 RadialRGB,RadialAlpha;
							UInterface		 AngularRGB,AngularAlpha;
							UInterface		 Shape2;
							UInterface		 Shape;
							UInterface		*Active;
							UInterface		*Link;

							int				 RealTimePreview;

							int				 DrawChecker;
							int				 ActivePreview;

							int				 EditActivePreview;
							int				 BuildActivePreview;
							int				 InfernoActivePreview;

							int				 RadialLock;
							int				 AngularLock;

							int				 BuildColour;
							int				 BuildShape;

							float			 BuildWidth;
							float			 BuildTransparency;
							int				 BuildNoStreaks;
							int				 BuildNoSides;
							int				 BuildSeed;
							float			 BuildFactor;
							float			 BuildAspect;
							BMM_Color_64	 BuildCol;

							LensFlares		 LensFlare;

							int				 MorphNumber;
							int				 MorphSeed;
							int				 MorphSelectFrom,MorphSelectTo;
							float			 MorphSizeVariation;
							float			 MorphLightVariation;
							float			 MorphDistanceVariation;
							float			 MorphAngleOffsetVariation;
							float			 MorphAngleVariation;
							float			 MorphXPositionVariation;
							float			 MorphYPositionVariation;
							float			 MorphColouriseVariation;
							float			 MorphDepthVariation;
							float			 MorphOcclusionVariation;
							int				 MorphInterplay;
							char			 MorphName[256];

							char			 SelectedElement[MAXFLARES];

							int				AnimationMax;
							int				AnimationPosition;

							int				AnimationSelect1;
							int				AnimationSelect2;
							int				AnimationSelectedFlag;

							//*****************************************************
							// Photo Stuff For  Rendering

							int				RealTime;
							float			ImageScale;
							int				SourceMode;
							RGBColour		Col1;
							RGBColour		Col2;
							float			Tolerance;

							int				ImageWidth,ImageHeight;
							unsigned char*	in;
							int				inlinelen;
							int				inpixelsize;
							unsigned char*	out;
							int				outlinelen;
							int				outpixelsize;
							MaskData*		maskdata;

							int				ScrollingX;
							int				ScrollingY;
							int				ScrollX;
							int				ScrollY;

							// Max render compatibility
							int				AntiAliasing;
							int				Single;
							int				CSE;
							float			ExternalSmoothness;
							float			InternalSmoothness;
							float			ColouriseSmoothness;
							int				Colourise;
							int				MaskInside;
							int				ClipMode;
							float			PosX;
							float			PosY;

							float			CameraDepth;

							// complain about non-editable transparency
							int				complain;

} LensFDATA;

class ImageFilter_LensF {
		public:
            LensFDATA data;
			uchar	**Img_R, **Img_G, **Img_B, **Img_A;
			BYTE	*mbuffer;

        private:
			RenderNFO	rinfo;

			int		xres, yres;


			float	*zbuffer;
			WORD	*obuffer;

			float	drand48()						{ return (float)rand()/(float)RAND_MAX; }
			void	srand48(unsigned int Seed)		{ srand(Seed); }

			float	Distrib(float Var)				{ float tmp=drand48();
				                                      float Val=(1.0f-2.0f*tmp+tmp*tmp)*Var;
													  if (drand48()>0.5f) return -Val;
													  else return Val;
													}

			void	 fSwap(float &a1,float &a2)		{ float tmp=a2; a2=a1; a1=tmp; }
			void	fCSwap(fColour &a1,fColour &a2)	{ fSwap(a1.r,a2.r); fSwap(a1.g,a2.g); 
			                                          fSwap(a1.b,a2.b); fSwap(a1.a,a2.a); 
			                                        }
			void	 iSwap(int &a1,int &a2)			{ int tmp=a2; a2=a1; a1=tmp; }
			void	 WORDSwap(WORD &a1,WORD &a2)	{ WORD tmp=a2; a2=a1; a1=tmp; }

			float	Grid(int x, int y)	{ if ((x%8==0)||(y%8==0)) return 0.4f;
										  else if (((x/8)+(y/8))%2) return 0.2f;
										  else return 0.1f;
										}
			float Sqr(float a) { return a*a; }

		public:

			void	LinearResample(HDC,HBITMAP,int,int,HDC,HBITMAP,int,int);

			// Returns True If A TimeStep Is With Selected Region
			int Valid(int i)  { if ((data.AnimationSelect1==-1)&&(i==data.AnimationPosition)) return 1;
			                    if (data.AnimationSelect1==-1) return 0;
				                if ((i>=data.AnimationSelect1)&&(i<=data.AnimationSelect2)) return 1;
			                    return 0;
							  }

			 ImageFilter_LensF( );
            //~ImageFilter_LensF( ) {}
               
			const TCHAR    *Description			(void);
			const TCHAR    *AuthorName			(void)		{ return _T("_Deadline_ Software");}
			const TCHAR    *CopyrightMessage	(void)		{ return _T("(c)1997 _Deadline_ Software");}
			UINT			Version				(void)		{ return (LensFVERSION); }
			BOOL			LoadConfigure		(void *ptr);
			BOOL			SaveConfigure		(void *ptr);
			DWORD			EvaluateConfigure	(void);
			void			ShowAbout			(HWND);
			BOOL			EditorShowControl	(HWND,Flare *);
			BOOL			BuildShowControl	(HWND,Flare *);
			BOOL			ShowControl			(HWND);
			BOOL			MorphShowControl	(HWND);
			BOOL			Control				(HWND,UINT,WPARAM,LPARAM);
			BOOL			MorphControl		(HWND,UINT,WPARAM,LPARAM);
			BOOL			Master_Control		(HWND,UINT,WPARAM,LPARAM);
			BOOL			Physics_Control		(HWND,UINT,WPARAM,LPARAM);
			BOOL			BuildControl		(HWND,UINT,WPARAM,LPARAM);
			void			BuildPreview		(int);
			void			BuildRenderIntoFlare(void);
			BOOL			Render				(void);
			void			Preview				(int);
			void			RealPreview			(int);
			void			PreviewCurve		(void);
			void			SetControls			(HWND,int);
			void			RenderIntoFlare		(Flare &);
			void			SendTextToListBox	(void);
			void			EditRealTimePreview	(void);
			void			MasterSetValues		(HWND);
			void			MasterPreview		(int);
			void			SmallMasterPreview	(int);
			void			MorphPreview		(int);
			void			BuildUpdateControls	(HWND);
			void			RealTimePreview		(int);
			void			MorphRealTimePreview(int);
			void			Load				(HWND,char* fname=NULL);
			void			Save				(HWND);
			void			LoadElements		(HWND);
			void			SaveElements		(HWND);
			void			MorphCreate			(void);
			void			MorphDelete			(void);
			void			MorphGetValues		(HWND);
			void			FromMorphPreview	(int);
			void			ToMorphPreview		(int);
			int				GetSelected			(void);
			void			SetAnimation		(void);
			void			PutPosKeyFrame		(void);
			void			PutShpKeyFrame		(void);
			void			MegaScrollBar		(void);
			void			MorphMegaScrollBar	(void);
			LRESULT			MasterPreviewProcCallBack(HWND,UINT,WPARAM,LPARAM);
			BOOL			ShowRenderControl	(HWND);							// #### MAX STUFF
			BOOL			Render_Control		(HWND,UINT,WPARAM,LPARAM);		// #### MAX STUFF
			void			Render_DlgSet		(HWND);							// #### MAX STUFF
			void			Render_DlgGet		(void);							// #### MAX STUFF
			BOOL			PhysicsControl		(HWND);
			void			PhysicsComboBox		(void);
			void			MultiplePreview		(HWND &,char **,BITMAPINFO &,Flare &,int);
			void			SetEditPage			(HWND,int);
			void			DrawRenderBoxes		(void);							// #### MAX STUFF

			BOOL			Inferno_Control		(HWND,UINT,WPARAM,LPARAM);		
			BOOL			ShowInfernoControl	(HWND,Flare *);					
			void			InfernoPreview		(int);
			void			Inferno_GetControls	(HWND);
			void			Inferno_SetControls	(HWND);

			void			MasterBuildButtons(void);
			void			MasterFreeButtons(void);
			void			InfernoBuildButtons(void);
			void			InfernoFreeButtons(void);
			void			BuildBuildButtons(void);
			void			BuildFreeButtons(void);
			void			EditBuildButtons(void);
			void			EditFreeButtons(void);
			void			MorphBuildButtons(void);
			void			MorphFreeButtons(void);

			void			MasterReleaseButtons();
			void			BuildReleaseButtons();
			void			EditReleaseButtons();
			void			MorphReleaseButtons();
			void			InfernoReleaseButtons();



			void			SetPhotoInput(MaskData*);
			
			void			SetPosn				(int,int,float,float,float,float);

			int				MemSave(char **FP)	{	int i,Ret=0;

													Ret+=_MemWriteS(FP,genesisDir,1024);
													Ret+=_MemWriteI(FP,data.MorphInterplay);
													Ret+=_MemWriteI(FP,data.AnimationMax);
													Ret+=_MemWriteI(FP,data.AnimationPosition);
													Ret+=_MemWriteI(FP,data.RealTimePreview);
													Ret+=_MemWriteI(FP,data.DrawChecker);
													Ret+=_MemWriteI(FP,data.ActivePreview);
													Ret+=_MemWriteI(FP,data.EditActivePreview);
													Ret+=_MemWriteI(FP,data.BuildActivePreview);
													Ret+=_MemWriteI(FP,data.InfernoActivePreview);
													Ret+=_MemWriteI(FP,data.MorphNumber);
													Ret+=_MemWriteI(FP,data.MorphSeed);
													Ret+=_MemWriteF(FP,data.MorphSizeVariation);
													Ret+=_MemWriteF(FP,data.MorphLightVariation);
													Ret+=_MemWriteF(FP,data.MorphDistanceVariation);
													Ret+=_MemWriteF(FP,data.MorphAngleOffsetVariation);
													Ret+=_MemWriteF(FP,data.MorphAngleVariation);
													Ret+=_MemWriteF(FP,data.MorphXPositionVariation);
													Ret+=_MemWriteF(FP,data.MorphYPositionVariation);
													Ret+=_MemWriteF(FP,data.MorphColouriseVariation);
													Ret+=_MemWriteF(FP,data.MorphDepthVariation);
													Ret+=_MemWriteF(FP,data.MorphOcclusionVariation);

													Ret+=_MemWriteI(FP,data.RealTime);
													Ret+=_MemWriteF(FP,data.ImageScale);
													Ret+=_MemWriteI(FP,data.SourceMode);
													Ret+=_MemWriteC(FP,data.Col1.r);
													Ret+=_MemWriteC(FP,data.Col1.g);
													Ret+=_MemWriteC(FP,data.Col1.b);
													Ret+=_MemWriteC(FP,data.Col2.r);
													Ret+=_MemWriteC(FP,data.Col2.g);
													Ret+=_MemWriteC(FP,data.Col2.b);

													Ret+=_MemWriteI(FP,data.ScrollingX);
													Ret+=_MemWriteI(FP,data.ScrollingY);

													Ret+=_MemWriteI(FP,data.ScrollX);
													Ret+=_MemWriteI(FP,data.ScrollY);

													Ret+=_MemWriteF(FP,data.PosX);
													Ret+=_MemWriteF(FP,data.PosY);

													Ret+=_MemWriteI(FP,data.AntiAliasing);
													Ret+=_MemWriteI(FP,data.Single);
													Ret+=_MemWriteI(FP,data.CSE);
													Ret+=_MemWriteI(FP,data.MaskInside);
													Ret+=_MemWriteF(FP,data.CameraDepth);
													Ret+=_MemWriteI(FP,data.complain);

													Ret+=_MemWriteF(FP,data.ExternalSmoothness);
													Ret+=_MemWriteF(FP,data.InternalSmoothness);

													Ret+=_MemWriteI(FP,data.Colourise);
													Ret+=_MemWriteF(FP,data.ColouriseSmoothness);

													Ret+=data.LensFlare.MemSave(FP);

													for(i=0;i!=data.LensFlare.GetNoFlares();i++)
															Ret+=_MemWriteC(FP,data.SelectedElement[i]);

													return Ret;
												}

			void			MemLoad(char **FP)	{   int i;

													_MemReadS(FP,genesisDir,1024);
													_MemReadI(FP,&data.MorphInterplay);
													_MemReadI(FP,&data.AnimationMax);
													_MemReadI(FP,&data.AnimationPosition);
													_MemReadI(FP,&data.RealTimePreview);
													_MemReadI(FP,&data.DrawChecker);
													_MemReadI(FP,&data.ActivePreview);
													_MemReadI(FP,&data.EditActivePreview);
													_MemReadI(FP,&data.BuildActivePreview);
													_MemReadI(FP,&data.InfernoActivePreview);
													_MemReadI(FP,&data.MorphNumber);
													_MemReadI(FP,&data.MorphSeed);
													_MemReadF(FP,&data.MorphSizeVariation);
													_MemReadF(FP,&data.MorphLightVariation);
													_MemReadF(FP,&data.MorphDistanceVariation);
													_MemReadF(FP,&data.MorphAngleOffsetVariation);
													_MemReadF(FP,&data.MorphAngleVariation);
													_MemReadF(FP,&data.MorphXPositionVariation);
													_MemReadF(FP,&data.MorphYPositionVariation);
													_MemReadF(FP,&data.MorphColouriseVariation);
													_MemReadF(FP,&data.MorphDepthVariation);
													_MemReadF(FP,&data.MorphOcclusionVariation);

													_MemReadI(FP,&data.RealTime);
													_MemReadF(FP,&data.ImageScale);
													_MemReadI(FP,&data.SourceMode);
													_MemReadC(FP,&data.Col1.r);
													_MemReadC(FP,&data.Col1.g);
													_MemReadC(FP,&data.Col1.b);
													_MemReadC(FP,&data.Col2.r);
													_MemReadC(FP,&data.Col2.g);
													_MemReadC(FP,&data.Col2.b);

													_MemReadI(FP,&data.ScrollingX);
													_MemReadI(FP,&data.ScrollingY);

													_MemReadI(FP,&data.ScrollX);
													_MemReadI(FP,&data.ScrollY);
													_MemReadF(FP,&data.PosX);
													_MemReadF(FP,&data.PosY);

													_MemReadI(FP,&data.AntiAliasing);
													_MemReadI(FP,&data.Single);
													_MemReadI(FP,&data.CSE);
													_MemReadI(FP,&data.MaskInside);
													_MemReadF(FP,&data.CameraDepth);
													_MemReadI(FP,&data.complain);

													_MemReadF(FP,&data.ExternalSmoothness);
													_MemReadF(FP,&data.InternalSmoothness);

													_MemReadI(FP,&data.Colourise);
													_MemReadF(FP,&data.ColouriseSmoothness);

													data.LensFlare.MemLoad(FP);

													for(i=0;i!=data.LensFlare.GetNoFlares();i++)
															_MemReadC(FP,&data.SelectedElement[i]);
												}

			// rendering
			unsigned char	RedPixel(int,int);	
			unsigned char	GreenPixel(int,int);
			unsigned char	BluePixel(int,int);
			unsigned char	AlphaPixel(int,int);
			void			OutputAlphaPixel(int,int,unsigned char);
			void			GetImage(void);
			void			PutSquare(int,int);
			void			PutImage(void);
			BOOL			Delauney(float**,float**);
			BOOL			QuickBlur(float,float **,float **);
			BOOL			QuickBlur(float,uchar**);
			BOOL			Go();
			void			FloodFill(uchar**,signed short,signed short);
			void			FloodFillXY(uchar**,signed short,signed short,float&,float&);
			BOOL			FillAndRender(uchar**,signed short,signed short);
			BOOL			GoMulti(uchar **);
			void			RenderImage(void);
			void			RenderPreviewToArray(uchar**, uchar**, uchar**, uchar**,int,int,int);
			void			SetupRenderInfo(float);
			unsigned char	MaskVal(int,int);
			void			MakeSourceMask(uchar**,float,int);

			// SERIALISATION
			// INLINE MAKES CRACKING MUCH HARDER
			int				Register(HWND);

			private:		char Serialise2[32];
			public:
			inline unsigned long int Code(void)		
												{
if( _REGISTERED ) {
														unsigned long int Serial;
														sscanf(Serialise+10,"%d",&Serial);

														int hlock=HardwareLockID()+0x1234; 
														int temp2=((LOWORD(hlock)^HIWORD(hlock))<<16)+(Serial^0xabcd); 

														if (temp2==0) temp2=1;

														return temp2;
} else {
														return 0;
}
												}

// Noone will suspect this !
#define encr _MALLOC2_
			inline unsigned long int encr(unsigned long int a)
												{
if( _REGISTERED ) {
													srand(0x1049a9a);
 
													  for(int i=0;i!=100;i++)
													   { a^=rand();
														a^=(rand()<<4);
														a^=(rand()<<8);
														a^=(rand()<<16);
														a^=(rand()<<20);
														a^=(rand()<<24);
													   }
 
													  return a;
} else {
														return 0;
}
												}
			
			inline unsigned long int GetCode()		
												{
if( _REGISTERED ) {
														DWORD Serial;

														HKEY key;
														DWORD size=11;
														DWORD type;
														if( RegOpenKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\FutureFantastic\\GenesisVfX\\1.04",0,KEY_QUERY_VALUE,&key)==ERROR_SUCCESS ) {
															if( RegQueryValueEx(key,"Serial",NULL,&type,(LPBYTE)(Serialise+10),&size)!=ERROR_SUCCESS )
																strncpy(Serialise+10,"1234567890",10);
														} else	{
															strncpy(Serialise+10,"1234567890",10);
															return 0;
														}
														if( atoi(Serialise+10)<40000 ) strncpy(Serialise+10,"1234567890",10);

														size=4;
														if( RegQueryValueEx(key,"Auth",NULL,&type,(LPBYTE)(&Serial),&size)!=ERROR_SUCCESS )
															Serial=0;
														RegCloseKey(key);												

														return Serial;
} else {
														return 0;
}
												}					

			inline void PutCode(char *Code)			
												{
if( _REGISTERED ) {
														HKEY key;

														DWORD MyCode=(DWORD)atoi(Code);
														DWORD status;

														if( RegCreateKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\FutureFantastic\\GenesisVfX\\1.04",0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&key,&status)==ERROR_SUCCESS ) {
															RegSetValueEx(key,"Serial",0,REG_SZ,(LPBYTE)(Serialise+10),strlen(Serialise+10)+1);
															RegSetValueEx(key,"Auth",0,REG_DWORD,(LPBYTE)&MyCode,4);
															RegCloseKey(key);
														}

				}
												}	
};

/////////////////////////////////////////////////////////////////
// Checksum

// steps
// compile the filter
// run ReadFile - copy down the numbers for the defines below and the hex number in the first message box
// put in numbers for defines and recompile
// load filter into developer studio as a binary
// look around the hex address you noted earlier for !"£$ and fill with zeros - keep note of position
// run ReadFile again
// fill in 4 numbers from last message box where the zeros are
// for a quick check run ReadFile - should return zero's
//
// you will need to modify ReadFile program

#define IDSTART 224900
#define IDEND 225168
#define SRSTART 226280
#define SREND 226296

//#define CHECKSUM

inline void DrawDisplay(uchar **ImgR,uchar** ImgG,uchar** ImgB,uchar**ImgA)
{
#ifdef CHECKSUM
	char fname[MAX_PATH];
	unsigned long buffer[25000];
	DWORD i,size,read;
	int j,end;
	unsigned long check=0;
	HANDLE fp;

	if( GetModuleFileName(hInst,fname,MAX_PATH)==0 )
		goto Failed;
	size=time(NULL);
	if( size%8!=0 ) return;
	fp=CreateFile(fname,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if( fp==NULL ) goto Failed;

	size=GetFileSize(fp,NULL);

	for(i=0;i<size;i+=100000) {
		ReadFile(fp,buffer,100000,&read,NULL);
		if( i+100000>size ) end=size%100000;
		else end=100000;
		for(j=0;j<end/4;j++)
			if( (i+j*4<IDSTART || i+j*4>=IDEND) && (i+j*4<SRSTART || i+j*4>=SREND) )
				check+=buffer[j];
	}

Failed:	
	ImgR[0]=(uchar *)((unsigned long)(ImgR[0])*(check-0x431ad7ba));
	CloseHandle(fp);
#endif
}

// end checksum


#endif

//-- EOF: LensF.h ----------------------------------------------------------
