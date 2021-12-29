/*
	File: PIAcquire.h

	Copyright (c) 1990-1, Thomas Knoll.
	Copyright (c) 1992-6, Adobe Systems Incorporated.
	All rights reserved.

	This file describes version 4.0 of Photoshop's Acquisition module interface.
*/

#ifndef __PIAcquire__
#define __PIAcquire__

#include "PIActions.h"
#include "PIGeneral.h"

/* Operation selectors */

#define acquireSelectorAbout	0
#define acquireSelectorStart	1
#define acquireSelectorContinue 2
#define acquireSelectorFinish	3
#define acquireSelectorPrepare	4
#define acquireSelectorFinalize 5

/*	Error return values. The plug-in module may also return standard Macintosh
	operating system error codes, or report its own errors, in which case it
	can return any positive integer. */

#define acquireBadParameters	-30000	/* "a problem with the acquisition module interface" */
#define acquireNoScanner		-30001	/* "there is no scanner installed" */
#define acquireScannerProblem	-30002	/* "a problem with the scanner" */

/******************************************************************************/
/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif

/******************************************************************************/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

typedef struct AcquireRecord
	{

	int32			serialNumber;	/* Host's serial number, to allow
									   copy protected plug-in modules. */

	TestAbortProc	abortProc;		/* The plug-in module may call this no-argument
									   Boolean function (using Pascal calling
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
									   passed back at once, plus the size of any
									   interal buffers. The plug-in may reduce this
									   value in the acquireSelectorPrepare routine. */

	int16			imageMode;		/* Image mode */
	Point			imageSize;		/* Size of image */
	int16			depth;			/* Bits per sample, currently must be 1 or 8 */
	int16			planes; 		/* Samples per pixel */

	Fixed			imageHRes;		/* Pixels per inch */
	Fixed			imageVRes;		/* Pixels per inch */

	LookUpTable		redLUT; 		/* Red LUT, only used for Indexed Color images */
	LookUpTable		greenLUT;		/* Green LUT, only used for Indexed Color images */
	LookUpTable		blueLUT;		/* Blue LUT, only used for Indexed Color images */

	void *			data;			/* A pointer to the returned image data. The
									   plug-in module is now responsible for freeing
									   this buffer (this is a change from previous
									   versions). Should be set to NIL when
									   all the image data has been returned. */

	Rect			theRect;		/* Rectangle being returned */
	int16			loPlane;		/* First plane being returned */
	int16			hiPlane;		/* Last plane being returned */
	int16			colBytes;		/* Spacing between columns */
	int32			rowBytes;		/* Spacing between rows */
	int32			planeBytes; 	/* Spacing between planes (ignored if only one
									   plane is returned at a time) */

	Str255 			filename;		/* Document file name */
	int16			vRefNum;		/* Volume reference number, or zero if none */
	Boolean 		dirty;			/* Changes since last saved flag. The plug-in may clear
									   this field to prevent prompting the user when
									   closing the document. */

	OSType			hostSig;		/* Creator code for host application */
	HostProc		hostProc;		/* Host specific callback procedure */

	int32			hostModes;		/* Used by the host to inform the plug-in which
									   imageMode values it supports.  If the corresponding
									   bit (LSB = bit 0) is 1, the mode is supported. */

	PlaneMap		planeMap;		/* Maps plug-in plane numbers to host plane
									   numbers.  The host initializes this is a linear
									   mapping.  The plug-in may change this mapping if
									   it sees the data in a different order. */

	Boolean 		canTranspose;	/* Is the host able to transpose the image? */
	Boolean 		needTranspose;	/* Does the plug-in need the image transposed? */

	Handle			duotoneInfo;	/* Handle to duotone information, if returning a
									   duotone mode image.	The plug-in is responsible
									   for freeing this buffer. */

	int32			diskSpace;		/* Free disk space on the host's scratch disk or
									   disks.  Set to a negative number if host does
									   not use a scratch disk. */

	SpaceProc		spaceProc;		/* If not NIL, a pointer to a no-argument
									   in32 valued function (using Pascal calling
									   conventions) which uses the current settings of
									   the imageMode, imageSize, depth, and planes
									   fields to compute the amount of scratch disk
									   space required to hold the image.  Returns -1
									   if the settings are not valid. */

	PlugInMonitor	monitor;		/* Information on current monitor */

	void *			platformData;	/* Platform specific information. */

	BufferProcs *	bufferProcs;	/* The buffer procedures.		  */
	
	ResourceProcs *	resourceProcs;	/* The plug-in resource procedures. */
	
	ProcessEventProc processEvent;	/* Pass events to the application. */

	Boolean 		canReadBack;	/* Can we return data for filtering? */

	Boolean 		wantReadBack;	/* Does the plug-in want to read the data back? */

	Boolean 		acquireAgain;	/* If true at finish, call the plug-in again.
									   Not all hosts may pay attention to this flag. */

	Boolean 		canFinalize;	/* Can the host make the finalize call? */
	
	DisplayPixelsProc displayPixels;/* Display dithered pixels. */
	
	HandleProcs		*handleProcs;	/* Platform independent handle manipulation. */
	
	/* New in 3.0. */
	
	Boolean			wantFinalize;	/* Requests an acquireSelectorFinalize call
									   if the host provides the newer protocol
									   (canFinalize). */

	char			reserved1[3];	/* Get us aligned to a four byte */
									/* boundary. */

	ColorServicesProc colorServices; /* Routine to access color services. */
	
	AdvanceStateProc advanceState;	/* Advance state.  Valid to call only
	                                   within an acquireSelectorContinue
									   call. */

	/* New in 3.0.4. */
	
	ImageServicesProcs *imageServicesProcs;
									/* The suite of image processing services
								       callbacks. */
	
	int16			tileWidth;		/* The width of the tiles. Zero if not set. */
	int16			tileHeight;		/* The height of the tiles. Zero if not set. */
	
	Point			tileOrigin;		/* The origin point for the tiles. */

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

	char			reserved [192]; /* Set to zero */

	}
AcquireRecord, *AcquireRecordPtr;

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
