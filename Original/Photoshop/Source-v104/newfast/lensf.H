#ifndef _LFCLASS_
#define _LFCLASS_

#define DLLEXPORT __declspec(dllexport)

// Anti Cracking #define ! :)
#define PutImage CleanupMemory

//-----------------------------------------------------------------------------
//-- Lens Flare Definitions ---------------------------------------------------
//

#include "VersionInfo.cpp"

void SetBit(int &V,int n,int bit)
		{ int tmp=1<<(n-1);
	      bit=(bit)?1:0;
          if (V&tmp)	{ if (bit) return;
                          else V-=tmp;
						}
		  else			{ if (bit) V+=tmp;
						}
		}

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
				  for(i=0;i!=FLARERESOLUTION;i++) Data[i]=tmp*(Data[i]-Min);			// 9.04.97
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
				  for(i=0;i!=FLARERESOLUTION;i++) Data[i]=tmp*(Data[i]-Min);		// 9.04.97
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
							// Max Stuff For Tracking Nodes And Rendering
								int			RenderLink;
								NodeDlg		*nodeDlg;
								int			NoNames;
								NodeName	*Names;
								float		RenderRadius;
								int			LinkPoint;

								int			RenderMat;
								int			MaterialID1,MaterialID2;

								int			RenderObj;
								int			ObjectID1,ObjectID2;

								int			RenderCol;
								float		HueStart,HueEnd;
								float		IntStart,IntEnd;
								float		SatStart,SatEnd;
								int			ColObjectID,ColMatID;
								
								int			RenderFxd;
								float		PosX,PosY,PosZ;

								int			AntiAliasing;
								int			Single;
								int			CSE;
								int			OutputOID;
								int			OutputMID;
								int			CompositeZ;
								int			ProcessZ;
								int			MaskInside;

								float		ExternalSmoothness;
								float		InternalSmoothness;

								int			Colourise;
								float		ColouriseSmoothness;

								float		CameraDepth;

								int			Test;

								float		TimeConvert;
} LensFDATA;

class ImageFilter_LensF : public ImageFilter {
		public:
            LensFDATA data;

        private:
			RenderNFO	rinfo;

			int		xres, yres, Modified;
			int		TicksPerFrame;

			int		FieldRender;
			int		StartLine,SkipLine;
			float	MaxTime;

			uchar	**Img_R, **Img_G, **Img_B, **Img_A;
			uchar	**Tmg_R, **Tmg_G, **Tmg_B, **Tmg_A;

			float		*zbuffer;
			BYTE		*mbuffer;
			WORD		*obuffer;
			RealPixel	*rbuffer;

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

			int		Grid2(int x, int y)	{ if ((x%8==0)||(y%8==0)) return 1;
										  else return 0;
										}

			float Sqr(float a) { return a*a; }

			unsigned char	f2i(float a) { return (unsigned char)((float)(a*255.9999f)); }
			float			i2f(unsigned char a) { return ((float)a)*3.92156862745E-3f; }

		public:

			// Returns True If A TimeStep Is With Selected Region
			int Valid(int i)  { if ((data.AnimationSelect1==-1)&&(i==data.AnimationPosition)) return 1;
			                    if (data.AnimationSelect1==-1) return 0;
				                if ((i>=data.AnimationSelect1)&&(i<=data.AnimationSelect2)) return 1;
			                    return 0;
							  }

			 ImageFilter_LensF( );
            ~ImageFilter_LensF( ) { if (data.Names) { free(data.Names); data.Names==NULL; } }
               
			const TCHAR    *Description			(void);
			const TCHAR    *AuthorName			(void)		{ return _T("(c)1997 Andrew Cross");}
			const TCHAR    *CopyrightMessage	(void)		{ return _T("(c)1997 Andrew Cross");}
			UINT			Version				(void)		{ return (LensFVERSION); }
			BOOL			LoadConfigure		(void *ptr);
			BOOL			SaveConfigure		(void *ptr);
			DWORD			EvaluateConfigure	(void);
			DWORD			ChannelsRequired	(void);
			DWORD			Capability			(void)		{ return(IMGFLT_COMPOSITOR|IMGFLT_MASK|IMGFLT_FILTER|IMGFLT_CONTROL); }
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
			BOOL			Render				(HWND);
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
			void			Load				(HWND);
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
			void			Register(HWND);
			
			void			SetPosn				(int,int,float,float,float,float);

			void			MasterFreeButtons(void);
			void			MasterBuildButtons(void);
			void			EditFreeButtons(void);
			void			EditBuildButtons(void);
			void			BuildFreeButtons(void);
			void			BuildBuildButtons(void);
			void			MorphFreeButtons(void);
			void			MorphBuildButtons(void);
			void			InfernoFreeButtons(void);
			void			InfernoBuildButtons(void);
			void			RenderFreeButtons(void);
			void			RenderBuildButtons(void);

			void			ReleaseBuildControls(void);
			void			ReleaseEditControls(void);
			void			ReleaseFlaresEditControls(void);
			void			ReleaseInfernoControls(void);
			void			ReleaseMorphControls(void);
			void			ReleaseRenderControls(void);

			int				AreYouSure(HWND hWnd)
							{ return (MessageBox(hWnd,_T("You are about to perform an operation that will cause you to loose unsaved work. Are you sure that you want to proceed ?"),
							                          _T("WARNING !!!"),MB_ICONEXCLAMATION|MB_YESNO|MB_APPLMODAL|MB_DEFBUTTON2)==IDYES);
							}

			//******************************************************
			// Max Rendering Routines
			//******************************************************
			RenderInfo		*RendInfo;

			void			GetImage();
			void			PutImage();
			BOOL			Delauney(float **,float **);
			BOOL			QuickBlur(float,float **,float **);
			BOOL			QuickBlur(float,uchar **);
			BOOL			QuickBlur(float Size,uchar **,uchar **,uchar **,uchar **);
			BOOL			RenderLinked();
			BOOL			RenderObject();
			BOOL			RenderMaterial();
			BOOL			RenderColour();
			BOOL			RenderFixed();
			BOOL			Go(void);
			BOOL			GoMulti(uchar **);
			void			FloodFill(uchar **,signed short,signed short);
			void			SetupRenderInfo(float);
			BOOL			RenderPosn(float,float,float,float,float,float,float,float,float);
			int				ScanINodes(INode *,int,float *,float *,float *,float *,float *,float *,float *,float *,float *);
			void			LinearResample(HDC,HBITMAP,int,int,HDC,HBITMAP,int,int);

			// For Testing Links
			void			TestImage(void)	{ int x,y;
			                                  for(x=0;x!=xres;x++)
											  for(y=0;y!=yres;y++)
													{ Img_B[y][x]=(int)((Img_R[y][x]+Img_G[y][x]+Img_B[y][x])/3);
													  Img_R[y][x]=0;
													  Img_G[y][x]=0;
													  Img_A[y][x]=0;
													}
											}

			void			TestPixel(int x,int y)	{ if ((x>0)&&(x<xres))
				                                      if ((y>0)&&(y<yres)) { Img_R[y][x]=255;
																			 Img_G[y][x]=255;
																			 Img_A[y][x]=255;
																		   }
													}

			//******************************************************

			int				MemSave(char **FP)	{	int Ret=0;
													char FileMode[10]="GFX101";
													Ret+=_MemWriteS(FP,FileMode,10);

													Ret+=_MemWriteI(FP,data.NoNames);

													for(int i=0;i!=data.NoNames;i++)
														Ret+=_MemWriteS(FP,data.Names[i],sizeof(NodeName));

													Ret+=_MemWriteI(FP,data.MorphInterplay);

													if (Modified) Ret+=_MemWriteI(FP,-data.AnimationMax);
													else Ret+=_MemWriteI(FP,data.AnimationMax);

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

													Ret+=_MemWriteI(FP,data.RenderLink);
													Ret+=_MemWriteI(FP,data.RenderMat);
													Ret+=_MemWriteI(FP,data.RenderObj);
													Ret+=_MemWriteI(FP,data.RenderCol);
													Ret+=_MemWriteI(FP,data.RenderFxd);

													Ret+=_MemWriteI(FP,data.MaterialID1);
													Ret+=_MemWriteI(FP,data.MaterialID2);

													Ret+=_MemWriteI(FP,data.ObjectID1);
													Ret+=_MemWriteI(FP,data.ObjectID2);

													Ret+=_MemWriteF(FP,data.HueStart);
													Ret+=_MemWriteF(FP,data.HueEnd);
													Ret+=_MemWriteF(FP,data.IntStart);
													Ret+=_MemWriteF(FP,data.IntEnd);
													Ret+=_MemWriteF(FP,data.SatStart);
													Ret+=_MemWriteF(FP,data.SatEnd);;
													Ret+=_MemWriteI(FP,data.ColObjectID);
													Ret+=_MemWriteI(FP,data.ColMatID);
													Ret+=_MemWriteF(FP,data.PosX);
													Ret+=_MemWriteF(FP,data.PosY);
													Ret+=_MemWriteF(FP,data.PosZ);

													Ret+=_MemWriteI(FP,data.AntiAliasing);
													Ret+=_MemWriteI(FP,data.Single);
													Ret+=_MemWriteI(FP,data.CSE);
													Ret+=_MemWriteI(FP,data.OutputOID);
													Ret+=_MemWriteI(FP,data.OutputMID);
													Ret+=_MemWriteI(FP,data.CompositeZ);
													Ret+=_MemWriteI(FP,data.ProcessZ);
													Ret+=_MemWriteI(FP,data.MaskInside);
													Ret+=_MemWriteF(FP,data.CameraDepth);

													Ret+=_MemWriteF(FP,data.ExternalSmoothness);
													Ret+=_MemWriteF(FP,data.InternalSmoothness);

													Ret+=_MemWriteI(FP,data.Colourise);
													Ret+=_MemWriteF(FP,data.ColouriseSmoothness);

													Ret+=_MemWriteF(FP,data.RenderRadius);

													Ret+=_MemWriteI(FP,data.Test);
													Ret+=_MemWriteI(FP,data.LinkPoint);

													Ret+=_MemWriteF(FP,data.TimeConvert);

													Ret+=data.LensFlare.MemSave(FP);

													for(i=0;i!=data.LensFlare.GetNoFlares();i++)
															Ret+=_MemWriteC(FP,data.SelectedElement[i]);

													return Ret;
												}

			BOOL			MemLoad(char **FP)	{ char MemMode[10];
												  _MemReadS(FP,MemMode,10);

												  if ((!strncmp(MemMode,"GFX100",6))||(!strncmp(MemMode,"GFX101",6)))
														  { if (data.NoNames) { free(data.Names); data.Names==NULL; }
															_MemReadI(FP,&data.NoNames);

															if (data.NoNames) { data.Names=(NodeName *)malloc(sizeof(NodeName)*data.NoNames); }

															for(int i=0;i!=data.NoNames;i++)
																_MemReadS(FP,data.Names[i],sizeof(NodeName));

															_MemReadI(FP,&data.MorphInterplay);

															_MemReadI(FP,&data.AnimationMax);
															if (data.AnimationMax<0) { Modified=1; data.AnimationMax=-data.AnimationMax; }
															else Modified=0;

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

															_MemReadI(FP,&data.RenderLink);
															_MemReadI(FP,&data.RenderMat);
															_MemReadI(FP,&data.RenderObj);
															_MemReadI(FP,&data.RenderCol);
															_MemReadI(FP,&data.RenderFxd);

															_MemReadI(FP,&data.MaterialID1);
															_MemReadI(FP,&data.MaterialID2);

															_MemReadI(FP,&data.ObjectID1);
															_MemReadI(FP,&data.ObjectID2);

															_MemReadF(FP,&data.HueStart);
															_MemReadF(FP,&data.HueEnd);
															_MemReadF(FP,&data.IntStart);
															_MemReadF(FP,&data.IntEnd);
															_MemReadF(FP,&data.SatStart);
															_MemReadF(FP,&data.SatEnd);;
															_MemReadI(FP,&data.ColObjectID);
															_MemReadI(FP,&data.ColMatID);
															_MemReadF(FP,&data.PosX);
															_MemReadF(FP,&data.PosY);
															_MemReadF(FP,&data.PosZ);

															_MemReadI(FP,&data.AntiAliasing);
															_MemReadI(FP,&data.Single);
															_MemReadI(FP,&data.CSE);
															_MemReadI(FP,&data.OutputOID);
															_MemReadI(FP,&data.OutputMID);
															_MemReadI(FP,&data.CompositeZ);
															_MemReadI(FP,&data.ProcessZ);
															_MemReadI(FP,&data.MaskInside);
															_MemReadF(FP,&data.CameraDepth);

															_MemReadF(FP,&data.ExternalSmoothness);
															_MemReadF(FP,&data.InternalSmoothness);

															_MemReadI(FP,&data.Colourise);
															_MemReadF(FP,&data.ColouriseSmoothness);

															_MemReadF(FP,&data.RenderRadius);

															_MemReadI(FP,&data.Test);
															_MemReadI(FP,&data.LinkPoint);

															_MemReadF(FP,&data.TimeConvert);

															data.LensFlare.MemLoad(FP);

															for(i=0;i!=data.LensFlare.GetNoFlares();i++)
																	_MemReadC(FP,&data.SelectedElement[i]);

															return TRUE;
														  }
												  else return FALSE;
												}

			// SERIALISATION
			// INLINE MAKES CRACKING MUCH HARDER
			private:	char Serialise[32];
						char Serialise2[32];

			inline unsigned long int Code(void)		
												{		unsigned long int Serial;
														sscanf(Serialise+10,"%d",&Serial);

														//**** MAX HardwareCode
															Serial^=HardwareLockID();

														//**** Get System Info
															SYSTEM_INFO Temp;
															GetSystemInfo(&Temp);

															Serial^=((unsigned long int)Temp.dwNumberOfProcessors	+0xabcdef12);
															Serial^=((unsigned long int)Temp.dwProcessorType		+0x01234561);
															
														//**** Get Max Configuration Path
															char filename[256];
															_tcscpy(filename,Max()->GetDir(APP_PLUGCFG_DIR)); filename[3]=0; filename[4]=0;

															unsigned long int Ser;
															GetVolumeInformation(filename,NULL,NULL,&Ser,NULL,NULL,NULL,NULL);
															Serial^=((unsigned long int)Ser+0x15672633);

														if (Serial==0) Serial=1;

														return Serial;
												}

// Noone will suspect this !
#define encr _MALLOC2_
			
		/////////////////////////////////////////////////////////////////
		// Checksum
		// steps :
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

		#define IDSTART			242812
		#define IDEND			243080
		#define SRSTART			248488
		#define SREND			248504

		#undef					CHECKSUM
		#define					REGISTERED

		inline unsigned long int encr(unsigned long int in)
												{		
		#ifdef REGISTERED
														srand(in^0xafbabafa);
					
														for(int i=0;i!=10;i++)
															{ in^=0x12577345; in^=(unsigned long int)rand();
														      in^=(unsigned long int)(rand()<<16);
															}
				
														return in;
		#else											
														return 0;
		#endif
												}
			
		inline unsigned long int GetCode(void)	{		
		#ifdef REGISTERED
														unsigned long int Serial;
														char filename[256];
														sprintf(filename,"%s\\GenMAX.reg",Max()->GetDir(APP_PLUGCFG_DIR)); // Use 8 char filenames

														FILE *FP=fopen(filename,"rb");
														if (FP==NULL) return 0;

														int Succeed=fscanf(FP,"%d",&Serial);
														fclose(FP);

														if (Succeed==0) return 0;

														return Serial;
		#else											
														return 0;
		#endif
												}					

		inline void PutCode(char *Code)			{		
		#ifdef REGISTERED
														char filename[256];
														sprintf(filename,"%s\\GenMAX.reg",Max()->GetDir(APP_PLUGCFG_DIR)); // Use 8 char filenames

														FILE *FP=fopen(filename,"wb");
														if (FP==NULL) return;

														fprintf(FP,"%s\n",Code);
														fclose(FP);
		#endif
												}
};

#define PIRACYCHECK_uchar(a) ((uchar *)((unsigned long)(a)+(PC-0x431ad7ba-1)));
#define PIRACYCHECK_set(a) (a-1+(PC-0x431ad7ba))

inline unsigned long int ChkDisplay(HINSTANCE hInst,int ii)
{	unsigned long check=0;
#ifdef CHECKSUM
	char fname[MAX_PATH];
	unsigned long buffer[25000];
	DWORD i,size,read;
	int j,end;
	HANDLE fp;

	if( GetModuleFileName(hInst,fname,MAX_PATH)==0 ) goto Failed;
	size=time(NULL);
	if ((ii)&&(size%8!=0)) return 0;
	fp=CreateFile(fname,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if( fp==NULL ) goto Failed;

	size=GetFileSize(fp,NULL);

	for(i=0;i<size;i+=100000) {
		ReadFile(fp,buffer,100000,&read,NULL);
		if( i+100000>size ) end=size%100000;
		else end=100000;
		for(j=0;j<end/4;j++)
			if( (i+j*4<IDSTART||i+j*4>=IDEND)&&(i+j*4<SRSTART||i+j*4>=SREND)) check+=buffer[j];
	}

Failed:	
	//ImgR[0]=(uchar *)((unsigned long)(ImgR[0])*(check-0x431ad7ba));
	CloseHandle(fp);

	return check;
#else	
	return 0x431ad7ba+1;
#endif
}
// end checksum

#endif

//-- EOF: LensF.h ----------------------------------------------------------
