//  (c) Copyright 1996.  Adobe Systems, Incorporated.  All rights reserved.

#ifndef __WinUtilities_H__
#define __WinUtilities_H__

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include "PITypes.h"
#include "WinDialogUtils.h"

#define noErr 0
#define memFullErr (-108)
#define memWZErr  (-111)
#define nilHandleErr (-109)

void NumToString (const long x, Str255 s);

#ifndef __PITypes__
typedef LPSTR Ptr;			/* Mac pointer type */
typedef LPSTR *Handle;	/* Mac handle type */
typedef short OSErr;
typedef unsigned char Boolean;
typedef struct
{
	short top;
	short left;
	short bottom;
	short right;
} Rect;

typedef struct
{
	short v;
	short h;
} Point;

#define false FALSE
#define true TRUE
#endif

Handle	NewHandle( long size );
Handle	NewHandleClear( long size );
long	GetHandleSize (Handle handle);
void	SetHandleSize (Handle handle, long newSize);
void	DisposHandle (Handle handle);
Handle	 DupHandle(Handle oldhand);
void	HLock (Handle h);
void	HUnlock (Handle h);
void	MoveHHi (Handle h);
void	DisposPtr(Ptr p);
Ptr	NewPtr( long size );
Ptr	NewPtrClear( long size );
OSErr	MemError(void);

LPSTR WAllocBuffer(long size);
VOID WFreeBuffer(LPSTR ptr);
DWORD WSizeBuffer(LPSTR ptr);
HANDLE HandFromPtr(LPSTR ptr);

#if 0
BOOL UserInputAvail(UINT flags, BOOL newOnly);
VOID DoYield(BOOL yield);

BOOL ValidateDlgLong(HWND hDlg, int item, long minVal, long maxVal, long *retVal, BOOL pin);
BOOL UserChangedEditText(HWND hDlg, WPARAM wParam, LPARAM lParam);
void InitEditWnd(HWND hDlg, int id);
void EditFail(HWND hDlg, int id, BOOL issigned, BOOL inedit);

VOID CenterWindow(HWND hChild, WORD wHow);
/* Defines for CenterWindow */
#define CW_HORZ     0x0001
#define CW_VERT     0x0002
#define CW_BOTH     0x0003
#define CW_V1THIRD  0x0010
#define CW_CLIENT   0x0000
#define CW_SCREEN   0x0100

VOID MapRect(LPRECT lprct, HWND srcw, HWND destw);
VOID GetDlgItemRect(HWND hDlg, int item, LPRECT lprct);
void	InitCursor( void );
void InitHypotTable(unsigned short *HypotTable);
short Hypot(unsigned short *HypotTable,register short x,register short y);

#endif

Boolean PISetRect(Rect* pRect, short nLeft, short nTop, short nRight, short nBottom);
Boolean PIOffsetRect(Rect* pRect, short xAmt, short yAmt);
Boolean PIPtInRect(Point pt, Rect* r);
Boolean PIInsetRect(Rect *pRect, short xAmt, short yAmt);
Boolean EmptyRect( Rect *pRect );

long	TickCount();
short	Random( void );

/****************************************************************************/

/* Initialization and termination code for window's dlls. */

// Win32 Change
#ifdef WIN32

// Every 32-Bit DLL has an entry point DLLInit

BOOL APIENTRY DLLInit(HANDLE hInstance, DWORD fdwReason, LPVOID lpReserved);

#else
/* ------------------------------------------------
 *   Code from Borland's window's dll example code.
 * ------------------------------------------------
 */
#if defined(__BORLANDC__)
// Turn off warning: Parameter '' is never used; effects next function only
#pragma argsused
#endif

// Every DLL has an entry point LibMain and an exit point WEP.
int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
								   WORD wHeapSize, LPSTR lpszCmdLine );

int FAR PASCAL WEP(int nParam);
#endif

#endif /* __WinUtilities_H__ */
