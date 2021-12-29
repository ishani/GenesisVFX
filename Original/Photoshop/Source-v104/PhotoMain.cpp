//////////////////////////////////////////////////////////////////////////////
//
// PhotoMain.cpp : Implements the interface between photoshop and the filter
//
//	R.C. Wilson		9/3/97

#include <TCHAR.H>
#include <math.h>

#include "CustomControls.h"

#include "Utils.h"
#include "PhotoUtil.h"

#include "FColour.h"
#include "Flare.h"
#include "LensFlares.h"

#include "version.h"
#include "LensF.h"
#include "resource.h"
#include "PhotoMain.h"
#include "Exception.h"

extern int HaveWeInitCustomControls;

//////////////////////////////////////////////////////////////
//
// dll entry point
//

BOOL WINAPI DLLInit(HANDLE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
  try {
	if (fdwReason == DLL_PROCESS_ATTACH)
		hInst = (HINSTANCE)hInstance;

	if( !HaveWeInitCustomControls ) {
		InitCustomControls(hDllInstance);
		HaveWeInitCustomControls=TRUE;
	}
	return TRUE;   // Indicate that the DLL was initialized successfully.
  }
  catch(exception ex)
		{	ex.handle();
			return FALSE;
		}
}


/////////////////////////////////////////////////////
//
// Globals
//

int HaveWeInitCustomControls=FALSE;
HINSTANCE hInst;

int nPlanes;

#define MAXDATASIZE 1000000

//////////////////////////////////////////////////////
//
// Debugging stuff
//

void message(char *s,int k)
{
	char t[256];

	sprintf(t,"%s n:%d",s,k);
	MessageBox(NULL,t,"debug message",MB_OK);
}

//////////////////////////////////////////////////////
//
// global data 

// instance of DLL
Handle hDllInstance = NULL;

//////////////////////////////////////////////////////
//
// functions within photoshop interface
//

// called the first time the filter is called only; sets up data
void InitGlobals ();

// get parameters, either last used parameters or show user interface 
void DoParameters ();

// prepare for filtering; ask for data
void DoPrepare ();

// start the filter
void DoStart ();

// call repeatedly to filter and then ask for new data
void DoContinue ();

// clean up
void DoFinish ();
void LoadFilter();

// check parameters are ok
void ValidateParameters ();

// show about box
void DoAbout();

// show user interface
int PromptUserForInput();


///////////////////////////////////////////////////////////
//
// The one and only routine called by photoshop
//

ImageFilter_LensF*	Filter;
int					Result;
FilterRecord*		Stuff;

void ENTRYPOINT (short selector,FilterRecord *filterParamBlock,long *data,short *result)
{
  try{	
	Stuff = filterParamBlock;
	Result = noErr;
	
	if( Stuff->parameters==NULL )
		InitGlobals();

	if( (gParams)!=NULL )
		Filter=gParams->filter;
	if( Filter==NULL )
		LoadFilter();

	switch (selector)
		{
		
		case filterSelectorAbout:
			DoAbout ();
			break;
			
		case filterSelectorParameters:
			DoParameters ();
			break;
			
		case filterSelectorPrepare:
			DoPrepare ();
			break;
		
		case filterSelectorStart:
			DoStart ();
			break;
		
		case filterSelectorContinue:
			DoContinue ();
			break;
		
		case filterSelectorFinish:
			DoFinish ();
			break;
			
		default:
			Result = filterBadParameters;
		
		}
		
	*result = Result;
  }
  catch(exception ex)
	{	ex.handle();
		return;
	}

}

/*****************************************************************************/

void InitGlobals ()
{
	// create a new filter and initialise
	// this filter data is kept by photoshop between calls
	
	Stuff->parameters = NewHandle ((long) sizeof (TParameters));
	gParams->filter=new ImageFilter_LensF;

	if( !gStuff->parameters || !gParams->filter ) {
		Result=memFullErr;
		return;
	}
}


/*****************************************************************************/

/* Asks the user for the plug-in filter module's parameters. Note that
   the image size information is not yet defined at this point. Also, do
   not assume that the calling program will call this routine every time the
   filter is run (it may save the data held by the parameters handle in
   a macro file). Initialize any single-time info here. */

void DoParameters ()
{

	ValidateParameters ();
	gQueryForParameters = TRUE;
	/* If we're here, that means we're being called for the first time. */
}


/*****************************************************************************/

/* Check parameters and make sure they are valid, or initialize them. */

void ValidateParameters ()
{
	if (!Stuff->parameters)
	{

		short loop = 0;
		InitGlobals();
		if (!Stuff->parameters)
		{
			Result = memFullErr;
			return;
		}
	}
}

/*****************************************************************************/

/* Prepare to filter an image.	If the plug-in filter needs a large amount
   of buffer memory, this routine should set the bufferSpace field to the
   number of bytes required. Also check for */

void DoPrepare ()
	{
	int MemNeeded;

	// roughly maximum needed
	MemNeeded=27*Stuff->imageSize.h*Stuff->imageSize.v;
	if( MemNeeded>Stuff->maxSpace ) MemNeeded=Stuff->maxSpace;
	Stuff->bufferSpace = 0; // MemNeeded;
	}


//////////////////////////////////////////
//
// Complain if no editable transparency
//

BOOL CALLBACK ComplainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	if( msg==WM_INITDIALOG )
		CenterWindow(hwnd,GetParent(hwnd));

	if( msg==WM_COMMAND ) {
		if( LOWORD(wParam)==IDOK ) {
			int check=!SendDlgItemMessage(hwnd,IDC_CHECK,BM_GETCHECK,0,0);
			EndDialog(hwnd,check*2+1);
		}
		if( LOWORD(wParam)==IDCANCEL ) {
			int check=!SendDlgItemMessage(hwnd,IDC_CHECK,BM_GETCHECK,0,0);
			EndDialog(hwnd,check*2);
		}
	}
	return FALSE;
}

/*****************************************************************************/
// Line by line get image pixels
// reduces memory requirements
//

//////////////////////////////////////////////////////////////////
//
// Input Pixel

unsigned char* DataIn;
int InLineLen,InPixelSize;

unsigned char RedPixel(int x,int y)
{
			return DataIn[y*InLineLen+x*InPixelSize+0];
}

unsigned char GreenPixel(int x,int y)
{
	if( InPixelSize>=3 )
			return DataIn[y*InLineLen+x*InPixelSize+1];
	else	return DataIn[y*InLineLen+x*InPixelSize+0];	// grayscale
}

unsigned char BluePixel(int x,int y)
{
	if( InPixelSize>=3 )
			return DataIn[y*InLineLen+x*InPixelSize+2];
	else	return DataIn[y*InLineLen+x*InPixelSize+0];
}


unsigned char AlphaPixel(int x,int y)
{
	if( gStuff->filterCase<4 || gStuff->filterCase==6 || gStuff->filterCase==7 ) return 0;
	if( InPixelSize>=3 )
			return DataIn[y*InLineLen+x*InPixelSize+3];
	else	return DataIn[y*InLineLen+x*InPixelSize+1];	// grayscale
}

//**********************************************************************
//* Get Image From Input
void GetImage()
{	
	uchar** Img_R;
	uchar** Img_G;
	uchar** Img_B;
	uchar** Img_A;

	int xres=gStuff->imageSize.h;
	int yres=gStuff->imageSize.v;

	Alloc2D(Img_R,uchar,yres,xres);
	Alloc2D(Img_G,uchar,yres,xres);
	Alloc2D(Img_B,uchar,yres,xres);
	Alloc2D(Img_A,uchar,yres,xres);

	int i,j;
	int nlines=MAXDATASIZE/(nPlanes*xres);
	if( nlines<1 ) nlines=1;
	if( nlines>yres ) nlines=yres;

	int BaseLine=0;
	int getnlines;

	while( BaseLine<yres ) {
		getnlines=nlines;
		if( BaseLine+getnlines>yres ) getnlines=yres-BaseLine;

		PISetRect (&gStuff->inRect, 0,BaseLine,xres,BaseLine+getnlines);
		AdvanceState();
		InLineLen=gStuff->inRowBytes;
		InPixelSize=nPlanes;
		DataIn=(unsigned char *)gStuff->inData;

		if( DataIn )
		for(j=0;j<getnlines;j++)
			  for(i=0;i<xres;i++) {
					  Img_R[j+BaseLine][i]=RedPixel(i,j);
					  Img_G[j+BaseLine][i]=GreenPixel(i,j);
					  Img_B[j+BaseLine][i]=BluePixel(i,j);
					  Img_A[j+BaseLine][i]=AlphaPixel(i,j);
					}
		
		BaseLine+=getnlines;
	}

	gParams->filter->Img_R=Img_R;
	gParams->filter->Img_G=Img_G;
	gParams->filter->Img_B=Img_B;
	gParams->filter->Img_A=Img_A;
}


//**********************************************************************
//* put image to output
////////////////////////////////////////////////////////////////////
//
// Output pixel
//
unsigned char* DataOut;
int OutLineLen,OutPixelSize;

void OutputAlphaPixel(int x,int y,unsigned char c)
{
	if( gStuff->filterCase<4 || gStuff->filterCase==6 || gStuff->filterCase==7) return;
	if( OutPixelSize>=4 )
			DataOut[y*OutLineLen+x*OutPixelSize+3]=c;
	else	DataOut[y*OutLineLen+x*OutPixelSize+1]=c;	// grayscale
}

void PutImage()
{
	unsigned char r,g,b,a;
	uchar** Img_R;
	uchar** Img_G;
	uchar** Img_B;
	uchar** Img_A;

	int xres=gStuff->imageSize.h;
	int yres=gStuff->imageSize.v;

	Img_R=gParams->filter->Img_R;
	Img_G=gParams->filter->Img_G;
	Img_B=gParams->filter->Img_B;
	Img_A=gParams->filter->Img_A;

	int i,j;
	int nlines=MAXDATASIZE/(nPlanes*xres);
	if( nlines<1 ) nlines=1;
	if( nlines>yres ) nlines=yres;

	int BaseLine=0;
	int getnlines;

	while( BaseLine<yres ) {
		getnlines=nlines;
		if( BaseLine+getnlines>yres ) getnlines=yres-BaseLine;

		PISetRect (&gStuff->outRect, 0,BaseLine,xres,BaseLine+getnlines);
		AdvanceState();
		OutLineLen=gStuff->outRowBytes;
		OutPixelSize=nPlanes;
		DataOut=(unsigned char *)gStuff->outData;

		if( DataOut )
		for(j=0;j<getnlines;j++)
			  for(i=0;i<xres;i++) {
					r=Img_R[j+BaseLine][i];
					g=Img_G[j+BaseLine][i];
					b=Img_B[j+BaseLine][i];
					a=Img_A[j+BaseLine][i];

					if( OutPixelSize<3 ) {
						r=(r+g+b)/3;
						DataOut[j*OutLineLen+i*OutPixelSize]=r;
					} else {
						DataOut[j*OutLineLen+i*OutPixelSize+0]=r;
						DataOut[j*OutLineLen+i*OutPixelSize+1]=g;
						DataOut[j*OutLineLen+i*OutPixelSize+2]=b;
					}
					OutputAlphaPixel(i,j,a);
			}
		
		BaseLine+=getnlines;
	}

	Free2D(Img_R,yres);
	Free2D(Img_G,yres);
	Free2D(Img_B,yres);
	Free2D(Img_A,yres);
}


//**********************************************************************
//* Get Mask From Input
void GetMask()
{	
	uchar* mask;

	int xres=gStuff->imageSize.h;
	int yres=gStuff->imageSize.v;

	mask=(uchar *)malloc(xres*yres);
	memset(mask,255,xres*yres);
	gParams->filter->mbuffer=mask;

	int mxres=gStuff->filterRect.right-gStuff->filterRect.left;
	int mxstart=gStuff->filterRect.left;
	int myres=gStuff->filterRect.bottom-gStuff->filterRect.top;
	int mystart=gStuff->filterRect.top;
	
	int i,j;

	if( !gStuff->haveMask ) {
		for(j=mystart;j<mystart+myres;j++)
			for(i=mxstart;i<mxstart+mxres;i++)
				mask[j*xres+i]=0;
		return;
	}

	int nlines=MAXDATASIZE/(nPlanes*mxres);
	if( nlines<1 ) nlines=1;
	if( nlines>myres ) nlines=myres;

	int BaseLine=mystart;
	int getnlines;

	while( BaseLine<mystart+myres ) {
		getnlines=nlines;
		if( BaseLine+getnlines>mystart+myres ) getnlines=myres+mystart-BaseLine;

		PISetRect (&gStuff->maskRect, mxstart,BaseLine,mxstart+mxres,BaseLine+getnlines);
		AdvanceState();

		for(j=0;j<getnlines;j++)
			  for(i=0;i<xres;i++)
			    if( gStuff->maskData )
					mask[(j+BaseLine)*xres+(i+mxstart)]=((uchar *)gStuff->maskData)[j*gStuff->maskRowBytes+i];
				else mask[(j+BaseLine)*xres+(i+mxstart)]=0;

		BaseLine+=getnlines;
	}
}

/*****************************************************************************/

/* Does all of the filtering. */

void DoStart ()
	{

	// make status window
	PlatformData	*platform;
	platform=(PlatformData *)(gStuff->platformData);
	StatusCreate((HWND)(platform->hwnd));

	// need colour services for colour selector dialog
	if (!WarnColorServicesAvailable())
	{
		Result = userCanceledErr; // exit gracefully
		goto done;
	}

	if( (gStuff->imageMode!=plugInModeRGBColor) && (gStuff->imageMode!=plugInModeGrayScale) ) {
		Result=filterBadMode;
		goto done;
	}

	if( (gStuff->outLayerPlanes!=0)+(gStuff->outNonLayerPlanes!=0) != 1 ) {
		Result=userCanceledErr;
		goto done;
	}

	// number of planes
	nPlanes=gStuff->outLayerPlanes+gStuff->outTransparencyMask+gStuff->outNonLayerPlanes;
	if( nPlanes>4 ) nPlanes=4;

	// modes with non-editable transparency
	if( gParams->filter->data.complain && gStuff->filterCase>=6
		&& gStuff->filterCase<=7 ) {
		int ResultA;
		PlatformData	*platform;

		platform=(PlatformData *)(gStuff->platformData);
		ResultA=DialogBox(hInst,MAKEINTRESOURCE(IDD_COMPLAIN),(HWND)(platform->hwnd),(DLGPROC)ComplainProc);
		gParams->filter->data.complain=ResultA/2;
		if( ResultA%2==0 ) {
			Result=userCanceledErr;
			goto done;
		}
	}


	// ask for the required image planes
	// these are interleaved in the data
	gStuff->outHiPlane=nPlanes-1;
	gStuff->inHiPlane=nPlanes-1;
	gStuff->outLoPlane=0;
	gStuff->inLoPlane=0;

	// get and store input image
	PISetRect (&gStuff->outRect, 0,0,0,0);
	PISetRect (&gStuff->maskRect, 0,0,0,0);
	GetImage();

	// get and store mask
	PISetRect(&gStuff->inRect,0,0,0,0);
	GetMask();

	// ask for the image data required here
	// gStuff->filterRect contains the bounding rect of the current selection
	PISetRect (&gStuff->inRect, 0,0,1,1);	// make sure we come back
	PISetRect (&gStuff->outRect, 0,0,0,0);
	PISetRect (&gStuff->maskRect, 0,0,0,0);

	return;

done:
	DoFinish();
	}

/*****************************************************************************/

// This is where the filtering takes place
// iteratively filter the given rect and then ask for a new one
// terminates when the rects are set to zero

void DoContinue ()
	{
	MaskData maskdata;
	// check parameters before we filter
	ValidateParameters ();

	// turn off automasking
	gStuff->autoMask=FALSE;

	// set up image parameters
	maskdata.havemask=gStuff->haveMask;
	maskdata.mask=(unsigned char *)gStuff->maskData;
	maskdata.x0=gStuff->filterRect.left;
	maskdata.y0=gStuff->filterRect.top;
	maskdata.x1=gStuff->filterRect.right;
	maskdata.y1=gStuff->filterRect.bottom;
	maskdata.masklinelen=gStuff->maskRowBytes;
	maskdata.ImageMode=gStuff->imageMode;
	maskdata.FilterCase=gStuff->filterCase;
	maskdata.Height=gStuff->imageSize.v;
	maskdata.Width=gStuff->imageSize.h;
	maskdata.InData=(unsigned char *)gStuff->inData;
	maskdata.OutData=(unsigned char *)gStuff->outData;
	maskdata.InRowBytes=gStuff->inRowBytes;
	maskdata.OutRowBytes=gStuff->outRowBytes;
	maskdata.InPixelSize=maskdata.OutPixelSize=nPlanes;

	gParams->filter->SetPhotoInput(&maskdata);


	// if we need the user interface, call it
	if (gQueryForParameters)
	{
		gQueryForParameters = FALSE;
		if (!PromptUserForInput ()) {
			Result=userCanceledErr;
			goto done; // canceled
		}
	}

	if (Result != noErr)
		goto done;
	
	PISetRect (&gStuff->inRect, 0, 0, 0, 0);
	PISetRect (&gStuff->maskRect, 0, 0, 0, 0);

	gParams->filter->Render();

	PutImage();

done:
	PISetRect (&gStuff->inRect, 0, 0, 0, 0);
	PISetRect (&gStuff->maskRect, 0, 0, 0, 0);
	PISetRect (&gStuff->outRect, 0, 0, 0, 0);
	
	if( gParams->filter->mbuffer ) free(gParams->filter->mbuffer);
	}

/*****************************************************************************/

/* Do any necessary clean-up. */

void DoFinish ()
{
	// destroy status
	StatusKill();

	char *x=NULL;
	int size=Filter->MemSave(&x);
	DisposHandle(Stuff->parameters);
	Stuff->parameters=NewHandle(size+sizeof(TParameters));
	x=(char*)&gParams->SaveData;
	Filter->MemSave(&x);
	gParams->SavedFilter=TRUE;
	delete Filter;
	gParams->filter=NULL;
}

void LoadFilter()
{
	char* x;
	if( !gParams->SavedFilter ) return;
	gParams->filter=new ImageFilter_LensF;
	Filter=gParams->filter;
	x=(char*)&gParams->SaveData;
	Filter->MemLoad(&x);
}
/*******************************************************************/
BOOL CALLBACK AboutCtrlDlgProc(HWND,UINT,WPARAM,LPARAM);
void DoAbout()
{
	DialogBox(hInst,MAKEINTRESOURCE(IDD_GEN_ABOUT),NULL,(DLGPROC)AboutCtrlDlgProc);
}

////////////////////////////////////////////////////////////////
//
// show dialog

int PromptUserForInput()
{

	PlatformData	*platform;

	platform=(PlatformData *)(gStuff->platformData);
	if( !gParams->filter->ShowControl((HWND)(platform->hwnd)) ) {
		Result=userCanceledErr;
		return 0;
	}

	return 1;
}



void main()
{
	MessageBox(NULL,"What are you doing here?","User Input",MB_OK);
}



/////////////////////////////////////////////////////////////
//
// debugging routine
//

void test(HINSTANCE hInst)
{
	ImageFilter_LensF* f;

	f=new ImageFilter_LensF;
	f->ShowControl(NULL);
}

////////////////////////////////////////////////////
//
// include utility routines
//

#include "PhotoUtil.cpp"
