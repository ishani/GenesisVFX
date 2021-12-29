//*****************************************
// Fractal Noise Classes Come From Here
#include "FileBin.h"

class Point3 {

public:
	float x,y,z;

public:
	Point3(float a=0.0f,float b=0.0f,float c=0.0f) { x=a;y=b;z=c; };

	Point3& operator*(float a) const { return Point3(a*x,a*y,a*z); };
};

extern float turbulence(float*,float);
extern float noise3(float*);


//*****************************************
class Fractal
	{ private:
		float Mult,iMult;
		float cx,cy;
		float Time;
		float fSeed;

      public:
        int Seed;

		int		UseSimpleNoise;
		float	SimpleXScale,SimpleYScale;
		float	SimpleXSize,SimpleYSize;
		float	SimplePhaseSpeed;
		int		SimpleLevels;
		int		SimpleSizeLock,SimpleDeformLock;

		int		UseFracNoise;
		float	FracXScale,FracYScale;
		float	FracXSize,FracYSize;
		float	FracPhaseSpeed;
		float	FracFreq;
		int		FracSizeLock,FracDeformLock;

		int RenderNoise;

		void GetFractal(Fractal &a)
					  { Seed=a.Seed;

						UseSimpleNoise=a.UseSimpleNoise;
						SimpleXScale=a.SimpleXScale;
						SimpleYScale=a.SimpleYScale;
						SimpleXSize=a.SimpleXSize;
						SimpleYSize=a.SimpleYSize;
						SimplePhaseSpeed=a.SimplePhaseSpeed;
						SimpleLevels=a.SimpleLevels;
						SimpleSizeLock=a.SimpleSizeLock;
						SimpleDeformLock=a.SimpleDeformLock;

						UseFracNoise=a.UseFracNoise;
						FracXScale=a.FracXScale;
						FracYScale=a.FracYScale;
						FracXSize=a.FracXSize;
						FracYSize=a.FracYSize;
						FracPhaseSpeed=a.FracPhaseSpeed;
						FracFreq=a.FracFreq;
						FracSizeLock=a.FracSizeLock;
						FracDeformLock=a.FracDeformLock;
					  }

		int	 IsUsed() { return (UseSimpleNoise|UseFracNoise); }

		void Initialise(void) 
					  { Seed=0;

						UseSimpleNoise=0;
						SimpleXScale=5.0f; SimpleYScale=5.0f;
						SimpleXSize=1.0f; SimpleYSize=1.0f;
						SimplePhaseSpeed=0.05f; 
						SimpleLevels=1;

						UseFracNoise=0;
						FracXScale=5.0f; FracYScale=5.0f;
						FracXSize=1.0f; FracYSize=1.0f;
						FracPhaseSpeed=0.05f; 
						FracFreq=1.0f;

						SimpleSizeLock=1;
						SimpleDeformLock=1;
						FracSizeLock=1;
						FracDeformLock=1;
					  }

		void Setup(float RenderSize,float cenx,float ceny,float Tm)
					  { cx=cenx;
		                cy=ceny;
						Mult=RenderSize;
						iMult=1.0f/Mult;
						Time=Tm;
						fSeed=Seed*1000.0f;

						RenderNoise=IsUsed();
					  }

		void NoNoise(void) { RenderNoise=0; }

		float Turb(Point3 p) 
					{	float sum = 0.0f;
						float l,f = 1.0f;			

						for(l=(float)SimpleLevels;l>=1.0f;l-=1.0f) 
							{ sum += noise3(&((p*f).x))/f;
							  f*=2.0f;
							}

						if (l>0.0f)	sum+=l*noise3(&((p*f).x))/f;
						
						return sum;
					}

		void Distort(float &xnew,float &ynew,float xin,float yin)
					  { if (RenderNoise)
							  { Point3 p;
		                        float RealX=(xin-cx)*iMult;
								float RealY=(yin-cy)*iMult;

								xnew=ynew=0.0f;

								//*********************************************
								// Standard Fractal Noise
								//*********************************************
								if (UseSimpleNoise)
									{ float Tm=SimplePhaseSpeed*(Time)+fSeed;

									  // X Peturbation
										p.x=RealX*SimpleXScale;
										p.y=RealY*SimpleYScale;
										p.z=Tm;
										xnew+=Turb(p)*SimpleXSize;

									  // Y Peturbation
										p.x+=23.0f; p.y+=6.0f; p.z+=1971.0f;
										ynew+=Turb(p)*SimpleYSize;
									}

								//*********************************************
								// Fractal Noise
								//*********************************************
								if (UseFracNoise)
									{ float Tm=FracPhaseSpeed*(Time)+fSeed;

									  // X PeFracation
										p.x=RealX*FracXScale;
										p.y=RealY*FracYScale;
										p.z=Tm;
										xnew+=(turbulence(&p.x,FracFreq)-0.137f)*FracXSize;
										
									  // Y PeFracation
										p.x+=23.0f; p.y+=6.0f; p.z+=1971.0f;
										ynew+=(turbulence(&p.x,FracFreq)-0.137f)*FracYSize;
									}

								//*********************************************
								// Scale And Prepare Output
								//*********************************************
								xnew=xin+xnew*Mult;
								ynew=yin+ynew*Mult;
							  }
		                else { xnew=xin; ynew=yin; }
					  }

		int MemSave(char **FP){ int Ret=0;
								Ret+=_MemWriteI(FP,Seed);
								Ret+=_MemWriteI(FP,UseSimpleNoise);
								Ret+=_MemWriteF(FP,SimpleXScale);
								Ret+=_MemWriteF(FP,SimpleYScale);
								Ret+=_MemWriteF(FP,SimpleXSize);
								Ret+=_MemWriteF(FP,SimpleYSize);
								Ret+=_MemWriteF(FP,SimplePhaseSpeed);
								Ret+=_MemWriteI(FP,SimpleLevels);
								Ret+=_MemWriteI(FP,SimpleSizeLock);
								Ret+=_MemWriteI(FP,SimpleDeformLock);

								Ret+=_MemWriteI(FP,UseFracNoise);
								Ret+=_MemWriteF(FP,FracXScale);
								Ret+=_MemWriteF(FP,FracYScale);
								Ret+=_MemWriteF(FP,FracXSize);
								Ret+=_MemWriteF(FP,FracYSize);
								Ret+=_MemWriteF(FP,FracPhaseSpeed);
								Ret+=_MemWriteF(FP,FracFreq);
								Ret+=_MemWriteI(FP,FracSizeLock);
								Ret+=_MemWriteI(FP,FracDeformLock);

								return Ret;
							 }

		void MemLoad(char **FP){ _MemReadI(FP,&Seed);
		                         _MemReadI(FP,&UseSimpleNoise);
								 _MemReadF(FP,&SimpleXScale);
								 _MemReadF(FP,&SimpleYScale);
								 _MemReadF(FP,&SimpleXSize);
								 _MemReadF(FP,&SimpleYSize);
								 _MemReadF(FP,&SimplePhaseSpeed);
								 _MemReadI(FP,&SimpleLevels);
								 _MemReadI(FP,&SimpleSizeLock);
								 _MemReadI(FP,&SimpleDeformLock);

								 _MemReadI(FP,&UseFracNoise);
								 _MemReadF(FP,&FracXScale);
								 _MemReadF(FP,&FracYScale);
								 _MemReadF(FP,&FracXSize);
								 _MemReadF(FP,&FracYSize);
								 _MemReadF(FP,&FracPhaseSpeed);
								 _MemReadF(FP,&FracFreq);
								 _MemReadI(FP,&FracSizeLock);
								 _MemReadI(FP,&FracDeformLock);
							 }

		void Save(FILE *FP)  {   _FileWriteI(FP,Seed);
		                         _FileWriteI(FP,UseSimpleNoise);
								 _FileWriteF(FP,SimpleXScale);
								 _FileWriteF(FP,SimpleYScale);
								 _FileWriteF(FP,SimpleXSize);
								 _FileWriteF(FP,SimpleYSize);
								 _FileWriteF(FP,SimplePhaseSpeed);
								 _FileWriteI(FP,SimpleLevels);
								 _FileWriteI(FP,SimpleSizeLock);
								 _FileWriteI(FP,SimpleDeformLock);

								 _FileWriteI(FP,UseFracNoise);
								 _FileWriteF(FP,FracXScale);
								 _FileWriteF(FP,FracYScale);
								 _FileWriteF(FP,FracXSize);
								 _FileWriteF(FP,FracYSize);
								 _FileWriteF(FP,FracPhaseSpeed);
								 _FileWriteF(FP,FracFreq);
								 _FileWriteI(FP,FracSizeLock);
								 _FileWriteI(FP,FracDeformLock);
							 }

		void Load(FILE *FP)  {   _FileReadI(FP,&Seed);
		                         _FileReadI(FP,&UseSimpleNoise);
								 _FileReadF(FP,&SimpleXScale);
								 _FileReadF(FP,&SimpleYScale);
								 _FileReadF(FP,&SimpleXSize);
								 _FileReadF(FP,&SimpleYSize);
								 _FileReadF(FP,&SimplePhaseSpeed);
								 _FileReadI(FP,&SimpleLevels);
								 _FileReadI(FP,&SimpleSizeLock);
								 _FileReadI(FP,&SimpleDeformLock);

								 _FileReadI(FP,&UseFracNoise);
								 _FileReadF(FP,&FracXScale);
								 _FileReadF(FP,&FracYScale);
								 _FileReadF(FP,&FracXSize);
								 _FileReadF(FP,&FracYSize);
								 _FileReadF(FP,&FracPhaseSpeed);
								 _FileReadF(FP,&FracFreq);
								 _FileReadI(FP,&FracSizeLock);
								 _FileReadI(FP,&FracDeformLock);
							 }
	};