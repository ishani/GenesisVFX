/*
	File: WinDialogUtils.c

	Copyright (c) 1996, Adobe Systems Incorporated.
	All rights reserved.

	C source file for MS-Windows specific dialog code
*/

#include "WinDialogUtils.h"

/*****************************************************************************/

void  ShowAbout (AboutRecordPtr aboutPtr, Handle hDllInstance, int32 resID)
{
	char message[256] = "";
	char caption[60] = "";
	PlatformData *platform;
	HWND h = NULL;

	platform = (PlatformData *) (aboutPtr->platformData);

	h = (HWND)platform->hwnd;

	LoadString(hDllInstance, resID, message, 256);
	LoadString(hDllInstance, resID+1, caption, 60);
	MessageBox(h,message,caption,MB_APPLMODAL|MB_ICONINFORMATION|MB_OK);
}

/*****************************************************************************/

short ShowVersionAlert (Handle hDllInstance,
						HWND hDlg,
						short alertID, 
						short stringID,
						Str255 versText1,
						Str255 versText2)
{
	short 	result = 0;
	Str255	ds = "";
	char	message[256];
	Ptr		p = 0;
	Handle	h = 0;
		
	if (alertID)
	{		
		if (stringID > 0)
		{
			LoadString(hDllInstance, stringID, message, 256);
			AppendString(ds, message, 0, (short)strlen(message));
		}
		
		PIParamText(ds, NULL, versText1, versText2);
		ds [ ++ds[0] ] = 0;
		strcpy(message, &ds[1]);

		result = ( MessageBox(hDlg, message, NULL, 
				   MB_OK | 
				   MB_ICONEXCLAMATION));
	}
	return result;
}

/*****************************************************************************/

short ShowAlertType (Handle hDllInstance,
					 HWND hDlg,
					 short alertID, 
					 short stringID, 
					 Str255 minText, 
					 Str255 maxText,
					 short alertType)
{
	short 	result = 0;
	Str255	ds = "";
	char	message[256];
	Ptr		p = 0;
	long	size = 0;
	Handle	h = 0;
		
	if (alertID)
	{		
		if (stringID > 0)
		{
			LoadString(hDllInstance, stringID, message, 256);
			AppendString(ds, message, 0, (short)strlen(message));
		}
		
		PIParamText(ds, NULL, minText, maxText);
		ds [ ++ds[0] ] = 0;
		strcpy(message, &ds[1]);

		switch (alertType)
		{ // will pop either caution or alert
			case PIAlertStop:
				result = (MessageBox(hDlg, message, NULL, MB_OK |
									 MB_ICONSTOP));
				break;
			default:
			case PIAlertCaution:
				result = (MessageBox(hDlg, message, NULL, MB_OK |
							         MB_ICONEXCLAMATION));
				break;
		}
	}
	return result;
}

/*****************************************************************************/

/* Fixpoint function. */

Fixed FixRatio(short numer, short denom)
{
	if (denom == 0)
	{
		if (numer >=0)
			return   LONG_MAX;

		else
			return -(LONG_MAX);
	}
	else
			return ((long)numer << 16) / denom;
}

/*******************************************************************************/

/* Centers a dialog template 1/3 of the way down on the main screen */

void CenterDialog(HWND hDlg)
{
    HWND hParent;
	int  nHeight;
    int  nWidth;
    int  nTitleBits;
    RECT rcDialog;
    RECT rcParent;
    int  xOrigin;
    int  yOrigin;
    int  xScreen;
    int  yScreen;

    if  ( ! (hParent = GetParent(hDlg)))
        hParent = GetDesktopWindow();

    GetClientRect(hParent, &rcParent);
    ClientToScreen(hParent, (LPPOINT)&rcParent.left);  // point(left,  top)
    ClientToScreen(hParent, (LPPOINT)&rcParent.right); // point(right, bottom)

    // Center on Title: title bar has system menu, minimize,  maximize bitmaps
    // Width of title bar bitmaps - assumes 3 of them and dialog has a sysmenu
    nTitleBits = GetSystemMetrics(SM_CXSIZE);

    // If dialog has no sys menu compensate for odd# bitmaps by sub 1 bitwidth
    if  ( ! (GetWindowLong(hDlg, GWL_STYLE) & WS_SYSMENU))
        nTitleBits -= nTitleBits / 3;

    GetWindowRect(hDlg, &rcDialog);
    nWidth  = rcDialog.right  - rcDialog.left;
    nHeight = rcDialog.bottom - rcDialog.top;

    xOrigin = max(rcParent.right - rcParent.left - nWidth, 0) / 2
            + rcParent.left - nTitleBits;
    xScreen = GetSystemMetrics(SM_CXSCREEN);
    if  (xOrigin + nWidth > xScreen)
        xOrigin = max (0, xScreen - nWidth);

	yOrigin = max(rcParent.bottom - rcParent.top - nHeight, 0) / 3
            + rcParent.top;
    yScreen = GetSystemMetrics(SM_CYSCREEN)-20;
    if  (yOrigin + nHeight > yScreen)
        yOrigin = max(0 , yScreen - nHeight);

    SetWindowPos(hDlg, NULL, xOrigin, yOrigin, nWidth, nHeight, SWP_NOZORDER);
}

/************************************************************************/
/* Utility function to return which item of a group of radio buttons is */
/* checked.																*/

short GetRadioGroupState (HWND hDlg, short first, short last)
{
	short	item = 0;
	while (first <= last)
	{
		if (IsDlgButtonChecked(hDlg, first))
		{
			item = first;
			first = last;
		}
		first++;
	}
	return item;
}

/**********************************************************/
/* Sets the default item */

void SetDialogDefaultItem(HWND hDlg, short item)
{
	SendDlgItemMessage(hDlg, item, BM_SETSTYLE,
					   (WPARAM)BS_DEFPUSHBUTTON,
					   (LPARAM)TRUE);
	SetFocus(GetDlgItem(hDlg, item));
}

/**********************************************************/
/* Selects a text item */

void SelectTextItem(HWND hDlg, short item)
{
	SendDlgItemMessage(hDlg, item, EM_SETSEL, 0, (LPARAM)-1);
	SetFocus(GetDlgItem(hDlg, item));
}
/**********************************************************/
/* Shows a dialog item */

void ShowDialogItem(HWND hDlg, short item)
{
	ShowWindow(GetDlgItem(hDlg, item),SW_SHOW);
}

/**********************************************************/
/* Hides a dialog item */

void HideDialogItem(HWND hDlg, short item)
{
	ShowWindow(GetDlgItem(hDlg, item),SW_HIDE);
}

/**********************************************************/
/* Hides or shows a dialog item */

void ShowHideItem(HWND hDlg, short item, BOOL state)
{
	if (state)
		ShowWindow(GetDlgItem(hDlg, item), SW_SHOW);
	else
		ShowWindow(GetDlgItem(hDlg, item), SW_HIDE);
}

/**********************************************************/
/* Shows a dialog item */

void EnableControl(HWND hDlg, short item)
{
	EnableWindow(GetDlgItem(hDlg, item),TRUE);
}

/**********************************************************/
/* Hides a dialog item */

void DisableControl(HWND hDlg, short item)
{
	EnableWindow(GetDlgItem(hDlg, item),FALSE);
}

/**********************************************************/
/* Hides or shows a dialog item */

void EnableDisableControl(HWND hDlg, short item, BOOL state)
{
	EnableWindow(GetDlgItem(hDlg, item), state);
}

/**********************************************************/
/* Toggles and returns the value of a dialog item */

BOOL ToggleCheckBoxState(HWND hDlg, short item)
{
	BOOL	x;

	x = !GetCheckBoxState(hDlg, item);
	SetCheckBoxState(hDlg, item, x);
	return x;
}

/*****************************************************************************/
/* The following routine retrieves a pascal text string from a text field. */

void FetchText (HWND hDlg, short item, Str255 s)
{
	char c[256] = "";
	
	s [ (s[0] = 0)+1 ] = 0;
	GetDlgItemText(hDlg, item, c, 255);
	AppendString(s, c, 0, (short)strlen(c));
}

/*****************************************************************************/
/* The following routine stuffs a text into a text field. */

void StuffText (HWND dp, short item, Str255 s)
{
	if (s[0] > 254) s[0]--; // subtract one
	s[ s[0]+1 ] = 0; // null terminate pascal string
	SetDlgItemText(dp, item, &s[1]); // stuff string
}

/**********************************************************/
/* Retrieves a number from a dialog item */

short FetchNumber(HWND hDlg, 
				short item, 
				int32 min, 
				int32 max, 
				int32 *value)
{
	Str255		s = "";
	Str255		minText = "";
	Str255		maxText = "";
	long		x = 0;
	short		retn = noErr;


	FetchText(hDlg, item, s);
	if (!StringToNumber(s, &x))
	{
		x = 0;
		retn = errNotANumber;
	}
	else if (x < min || x > max) retn = errOutOfRange;

	*value = x; // return something to work with
	return retn;
}

/*****************************************************************************/

/* Display corresponding alert for number */

void AlertNumber(HWND hDlg, 
				short item, 
				int32 min, 
				int32 max, 
				int32 *value,
				Handle hDllInstance, 
				short alertID, 
				short numberErr)
{
	Str255		s = "";
	Str255		minText = "";
	Str255		maxText = "";
	long		x = *value;

	x = ((x < min) ? min : max);
	*value = x;
	StuffNumber(hDlg, item, x);

	ltoa (min, &minText[1], 10);
	minText[0] = (char)strlen(&minText[1]);

	ltoa (max, &maxText[1], 10);
	maxText[0] = (char)strlen(&maxText[1]);

	MessageBeep (MB_ICONEXCLAMATION);

	(void) ShowCaution (hDllInstance,
						hDlg,
						alertID,
					  	kBadNumberID,			// could use notANumber
						minText,			   // to display a "Type a number"
						maxText);			   // alert.

	SelectTextItem(hDlg, item);
	SetFocus(GetDlgItem(hDlg, item));
}

/*****************************************************************************/
/* The following routine stuffs a double into a text field. */

void StuffDouble (HWND dp, short item, double value, short precision)
{
	Str255 	s = "";
	DoubleToString(value, s, precision);
	StuffText(dp, item, s);
}

/*****************************************************************************/

/* 
   Here is the corresponding routine to retrieve the floating value from a text
   field.  It will do range checking and validate that it has been
   handed a number.
   
   It returns noErr if it gets a valid value. */
   
short FetchDouble (HWND hDlg,
					short item,
					double min,
					double max,
					double *value)
{
	Str255 s1 = "";
	Str255 s2 = "";
		
	long x1 = 0;
	long x2 = 0;
	short precision = 0;
	Boolean notAWholeNumber = false;
	Boolean notADecimalNumber = false;
	Boolean notANumber = false;
	double x = 0;
	short retn = noErr;
	
	FetchText(hDlg, item, s1);
	
	DivideAtDecimal(s1, s2);
	
	notAWholeNumber = !StringToNumber (s1, &x1);
	
	notADecimalNumber = !StringToNumber (s2, &x2);
	
	precision = s2[0]; //length
	
	notANumber = (notAWholeNumber && notADecimalNumber);
	
	x = (double)x1 + ((double)x2 / (double)power(10, s2[0]));
	
	if (notANumber)
	{
		x = 0;
		retn = errNotANumber;
	}
	else if (x < min || x > max) retn = errOutOfRange;
	
	*value = x;
	return retn;
}


/*****************************************************************************/
/*
 	If it has not been handed a number, it brings up
	an appropriate error dialog, inserts an appropriately pinned value,
	and selects the item. */
  
void AlertDouble (HWND hDlg,
				 short item,
				 double min,
				 double max,
				 double *value,
				 Handle hDllInstance,
				 short alertID,
				 short numberErr)
{ 
	Str255 minText = "";
	Str255 maxText = "";
	short precision = 0;
	double x = *value;
	
	// Figure out precision
	FetchText(hDlg, item, minText);
	DivideAtDecimal(minText, maxText);
	precision = maxText[0]; // no more than number of digits in decimal
	
	x = (x < min ? min : max);
	*value = x;		
	StuffDouble (hDlg, item, x, precision);


	DoubleToString (min, minText, precision);
	DoubleToString (max, maxText, precision);
	
	MessageBeep (MB_ICONEXCLAMATION);

	(void) ShowCaution (hDllInstance,
						hDlg,
						alertID,
				  	    kBadDoubleID, // could use numberErr==errNotANumber
						minText,	  // to pop a "that's not a number"
						maxText);	  // alert.	

	SelectTextItem (hDlg, item);
	SetFocus(GetDlgItem(hDlg, item));
}

