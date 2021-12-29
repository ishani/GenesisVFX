/////////////////////////////////////////////////////////////
//
// File: PhotoUtil.cpp
// R. C. Wilson 12/3/97
//

// convert a string to a float
float TextToFloat(char *s)
{
	float f=0.0f;

	f=(float)atof(s);
	return f;
}

// put a float as a number in an edit box
// uses inc to determine how many decimal places
void PutFloatInEdit(HWND edit,float f,float inc)
{
	char s[40];
	char *t;
	int places,pnt;

	// how many decimal places in inc
	places=(int)(-log10(inc)+1.99);

	// convert number to string
	sprintf(s,"%f",f);

	// copy number off as requisite number of places
	t=s;
	pnt=FALSE;
	while( *t++!=0 )
		if( pnt ) {
			if( (--places)==0 ) *t=0;
		} else {
			if( *t=='.' ) pnt=TRUE;
		}
		
	// set the edit box text
	SendMessage(edit,WM_SETTEXT,0,(LPARAM)s);
}


// Put an int in an edit box
void PutIntInEdit(HWND edit,int i)
{
	char s[40];

	sprintf(s,"%d",i);
	SendMessage(edit,WM_SETTEXT,0,(LPARAM)s);
}


// use photoshop routine to select colour
COLORREF ChooseColor(char* title,COLORREF col)
{
	static ColorServicesInfo csinfo;
	int red,green,blue;

	// set colour
	csinfo.colorComponents[0]=GetRValue(col);
	csinfo.colorComponents[1]=GetGValue(col);
	csinfo.colorComponents[2]=GetBValue(col);
	csinfo.colorComponents[3]=0;

	// select type of service=choose colour
	csinfo.selector = plugIncolorServicesChooseColor;

	// select colour space for selection
	csinfo.sourceSpace = 0;			// RGB?
	csinfo.resultSpace = 0;

	// set some other data
	csinfo.reservedSourceSpaceInfo = NULL;			// must be null
	csinfo.reservedResultSpaceInfo = NULL;			// must be null
	csinfo.reserved = NULL;							// must be null
	csinfo.selectorParameter.pickerPrompt = NULL;	// not needed
	csinfo.infoSize = sizeof(csinfo);

	// call routine
	if( ColorServices(&csinfo)==noErr ) {

		// recover new colour
		red=csinfo.colorComponents[0];
		green=csinfo.colorComponents[1];
		blue=csinfo.colorComponents[2];
		return RGB(red,green,blue);

	}	

	// error or cancel
	return col;
}


// centre dialog wrt photoshop main window
void CenterWindow(HWND me,HWND parent)
{
	CenterDialog(me);
}





int AbortIt()
{
	return TestAbort();
}

unsigned int HardwareLockID()
{
	return (unsigned int)gStuff->serialNumber;
}

/////////////////////////////////////////////////////////////////////////////
//
// little status window
//

HWND StatusDlg;


BOOL CALLBACK ProgDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
		return FALSE;
}

void StatusCreate(HWND hwnd)
{
	if( StatusDlg==NULL ) StatusDlg=CreateDialog(hInst,MAKEINTRESOURCE(IDD_PROGRESS),hwnd,ProgDlgProc);
	CenterDialog(StatusDlg);
}

void StatusHide()
{
	if( StatusDlg==NULL ) return;
	ShowWindow(StatusDlg,SW_HIDE);
}

// set status text
void StatusText(char *s)
{
	
	if( StatusDlg==NULL ) return;
	ShowWindow(StatusDlg,SW_SHOW);
	SetDlgItemText(StatusDlg,IDC_PROGRESS_TEXT,s);
	RedrawWindow(StatusDlg,NULL,NULL,0);
}

// set progress
void StatusProgress(int done,int total)
{
	UpdateProgress((long)done,(long)total);

	if( StatusDlg==NULL ) return;

	ShowWindow(StatusDlg,SW_SHOW);
	SendDlgItemMessage(StatusDlg,IDC_PROGRESS_BAR,PBM_SETRANGE,0,MAKELPARAM(0,total));
	SendDlgItemMessage(StatusDlg,IDC_PROGRESS_BAR,PBM_SETPOS,done,0);
	RedrawWindow(StatusDlg,NULL,NULL,0);
}

void StatusKill()
{
	if( StatusDlg!=NULL ) DestroyWindow(StatusDlg);
	StatusDlg=NULL;
	UpdateProgress(0,0);
}

void SimpleStatus(int done,int total)
{	
	UpdateProgress((long)done,(long)total);
}
