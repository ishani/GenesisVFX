#ifndef _FLSCLASS_
#define _FLSCLASS_

#include <stdio.h>
#include "FileBin.h"

#define MAXFLARES		4096

class LensFlares
	{ private:
			// Flare Name
			char Name[256];

			// Flare Storage
			int NoFlares;
			Flare *Flares[MAXFLARES];
			
			void SwapFlares(Flare *&F1,Flare *&F2) { Flare *FT=F2; F2=F1; F1=FT; }
			void Swapchar(char &a,char &b) { char c=a; a=b; b=c; }

	  public:
			// Rendering Modes Stuff
			int ZSort;
			float TimePerFrame;

			void Reset(void);
//********************** 12.4.97 *****************************
			void DeleteAll(void) { for(int i=0;i!=NoFlares;i++) { delete Flares[i]; Flares[i]=NULL; } }
//************************************************************

		     LensFlares(void)			{ Reset(); TimePerFrame=1.0f/15.0f; }
			~LensFlares(void)			{ DeleteAll(); }

			BOOL RenderHQArea(RenderNFO &);

			char *GetName     (void)	{ return Name; }
			char *GetName	  (int i)	{ return Flares[i]->GetName(); }
			int  GetNoFlares  (void)	{ return NoFlares; }
			Flare *GetFlare	  (int i)   { return Flares[i]; }

			int MoveUp		  (int i)	{ if (i>0) { SwapFlares(Flares[i],Flares[i-1]); return -1; }
			                              else return 0;
										}

			void MoveUp		  (int i,char *L)	
										{ if (i>0) { MoveUp(i); Swapchar(L[i],L[i-1]); }
										}

			int MoveDown	  (int i)	{ if (i<NoFlares-1) { SwapFlares(Flares[i],Flares[i+1]); return 1; }
			                              else return 0;
										}

			void MoveDown	  (int i,char *L)	
										{ if (i<NoFlares-1) { MoveDown(i); Swapchar(L[i],L[i+1]); }
										}

			void Delete		  (int i)	{ delete Flares[i];

			                              for(int j=i;j<MAXFLARES-1;j++) Flares[j]=Flares[j+1];
										  NoFlares--;
										}

			void Delete		  (char *L)	{ int n=NoFlares;
			                              
			                              for(int i=0;i!=NoFlares;i++)
											{ if (L[i]) { delete Flares[i]; n--; }
											}
				
										  int j=0;
			                              for(i=0;i!=NoFlares;i++)
										  if (L[i]==0) { Flares[j]=Flares[i]; j++; }

										  NoFlares=n;
										}

			void Copy		  (int i);
			void New		  (void);

			int MaxKeyValue(void)			{ int Max=0;
			                                  for(int i=0;i!=NoFlares;i++)
													{ if (Flares[i]->PosKeys[Flares[i]->NoPosKeys-1].Time>Max) Max=Flares[i]->PosKeys[Flares[i]->NoPosKeys-1].Time;
											          if (Flares[i]->ShpKeys[Flares[i]->NoShpKeys-1].Time>Max) Max=Flares[i]->ShpKeys[Flares[i]->NoShpKeys-1].Time;
													}

											  return Max;
											}

			void GetFrame(int);
			void GetFrame(float);

			void Load			(FILE *FP)	{ int i;
											  for(i=0;i!=NoFlares;i++) delete Flares[i];
											  
											  _FileReadS(FP,Name,256);
											  _FileReadI(FP,&NoFlares);
											  _FileReadI(FP,&ZSort);

											  for(i=0;i!=NoFlares;i++)
												{ Flares[i]=new Flare("Loading !"); 
												  Flares[i]->Load(FP);
												}
											}
							
			void Save			(FILE *FP)	{ 
				                              _FileWriteS(FP,Name,256);
											  _FileWriteI(FP,NoFlares);
											  _FileWriteI(FP,ZSort);

											  for(int i=0;i!=NoFlares;i++)
													Flares[i]->Save(FP);
											}

			void MemLoad	(char **FP)		{ char MemMode[10];
			                                  _MemReadS(FP,MemMode,10);

											  if (!strncmp(MemMode,"GFX002",6))
													{ int i;
													  for(i=0;i!=NoFlares;i++) delete Flares[i];
											  
													  _MemReadS(FP,Name,256);
													  _MemReadI(FP,&NoFlares);
													  _MemReadI(FP,&ZSort);

													  for(i=0;i!=NoFlares;i++)
															{ Flares[i]=new Flare("Loading !"); 
															  Flares[i]->MemLoad(FP);
															}
													}
											}
							
			int MemSave		(char **FP)		{ int Ret=0;
											  char MemMode[10]="GFX002";
                                              Ret+=_MemWriteS(FP,MemMode,10);
				                              Ret+=_MemWriteS(FP,Name,256);
											  Ret+=_MemWriteI(FP,NoFlares);
											  Ret+=_MemWriteI(FP,ZSort);

											  for(int i=0;i!=NoFlares;i++)
													Ret+=Flares[i]->MemSave(FP);

											  return Ret;
											}
	};	

#endif _FLSCLASS_
