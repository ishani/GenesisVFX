#define RealTimePreview_Add			32
#define RealTimePreview_AngleAdd	(FLARERESOLUTION/RealTimePreview_Add)


#define XImageCoord	((data->PosX+1.0f)*(float)(data->ImageWidth-1)/2.0f)
#define YImageCoord	((data->PosY+1.0f)*(float)(data->ImageHeight-1)/2.0f)

#define plugInModeRGBColor	   3
#define plugInModeGrayScale    1

class RenderDialog {

public:
	HWND		myhwnd;
	LensFDATA*	data;
	ImageFilter_LensF*	MainFilter;
	HBITMAP				RenderLoadBitmap;
	HBITMAP				RenderBackBitmap;

private:
	ICustButton*		RenderClipSel;
	ICustButton*		RenderClipOutside;
	ICustButton*		RenderNoClip;

	ICustButton*		MaskOut;
	ICustButton*		MaskIn;
	ICustButton*		NoMask;
	
	ICustButton*		AntiNormal;
	ICustButton*		AntiMedium;
	ICustButton*		AntiHigh;

	ICustButton*		SourceSel;
	ICustButton*		SourceInt;
	ICustButton*		SourceCol;
	ICustButton*		SourceExt;

	ICustButton*		ScaleToFit;
	ICustButton*		Multi;
	ICustButton*		Centroid;
	ICustButton*		Colourise;

	ICustButton*		RenderExit;
	ICustButton*		Close;

	ISpinnerControl*	XPos;
	ISpinnerControl*	YPos;
	ISpinnerControl*	ImageScale;
	ISpinnerControl*	Tolerance;
	
	ISpinnerControl*	ExtSmooth;
	ISpinnerControl*	IntSmooth;
	ISpinnerControl*	ColSmooth;

	IColorSwatch*		Col1;
	IColorSwatch*		Col2;

	HWND				hwPreview;
	char*				hwPreviewMem;
	BITMAPINFO			hwPreviewInfo;

	int					ScrollXMin;
	int					ScrollYMin;
	int					ShowWhat;

	int					lxres,lyres;
	int					xsize;

	float				colorx,colory,colorz;
	float				basex,basey,basez;
	float				colorlen;

	int					Showing;
	int					dragging;

	int					GREEN;
	int					BLUE;

	void	ReleaseControls();
	void	GetVars();
	void	SetUpMemAndScroll(HWND);
	void	Scroll(HWND,WPARAM,int&,int);

	void	SetClipRadios();
	void	SetMaskRadios();
	void	SetAntiRadios();
	void	SetSourceRadios();
	void	EnableControls(int);


	void	PhotoToImage(int&,int&);
	unsigned char MaskVal(int,int);
	void	PrepareColourVector();
	int		IsSource(int x,int y);
	void	ShowBitmap();
	void	RenderBackground(uchar**,uchar**,uchar**,uchar**,int,int,int);
	
	void	FullPreview();
	void	BackPreview();
	void	RenderOutline();
	void	OutlinePreview();


public:
	void	SetVars();
	void	InitControls(HWND);
	BOOL	Callback(HWND,UINT,WPARAM,LPARAM);	
	BOOL	PreviewCallback(HWND,UINT,WPARAM,LPARAM);

	RenderDialog(HWND,LensFDATA*,ImageFilter_LensF*,int&);
   ~RenderDialog();

};


//////////////////////////////////////////////////////////////////
//
// InitControls
//		get controls

BOOL CALLBACK PreviewCallbackProc(HWND,UINT,WPARAM,LPARAM);

HIMAGELIST		HIL_Inside;
HIMAGELIST		HIL_Outside;
HIMAGELIST		HIL_None;
HIMAGELIST		HIL_Normal;
HIMAGELIST		HIL_Medium;
HIMAGELIST		HIL_High;
HIMAGELIST		HIL_Internal;
HIMAGELIST		HIL_Single;
HIMAGELIST		HIL_External;
HIMAGELIST		HIL_Colour;
HIMAGELIST		HIL_Resize;
HIMAGELIST		HIL_Centroid;
HIMAGELIST		HIL_Multi;
HIMAGELIST		HIL_Colorise;
HIMAGELIST		HIL_Nomask;
HIMAGELIST		HIL_Maskint;
HIMAGELIST		HIL_Maskext;
HIMAGELIST		HIL_Renderexit;
HIMAGELIST		HIL_Close;

void RenderDialog::InitControls(HWND hwnd)
{
	RenderClipSel=GetICustButton(GetDlgItem(hwnd,IDC_RenderClipSel));
	RenderClipSel->SetType(CBT_CHECK);
	RenderClipOutside=GetICustButton(GetDlgItem(hwnd,IDC_RenderClipOutside));
	RenderClipOutside->SetType(CBT_CHECK);
	RenderNoClip=GetICustButton(GetDlgItem(hwnd,IDC_RenderNoClip));
	RenderNoClip->SetType(CBT_CHECK);

	NoMask=GetICustButton(GetDlgItem(hwnd,IDC_NoMask));
	NoMask->SetType(CBT_CHECK);
	MaskOut=GetICustButton(GetDlgItem(hwnd,IDC_MaskOut));
	MaskOut->SetType(CBT_CHECK);
	MaskIn=GetICustButton(GetDlgItem(hwnd,IDC_MaskIn));
	MaskIn->SetType(CBT_CHECK);

	AntiNormal=GetICustButton(GetDlgItem(hwnd,IDC_AntiNormal));
	AntiNormal->SetType(CBT_CHECK);
	AntiMedium=GetICustButton(GetDlgItem(hwnd,IDC_AntiMedium));
	AntiMedium->SetType(CBT_CHECK);
	AntiHigh=GetICustButton(GetDlgItem(hwnd,IDC_AntiHigh));
	AntiHigh->SetType(CBT_CHECK);
	
	SourceSel=GetICustButton(GetDlgItem(hwnd,IDC_SourceSel));
	SourceSel->SetType(CBT_CHECK);
	SourceInt=GetICustButton(GetDlgItem(hwnd,IDC_SourceInt));
	SourceInt->SetType(CBT_CHECK);
	SourceCol=GetICustButton(GetDlgItem(hwnd,IDC_SourceCol));
	SourceCol->SetType(CBT_CHECK);
	SourceExt=GetICustButton(GetDlgItem(hwnd,IDC_SourceExt));
	SourceExt->SetType(CBT_CHECK);

	ScaleToFit=GetICustButton(GetDlgItem(hwnd,IDC_ScaleToFit));
	Multi=GetICustButton(GetDlgItem(hwnd,IDC_Multi));
	Multi->SetType(CBT_CHECK);
	Centroid=GetICustButton(GetDlgItem(hwnd,IDC_Centroid));
	Centroid->SetType(CBT_CHECK);
	Colourise=GetICustButton(GetDlgItem(hwnd,IDC_Colourise));
	Colourise->SetType(CBT_CHECK);


	XPos=SetupFloatSpinner(hwnd,IDC_XSpin,IDC_XEdit,-10.0f,10.0f,0.0f,0.01f,0.0f);
	YPos=SetupFloatSpinner(hwnd,IDC_YSpin,IDC_YEdit,-10.0f,10.0f,0.0f,0.01f,0.0f);
	ImageScale=SetupFloatSpinner(hwnd,IDC_ImageSizeSpin,IDC_ImageSizeEdit,0.01f,4.0f,0.0f,0.01f,1.0f);
	Tolerance=SetupFloatSpinner(hwnd,IDC_ToleranceSpin,IDC_ToleranceEdit,0.0f,400.0f,0.0f,0.5f,30.0f);

	ExtSmooth=SetupFloatSpinner(hwnd,IDC_ExtSpin,IDC_ExtEdit,0.0f,1.0f,0.0f,0.001f,0.0f);
	IntSmooth=SetupFloatSpinner(hwnd,IDC_IntSpin,IDC_IntEdit,0.0f,1.0f,0.0f,0.001f,0.0f);
	ColSmooth=SetupFloatSpinner(hwnd,IDC_ColSpin,IDC_ColEdit,0.0f,1.0f,0.0f,0.001f,0.0f);

	Col1=GetIColorSwatch(GetDlgItem(hwnd,IDC_COL1),RGB(data->Col1.r,data->Col1.g,data->Col1.b),_T("Color Bound"));
	Col2=GetIColorSwatch(GetDlgItem(hwnd,IDC_Col2),RGB(data->Col2.r,data->Col2.g,data->Col2.b),_T("Color Bound"));

	RenderExit=GetICustButton(GetDlgItem(hwnd,IDC_RenderExit));
	Close=GetICustButton(GetDlgItem(hwnd,IDC_CLOSE));

	// set up images
	BuildButton(RenderClipSel,HIL_Inside,IDB_INSIDE,IDB_INSIDE,100,48);
	BuildButton(RenderClipOutside,HIL_Outside,IDB_OUTSIDE,IDB_OUTSIDE,100,48);
	BuildButton(RenderNoClip,HIL_None,IDB_NONE,IDB_NONE,100,48);
	BuildButton(NoMask,HIL_Nomask,IDB_NOMASK,IDB_NOMASK,100,48);
	BuildButton(MaskOut,HIL_Maskext,IDB_MASKEXT,IDB_MASKEXT,100,48);
	BuildButton(MaskIn,HIL_Maskint,IDB_MASKINT,IDB_MASKINT,100,48);
	BuildButton(AntiNormal,HIL_Normal,IDB_NORMAL,IDB_NORMAL,100,48);
	BuildButton(AntiMedium,HIL_Medium,IDB_MEDIUM,IDB_MEDIUM,100,48);
	BuildButton(AntiHigh,HIL_High,IDB_HIGH,IDB_HIGH,100,48);
	BuildButton(SourceSel,HIL_Single,IDB_SINGLE,IDB_SINGLE,80,80);
	BuildButton(SourceInt,HIL_Internal,IDB_INTERNAL,IDB_INTERNAL,80,80);
	BuildButton(SourceCol,HIL_Colour,IDB_COLOUR,IDB_COLOUR,80,80);
	BuildButton(SourceExt,HIL_External,IDB_EXTERNAL,IDB_EXTERNAL,80,80);
	BuildButton(ScaleToFit,HIL_Resize,IDB_RESIZE,IDB_RESIZE,40,40);
	BuildButton(Multi,HIL_Multi,IDB_MULTI,IDB_MULTI,100,48);
	BuildButton(Centroid,HIL_Centroid,IDB_CENTROID,IDB_CENTROID,100,48);
	BuildButton(Colourise,HIL_Colorise,IDB_COLORISE,IDB_COLORISE,100,48);
	BuildButton(RenderExit,HIL_Renderexit,IDB_RENDEREXIT,IDB_RENDEREXIT,100,48);
	BuildButton(Close,HIL_Close,IDB_CLOSE,IDB_CLOSE,100,48);

			 // Setup background bitmap rcw 18/4/97
				// load bitmap and get window dc
				RenderLoadBitmap=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_RENDER));
				HDC hdc=GetDC(hwnd);
				RECT	rect;
				BITMAP	bm;

				// get sizes of bitmap and window
				GetClientRect(hwnd,&rect);
				GetObject(RenderLoadBitmap,sizeof(BITMAP),&bm);

				if( abs(rect.right-bm.bmWidth)<=2 && abs(rect.bottom-bm.bmHeight)<=2 )
					// keep original
					RenderBackBitmap=RenderLoadBitmap;
				else {
					// get the final background bitmap
					RenderBackBitmap=CreateCompatibleBitmap(hdc,(rect.right+3)&0xfffffffc,rect.bottom);
					// interpolate new size
					// note this deletes old bitmap internally
					MainFilter->LinearResample(hdc,RenderLoadBitmap,bm.bmWidth,bm.bmHeight,hdc,RenderBackBitmap,rect.right,rect.bottom);
				}

				// clean up DC
				ReleaseDC(hwnd,hdc);

	// set scale
	hwPreview=GetDlgItem(hwnd,IDC_RenderPreview);
	GetClientRect(hwPreview,&rect);
	float yscale=(float)(rect.bottom-rect.top)/(float)(data->ImageHeight);
	float xscale=(float)(rect.right-rect.left)/(float)(data->ImageWidth);
	if( yscale<xscale)	data->ImageScale=yscale;
	else				data->ImageScale=xscale;

	// setup vars and scrolling
	dragging=FALSE;
	hwPreviewMem=NULL;
	SetVars();
	SetUpMemAndScroll(hwnd);
	ShowWhat=data->RealTime;
	if( data->maskdata->InPixelSize<3 ) { GREEN=BLUE=0; }
	else { GREEN=1;BLUE=2; }

	SetWindowLong(hwPreview,GWL_WNDPROC,(long)PreviewCallbackProc);

	if( data->RealTime )	{ FullPreview();Showing=TRUE; }
	else					{ BackPreview();Showing=FALSE; }
}

void RenderDialog::ReleaseControls()
{
	ReleaseButton(HIL_Inside);
	ReleaseButton(HIL_Outside);
	ReleaseButton(HIL_None);
	ReleaseButton(HIL_Nomask);
	ReleaseButton(HIL_Maskext);
	ReleaseButton(HIL_Maskint);
	ReleaseButton(HIL_Normal);
	ReleaseButton(HIL_Medium);
	ReleaseButton(HIL_High);
	ReleaseButton(HIL_Single);
	ReleaseButton(HIL_Internal);
	ReleaseButton(HIL_Colour);
	ReleaseButton(HIL_External);
	ReleaseButton(HIL_Resize);
	ReleaseButton(HIL_Multi);
	ReleaseButton(HIL_Centroid);
	ReleaseButton(HIL_Colorise);
	ReleaseButton(HIL_Renderexit);
	ReleaseButton(HIL_Close);

	ReleaseICustButton(RenderClipSel);
	ReleaseICustButton(RenderClipOutside);
	ReleaseICustButton(RenderNoClip);

	ReleaseICustButton(NoMask);
	ReleaseICustButton(MaskOut);
	ReleaseICustButton(MaskIn);
	
	ReleaseICustButton(AntiNormal);
	ReleaseICustButton(AntiMedium);
	ReleaseICustButton(AntiHigh);

	ReleaseICustButton(SourceSel);
	ReleaseICustButton(SourceInt);
	ReleaseICustButton(SourceCol);
	ReleaseICustButton(SourceExt);

	ReleaseICustButton(ScaleToFit);
	ReleaseICustButton(Multi);
	ReleaseICustButton(Centroid);
	ReleaseICustButton(Colourise);
	
	ReleaseSpinner(XPos);
	ReleaseSpinner(YPos);
	ReleaseSpinner(ImageScale);
	ReleaseSpinner(Tolerance);
	ReleaseSpinner(ExtSmooth);
	ReleaseSpinner(IntSmooth);
	ReleaseSpinner(ColSmooth);

	ReleaseIColorSwatch(Col1);
	ReleaseIColorSwatch(Col2);

	ReleaseICustButton(RenderExit);
	ReleaseICustButton(Close);

	free(hwPreviewMem);

	DeleteObject(RenderBackBitmap);
}

////////////////////////////////////////////////////////////////
//
// Get mem for preview bitmap
// decide if scrolling is needed
// enable/disable scroll bars

void RenderDialog::SetUpMemAndScroll(HWND hwnd)
{

	float oldscale=data->ImageScale;
	GetVars();

	// if no change then return
	if( oldscale==data->ImageScale && hwPreviewMem!=NULL ) return;
	else if( hwPreviewMem!=NULL ) free(hwPreviewMem);
	
	lxres=((int)((float)data->ImageWidth*data->ImageScale)+16)&0xfffffff0;
	xsize=(int)((float)data->ImageWidth*data->ImageScale);
	lyres=(int)((float)data->ImageHeight*data->ImageScale);
	hwPreviewMem=(char *)malloc(lxres*lyres*3);

	hwPreviewInfo.bmiHeader.biSize			=sizeof(BITMAPINFOHEADER);
	hwPreviewInfo.bmiHeader.biPlanes		=1;
	hwPreviewInfo.bmiHeader.biBitCount		=24;
	hwPreviewInfo.bmiHeader.biCompression	=BI_RGB;
	hwPreviewInfo.bmiHeader.biClrUsed		=0;
	hwPreviewInfo.bmiHeader.biClrImportant	=0;
	hwPreviewInfo.bmiHeader.biWidth			=lxres;
	hwPreviewInfo.bmiHeader.biHeight		=lyres;
	hwPreviewInfo.bmiHeader.biSizeImage		=lxres*lyres*3;

	RECT	rect;

	// get actual size of displayed image
	GetClientRect(hwPreview,&rect);
	if( (rect.right-rect.left>=xsize) ) {
		// Not scrolling
		data->ScrollingX=FALSE;
		data->ScrollX=(rect.right-rect.left-xsize)/2;
	} else {
		data->ScrollingX=TRUE;
		data->ScrollX=(int)((float)xsize/2-(float)(xsize/2-data->ScrollX)*data->ImageScale/oldscale);
		ScrollXMin=(rect.right-rect.left-xsize);
		SetScrollRange(GetDlgItem(hwnd,IDC_HSCROLLBAR),SB_CTL,0,-ScrollXMin,FALSE);
		SetScrollPos(GetDlgItem(hwnd,IDC_HSCROLLBAR),SB_CTL,-data->ScrollX,TRUE);
		if( data->ScrollX>0 )			data->ScrollX=0;
		if( data->ScrollX<ScrollXMin)	data->ScrollX=ScrollXMin;
	}

	if( (rect.bottom-rect.top>=lyres) ) {
		// Not scrolling
		data->ScrollingY=FALSE;
		data->ScrollY=(rect.bottom-rect.top-lyres)/2;
	} else {
		data->ScrollingY=TRUE;
		data->ScrollY=(int)((float)lyres/2-(float)(lyres/2-data->ScrollY)*data->ImageScale/oldscale);
		ScrollYMin=(rect.bottom-rect.top-lyres);
		SetScrollRange(GetDlgItem(hwnd,IDC_VSCROLLBAR),SB_CTL,0,-ScrollYMin,FALSE);
		SetScrollPos(GetDlgItem(hwnd,IDC_VSCROLLBAR),SB_CTL,-data->ScrollY,TRUE);
		if( data->ScrollY>0 )			data->ScrollY=0;
		if( data->ScrollY<ScrollYMin)	data->ScrollY=ScrollYMin;
	}

	// clear image rect if necessary
	if( ((!data->ScrollingY) || (!data->ScrollingX)) && (data->ImageScale<oldscale) )
	{
		RECT	rect;
		GetClientRect(hwPreview,&rect);
		MapWindowPoints(hwPreview,hwnd,(LPPOINT)&rect,2);
		InvalidateRect(hwnd,&rect,TRUE);
		
	}

	EnableWindow(GetDlgItem(hwnd,IDC_HSCROLLBAR),data->ScrollingX);
	EnableWindow(GetDlgItem(hwnd,IDC_VSCROLLBAR),data->ScrollingY);
}


////////////////////////////////////////////////////////////////
//
// Get and set variables
//

void RenderDialog::SetClipRadios()
{
	RenderClipSel		->SetCheck(data->ClipMode==1);
	RenderClipOutside	->SetCheck(data->ClipMode==0);
	RenderNoClip		->SetCheck(data->ClipMode==2);
}

void RenderDialog::SetMaskRadios()
{
	MaskOut	->SetCheck(data->MaskInside==1);
	MaskIn	->SetCheck(data->MaskInside==0);
	NoMask	->SetCheck(data->MaskInside==2);
}

void RenderDialog::SetAntiRadios()
{
	AntiNormal	->SetCheck(data->AntiAliasing==0);
	AntiMedium	->SetCheck(data->AntiAliasing==1);
	AntiHigh	->SetCheck(data->AntiAliasing==2);
}

#define EnableCtrl(a,b) { HWND hDlg=GetDlgItem(myhwnd,a); EnableWindow(hDlg,b); }

void RenderDialog::EnableControls(int CMode)
{
	if( CMode==3 ) {
		Col1->Enable();
		Col2->Enable();
	}
	else {
		Col1->Disable();
		Col2->Disable();
	}
	EnableCtrl(IDC_ToleranceSpin,CMode==3);
	EnableCtrl(IDC_ToleranceEdit,CMode==3);

	EnableCtrl(IDC_XEdit,CMode==0);
	EnableCtrl(IDC_XSpin,CMode==0);
	EnableCtrl(IDC_YEdit,CMode==0);
	EnableCtrl(IDC_YSpin,CMode==0);
}

void RenderDialog::SetSourceRadios()
{
	SourceSel->SetCheck(data->SourceMode==0);
	SourceInt->SetCheck(data->SourceMode==1);
	SourceExt->SetCheck(data->SourceMode==2);
	SourceCol->SetCheck(data->SourceMode==3);

	EnableControls(data->SourceMode);
}

void RenderDialog::SetVars()
{
	SetClipRadios();
	SetMaskRadios();
	SetAntiRadios();
	SetSourceRadios();

	Multi		->SetCheck(!data->Single);
	Centroid	->SetCheck(!data->CSE);
	Colourise	->SetCheck(data->Colourise);

	XPos		->SetValue(data->PosX,FALSE);
	YPos		->SetValue(data->PosY,FALSE);
	ImageScale	->SetValue(data->ImageScale,FALSE);
	Tolerance	->SetValue(data->Tolerance,FALSE);
	ExtSmooth	->SetValue(data->ExternalSmoothness,FALSE);
	IntSmooth	->SetValue(data->InternalSmoothness,FALSE);
	ColSmooth	->SetValue(data->ColouriseSmoothness,FALSE);

	Col1		->SetColor(RGB(data->Col1.r,data->Col1.g,data->Col1.b),FALSE);
	Col2		->SetColor(RGB(data->Col2.r,data->Col2.g,data->Col2.b),FALSE);
}

void RenderDialog::GetVars()
{
	COLORREF col;

	data->PosX=				XPos->GetFVal();
	data->PosY=				YPos->GetFVal();
	data->ImageScale=		ImageScale->GetFVal();
	data->Tolerance=		Tolerance->GetFVal();
	data->ExternalSmoothness=ExtSmooth->GetFVal();
	data->InternalSmoothness=IntSmooth->GetFVal();
	data->ColouriseSmoothness=ColSmooth->GetFVal();

	col=Col1->GetColor();
	data->Col1.r=GetRValue(col);
	data->Col1.g=GetGValue(col);
	data->Col1.b=GetBValue(col);
	col=Col2->GetColor();
	data->Col2.r=GetRValue(col);
	data->Col2.g=GetGValue(col);
	data->Col2.b=GetBValue(col);
}

/////////////////////////////////////////////////////////////////
//
// Utility routines for preview
//

void RenderDialog::PhotoToImage(int& x,int& y)
{
	x=(int)((float)x/data->ImageScale);
	y=(int)((lyres-y-1)/data->ImageScale);
}

unsigned char RenderDialog::MaskVal(int x,int y)
{
	return 255-MainFilter->mbuffer[y*data->maskdata->Width+x];
}

void RenderDialog::PrepareColourVector()
{
	COLORREF colour1,colour2;

	colour1=Col1->GetColor();
	colour2=Col2->GetColor();

	basex=(float)GetRValue(colour1);
	colorx=(float)GetRValue(colour2)-basex;
	basey=(float)GetGValue(colour1);
	colory=(float)GetGValue(colour2)-basey;
	basez=(float)GetBValue(colour1);
	colorz=(float)GetBValue(colour2)-basez;

	colorlen=(float)sqrt(colorx*colorx+colory*colory+colorz*colorz);
	colorx/=colorlen;
	colory/=colorlen;
	colorz/=colorlen;
}

int RenderDialog::IsSource(int x,int y)
{
	switch(data->SourceMode) {
	case 0:							// from point
		if( (int)XImageCoord==x && (int)YImageCoord==y )
				return TRUE;
		else	return FALSE;
	case 1:							// selected
		if( MaskVal(x,y)==0 ) return TRUE;
		else				  return FALSE;
	case 2:							// external of selected
		if( MaskVal(x,y)==255 ) return TRUE;
		else					return FALSE;
	case 3:							// by colour
		{
			float x0,y0,z0;
			float x1,y1,z1;

			x1=(float)MainFilter->Img_R[y][x]-basex;
			y1=(float)MainFilter->Img_G[y][x]-basey;
			z1=(float)MainFilter->Img_B[y][x]-basez;

			x0=x1*colorx+y1*colory+z1*colorz;
			if( x0<0 || x0>colorlen ) return FALSE;

			x0=colory*z1-colorz*y1;
			y0=colorz*x1-colorx*z1;
			z0=colorx*y1-colory*x1;

			if( sqrt(x0*x0+y0*y0+z0*z0)<data->Tolerance ) return TRUE;
			else return FALSE;
		}

	default: return FALSE;
	}
}

void RenderDialog::ShowBitmap()
{
	HDC		hdc=GetDC(hwPreview);
	RECT	rect;
	int		x=data->ScrollX;
	int		y=data->ScrollY;

	GetClientRect(hwPreview,&rect);
	HRGN	region=CreateRectRgnIndirect(&rect);
	SelectClipRgn(hdc,region);

	SetDIBitsToDevice(hdc,x,y,xsize,lyres,0,0,0,lyres,hwPreviewMem,
			                      &hwPreviewInfo,DIB_RGB_COLORS);
	DeleteObject(region);
	ReleaseDC(hwPreview,hdc);
}


void RenderDialog::RenderBackground(uchar** R,uchar** G,uchar** B,uchar** A,int Sourcing,int Clipping,int Reverse)
{
	char*			tmp=hwPreviewMem;
	unsigned char	r,g,b;
	int				x,y;
	int				xpos,ypos;
	float			Factor;

	if( data->SourceMode==3 ) PrepareColourVector();

	for(y=0;y<lyres;y++)
		for(x=0;x<lxres;x++) {

			// get photoshop image coords and data pointer
			xpos=x;ypos=y;
			PhotoToImage(xpos,ypos);
			// get r,g,b values
			r=MainFilter->Img_R[ypos][xpos];
			g=MainFilter->Img_G[ypos][xpos];
			b=MainFilter->Img_B[ypos][xpos];

			// modify colour if necessary
			if( Sourcing && Clipping && data->SourceMode!=0 )
				if( IsSource(xpos,ypos) ) { r=128+r/2;g=g/2;b=b/2; }
			if( Clipping ) {
				Factor=1.0f-(float)MaskVal(xpos,ypos)/255.0f;
				switch(data->ClipMode) {
				case 1:	break;						// clip to mask
				case 0: Factor=1.0f-Factor;break;	// clip to outside mask
				case 2: Factor=1.0f;break;			// no clip
				}
				r=(unsigned char)(0.666*r*Factor+0.333*r);
				g=(unsigned char)(0.666*g*Factor+0.333*g);
				b=(unsigned char)(0.666*b*Factor+0.333*b);
			}

			// into arrays
			if( Reverse ) ypos=lyres-1-y;
			else		  ypos=y;
			R[ypos][x]=r;
			G[ypos][x]=g;
			B[ypos][x]=b;
			A[ypos][x]=0;
		}

}

/////////////////////////////////////////////////////////////////
//
// Full Preview
//

void RenderDialog::FullPreview()
{
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	ShowWhat=1;
	GetVars();

	uchar **TempImgR,**TempImgG,**TempImgB,**TempImgA;

	// get memory for flare render
	Alloc2D(TempImgR,uchar,lyres,lxres);
	Alloc2D(TempImgG,uchar,lyres,lxres);
	Alloc2D(TempImgB,uchar,lyres,lxres);
	Alloc2D(TempImgA,uchar,lyres,lxres);

	// fill in background
	RenderBackground(TempImgR,TempImgG,TempImgB,TempImgA,FALSE,FALSE,TRUE);

	MainFilter->RenderPreviewToArray(TempImgR,TempImgG,TempImgB,TempImgA,lxres,lyres,xsize);

	// put into bitmap
	char *tmp=hwPreviewMem;
	int x,y,xpos,ypos;

	for(y=0;y<lyres;y++)
		for(x=0;x<lxres;x++) {
			xpos=x;ypos=y;
			PhotoToImage(xpos,ypos);
			*(tmp++)=(unsigned char)(TempImgB[lyres-y-1][x]);
			*(tmp++)=(unsigned char)(TempImgG[lyres-y-1][x]);
			*(tmp++)=(unsigned char)(TempImgR[lyres-y-1][x]);
		}

	// free mem
	Free2D(TempImgR,lyres);
	Free2D(TempImgG,lyres);
	Free2D(TempImgB,lyres);
	Free2D(TempImgA,lyres);

	SetCursor(LoadCursor(NULL,IDC_ARROW));

	ShowBitmap();
}

/////////////////////////////////////////////////////////////////
//
// Background Preview
//

void RenderDialog::BackPreview()
{
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	GetVars();
	ShowWhat=0;

	uchar **TempImgR,**TempImgG,**TempImgB,**TempImgA;

	// get memory for background render
	Alloc2D(TempImgR,uchar,lyres,lxres);
	Alloc2D(TempImgG,uchar,lyres,lxres);
	Alloc2D(TempImgB,uchar,lyres,lxres);
	Alloc2D(TempImgA,uchar,lyres,lxres);

	// fill in background
	RenderBackground(TempImgR,TempImgG,TempImgB,TempImgA,TRUE,TRUE,FALSE);

	// put into bitmap
	char *tmp=hwPreviewMem;

	int x,y;
	for(y=0;y<lyres;y++)
		for(x=0;x<lxres;x++) {
			*(tmp++)=TempImgB[y][x];
			*(tmp++)=TempImgG[y][x];
			*(tmp++)=TempImgR[y][x];
		}

	// free mem
	Free2D(TempImgR,lyres);
	Free2D(TempImgG,lyres);
	Free2D(TempImgB,lyres);
	Free2D(TempImgA,lyres);

	SetCursor(LoadCursor(NULL,IDC_ARROW));

	ShowBitmap();

	RenderOutline();
}

///////////////////////////////////////////////////////////////////
//
// Draw Flare Outline
//

void RenderDialog::RenderOutline()
{
	if( data->SourceMode!=0 ) return;		// only outline in single point mode

	HDC		hdc = GetDC(hwPreview);
	RECT	rect;

	GetVars();
	ShowWhat=0;
	
	// Clip Region
	GetClientRect(hwPreview,&rect);
	int		a=max(0,data->ScrollX);
	int		b=max(0,data->ScrollY);
	int		c=min(rect.right,data->ScrollX+xsize);
	int		d=min(rect.bottom,data->ScrollY+lyres);
	HRGN	ClipRegion=CreateRectRgn(a,b,c,d);
	SelectClipRgn(hdc,ClipRegion);

	// Lotsa Nice Pens
	HPEN	CirclePen2 =CreatePen(PS_SOLID,0,RGB(255,128,128));
	HPEN	CrossPen=CreatePen(PS_SOLID,0,RGB(255,0,0));
	HPEN	OldPen=(HPEN)SelectObject(hdc,CirclePen2);
	SetROP2(hdc,R2_COPYPEN);

	// Compute Centre
	

	float FlareX=(float)(XImageCoord*data->ImageScale);
	float FlareY=(float)(YImageCoord*data->ImageScale);


	// Draw All Elements
	for(int j=0;j!=data->LensFlare.GetNoFlares();j++)
		{  Flare *Local=data->LensFlare.GetFlare(j);
				
				   
	 	if (TRUE) 
		{ 
			float xps=(0.5f+Local->PositionX*0.5f)*lxres;
			float yps=(0.5f+Local->PositionY*0.5f)*lyres;
											  
			// Draw Selected Element ?
			SelectObject(hdc,CirclePen2);

			// Draw Element
			if ( /*Local->IsVisible(data->CameraDepth)*/ TRUE )
			{	Local->PreProcess(lxres,lyres,FlareX,FlareY,data->CameraDepth,(float)data->AnimationPosition,0.0f);

				float xpos=Local->RenderCX;
				float ypos=Local->RenderCY;

				float AngleAdd=Local->AngleOffSet*3.14159f;
				if (Local->ReAlign)	{ 
					float ScreenCX=(0.5f+Local->PositionX*0.5f)*lxres;
					float ScreenCY=(0.5f+Local->PositionY*0.5f)*lyres;
					AngleAdd+=(float)atan2(ypos-ScreenCY,xpos-ScreenCX);
				}

				int		i=0;
				float	Angle=AngleAdd,dAngle=6.283185f/RealTimePreview_AngleAdd;
				float	Amult=lxres*Local->RenderSize;
											  
				float	tmp; Local->GetShape(tmp,i);
				float	x=xpos+tmp*Amult*(float)cos(Angle);
				float	y=ypos+tmp*Amult*(float)sin(Angle);
				MoveToEx(hdc,(int)x+data->ScrollX,(int)y+data->ScrollY,NULL);

				for(;i<FLARERESOLUTION;i+=RealTimePreview_Add,Angle+=dAngle)
				{ 
					Local->GetShape(tmp,i);
					x=xpos+tmp*Amult*(float)cos(Angle);
					y=ypos+tmp*Amult*(float)sin(Angle);

					LineTo(hdc,(int)x+data->ScrollX,(int)y+data->ScrollY);
				}

				Local->GetShape(tmp,0); 
				x=xpos+tmp*Amult*(float)cos(Angle);
				y=ypos+tmp*Amult*(float)sin(Angle);
				LineTo(hdc,(int)x+data->ScrollX,(int)y+data->ScrollY);
			}
		}
	}

	SetROP2(hdc,R2_XORPEN);
	SelectObject(hdc,CrossPen);
	MoveToEx(hdc,(int)FlareX-5+data->ScrollX,(int)FlareY+data->ScrollY,NULL);
	LineTo(hdc,(int)FlareX+6+data->ScrollX,(int)FlareY+data->ScrollY);
	MoveToEx(hdc,(int)FlareX+data->ScrollX,(int)FlareY-5+data->ScrollY,NULL);
	LineTo(hdc,(int)FlareX+data->ScrollX,(int)FlareY+6+data->ScrollY);

	DeleteObject(ClipRegion);
	SelectObject(hdc,OldPen);
	DeleteObject(CirclePen2);
	DeleteObject(CrossPen);

	ReleaseDC(hwPreview,hdc);
}

void RenderDialog::OutlinePreview()
{
	ShowBitmap();
	RenderOutline();
}

/////////////////////////////////////////////////////////////////
//
// Scrollbar handler
//

void RenderDialog::Scroll(HWND hwnd,WPARAM wParam,int& pos,int min)
{
	switch(LOWORD(wParam)) {
	case SB_PAGEUP:
		pos+=50;break;
	case SB_PAGEDOWN:
		pos-=50;break;
	case SB_LINEUP:
		pos+=5;break;
	case SB_LINEDOWN:
		pos-=5;break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		pos=-HIWORD(wParam);break;
	}
	if( pos>0 ) pos=0;
	if( pos<min ) pos=min;
	SetScrollPos(hwnd,SB_CTL,-pos,TRUE);

	ShowBitmap();
	if( !ShowWhat ) RenderOutline();
}
/////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
//
BOOL CALLBACK RenderMasterCtrlDlgProc(HWND,UINT,WPARAM,LPARAM);	

RenderDialog::RenderDialog(HWND hwnd,LensFDATA* d,ImageFilter_LensF* pfilter,int& retval)
{
	data=d;
	MainFilter=pfilter;
	retval=DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_RENDERPARAMETERS),hwnd,(DLGPROC)RenderMasterCtrlDlgProc,(LPARAM)this);
}

RenderDialog::~RenderDialog()
{
	ReleaseControls();
}

//////////////////////////////////////////////////////////////////
//
// PREVIEW WINDOW CALLBACK
//
static RenderDialog* rd = NULL;

BOOL RenderDialog::PreviewCallback(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message) {
	case WM_PAINT:
		{
			DefWindowProc(hWnd,message,wParam,lParam);
			ShowBitmap();
			if( ShowWhat==0 ) RenderOutline();
			return TRUE;
		}

	case WM_MOUSEMOVE:
		if( !(wParam&MK_LBUTTON) )
			return TRUE;
		if( !dragging ) {
			dragging=TRUE;
			SetCapture(hWnd);
			SetCursor(LoadCursor(NULL,IDC_CROSS));
		}
		// run through to implement mouse position
	case WM_LBUTTONDOWN:
		{
			float xpos=(float)(short)LOWORD(lParam);
			float ypos=(float)(short)HIWORD(lParam);

			data->PosX=2.0f*(xpos-(float)data->ScrollX)/(float)(xsize-1) - 1.0f;
			data->PosY=2.0f*(ypos-(float)data->ScrollY)/(float)(lyres-1) - 1.0f;
			XPos->SetValue(data->PosX,FALSE);
			YPos->SetValue(data->PosY,FALSE);
			OutlinePreview();
		}
		return TRUE;
	case WM_RBUTTONDOWN:
		FullPreview();
		return TRUE;
	case WM_LBUTTONUP:
		if( dragging ) {
			dragging=FALSE;
			SetCapture(NULL);
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		return TRUE;
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}
		
//////////////////////////////////////////////////////////////////
//
// MAIN WINDOW CALLBACK
//


BOOL RenderDialog::Callback(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)	
{	
	int Changed=-1;

	switch(message) {

	case WM_COMMAND:
	  if( HIWORD(wParam)==BN_CLICKED )
		switch(LOWORD(wParam)) {

		case IDC_RenderClipSel:
			data->ClipMode=1;SetClipRadios();
			Changed=4;break;
		case IDC_RenderClipOutside:
			data->ClipMode=0;SetClipRadios();
			Changed=4;break;
		case IDC_RenderNoClip:
			data->ClipMode=2;SetClipRadios();
			Changed=4;break;

		case IDC_MaskOut:
			data->MaskInside=1;SetMaskRadios();
			Changed=3;break;
		case IDC_MaskIn:
			data->MaskInside=0;SetMaskRadios();
			Changed=3;break;
		case IDC_NoMask:
			data->MaskInside=2;SetMaskRadios();
			Changed=3;break;

		case IDC_AntiNormal:
			data->AntiAliasing=0;SetAntiRadios();
			Changed=3;break;
		case IDC_AntiMedium:
			data->AntiAliasing=1;SetAntiRadios();
			Changed=3;break;
		case IDC_AntiHigh:
			data->AntiAliasing=2;SetAntiRadios();
			Changed=3;break;

		case IDC_SourceSel:
			data->SourceMode=0;SetSourceRadios();
			Changed=2;break;
		case IDC_SourceInt:
			data->SourceMode=1;SetSourceRadios();
			Changed=2;break;
		case IDC_SourceExt:
			data->SourceMode=2;SetSourceRadios();
			Changed=2;break;
		case IDC_SourceCol:
			data->SourceMode=3;SetSourceRadios();
			Changed=2;break;


		case IDC_RenderReal:
			Changed=3;
			data->RealTime=(SendDlgItemMessage(hWnd,IDC_RenderReal,BM_GETCHECK,0,0)==BST_CHECKED);
			break;
		case IDC_ScaleToFit:
			{
				RECT rect;
				GetClientRect(hwPreview,&rect);
				float yscale=(float)(rect.bottom-rect.top)/(float)(data->ImageHeight);
				float xscale=(float)(rect.right-rect.left)/(float)(data->ImageWidth);
				if( yscale<xscale)	ImageScale->SetValue(yscale,FALSE);
				else				ImageScale->SetValue(xscale,FALSE);
				SetUpMemAndScroll(hWnd);
				if( data->RealTime ) FullPreview();
				else				 BackPreview();
			}
			break;

		case IDC_CLOSE:
			EndDialog(hWnd,1);
			return TRUE;
		case IDC_RenderExit:
			EndDialog(hWnd,0);
			return TRUE;
	  } else switch( LOWORD(wParam) ) {
		case IDC_Multi:
			if( HIWORD(wParam)==BN_BUTTONDOWN ) data->Single=FALSE;
			if( HIWORD(wParam)==BN_BUTTONUP   ) data->Single=TRUE;
			Changed=3;break;
		case IDC_Centroid:
			if( HIWORD(wParam)==BN_BUTTONDOWN ) data->CSE=0;
			if( HIWORD(wParam)==BN_BUTTONUP   ) data->CSE=1;
			Changed=3;break;
		case IDC_Colourise:
			if( HIWORD(wParam)==BN_BUTTONDOWN ) data->Colourise=1;
			if( HIWORD(wParam)==BN_BUTTONUP   ) data->Colourise=0;
			Changed=3;break;
	  }
		break;
	
	case CC_SPINNER_CHANGE:
		switch(LOWORD(wParam)) {
		case IDC_XSpin:
		case IDC_YSpin:
			Changed=1;
			break;
		case IDC_ExtSpin:
		case IDC_IntSpin:
		case IDC_ColSpin:
			if( HIWORD(wParam)==0 )
				Changed=3;
			break;
		case IDC_ToleranceSpin:
			if( HIWORD(wParam)==0 )
				Changed=2;
			break;
		case IDC_ImageSizeSpin:
			if( HIWORD(wParam)==0 ) {
				SetUpMemAndScroll(hWnd);
				if( data->RealTime ) FullPreview();
				else				 BackPreview();
			}
			break;
		}
		break;

	case CC_COLOR_CHANGE:
		Changed=2;break;

	case WM_HSCROLL:
		if( GetDlgItem(hWnd,IDC_HSCROLLBAR)==(HWND)lParam )
			Scroll((HWND)lParam,wParam,data->ScrollX,ScrollXMin);
		break;
	case WM_VSCROLL:
		if( GetDlgItem(hWnd,IDC_VSCROLLBAR)==(HWND)lParam )
			Scroll((HWND)lParam,wParam,data->ScrollY,ScrollYMin);
		break;

	default:
		return FALSE;
	}

	// update view
	switch( Changed ) {
	case 1:						// outline only
		if( data->RealTime )	{ FullPreview();Showing=TRUE; }
		else					{ OutlinePreview();Showing=FALSE; }
		break;
	case 2:						// background view
		BackPreview();
		Showing=FALSE;
		break;
	case 3:						// full preview
		if( data->RealTime )	{ FullPreview();Showing=TRUE; }
		else					GetVars();
		break;
	case 4:
		if( data->RealTime )	{ FullPreview();Showing=TRUE; }
		else					{ BackPreview();Showing=FALSE; }
		break;
	}

	return Changed!=-1;
}

// callback for preview window
BOOL CALLBACK PreviewCallbackProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	if( rd!=NULL )
		return rd->PreviewCallback(hWnd,message,wParam,lParam);
	return FALSE;
}

// callback for master window
BOOL CALLBACK RenderMasterCtrlDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	if( message==WM_INITDIALOG ) {
		rd=(RenderDialog*)lParam;
		rd->myhwnd=hWnd;
		rd->InitControls(hWnd);
		return TRUE;
	}

	if( message == WM_PAINT ) {
		 PAINTSTRUCT ps;
		 BeginPaint(hWnd,&ps);
		 EndPaint(hWnd,&ps);
		 return TRUE;
	 }

	 if( message == WM_ERASEBKGND ) {
		 HDC hdc=(HDC)wParam;
		 RECT	rect;
		 GetClientRect(hWnd,&rect);
		 HDC hdcBitmap=CreateCompatibleDC(hdc);
		 SelectObject(hdcBitmap,rd->RenderBackBitmap);
		 BitBlt(hdc,0,0,rect.right,rect.bottom,hdcBitmap,0,0,SRCCOPY);
		 DeleteDC(hdcBitmap);
		 return TRUE;
	}

	if( rd==NULL ) return FALSE;
	return rd->Callback(hWnd,message,wParam,lParam);
}

BOOL ImageFilter_LensF::ShowRenderControl(HWND hWnd) 
{
	int retval;
	RenderDialog(hWnd,&data,this,retval);
	return retval;
}


#undef RealTimePreview_AngleAdd
#undef RealTimePreview_Add
