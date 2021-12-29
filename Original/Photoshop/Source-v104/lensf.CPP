#include <stdio.h>
#include <math.h>
#include <TCHAR.H>
#include "Exception.h"
#include "CustomControls.h"
#include "PhotoUtil.h"

#include "Utils.h"
#include "FColour.h"
#include "Flare.h"
#include "LensFlares.h"
#include "LensF.h"
#include "resource.h"
#include "GFXfileman.h"

char prog[]="GenesisVfX v1.04";
char InstallLoc[MAX_PATH+10]="InstallDirc:\\photoshop\\plugins\\filters";

//-- Globals ------------------------------------------------------------------

void SetBit(int &V,int n,int bit)
		{ int tmp=1<<(n-1);
	      bit=(bit)?1:0;
          if (V&tmp)	{ if (bit) return;
                          else V-=tmp;
						}
		  else			{ if (bit) V+=tmp;
						}
		}


extern float MorphEditX,MorphEditY;
extern int	 ChangedFlare;
int   MallocSize;

int seed;
//-----------------------------------------------------------------------------
// *> AboutCtrlDlgProc()
//

BOOL CALLBACK AboutCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {

     switch (message) {
        
        case WM_INITDIALOG: {
             CenterWindow(hWnd,GetParent(hWnd));
             SetCursor(LoadCursor(NULL,IDC_ARROW));
			 return 1;
        }

        case WM_COMMAND:
             switch (LOWORD(wParam)) {
                case IDOK:		 EndDialog(hWnd,1); break;
				case IDCANCEL:	 EndDialog(hWnd,0); break;
				case IDC_HELPB: {
						char helpcmd[MAX_PATH+30];
						sprintf(helpcmd,"%s\\GenesisVFX\\genVFXps.chm",InstallLoc+10);
						ShellExecute(NULL,"open",helpcmd,NULL,NULL,SW_SHOW);
					}

             }
             return 1;
     }     
     return 0;
}

void ImageFilter_LensF::ShowAbout(HWND hWnd) {
		DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_GEN_ABOUT),hWnd,(DLGPROC)AboutCtrlDlgProc,(LPARAM)this);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ImageFilter_LensF *ImageFilterData;

#define  EnableIt(a,b) { HWND hDlg=GetDlgItem(hWnd,a); EnableWindow(hDlg,b); }

#define BuildButton(Button,ImageList,UnPushed,Pushed,x,y)\
				{ ImageList = ImageList_Create(x, y, ILC_COLOR24, 1, 0);\
				  HBITMAP hBitmap1 = LoadBitmap(hInst,MAKEINTRESOURCE(UnPushed));\
				  HBITMAP hBitmap2 = LoadBitmap(hInst,MAKEINTRESOURCE(Pushed));\
				  ImageList_Add(ImageList, hBitmap1, NULL);\
				  ImageList_Add(ImageList, hBitmap2, NULL);\
				  Button->SetImage(ImageList,0,1,0,1,x,y);\
				  DeleteObject(hBitmap1);\
				  DeleteObject(hBitmap2);\
				}

#define ReleaseButton(ImageList) ImageList_Destroy(ImageList);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// SERIALISE
HBITMAP Render_RegisterBitmap;
unsigned long int Cd;
char RegistrationCodeGet[256];
HIMAGELIST		HIL_REGISTER;
int REGISTERCREATING;
static ICustButton *Register=NULL;
unsigned int revision;
char Serialise[32];

/////////////////////////
// checksum
unsigned long Prmpt=0x21222324;			// look for pattern !"£$ in code
// end checksum

ImageFilter_LensF* myflare;

BOOL CALLBACK RegisterCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{	 if (REGISTERCREATING==TRUE) return FALSE;

     switch (message) {
        
        case WM_INITDIALOG: {
			 myflare=(ImageFilter_LensF*)lParam;
			 REGISTERCREATING=TRUE;
             CenterWindow(hWnd,GetParent(hWnd));
             SetCursor(LoadCursor(NULL,IDC_ARROW));

			 char tmp[256];
			 if( strcmp(Serialise+10,"1234567890")==0 )
					SetDlgItemText(hWnd,IDC_SERIAL,"");
			 else	SetDlgItemText(hWnd,IDC_SERIAL,Serialise+10);
			 sprintf(tmp,"PR%10d\0\0\0\n",Cd);
			 if( strcmp(Serialise+10,"1234567890")!=0 )
				 SetDlgItemText(hWnd,IDC_REG1,tmp);
			 int GCd=myflare->GetCode();
			 sprintf(tmp,"%10.10u\0\0\0\n",GCd);
			 if( GCd && strcmp(Serialise+10,"1234567890")!=0 ) 
				 SetDlgItemText(hWnd,IDC_REG2,tmp);

			 Register=GetICustButton(GetDlgItem(hWnd,IDC_REG_CLOSE));
//			 BuildButton(Register,HIL_REGISTER,IDB_REGOK,IDB_REGOK,100,50);
			 REGISTERCREATING=FALSE;
			 return 1;
        }

        case WM_COMMAND:
             switch (LOWORD(wParam)) {
                case IDOK:
				case IDC_REG_CLOSE:
										GetDlgItemText(hWnd,IDC_REG2,RegistrationCodeGet,256);
										REGISTERCREATING=TRUE;
//										ReleaseButton(HIL_REGISTER);
										ReleaseICustButton(Register);
										EndDialog(hWnd,1); 
										return TRUE;
				case IDC_EVAL:
				case IDCANCEL:
										ReleaseICustButton(Register);
										EndDialog(hWnd,2); 
										return TRUE;
										
				case IDC_SERIAL:
					if( HIWORD(wParam)==EN_CHANGE ) {
						int val=0;
						val=GetDlgItemInt(hWnd,IDC_SERIAL,NULL,FALSE);
						if( val>=10000 && val<65536) {
							char tmp[30];
							sprintf(tmp,"%u\0\0\0\n",val);
							strncpy(Serialise+10,tmp,10);
							Cd=myflare->Code();
							sprintf(tmp,"PR%10d\0\0\0\n",Cd);
							SetDlgItemText(hWnd,IDC_REG1,tmp);
						} else
							SetDlgItemText(hWnd,IDC_REG1,"");
					}
					break;
				case IDC_HELPB:
					{
						char helpcmd[MAX_PATH+30];
						sprintf(helpcmd,"%s\\GenesisVFX\\genVFXps.chm",InstallLoc+10);
						ShellExecute(NULL,"open",helpcmd,NULL,NULL,SW_SHOW);
					}
					break;
             }
             return 1;
     }     
     return 0;
}

int ImageFilter_LensF::Register(HWND hWnd) 
{		Cd=Code();
		REGISTERCREATING=FALSE;
		revision^=0x26542276;
		int retval=DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_REGISTER),hWnd,(DLGPROC)RegisterCtrlDlgProc,(LPARAM)this);
		if( retval==1 ) 
			PutCode(RegistrationCodeGet);
		return retval;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include "PhotoRender.cpp"
#include "Inferno.cpp"
#include "FlareBuild.cpp"
#include "FlareEdit.cpp"
#include "FlaresEdit.cpp"
#include "Morph.cpp"

ImageFilter_LensF::ImageFilter_LensF()
{	
	// SERIALISATION INFORMATION
	strcpy(Serialise,"42949672901234567890");
	strcpy(Serialise2,"1000000001");

	// Get the install location from the registry
	DWORD size=MAX_PATH;
	DWORD type;
	HKEY key;
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\FutureFantastic\\GenesisVfX\\1.04",0,KEY_QUERY_VALUE,&key)==ERROR_SUCCESS ) {
		if( RegQueryValueEx(key,"Path",NULL,&type,(LPBYTE)(InstallLoc+10),&size)!=ERROR_SUCCESS )
			MessageBox(NULL,"GenesisVfX is not correctly installed.","Warning",MB_OK);
		RegCloseKey(key);
	}
	else {
		MessageBox(NULL,"GenesisVfX is not correctly installed.","Warning",MB_OK);
	}

//	char tmp[256];
//	strcpy(tmp,"ID:Limited_BETA0.01");
	
	data.version				=LensFVERSION;
	data.DrawChecker			=0;	

	data.RadialRGB .EditColour		=1; data.RadialRGB.Wrap		=0;
	data.RadialAlpha .EditColour	=0; data.RadialAlpha.Wrap	=0;
	data.AngularRGB.EditColour		=1; data.AngularRGB.Wrap	=1;
	data.AngularAlpha.EditColour	=0; data.AngularAlpha.Wrap	=1;
	data.Shape.EditColour			=0; data.Shape.Wrap			=1;
	data.Shape2.EditColour			=0; data.Shape2.Wrap		=0;
	
	data.EditPage=0;
	data.RadialLock=0;
	data.AngularLock=0;

	data.SelectedElement[0]=1;
	for(int i=1;i!=MAXFLARES;i++) data.SelectedElement[i]=0;

	data.BuildColour=1;
	data.BuildShape=1;
	
	data.BuildWidth=0.05f;
	data.BuildTransparency=1.0f;
	data.BuildNoStreaks=6;
	data.BuildNoSides=6;
	data.BuildFactor=0.5f;
	data.BuildAspect=0.5f;
	data.BuildCol.r=65535;
	data.BuildCol.g=65535;
	data.BuildCol.b=65535;

	data.MorphNumber=10;
	data.MorphSizeVariation=0.0f;
	data.MorphLightVariation=0.0f;
	data.MorphDistanceVariation=0.0f;
	data.MorphAngleOffsetVariation=0.0f;
	data.MorphAngleVariation=0.0f;
	data.MorphXPositionVariation=0.0f;
	data.MorphYPositionVariation=0.0f;
	data.MorphColouriseVariation=0.0f;
	data.MorphInterplay=1;
	data.MorphSeed=0;
	data.MorphOcclusionVariation=0.0f;
	data.MorphDepthVariation=0.0f;

	data.AnimationMax=100;
	data.AnimationPosition=0;

	data.AnimationSelect1=-1;
	data.AnimationSelect2=-1;

	data.RealTimePreview=1;

	data.CameraDepth=1.0f;


	data.EditActivePreview=1;
	data.BuildActivePreview=1;

	data.BuildSeed=1;
	data.ActivePreview=1;

	// init photoshop render data
	data.RealTime=FALSE;
	data.ImageScale=1.0f;
	data.SourceMode=0;
	data.Col1.r=data.Col1.g=data.Col1.b=0;
	data.Col2.r=data.Col2.g=data.Col2.b=255;
	data.Tolerance=30.0f;

	data.PosX=0.0f;
	data.PosY=0.0f;
	data.AntiAliasing=0;
	data.Single=1;
	data.CSE=1;
	data.MaskInside=2;
	data.ClipMode=2;

	data.ExternalSmoothness=0.05f;
	data.InternalSmoothness=0.0f;

	data.Colourise=0;
	data.ColouriseSmoothness=0.05f;
	data.InfernoActivePreview=1;

	// complain
	data.complain=1;

	// morph light pos
	MorphEditX=-1.0f;
	MorphEditY=-1.0f;

	// flare changed flag
	ChangedFlare=FALSE;

}
//-----------------------------------------------------------------------------





///////////////////////////////////////////////////////////////////////////////
//
// Set render vars
//

void ImageFilter_LensF::SetPhotoInput(MaskData* md)
{
	data.ImageWidth		=md->Width;
	data.ImageHeight	=md->Height;
	data.in				=md->InData;
	data.inlinelen		=md->InRowBytes;
	data.inpixelsize	=md->InPixelSize;
	data.out			=md->OutData;
	data.outlinelen		=md->OutRowBytes;
	data.outpixelsize	=md->OutPixelSize;
	data.maskdata		=md;
};




// see render.cpp

#include "Render.cpp"

// #> ImageFilter_LensF::Render()
//
//    Private member Bitmap *srcmap has source bitmap

BOOL ImageFilter_LensF::Render() 
{

	BOOL			result=FALSE;

//	if (encr(GetCode())==encr(encr(Code()))) {
		result=TRUE;
		xres=data.ImageWidth;
		yres=data.ImageHeight;

		RenderImage();
		PutImage();
//	}

	return result;	
}


































BOOL ImageFilter_LensF::LoadConfigure (void *ptr) {
	 char *temp=(char *)ptr;

     MemLoad(&temp);
	 return TRUE;
}

BOOL ImageFilter_LensF::SaveConfigure (void *ptr) {
	 char *temp=(char *)ptr;

     if (ptr)	{ MemSave(&temp);
	              return TRUE;
	            }
	 else		return FALSE;
}

DWORD ImageFilter_LensF::EvaluateConfigure ( ) 
{	char *temp=NULL;
	int Size=MemSave(&temp);
	return Size;
}

// This is where it all ends.
// Enjoy ...
// Nathalie, Without you this isn't possible.
