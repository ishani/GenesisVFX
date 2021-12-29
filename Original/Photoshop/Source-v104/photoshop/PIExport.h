/*
	File: PIExport.h

	Copyright (c) 1990-1, Thomas Knoll.
	Copyright (c) 1992-6, Adobe Systems, Incorporated.
	All rights reserved.

	This file describes version 4.0 of Photoshop's Export module interface.
*/

#ifndef __PIExport__
#define __PIExport__

#include "PIActions.h"
#include "PIGeneral.h"

/* Operation selectors */

#define exportSelectorAbout    0
#define exportSelectorStart    1
#define exportSelectorContinue 2
#define exportSelectorFinish   3
#define exportSelectorPrepare  4

/* Error return values. The plug-in module may also return standard Macintosh
   operating system error codes, or report its own errors, in which case it
   can return any positive integer. */

#define exportBadParameters -30200	/* "a problem with the export module interface" */
#define exportBadMode		-30201	/* "the export module does not support <mode> images" */

/******************************************************************************/

#define PIExpFlagsProperty		0x65787066 /* 'expf' <Set> Export flags */

/******************************************************************************/

/* Export flag indices. */

#define PIExpSupportsTransparency 0

/******************************************************************************/
/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif

/******************************************************************************/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

typedef struct ExportRecord
	{

	int32			serialNumber;	/* Host's serial number, to allow
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

	int32			maxData;		/* Maximum number of bytes that should be
									   requested at once (the plug-in should reduce
									   its requests by the size any large buffers
									   it allocates). The plug-in may reduce this
									   value in the exportSelectorPrepare routine. */

	int16			imageMode;		/* Image mode */
	Point			imageSize;		/* Size of image */
	int16			depth;			/* Bits per sample, currently will be 1 or 8 */
	int16			planes; 		/* Samples per pixel */

	Fixed			imageHRes;		/* Pixels per inch */
	Fixed			imageVRes;		/* Pixels per inch */

	LookUpTable		redLUT; 		/* Red LUT, only used for Indexed Color images */
	LookUpTable		greenLUT;		/* Green LUT, only used for Indexed Color images */
	LookUpTable		blueLUT;		/* Blue LUT, only used for Indexed Color images */

	Rect			theRect;		/* Rectangle requested, set to empty rect when done */
	int16			loPlane;		/* First plane requested */
	int16			hiPlane;		/* Last plane requested */

	void *			data;			/* A pointer to the requested image data */
	int32			rowBytes;		/* Spacing between rows */

	Str255 			filename;		/* Document file name */
	int16			vRefNum;		/* Volume reference number, or zero if none */
	Boolean 		dirty;			/* Changes since last saved flag. The plug-in may clear
									   this field to prevent prompting the user when
									   closing the document. */

	Rect			selectBBox; 	/* Bounding box of current selection, or an empty
									   rect if there is no current selection. */

	OSType			hostSig;		/* Creator code for host application */
	HostProc		hostProc;		/* Host specific callback procedure */

	Handle			duotoneInfo;	/* Handle to duotone information. */

	int16			thePlane;		/* Currently selected channel,
									   or -1 if a composite color channel,
									   or -2 if all channels. */

	PlugInMonitor	monitor;		/* Information on current monitor */

	void *			platformData;	/* Platform specific hook. */

	BufferProcs *	bufferProcs;	/* Host buffer maintenance procedures. */
	
	ResourceProcs *	resourceProcs;	/* Host plug-in resource procedures. */
	
	ProcessEventProc processEvent;	/* Pass events to the application. */
	
	DisplayPixelsProc displayPixels;/* Display dithered pixels. */

	HandleProcs		*handleProcs;	/* Platform independent handle manipulation. */

	/* New in 3.0. */

	ColorServicesProc colorServices; /* Routine to access color services. */
	
	GetPropertyProc	getPropertyObsolete; /* Use the suite if available */
									   
	AdvanceStateProc advanceState;	/* Allowed from Start and Continue. */
	
	int16			layerPlanes;
	int16			transparencyMask;
	int16			layerMasks;				/* Zero. */
	int16			invertedLayerMasks;		/* Zero. */
	int16			nonLayerPlanes;
									   
	/* Note that the layer mask fields are necessarily zero in 3.0.4 since
	   the export module receives the merged data and the merged data doesn't
	   really have a layer mask associated with it. */
									   
	/* New in 3.0.4. */
	
	ImageServicesProcs *imageServicesProcs;
										/* Suite of image processing callbacks. */
	
	int16				tileWidth;		/* The width of the tiles. Zero if not set. */
	int16				tileHeight;		/* The height of the tiles. Zero if not set. */
	
	Point				tileOrigin;		/* The origin point for the tiles. */

	PropertyProcs	*propertyProcs;	/*	Routines to query and set document and
										view properties. The plug-in needs to
										dispose of the handle returned for
										complex properties (the plug-in also
										maintains ownership of handles for
										set properties.
									*/
	
	/* New in 4.0 */
	
	PIDescriptorParameters	*descriptorParameters;	/* For recording and playback */
	Str255					*errorString;			/* For silent and errReportString */

	ChannelPortProcs *channelPortProcs;
									/* Suite for passing pixels through channel ports. */
			
	ReadImageDocumentDesc *documentInfo;	/* The document info for the document being filtered. */
	
	char			reserved [178]; /* Set to zero */

	}
ExportRecord, *ExportRecordPtr;

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

/******************************************************************************/
/* turn off the byte align pragma back to its original state; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a.
#endif

/******************************************************************************/

#endif
