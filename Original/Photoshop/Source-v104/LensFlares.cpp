#include <string.h>
#include "CustomControls.h"
#include "PhotoUtil.h"
#include "FColour.h"
#include "Flare.h"
#include "LensFlares.h"
#include "Utils.h"

void LensFlares::Reset()      
	{ NoFlares=1;
	  ZSort=0;

	  Flares[0]=new Flare("Default Element"); 
	  strcpy(Name,"Default Flare");
	}

BOOL LensFlares::RenderHQArea(RenderNFO &rinfo)
	{ int i;
      BOOL abort=FALSE;
      char *Flag=new char[NoFlares];

	  for(i=0;i!=NoFlares;i++) Flag[i]=0;

	  Alloc2D(rinfo.Bak_R,uchar,rinfo.yres,rinfo.xres);
	  Alloc2D(rinfo.Bak_G,uchar,rinfo.yres,rinfo.xres);
	  Alloc2D(rinfo.Bak_B,uchar,rinfo.yres,rinfo.xres);
	  Alloc2D(rinfo.Bak_A,uchar,rinfo.yres,rinfo.xres);
			        
	  for(i=0;((i!=NoFlares)&&(abort==FALSE));i++)
			{ StatusProgress(i,NoFlares-1);
		      
		      float Dst=(float)-1E20;
	          int Draw;
  
			  if (ZSort)	{ for(int j=0;j!=NoFlares;j++)
								if ((!Flag[j])&&(Flares[j]->Depth>=Dst)) { Dst=Flares[j]->Depth; Draw=j; }
							  Flag[Draw]=1;
							}
			  else Draw=i;


			  if (Flares[Draw]->Active) abort=Flares[Draw]->RenderCDHQArea(rinfo);
			}

	  Free2D(rinfo.Bak_R,yres);
	  Free2D(rinfo.Bak_G,yres);
	  Free2D(rinfo.Bak_B,yres);
	  Free2D(rinfo.Bak_A,yres);

	  delete Flag;

	  StatusHide();

	  return abort;
	}

void LensFlares::Copy(int i)	
	{ if (NoFlares+1==MAXFLARES) return; // Can't Create Any More 
	
	  char Name[256];
	  strcpy(Name,GetName(i));
	  strcat(Name," [cpy]");	  

	  Flares[NoFlares]=new Flare(Name); 
	  Flares[NoFlares]->GetFlareAnim(Flares[i]);

	  NoFlares++;
	}

void LensFlares::New(void)
	{ if (NoFlares+1==MAXFLARES) return; // Can't Create Any More 
	  Flares[NoFlares++]=new Flare("New Element");
	}

//******************************************************************************************
void LensFlares::GetFrame(int Time)	{	// Get New Positions
										for(int i=0;i!=NoFlares;i++)
											{ Flares[i]->GetPosition(Time);
											  Flares[i]->GetShape(Time);
											}
									}

void LensFlares::GetFrame(float Time){	// Get New Positions
										for(int i=0;i!=NoFlares;i++)
											{ Flares[i]->GetPosition(Time);
											  Flares[i]->GetShape(Time);
											}
									}
