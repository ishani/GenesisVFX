/* Plug-in module utility routines */

/* Copyright 1993-6 by Adobe Systems, Inc.  All rights reserved. */

/* The routines in this module provide a collection of utilities for doing
 * things like centering dialogs, etc.. They expect that the host's A5-world
 * is the currently active A5-world.
 */
 
#ifndef __DialogUtilities_H__
#define __DialogUtilities_H__

#include <Dialogs.h>
#include <stddef.h>
#include <C:\MSDEV\include\sys\Types.h>
#include <Gestalt.h>
#include <string.h>

#include "PITypes.h"
#include "PIGeneral.h"
#include "PIUtilities.h"

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/* Define constants. */
#define AlertID			16990
#define kBadNumberID	AlertID
#define kBadDoubleID	kBadNumberID+1
#define kNeedVers		kBadDoubleID+1
#define kWrongHost		kNeedVers+1
#define StringResource	'STR '

/* Errors */
enum
{
	/* noErr = 0 */
	errOutOfRange = 1,
	errNotANumber
};

/*****************************************************************************/

/* Set the cursor to the arrow cursor. */

void SetArrowCursor ();

/*****************************************************************************/

/* Centers a dialog template 1/3 of the way down on the main screen. */

void CenterDialog (DialogTHndl dt);

/* The following routine sets up a moveable modal dialog. */

void SetUpMoveableModal (DialogTHndl dt, OSType hostSig);
						 
/* The following routine takes the place of ModalDialog in running moveable
 * modal dialogs.
 */
						 
void MoveableModalDialog (DialogPtr dp,
						  ProcessEventProc processEvent,
						  ModalFilterProcPtr filter,
						  short *item);
						  
/* Except within the filter procedure for such moveable modal dialogs, we need
   to call the following routine to get the window refcon since
   MoveableModalDialog stomps it. */
   
long GetMoveableWRefCon (DialogPtr dp);
						  
/* The following routine displays a centered alert. */

short ShowAlert (short alertID);

/* Displays a cross-platform alert with a version number. */

short ShowVersionAlert (Handle hDllInstance,
						short alertID, 
						short stringID,
						Str255 versText1,
						Str255 versText2);

/* The following routine displays a centered caution alert from a string. */

short ShowAlertType (Handle hDllInstance,
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

#define ShowCaution(h, alertID, stringID, min, max)	\
	ShowAlertType(h, alertID, stringID, min, max, PIAlertCaution)
	
#define ShowStop(h, alertID, stringID, min, max)	\
	ShowAlertType(h, alertID, stringID, min, max, PIAlertStop)

/* The following routine displays a standard about box dialog. */

void ShowAbout (OSType hostSign, short dialogID);

/*****************************************************************************/
/* Set an item hook so that it will outline the OK button in a dialog. */

void SetOutlineOKHook (DialogPtr dp, short hookItem);

/* The following routine sets a user item to be a group box.  It expects
   the next item to be the title for the group box. */

void SetOutlineGroup (DialogPtr dp, short groupItem);

/*****************************************************************************/

/* The following routine selects an edit text item. */

void SelectTextItem (DialogPtr dp, short item);

/* The following routine sets the text of a text item. */

void StuffText (DialogPtr dp, short item, Str255 text);

/* The following routine extracts the text of a text item. */

void FetchText (DialogPtr dp, short item, Str255 text);

					 
/* The following routine stuffs a numeric value into a text field. */

void StuffNumber (DialogPtr dp, short item, long value);

/* Here is the corresponding routine to retrieve the value from a text
   field.  It will do range checking and validate that it has been
   handed a number.
  
   It returns noErr (0) if it gets a valid value in the field. */
 
short FetchNumber (DialogPtr dp,
 					  short item,
					  long min,
					  long max,
					  long *value);

/* This is the corresponding alert routine when the range is not valid. */

void AlertNumber (DialogPtr dp,
				  short item,
				  long min,
				  long max,
				  long *value,
				  Handle hDllInstance,
				  short alertID,
				  short numberErr);
				  				 
/* Stuffs a double/float value into a text field. */

void StuffDouble (DialogPtr dp, short item, double value, short precision);
 
/* Grabs a double/float from a dialog. */

short FetchDouble (DialogPtr dp,
					 short item,
					 double min,
					 double max,
					 double *value);
					 
void AlertDouble (DialogPtr dp,
					 short item,
					 double min,
					 double max,
					 double *value,
					 Handle hDllInstance,
					 short alertID,
					 short numberErr);
					 
/*****************************************************************************/

/* Set the state of a check box (or radio button). */

void SetCheckBoxState (DialogPtr dp, short item, Boolean checkIt);

/* Determine the state of a check box (or radio button). */

Boolean GetCheckBoxState (DialogPtr dp, short item);

/* Toggle a check box and return the new state. */

Boolean ToggleCheckBoxState (DialogPtr dp, short item);

/* Set a radio group (from first to last item) to reflect the selection. */

void SetRadioGroupState (DialogPtr dp, short first, short last, short item);
						 
/* Get the selected button within a radio group. */

short GetRadioGroupState (DialogPtr dp, short first, short last);

/* Set the value for a pop-up menu. */

void SetPopUpMenuValue (DialogPtr dp, short item, short newValue);

/* Get the value for a pop-up menu. */

short GetPopUpMenuValue (DialogPtr dp, short item);

/* Utility routine to show or hide an item. */

void ShowHideItem (DialogPtr dp, short item, Boolean state);

/* Utility routine to disable a control. */

void DisableControl (DialogPtr dp, short item);

/* Utility routine to enable a control. */

void EnableControl (DialogPtr dp, short item);

/* Utility routine to enable (TRUE) or disable (FALSE) a control. */

void EnableDisableControl (DialogPtr dp, short item, Boolean state);

/* Utility routine to invalidate an item. */

void InvalItem (DialogPtr dp, short item);

/*****************************************************************************/

/* Little routine to flash a button set off by a keystroke. */

void FlashDialogButton (DialogPtr dp, short item);

/* Perform standard handling for check boxes and radio buttons. For radio
   buttons, we assume that the group for the radio button extends forward
   and backward in the DITL as long as the item type is radio button. */
   
void PerformStandardDialogItemHandling (DialogPtr dp, short item);

/*****************************************************************************/

QDGlobals *GetQDGlobals (void);

#ifdef __cplusplus
} /* End of extern "C" block. */
#endif

/*****************************************************************************/

#endif /* __DialogUtilities__ */