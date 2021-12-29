//////////////////////////////////////////////////////////
//
// File: CustomControls.h
// R. C. Wilson	12/3/97

#define STRICT
#include <windows.h>
#include <commctrl.h>

#define InitCustomControls(a) InitCommonControls()

// messages
#define CC_SPINNER_CHANGE	(WM_USER+1)
#define BN_BUTTONDOWN		(WM_USER+2)
#define BN_BUTTONUP			(WM_USER+3)
#define CC_COLOR_CHANGE		(WM_USER+4)
#define CC_COLOR_SEL		(WM_USER+5)
#define CC_SPINNER_BUTTONUP (WM_USER+6)
//////////////////////////////////////////////////////////
//
// ICustButton
//

// button types
enum { CBT_PUSH, CBT_CHECK };

// indexes to image indexes
enum { ICB_INENABLE, ICB_OUTENABLE, ICB_INDISABLE, ICB_OUTDISABLE };	// indexes

class ICustButton {
	
	short			index[4];				// indexs to images

public:
	long			oldCallback;			// info about button
	HWND			hWnd;
	unsigned char	type;
	unsigned char	state;
	HIMAGELIST		hImages;
	int				buttondown;

public:

	// interface functions
	void			SetImage(HIMAGELIST,int,int,int,int,int,int);
	void			SetType(int);
	BOOL			IsChecked();
	void			SetCheck(BOOL);
	int				Enable()  { return EnableWindow(hWnd,TRUE); };
	int				Disable() { return EnableWindow(hWnd,FALSE); };

	// painting
	void			Paint();

	// messages
	void	NotifyParent(int);

	// constructors
	ICustButton(HWND,long);
	~ICustButton() {};
};

// external functions
ICustButton*		GetICustButton(HWND hCtrl);
void				ReleaseICustButton(ICustButton *);
BOOL CALLBACK		ICustButtonCallback(HWND,UINT,WPARAM,LPARAM);



////////////////////////////////////////////////////////////////////
//
// ISpinnerControl
//

// spinner types
enum { SCT_FLOAT,SCT_INT };

class ISpinnerControl {

public:
	HWND	hWnd;					// Info about spinner
	HWND	buddy;					// buddy edit window

	float	min;					// parameters
	float	max;
	float	inc;
	float	def;

	long	oldSpinnerCallback;		// callback data
	long	oldEditCallback;

	int		drag;					// data for dragging
	int		xDrag,yDrag;
	float	ValCentreDrag;
	HCURSOR	oldCursor;
	HCURSOR	arrows;
	int		buttondown;

	int		type;					// spinner type

private:
	void	CheckValAndSet(float);	// checks val and sets edit
public:
	void	ChangeInSpinnerVal();	// called on spinner change
	void	ChangeInEditVal();		// called on edit change

public:
	int		GetIVal();				// user interface functions
	float	GetFVal();
	void	SetValue(int,int notify=TRUE);
	void	SetValue(float,int notify=TRUE);
	int		Enable();
	int		Disable();

	void	NotifyParent(unsigned int,int);		// parent message

	void	Setup(HWND,HWND,int,float,float,float,float);
									// generic setup

	ISpinnerControl();				// constructor/destructor
	~ISpinnerControl() {};
};

////////////////////////////////////////
//
// global functions for spinner

ISpinnerControl*	SetupFloatSpinner(HWND,int,int,float,float,float,float inc=0.1f,float def=0.0f);
ISpinnerControl*	SetupIntSpinner(HWND,int,int,int,int,int,int inc=1,int def=0);
void				ReleaseSpinner(ISpinnerControl*);

BOOL CALLBACK ISpinnerControlCallback(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ISpinnerEditCallback(HWND,UINT,WPARAM,LPARAM);



/////////////////////////////////////////////////////////////////////
//
// IColorSwatch
//
/////////////////////////////////////////////////////////////////////

class IColorSwatch {
public:
	HWND		hWnd;						// info
	COLORREF	color;
	char		DialogTitle[256];
	int			enabled;

	long		oldButtonCallback;			// callback data

	// user interface routines
	COLORREF	GetColor() { return color; };
	COLORREF	SetColor(COLORREF col,BOOL notify) { COLORREF tmp=color; color=col; return col; };
	int			Enable();
	int			Disable();

	void		Paint();					// repaint control

	void		NotifyParent(int,int);			// messages to parent
};


////////////////////////////////////////
//
// global functions for colorswatch

BOOL CALLBACK IColorSwatchCallback(HWND,UINT,WPARAM,LPARAM);
IColorSwatch* GetIColorSwatch(HWND ctrl,COLORREF col,TCHAR* name);
void		  ReleaseIColorSwatch(IColorSwatch*);


// end