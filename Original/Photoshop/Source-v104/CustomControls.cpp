///////////////////////////////////////////////////////////////
//
// File: CustomControls.cpp
// R. C. Wilson 12/3/97
//
///////////////////////////////////////////////////////////////
#define STRICT				// needed for strict types in c++
#include <windows.h>

#include "CustomControls.h"
#include "PhotoUtil.h"


/////////////////////////////////////
//
// non-classed routines for ICustButton
// creator, destructor, callback
//

ICustButton* GetICustButton(HWND hCtrl)
{
	ICustButton*	ICB;
	long style;

	// Create a button and set the oldCallback member
	ICB=new ICustButton(hCtrl,GetWindowLong(hCtrl,GWL_WNDPROC));
	if( ICB==NULL ) return NULL;

	// Set the new callback and a pointer to the new button
	SetWindowLong(hCtrl,GWL_USERDATA,(LONG)ICB);
	SetWindowLong(hCtrl,GWL_WNDPROC,(LONG)ICustButtonCallback);

	// since we have no images yet, set to no owner-draw
	style=GetWindowLong(hCtrl,GWL_STYLE);
	style=(style|BS_OWNERDRAW)^BS_OWNERDRAW;
	SetWindowLong(hCtrl,GWL_STYLE,style);

	return ICB;
}


void ReleaseICustButton(ICustButton *ICB)
{
	if( ICB==NULL ) return;
	SetWindowLong(ICB->hWnd,GWL_USERDATA,0);
	SetWindowLong(ICB->hWnd,GWL_WNDPROC,ICB->oldCallback);
	delete ICB;
}


BOOL CALLBACK ICustButtonCallback(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ICustButton* ICB;
	BOOL retval;

	// retrieve class pointer
	ICB=(ICustButton*)GetWindowLong(hWnd,GWL_USERDATA);
	if( ICB==NULL ) return FALSE;

	// If paint message and we have images, paint and return
	if( msg==WM_PAINT && !(ICB->hImages==NULL)) {
		ICB->Paint();
		return TRUE;
	}

	// not interested in button up unless we have got buttondown
	if( msg==WM_LBUTTONUP && !ICB->buttondown ) return FALSE;

	// test messages to detect changes in button state
	switch(msg) {

	case WM_LBUTTONDOWN:		// left mbutton clicked

		ICB->buttondown=TRUE;

		if( ICB->type==CBT_CHECK ) ICB->state=!ICB->state;
		else ICB->state=1;

		ICB->Paint();

		if( ICB->state==0 ) ICB->NotifyParent(BN_BUTTONUP);
		else				ICB->NotifyParent(BN_BUTTONDOWN);

		break;

	case WM_LBUTTONUP:			// left mbutton release

		ICB->buttondown=FALSE;

		if( ICB->type==CBT_PUSH ) {
			ICB->state=0;
			ICB->Paint();
			ICB->NotifyParent(BN_BUTTONUP);
		}

		break;

	case BM_SETCHECK:			// on setcheck must repaint the button
								// otherwise user sees no effect
		if( ICB->hImages!=NULL ) ICB->Paint();
//		else					 InvalidateRect(hWnd,NULL,FALSE);
		break;
	}


	// call original proc
	retval=CallWindowProc((WNDPROC)ICB->oldCallback,hWnd,msg,wParam,lParam);

	if( ICB->type==CBT_CHECK && msg==WM_LBUTTONUP )			// force state of button
			CallWindowProc((WNDPROC)ICB->oldCallback,hWnd,BM_SETCHECK,(WPARAM)ICB->state,0);
	return retval;				// result of old callback
}


//////////////////////////////////////////////////////////////
//
// Painting function for ICustButton
//

void ICustButton::Paint()
{
	HDC hdc=GetDC(hWnd);
	int offset=state;
	RECT r;

	// don't use custom paint if no user-defined images
	if( hImages==NULL ) {
		ReleaseDC(hWnd,hdc);
		return;
	}
							
	// set up button drawing style
	UINT style=DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;
	if( state ) style|=DFCS_PUSHED;
	if( !IsWindowEnabled(hWnd) ) style|=DFCS_INACTIVE;
	
	// Draw a blank button
	GetClientRect(hWnd,&r);
	DrawFrameControl(hdc,&r,DFC_BUTTON,style);

	// Clip region so we don't overwrite button border
	HRGN hRgn=CreateRectRgn(2,2,r.right,r.bottom);
	SelectClipRgn(hdc,hRgn);

	// find which image to draw
	if( !IsWindowEnabled(hWnd) ) offset+=2;

	// calc position
	int x,y;
	ImageList_GetIconSize(hImages,&x,&y);
	x=(r.right-x)/2;
	y=(r.bottom-y)/2;

	// draw
	ImageList_Draw(hImages,index[offset],hdc,x,y,ILD_NORMAL);

	// clean up
	ValidateRect(hWnd,NULL);
	DeleteObject(hRgn);
	ReleaseDC(hWnd,hdc);
}

////////////////////////////////////////////////////////////
//
// User access functions
//


void ICustButton::SetImage(HIMAGELIST hImageList,int i0,int i1,int i2,int i3,int w,int h)
{
	// store data in class
	hImages=hImageList;
	index[ICB_INENABLE]=i0;
	index[ICB_OUTENABLE]=i1;
	index[ICB_INDISABLE]=i2;
	index[ICB_OUTDISABLE]=i3;

	// we have got some images; set button to owner draw
	long style;
	style=GetWindowLong(hWnd,GWL_STYLE);
	SetWindowLong(hWnd,GWL_STYLE,style|BS_OWNERDRAW);
}


void ICustButton::SetType(int t)
{
	// get old button style
	long style=GetWindowLong(hWnd,GWL_STYLE);

	// remember type
	type=t;

	if( type==CBT_PUSH ) {
		
		// If push button style is PUSHBUTTON, not PUSHLIKE or AUTOCHECKBOX
		style=(style|BS_PUSHLIKE|BS_AUTOCHECKBOX)^(BS_PUSHLIKE|BS_AUTOCHECKBOX);
		style|=BS_PUSHBUTTON;

	} else {

		// if check button
		style=(style|BS_PUSHBUTTON)^BS_PUSHBUTTON;
		style=style|BS_PUSHLIKE|BS_AUTOCHECKBOX;

	}

	// store new style
	SetWindowLong(hWnd,GWL_STYLE,style);
}


void ICustButton::SetCheck(BOOL checked)
{

	if( checked )	state=1;
	else			state=0;

	// change actual state of button
	SendMessage(hWnd,BM_SETCHECK,(WPARAM)state,0);
}


BOOL ICustButton::IsChecked()
{
	return state;
}

////////////////////////////////////////////////////////////
//
// Messages
//

void ICustButton::NotifyParent(int msg)
{
	// get our window ID
	int ID=GetDlgCtrlID(hWnd);
	if( ID==0 ) return;
	
	// get parent window
	HWND parent=GetParent(hWnd);
	if( parent==NULL ) return;

	// send message to parent window
	PostMessage(parent,WM_COMMAND,MAKELONG((WORD)ID,(WORD)msg),(LPARAM)this);
}


/////////////////////////////////////////////////////////////
//
// constructor
//

ICustButton::ICustButton(HWND hwnd,LONG callback)
{
	hWnd=hwnd;
	oldCallback=callback;

	SetType(CBT_PUSH);
	state=0;

	index[0]=index[1]=index[2]=index[3]=0;
	hImages=0;

	buttondown=FALSE;
}










///////////////////////////////////////////////////////////////
//
// ISpinnerControl stuff
//
///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
//
// Internals
//

void ISpinnerControl::CheckValAndSet(float val)
{
	// check range and set
	if( val>max )	{
		val=max;
		SendMessage(hWnd,UDM_SETPOS,0,(LPARAM)MAKELONG((short)((max-min)/inc),0));
	}
	if( val<min ) {
		val=min;
		SendMessage(hWnd,UDM_SETPOS,0,(LPARAM)MAKELONG((short)0,0));
	}

	// put in buddy edit
	if( buddy==NULL ) return;
	if( type==SCT_FLOAT )	PutFloatInEdit(buddy,val,inc);
	else					PutIntInEdit(buddy,(int)val);
}

void ISpinnerControl::ChangeInSpinnerVal()
{
	// call when a change in value is suspected
	CheckValAndSet((float)LOWORD(SendMessage(hWnd,UDM_GETPOS,0,0))*inc+min);
}

void ISpinnerControl::ChangeInEditVal()
{
	char s[40];
	float val;

	// get edit text
	SendMessage(buddy,WM_GETTEXT,38,(LPARAM)s);
	val=TextToFloat(s);

	// check range and set
	if( val>max ) val=max;
	if( val<min ) val=min;

	short debug1,debug2;
	debug1=(short)((val-min)/inc);
	debug2=LOWORD(SendMessage(hWnd,UDM_GETPOS,0,0));
	
	if( (short)((val-min)/inc)==LOWORD(SendMessage(hWnd,UDM_GETPOS,0,0)) ) return;

	SendMessage(hWnd,UDM_SETPOS,0,(LPARAM)MAKELONG((short)((val-min)/inc),0));
	NotifyParent(CC_SPINNER_CHANGE,FALSE);
}
		

/////////////////////////////////////////////////////////////
//
// user routines
//

int ISpinnerControl::GetIVal()
{
	float val;

	// get value
	val=(float)LOWORD(SendMessage(hWnd,UDM_GETPOS,0,0))*inc+min;
	return (int)val;
}


float ISpinnerControl::GetFVal()
{
	float val;

	// get value
	val=(float)LOWORD(SendMessage(hWnd,UDM_GETPOS,0,0))*inc+min;
	return val;
}


int ISpinnerControl::Enable()
{
	return EnableWindow(hWnd,TRUE)&&
		((buddy==NULL)?TRUE:EnableWindow(buddy,TRUE));
}

int ISpinnerControl::Disable()
{
	return EnableWindow(hWnd,FALSE)&&
		((buddy==NULL)?TRUE:EnableWindow(buddy,FALSE));
}

void ISpinnerControl::SetValue(float val,int notify)
{
	// set value in spinner
	SendMessage(hWnd,UDM_SETPOS,0,(LPARAM)MAKELONG((short)((val-min)/inc),0));

	// check range and set edit
	CheckValAndSet(val);

	// tell parent
	if( notify ) NotifyParent(CC_SPINNER_CHANGE,drag);
}


void ISpinnerControl::SetValue(int val,int notify)
{
	// set value in spinner
	SendMessage(hWnd,UDM_SETPOS,0,(LPARAM)MAKELONG((short)(((float)val-min)/inc),0));

	// check range and set edit
	CheckValAndSet((float)val);

	// tell parent
	if( notify ) NotifyParent(CC_SPINNER_CHANGE,drag);
}


void ISpinnerControl::Setup(HWND spin,HWND edit,int Type,float Min,float Max,float Inc,float Def)
{
	// set handles
	hWnd=spin;
	buddy=edit;
	SendMessage(spin,UDM_SETBUDDY,(WPARAM)edit,0);
	
	// set min,max
	max=Max;
	min=Min;
	if( min>max ) {
		float tmp=max;
		max=min;
		min=tmp;
		SendMessage(spin,UDM_SETRANGE,0,MAKELONG((short)0,(short)10000));
	} else {
		SendMessage(spin,UDM_SETRANGE,0,MAKELONG((short)10000,(short)0));
	}

	// check inc and set
	if( (max-min)/10000.0f>Inc ) Inc=(max-min)/10000.0f;
	inc=Inc;

	// check default
	if( Def<min ) Def=min;
	if( Def>max ) Def=max;
	def=Def;

	// set type
	type=Type;

	// set callbacks
	SetWindowLong(spin,GWL_USERDATA,(long)this);
	oldSpinnerCallback=GetWindowLong(spin,GWL_WNDPROC);
	SetWindowLong(spin,GWL_WNDPROC,(long)ISpinnerControlCallback);
	if( edit!=NULL ) {
		SetWindowLong(edit,GWL_USERDATA,(long)this);
		oldEditCallback=GetWindowLong(edit,GWL_WNDPROC);
		SetWindowLong(edit,GWL_WNDPROC,(long)ISpinnerEditCallback);
	}

	// set drag
	drag=FALSE;
	buttondown=FALSE;
}

/////////////////////////////////////////////////////////////
//
// constructor
//

ISpinnerControl::ISpinnerControl()
{
	hWnd=buddy=NULL;
	max=100.0f;
	min=0.0f;
	inc=1.0f;
	type=SCT_FLOAT;
	oldSpinnerCallback=oldEditCallback=NULL;
	drag=FALSE;

	arrows=LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS));
}

/////////////////////////////////////////////////////////////
//
// messages
//

void ISpinnerControl::NotifyParent(unsigned int mess,int drag)
{
	int ID=GetDlgCtrlID(hWnd);
	if( ID==0 ) return;
	
	HWND parent=GetParent(hWnd);
	if( parent==NULL ) return;

	PostMessage(parent,mess,MAKELONG((WORD)ID,(WORD)drag),(LPARAM)this);
}

/////////////////////////////////////////////////////////////
//
// external routines
//

ISpinnerControl* SetupFloatSpinner(HWND hdlg,int idSpin,int idEdit,float min,float max,float val,float inc,float def)
{
	HWND hspin,hedit;
	ISpinnerControl* ISC;

	// get window handles for dialog items
	hspin=GetDlgItem(hdlg,idSpin);
	if( hspin==NULL ) return NULL;
	if( idEdit!=0 ) hedit=GetDlgItem(hdlg,idEdit);

	// alloc data and initialise
	ISC=new ISpinnerControl;
	if( ISC==NULL ) return NULL;

	// setup data
	ISC->Setup(hspin,hedit,SCT_FLOAT,min,max,inc,def);
	ISC->SetValue(val,FALSE);

	// return new control
	return ISC;
}


ISpinnerControl* SetupIntSpinner(HWND hdlg,int idSpin,int idEdit,int min,int max,int val,int inc,int def)
{
	HWND hspin,hedit;
	ISpinnerControl* ISC;

	// get windows for dialog items
	hspin=GetDlgItem(hdlg,idSpin);
	if( hspin==NULL ) return NULL;
	if( idEdit!=0 ) hedit=GetDlgItem(hdlg,idEdit);

	// alloc and init data
	ISC=new ISpinnerControl;
	if( ISC==NULL ) return NULL;

	// setup data
	ISC->Setup(hspin,hedit,SCT_INT,(float)min,(float)max,(float)inc,(float)def);
	ISC->SetValue(val,FALSE);

	// return new control
	return ISC;
}


void ReleaseSpinner(ISpinnerControl* ISC)
{
	if( ISC==NULL ) return;
	// reset callback
	SetWindowLong(ISC->hWnd,GWL_USERDATA,NULL);
	SetWindowLong(ISC->hWnd,GWL_WNDPROC,ISC->oldSpinnerCallback);
	if( ISC->buddy!=NULL ) {
		SetWindowLong(ISC->buddy,GWL_USERDATA,NULL);
		SetWindowLong(ISC->buddy,GWL_WNDPROC,ISC->oldEditCallback);
	}

	// delete data
	delete ISC;
}


/////////////////////////////////////////////////////////////
//
// callbacks
//

BOOL CALLBACK ISpinnerEditCallback(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	ISpinnerControl* ISC=(ISpinnerControl*)GetWindowLong(hWnd,GWL_USERDATA);
	if( ISC==NULL ) return FALSE;

	// change in value only on KILLFOCUS
	if( msg==WM_KILLFOCUS )	{
		ISC->ChangeInEditVal();
	}
	switch(msg) {
	case WM_KEYDOWN:							// arrow keys
		if( wParam==VK_UP || wParam==VK_DOWN ) {
			ISC->ChangeInSpinnerVal();
			ISC->NotifyParent(CC_SPINNER_CHANGE,TRUE);
		}
		break;
	case WM_KEYUP:
		if( wParam==VK_UP || wParam==VK_DOWN )
			ISC->ChangeInSpinnerVal();
			ISC->NotifyParent(CC_SPINNER_CHANGE,FALSE);
		break;
	}
	// call normal proc
	return CallWindowProc((WNDPROC)ISC->oldEditCallback,hWnd,msg,wParam,lParam);
}


BOOL CALLBACK ISpinnerControlCallback(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	BOOL retval;
	ISpinnerControl* ISC=(ISpinnerControl*)GetWindowLong(hWnd,GWL_USERDATA);
	if( ISC==NULL ) return FALSE;

	// ignore button up and drag unless got button down
	if( (msg==WM_LBUTTONUP || msg==WM_MOUSEMOVE) && !ISC->buttondown ) return FALSE;

	// detect mouse dragging
	if( msg==WM_MOUSEMOVE && (wParam&MK_LBUTTON)!=0 ) {		// mouse move with left button down

		if( ISC->drag )	{									// already dragging
			ISC->SetValue(ISC->ValCentreDrag+(ISC->yDrag-(short)HIWORD(lParam))*ISC->inc,TRUE);
		}

		else	{

			// setup drag
			// pretend that button has been released to prevent more changes
			CallWindowProc((WNDPROC)ISC->oldSpinnerCallback,hWnd,WM_LBUTTONUP,0,0);

			// setup drag by capturing the mouse and changing the cursor
			ISC->drag=TRUE;
			SetCapture(hWnd);
			ISC->oldCursor=SetCursor(ISC->arrows);

		}

		return TRUE;
	}

	// detect end of drag
	if( msg==WM_LBUTTONUP && ISC->drag ) {
		ISC->drag=FALSE;
		ReleaseCapture();
		SetCursor(ISC->oldCursor);
		ISC->buttondown=FALSE;
	}

	retval=CallWindowProc((WNDPROC)ISC->oldSpinnerCallback,hWnd,msg,wParam,lParam);

	switch( msg ) {

	case WM_LBUTTONDOWN:						// clicked in control
		ISC->xDrag=LOWORD(lParam);
		ISC->yDrag=HIWORD(lParam);
		ISC->ValCentreDrag=ISC->GetFVal();
		ISC->buttondown=TRUE;

	case WM_TIMER:								// auto repeat is done by timer
		ISC->ChangeInSpinnerVal();
		ISC->NotifyParent(CC_SPINNER_CHANGE,TRUE);
		break;

	case WM_RBUTTONDOWN:						// right button: set to default
		ISC->SetValue(ISC->def,TRUE);
		break;

	case WM_LBUTTONUP:
		ISC->NotifyParent(CC_SPINNER_CHANGE,FALSE);
		ISC->NotifyParent(CC_SPINNER_BUTTONUP,FALSE);
		ISC->buttondown=FALSE;
		break;

	}
	return retval;
}




////////////////////////////////////////////////////////////////////////////////
//
// IColorSwatch
//
////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK IColorSwatchCallback(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	IColorSwatch* ICS=(IColorSwatch *)GetWindowLong(hWnd,GWL_USERDATA);
	if( ICS==NULL ) return FALSE;
	
	// repaint
	if( msg==WM_PAINT ) {
		ICS->Paint();
		ValidateRect(hWnd,NULL);
		return TRUE;
	}

	// select new colour
	if( msg==WM_LBUTTONDOWN ) {

		// color sel message before selection
		ICS->NotifyParent(CC_COLOR_SEL,0);

		// select new colour
		ICS->color=ChooseColor(ICS->DialogTitle,ICS->color);

		// repaint control
		ICS->Paint();

		// color change message
		ICS->NotifyParent(CC_COLOR_CHANGE,1);

		return TRUE;
	}

	return CallWindowProc((WNDPROC)ICS->oldButtonCallback,hWnd,msg,wParam,lParam);
}


IColorSwatch* GetIColorSwatch(HWND ctrl,COLORREF col,TCHAR* name)
{
	IColorSwatch* ICS;

	// alloc data
	ICS=new IColorSwatch;
	if( ICS==NULL ) return NULL;

	// set data values
	ICS->hWnd=ctrl;
	ICS->color=col;
	strncpy(ICS->DialogTitle,name,255);
	ICS->enabled=TRUE;

	// set callback and pointer to data
	SetWindowLong(ctrl,GWL_USERDATA,(long)ICS);
	ICS->oldButtonCallback=SetWindowLong(ctrl,GWL_WNDPROC,(long)IColorSwatchCallback);

	// return new control
	return ICS;
}


void ReleaseIColorSwatch(IColorSwatch* ICS)
{
	if( ICS==NULL ) return;

	// reset callback
	SetWindowLong(ICS->hWnd,GWL_USERDATA,NULL);
	SetWindowLong(ICS->hWnd,GWL_WNDPROC,ICS->oldButtonCallback);

	// delete data
	delete ICS;
}


/////////////////////////////////////////////////////////////
//
// repaint control
//

void IColorSwatch::Paint()
{
		HPEN	pen;
		HBRUSH	brush;
		// black pen for border
		if( enabled )	pen=(HPEN)GetStockObject(BLACK_PEN);
		else			pen=(HPEN)GetStockObject(WHITE_PEN);

		// solid brush in color for centre
		if( enabled )	brush=CreateSolidBrush(color);
		else			brush=CreateHatchBrush(HS_DIAGCROSS,RGB(96,96,96));

		RECT r;
		HDC dc=GetDC(hWnd);

		HPEN oldpen=(HPEN)SelectObject(dc,pen);
		HBRUSH oldbrush=(HBRUSH)SelectObject(dc,brush);
		GetClientRect(hWnd,&r);

		// draw rectangle
		Rectangle(dc,r.left,r.top,r.right,r.bottom);

		// clean up
		SelectObject(dc,oldpen);
		SelectObject(dc,oldbrush);
		DeleteObject(pen);
		DeleteObject(brush);
		ReleaseDC(hWnd,dc);

		ValidateRect(hWnd,NULL);
}

////////////////////////////////////////////////////////////
//
// Messages
//

void IColorSwatch::NotifyParent(int msg,int flag)
{
	// get my ID
	int ID=GetDlgCtrlID(hWnd);
	if( ID==0 ) return;
	
	// get parent window
	HWND parent=GetParent(hWnd);
	if( parent==NULL ) return;

	// send message
	SendMessage(parent,msg,MAKELONG((WORD)ID,(WORD)flag),(LPARAM)this);
}

//////////////////////////////////////////////////////////////
//
// User Interface
//

int IColorSwatch::Enable()
{
	int retval;
	enabled=TRUE;
	retval=EnableWindow(hWnd,TRUE);
	Paint();
	return retval;
}

int IColorSwatch::Disable()
{
	int retval;
	enabled=FALSE;
	retval=EnableWindow(hWnd,FALSE);
	Paint();
	return retval;
}
