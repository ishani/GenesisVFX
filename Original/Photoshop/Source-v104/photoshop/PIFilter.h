/*
	File: PIFilter.h

	Copyright (c) 1990-1, Thomas Knoll.
	Copyright (c) 1992-6, Adobe Systems Incorporated.
	All rights reserved.

	This file describes version 4.0 of Photoshop's Filter module interface.
*/

#ifndef __PIFilter__
#define __PIFilter__

#include "PIActions.h"
#include "PIGeneral.h"

/* Operation selectors */

#define filterSelectorAbout 	 0
#define filterSelectorParameters 1
#define filterSelectorPrepare	 2
#define filterSelectorStart 	 3
#define filterSelectorContinue	 4
#define filterSelectorFinish	 5

/********************************************************************************/

/* We identify the following cases for filtering.  These cases are used to index
 * the behavior properties and are passed to the filter in the filterCase field.
 * of the parameter block.
 */

#define filterCaseUnsupported			 		   		   -1
#define filterCaseFlatImageNoSelection						1
#define filterCaseFlatImageWithSelection					2
#define filterCaseFloatingSelection							3
#define filterCaseEditableTransparencyNoSelection			4
#define filterCaseEditableTransparencyWithSelection			5
#define filterCaseProtectedTransparencyNoSelection			6
#define filterCaseProtectedTransparencyWithSelection		7

/********************************************************************************/

#define filterDataHandlingCantFilter		 0
#define filterDataHandlingNone				 1
#define filterDataHandlingBlackMat			 2
#define filterDataHandlingGrayMat			 3
#define filterDataHandlingWhiteMat			 4

/* The following modes are only useful for input. */

#define filterDataHandlingDefringe			 5
#define filterDataHandlingBlackZap			 6
#define filterDataHandlingGrayZap			 7
#define filterDataHandlingWhiteZap			 8

/* The following mode is only useful for output. */

#define filterDataHandlingFillMask			 9

/* More modes for input... */

#define filterDataHandlingBackgroundZap		10
#define filterDataHandlingForegroundZap		11

/********************************************************************************/

/* For FilterCaseInfo structure (defined below) used with PIFilterCaseInfoProperty */

/* Here are the bits in flag1. */

#define PIFilterDontCopyToDestinationBit	0
	/* Normally we copy the source data to the destination before filtering.
	   Setting this bit inhibits that behavior. */
	   
#define PIFilterWorksWithBlankDataBit	1
	/* In the editable transparency case, we want to know whether the filter
	   requires non-blank pixels to do any work. */
	   
#define PIFilterFiltersLayerMaskBit		2
	/* In the editable transparency case, do we also want to filter the layer
	   mask if it is positioned relative to the layer? */
	   
#define PIFilterWritesOutsideSelectionBit 3
	/* In the image with selection and layer with selection cases, does the
	   filter want to write beyond the confines of the selection? (This is
	   generally rude but in some cases its better than the alternatives. If
	   you use this, be sure you also think about supporting layer transparency
	   data as an alternate mask.) */

/********************************************************************************/

#define PIFilterCaseInfoProperty 0x66696369L
	/* 'fici' An array of filter case info records using 1 based indexing. */

/********************************************************************************/

/* Error return values. The plug-in module may also return standard Macintosh
   operating system error codes, or report its own errors, in which case it
   can return any positive integer. */

#define filterBadParameters -30100	/* "a problem with the filter module interface" */
#define filterBadMode		-30101	/* "the filter module does not support <mode> images" */

#ifndef RC_INVOKED

/********************************************************************************/
/* Pragma to byte align structures; only for Borland C 							*/
#if defined(__BORLANDC__)
#pragma option -a-
#endif
/********************************************************************************/
/* Packing for Microsoft compiler */
#if WIN32
#pragma pack(push,1)
#endif

/********************************************************************************/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

/********************************************************************************/

typedef struct FilterCaseInfo
	{
	char inputHandling;
	char outputHandling;
	char flags1;
	char flags2;
	}
FilterCaseInfo;

#if WIN32
#pragma pack(pop)
#endif

/*********************************************************************************/
/* turn off the byte align pragma back to its original state; only for Borland C */
#if defined(__BORLANDC__)
#pragma option -a.
#endif
/*********************************************************************************/

typedef unsigned char FilterColor [4];

#if WIN32
#pragma pack(push,4)
#endif

/********************************************************************************/

typedef struct FilterRecord 
	{

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

	Handle		parameters; 		/* A handle, initialized to NIL by Photoshop.
									   This should be used to hold the filter's
									   current parameters. */

	Point		imageSize;			/* Size of image */
	int16		planes; 			/* Samples per pixel */
	Rect		filterRect; 		/* Rectangle to filter */

	RGBColor	background; 		/* Current background color */
	RGBColor	foreground; 		/* Current foreground color */

	int32		maxSpace;			/* Maximum possible total of data and buffer space */

	int32		bufferSpace;		/* If the plug-in filter needs to allocate
									   large internal buffers, the filterSelectorPrepare
									   routine should set this field to the number
									   of bytes the filterSelectorStart routine is
									   planning to allocate.  Relocatable blocks should
									   be used if possible. */

	Rect		inRect; 			/* Requested input rectangle. Must be a subset of
									   the image's bounding rectangle. */
	int16		inLoPlane;			/* First requested input plane */
	int16		inHiPlane;			/* Last requested input plane */

	Rect		outRect;			/* Requested output rectangle. Must be a subset of
									   filterRect. */
	int16		outLoPlane; 		/* First requested output plane */
	int16		outHiPlane; 		/* Last requested output plane */

	void *		inData; 			/* Pointer to input rectangle. If more than one
									   plane was requested, the data is interleaved. */
	int32		inRowBytes; 		/* Offset between input rows */
	void *		outData;			/* Pointer to output rectangle. If more than one
									   plane was requested, the data is interleaved. */
	int32		outRowBytes;		/* Offset between output rows */

	Boolean 	isFloating; 		/* Set to true if the selection is floating */
	Boolean 	haveMask;			/* Set to true if there is a selection mask */
	Boolean 	autoMask;			/* If there is a mask, and the selection is not
									   floating, the plug-in can change this field to
									   false to turn off auto-masking. */

	Rect		maskRect;			/* Requested mask rectangle.  Must be a subset of
									   filterRect. Should only be used if haveMask is
									   true. */

	void *		maskData;			/* Pointer to (read only) mask data. */
	int32		maskRowBytes;		/* Offset between mask rows */

	FilterColor backColor;			/* Background color in native color space */
	FilterColor foreColor;			/* Foreground color in native color space */

	OSType		hostSig;			/* Creator code for host application */
	HostProc	hostProc;			/* Host specific callback procedure */

	int16		imageMode;			/* Image mode */

	Fixed		imageHRes;			/* Pixels per inch */
	Fixed		imageVRes;			/* Pixels per inch */

	Point		floatCoord; 		/* Top left coordinate of selection */
	Point		wholeSize;			/* Size of image selection is floating over */

	PlugInMonitor	monitor;		/* Information on current monitor */

	void 		*platformData;		/* Platform specific information. */

	BufferProcs *bufferProcs;		/* The host buffer procedures. */
	
	ResourceProcs *resourceProcs;	/* The host plug-in resource procedures. */
	
	ProcessEventProc processEvent;	/* Pass event to the application. */
	
	DisplayPixelsProc displayPixels;/* Display dithered pixels. */

	HandleProcs	*handleProcs;		/* Platform independent handle manipulation. */

	/* New in 3.0. */

	Boolean		supportsDummyChannels;	/* Does the host support dummy channels? */
	
	Boolean 	supportsAlternateLayouts;	/* Does the host support alternate
											   data layouts. */

	int16		wantLayout;			/* The layout to use for the data.
										See PIGeneral for the values.  */
	
	int16		filterCase;			/* Filter case. If zero, then the plug-in
									   should look at haveMask and isFloating 
									   since older hosts will not set this
									   field. */
									   
	int16		dummyPlaneValue;	/* 0..255 = fill value
									   -1 = leave undefined
									   All others generate errors. */
	
	void *		premiereHook;		/* A hook for Premiere.  See the Premiere
									   Developer's Kit. */
									   
	AdvanceStateProc advanceState;	/* Advance from start to continue or
									   continue to continue. */
									   
	Boolean		supportsAbsolute;	/* Does the host support absolute plane
									   indexing? */
									   
	Boolean		wantsAbsolute;		/* Does the plug-in want absolute plane
									   indexing? (input only) */
									   
	GetPropertyProc	getPropertyObsolete;	/* Use the suite if available */
									   
	Boolean		cannotUndo;			/* If set to TRUE, then undo will not
									   be enabled for this command. */
									   
	Boolean		supportsPadding;	/* Does the host support requests outside
									   the image area? */
	
	int16		inputPadding;		/* Instructions for padding the input. */
	
	int16		outputPadding;		/* Instructions for padding the output. */
	
	int16		maskPadding;		/* Padding instructions for the mask. */
	
	char	 	samplingSupport;	/* Does the host support sampling the
									   input and mask? */
									   
	char		reservedByte;		/* Alignment. */
	
	Fixed		inputRate;			/* Input sample rate. */
	Fixed		maskRate;			/* Mask sample rate. */

	ColorServicesProc colorServices; /* Routine to access color services. */
	
	/* Photoshop structures its data as follows for plug-ins when processing
	   layer data:
			target layer channels
			transparency mask for target layer
			layer mask channels for target layer
			inverted layer mask channels for target layer
			non-layer channels
		When processing non-layer data (including running a filter on the
		layer mask alone), Photoshop structures the data as consisting only
		of non-layer channels.  It indicates this structure through a series
		of short counts.  The transparency count must be either 0 or 1. */
		
	int16		inLayerPlanes;
	int16		inTransparencyMask;
	int16		inLayerMasks;
	int16		inInvertedLayerMasks;
	int16		inNonLayerPlanes;
			
	int16		outLayerPlanes;
	int16		outTransparencyMask;
	int16		outLayerMasks;
	int16		outInvertedLayerMasks;
	int16		outNonLayerPlanes;
			
	int16		absLayerPlanes;
	int16		absTransparencyMask;
	int16		absLayerMasks;
	int16		absInvertedLayerMasks;
	int16		absNonLayerPlanes;
	
	/* We allow for extra planes in the input and the output.  These planes
	   will be filled with dummyPlaneValue at those times when we build the
	   buffers.  These features will only be available if supportsDummyPlanes
	   is TRUE. */
	
	int16		inPreDummyPlanes;	/* Extra planes to allocate in the input. */
	int16		inPostDummyPlanes;
	
	int16		outPreDummyPlanes;	/* Extra planes to allocate in the output. */
	int16		outPostDummyPlanes;
	
	/* If the plug-in makes use of the layout options, then the following
	   fields should be obeyed for identifying the steps between components.
	   The last component in the list will always have a step of one. */
	
	int32		inColumnBytes;		/* Step between input columns. */
	int32		inPlaneBytes;		/* Step between input planes. */
			
	int32		outColumnBytes;		/* Step between output columns. */
	int32		outPlaneBytes;		/* Step between output planes. */
			
	/* New in 3.0.4. */
	
	ImageServicesProcs *imageServicesProcs;
									/* Suite of image processing callbacks. */
			
	PropertyProcs	*propertyProcs;	/*	Routines to query and set document and
										view properties. The plug-in needs to
										dispose of the handle returned for
										complex properties (the plug-in also
										maintains ownership of handles for
										set properties.
									*/
			
	int16		inTileHeight;		/* Tiling for the input. Zero if not set. */
	int16		inTileWidth;
	Point		inTileOrigin;
	
	int16		absTileHeight;		/* Tiling for the absolute data. */
	int16		absTileWidth;
	Point		absTileOrigin;
	
	int16		outTileHeight;		/* Tiling for the output. */
	int16		outTileWidth;
	Point		outTileOrigin;
	
	int16		maskTileHeight;		/* Tiling for the mask. */
	int16		maskTileWidth;
	Point		maskTileOrigin;
	
	/* New in 4.0 */
	
	PIDescriptorParameters	*descriptorParameters;	/* For recording and playback */
	Str255					*errorString;			/* For silent and errReportString */
			
	ChannelPortProcs *channelPortProcs;
									/* Suite for passing pixels through channel ports. */
			
	ReadImageDocumentDesc *documentInfo;	/* The document info for the document being filtered. */
	
	char		reserved [78]; 	/* Set to zero */

	}
FilterRecord, *FilterRecordPtr;

#if WIN32
#pragma pack(pop)
#endif

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

/*********************************************************************************/

// ifdef RC_INVOKED
#endif

#endif
