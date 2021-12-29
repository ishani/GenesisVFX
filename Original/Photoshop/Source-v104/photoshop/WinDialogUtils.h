/*
	File: WinDialogUtils.h

	Copyright 1996, Adobe Systems Incorporated.
	All rights reserved.

	Header file for C source file for MS-Windows specific dialog code
*/

#ifndef __WinDialogUtils_H__
#define __WinDialogUtils_H__

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <errno.h>

#include "PITypes.h"
#include "PIAbout.h"
#include "PIUtilities.h"

/*****************************************************************************/
/* Define constants. */


/* Define errors */

enum
{
	/* noErr = 0 */
	errOutOfRange = 1,
	errNotANumber
};

/*****************************************************************************/
/* The following routine displays a standard about box dialog. */

void ShowAbout(AboutRecordPtr aboutPtr, Handle hDllInstance, int32 resID);

/*****************************************************************************/
/* The following routine displays a centered alert. */

short ShowAlert (short alertID);

/* Displays a cross-platform alert with a version number. */

short ShowVersionAlert (Handle hDllInstance,
						HWND hDlg,
						short alertID, 
						short stringID,
						Str255 versText1,
						Str255 versText2);

/* The following routine displays a centered caution alert from a string. */

short ShowAlertType (Handle hDllInstance,
					 HWND hDlg,
				     short alertID, 
				     short stringID, 
				     Str255 minText, 
				     Str255 maxText,
				     short alertType);

enum
{ // constants defined for alert routine
	PIAlertCaution,
	PIAlertStop
};

#define ShowCaution(h, hDlg, alertID, stringID, min, max)	\
	ShowAlertType(h, hDlg, alertID, stringID, min, max, PIAlertCaution)
	
#define ShowStop(h, hDlg, alertID, stringID, min, max)	\
	ShowAlertType(h, hDlg, alertID, stringID, min, max, PIAlertStop)

/*****************************************************************************/

/* Fixpoint function used in Acquire. */

Fixed FixRatio(short numer, short denom);

/*******************************************************************************/
/* Centers a dialog template 1/3 of the way down on the main screen */

void CenterDialog(HWND hDlg);

/************************************************************************/
/* Utility function to return which item of a group of radio buttons is */
/* checked.																*/
short GetRadioGroupState (HWND hDlg, short nItem1, short nItemL);

/****************************************************************************/
/* Selects a text item */
void SelectTextItem(HWND hDlg, short item);

/****************************************************************************/
/* Sets the default dialog item */
void SetDialogDefaultItem(HWND hDlg, short item);

/*********************************************************************/
/* Shows a dialog item */
void ShowDialogItem(HWND hDlg, short item);

/*********************************************************************/
/* Hides a dialog item */
void HideDialogItem(HWND hDlg, short item);

/*********************************************************************/
/* Hides or shows a dialog item */
void ShowHideItem(HWND hDlg, short item, BOOL state);

/*********************************************************************/
/* Enables a dialog item */
void EnableControl(HWND hDlg, short item);

/*********************************************************************/
/* Disables a dialog item */
void DisableControl(HWND hDlg, short item);

/*********************************************************************/
/* Enables or disables a dialog item */
void EnableDisableControl(HWND hDlg, short item, BOOL state);

/*********************************************************************/
/* Toggles and returns value of check box */
BOOL ToggleCheckBoxState(HWND hDlg, short item);

/*****************************************************************************/
/* Retrieves a pascal text string from a text field. */
void FetchText (HWND hDlg, short item, Str255 s);

/********************************************************************/
/* Stuffs a pascal string into a text field */
void StuffText (HWND hDlg, short item, Str255 s);

/*********************************************************************/
/* Retrieves a number from a dialog item. Returns true if successful. */
/* Automatically pins value to min >= value >= max, setting dialog */
/* item and beeping if out of bounds. */

short FetchNumber(HWND hDlg, 
				 short item, 
				 int32 min, 
				 int32 max, 
				 int32 *value);

/* Corresponding alert routine to pop error */

void AlertNumber(HWND hDlg, 
				 short item, 
				 int32 min, 
				 int32 max, 
				 int32 *value,
				 Handle hDllInstance,
				 short alertID,
				 short numberErr);

void StuffDouble (HWND dp, short item, double value, short precision);

short FetchDouble (HWND dp,
					short item,
					double min,
					double max,
					double *value);


void AlertDouble (HWND dp,
				 short item,
				 double min,
				 double max,
				 double *value,
				 Handle hDllInstance,
				 short alertID,
				 short numberErr);

/*********************************************************************/
/* Standardize terminology */

#define	ok			1
#define cancel		2

#define SetRadioGroupState(dp, first, last, item)	\
	CheckRadioButton(dp, first, last, item)

#define SetCheckBoxState(dp, item, state)	\
	CheckDlgButton(dp, item, state)

#define GetCheckBoxState(dp, item)	\
	IsDlgButtonChecked(dp, item)

#define StuffNumber(dp, item, value)	\
	SetDlgItemInt(dp, item, value, TRUE)

#define GETWSTYLE(X)         GetWindowLong((HWND)X, GWL_STYLE)
#define COMMANDWND(lParam)   (HWND)(UINT)lParam
#define COMMANDID(X)         LOWORD(X)

#ifdef WIN32
#define COMMANDCMD(X, Y)	HIWORD(X)
#define huge
#ifndef PDECL
#define PDECL
#endif
#else
#define COMMANDCMD(X, Y)	HIWORD(Y)
#endif

#endif /* __WinDialogUtils_H__ */