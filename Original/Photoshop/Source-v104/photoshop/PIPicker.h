/*
	File: PIPicker.h

	Copyright (c) 1996, Adobe Systems Incorporated.
	All rights reserved.

	This file describes version 1.0 of Photoshop's color picker plug-in module
	interface.
*/

#ifndef __PIPicker__
#define __PIPicker__

#include "PIActions.h"
#include "PIGeneral.h"

/********************************************************************************/

/* Operation selectors */

#define pickerSelectorAbout 	 0
#define pickerSelectorPick		 1

/********************************************************************************/

/* Error return values. The plug-in module may also return standard Macintosh
   operating system error codes, or report its own errors, in which case it
   can return any positive integer. */
 
#define pickerBadParameters -30800	/* "of a problem with the plug-in module interface" */

/********************************************************************************/

typedef struct PickParms
	{
	int16 sourceSpace;				// The colorspace the original color is in
	int16 resultSpace;				// The colorspace of the returned result
									// Can be plugIncolorServicesChosenSpace
	unsigned16 colorComponents[4];	// Will contain the original color when the
									// plugin is called.  The plugin will put
									// the returned color here as well.
	Str255 *pickerPrompt;
	}
PickParms;

/********************************************************************************/

typedef struct PIPickerParams
	{
	
	/* We start with the fields that are common to pretty much all of the plug-ins. */
	
	int32		serialNumber;		/* Host's serial number, to allow
									   copy protected plug-in modules. */

	TestAbortProc	abortProc;		/* The plug-in module may call this no-argument
									   BOOLEAN function (using Pascal calling
									   conventions) several times a second during long
									   operations to allow the user to abort the operation.
									   If it returns TRUE, the operation should be aborted
									   (and a positive error code returned). */

	ProgressProc	progressProc;	/* The plug-in module may call this two-argument
									   procedure periodically to update a progress
									   indicator.  The first parameter is the number
									   of operations completed; the second is the total
									   number of operations. */

	OSType		hostSig;			/* Creator code for host application */
	HostProc	hostProc;			/* Host specific callback procedure */

	BufferProcs *bufferProcs;		/* The host buffer procedures. */
	
	ResourceProcs *resourceProcs;	/* The host plug-in resource procedures. */
	
	ProcessEventProc processEvent;	/* Pass event to the application. */
	
	DisplayPixelsProc displayPixels;/* Display dithered pixels. */

	HandleProcs	*handleProcs;		/* Platform independent handle manipulation. */

	ColorServicesProc colorServices; /* Routine to access color services. */
	
	ImageServicesProcs *imageServicesProcs;
									/* Suite of image processing callbacks. */
			
	PropertyProcs	*propertyProcs;	/*	Routines to query and set document and
										view properties. The plug-in needs to
										dispose of the handle returned for
										complex properties (the plug-in also
										maintains ownership of handles for
										set properties.
									*/
									
	ChannelPortProcs *channelPortProcs;
									/* Suite for passing pixels through channel ports. */
			
	PIDescriptorParameters	*descriptorParameters;	/* For recording and playback */
	Str255					*errorString;			/* For silent and errReportString */
	
	PlugInMonitor	monitor;		/* Information on current monitor */

	void 		*platformData;		/* Platform specific information. */

	char reserved [4];				/* 4 bytes of reserved space. */

	/* We follow this with the fields specific to this type of plug-in. */

	PickParms pickParms;
	
	/* And, of course, space for future expansion. */
	
	char reservedBlock [260];
			
	}
PIPickerParams, *PickerRecordPtr;

/********************************************************************************/

#endif /* __PIPicker__ */
