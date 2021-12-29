/*
	File: PIFormat.h

	Copyright (c) 1992-6, Adobe Systems Incorporated.
	All rights reserved.

	This file describes version 1 of Photoshop's plug-in image format interface.
*/

#ifndef __PIFormat__
#define __PIFormat__

#include "PIActions.h"
#include "PIGeneral.h"

/* Operation selectors */

#define formatSelectorAbout 			 0

#define formatSelectorReadPrepare		 1
#define formatSelectorReadStart 		 2
#define formatSelectorReadContinue		 3
#define formatSelectorReadFinish		 4

#define formatSelectorOptionsPrepare	 5
#define formatSelectorOptionsStart		 6
#define formatSelectorOptionsContinue	 7
#define formatSelectorOptionsFinish 	 8

#define formatSelectorEstimatePrepare	 9
#define formatSelectorEstimateStart 	10
#define formatSelectorEstimateContinue	11
#define formatSelectorEstimateFinish	12

#define formatSelectorWritePrepare		13
#define formatSelectorWriteStart		14
#define formatSelectorWriteContinue 	15
#define formatSelectorWriteFinish		16

#define formatSelectorFilterFile		17

/*	We keep various pieces of information about the file format in the PiMI resource.
	Here is the structure of that resource. */

/******************************************************************************/

#define PIFmtFileTypeProperty   0x666d5443 /* 'fmTC' <TypeCreatorPair> Default file type for file format. */
#define PIReadTypesProperty		0x52645479 /* 'RdTy' <TypeCreatorPair []> File type info for file format plugins */
#define PIFilteredTypesProperty	0x66667454 /* 'fftT' <TypeCreatorPair []> File types to filter for file format plugins */
#define PIReadExtProperty		0x52644578 /* 'RdEx' <OSType []> File type info for file format plugins */
#define PIFilteredExtProperty	0x66667445 /* 'fftE' <OSType []> File types to filter for file format plugins */
#define PIFmtFlagsProperty		0x666d7466 /* 'fmtf' <Set> Format flags */

#define PIFmtMaxSizeProperty	0x6d78737a /* 'mxsz' <Point> Max rows and cols supported by this format. */
#define PIFmtMaxChannelsProperty 0x6d786368 /* 'mxch' <int16 []> Max channels per mode for this format. */

/******************************************************************************/

/* Format flags indices. */

#define PIFmtReadsAllTypesFlag 0
#define PIFmtSavesImageResourcesFlag 1
#define PIFmtCanReadFlag 2
#define PIFmtCanWriteFlag 3
#define PIFmtCanWriteIfReadFlag 4

/******************************************************************************/

/* Pragma to byte align structures; for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif

/* Pragma to byte align structures; for Microsoft C */
#if WIN32
#pragma pack(push,1)
#endif

/******************************************************************************/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

typedef struct FormatInfo
	{

	PlugInInfo	plugInInfo; 			/* The information common to all plug-ins.			*/
	int16		typeCount;				/* The number of types in the type list.			*/
	int16		extensionCount; 		/* The number of extensions in the type list.		*/
	Boolean 	canRead;				/* Can we read using this format?					*/
	Boolean 	readsAllTypes;			/* Can this plug-in read from all files?			*/
	Boolean 	canWrite;				/* Can we write using this format?					*/
	Boolean		canWriteIfRead;			/* Can we write if we read using this format? 		*/
	Boolean 	savesImageResources;	/* Does this file format save the resource data.	*/
	Boolean		padFlag;				/* Padding */
	int16		maxChannels [16];		/* Maximum # of channels with each plug-in mode.	*/
	int16		maxRows;				/* Maximum rows allowed in document.				*/
	int16		maxCols;				/* Maximum columns allowed in document. 			*/
	OSType		fileType;				/* The file type if we create a file.				*/
	OSType		creatorType;			/* The creator type if we create a file.			*/
	OSType		typeList [1];			/* The types and extensions supported.				*/

	}
FormatInfo;

/******************************************************************************/
/* turn off the byte align pragma back to its original state; only for Borland C */
#if defined(__BORLANDC__)
#pragma option -a.
#endif

/* Pragma to turn off align structures for Microsoft C */
#if WIN32
#pragma pack(pop)
#endif
/******************************************************************************/

/*	Error return values. The plug-in module may also return standard Macintosh
	operating system error codes, or report its own errors, in which case it
	can return any positive integer. */

#define formatBadParameters -30500
#define formatCannotRead	-30501

#if WIN32
#pragma pack(push,4)
#endif

typedef struct FormatRecord
	{

	int32				serialNumber;	/* Host's serial number, to allow
										   copy protected plug-in modules. */

	TestAbortProc		abortProc;		/* The plug-in module may call this no-argument
										   BOOLEAN function (using Pascal calling
										   conventions) several times a second during long
										   operations to allow the user to abort the operation.
										   If it returns TRUE, the operation should be aborted
										   (and a positive error code returned). */

	ProgressProc		progressProc;	/* The plug-in module may call this two-argument
										   procedure periodically to update a progress
										   indicator.  The first parameter is the number
										   of operations completed; the second is the total
										   number of operations. */

	int32				maxData;		/* Maximum number of bytes that should be
										   passed back at once, plus the size of any
										   interal buffers. The plug-in may reduce this
										   value in the formatSelectorPrepare routine. */

	int32				minDataBytes;	/* Disk space needed for the data fork. */
	int32				maxDataBytes;	/* Disk space needed for the data fork. */

	int32				minRsrcBytes;	/* Disk space needed for the resource fork. */
	int32				maxRsrcBytes;	/* Disk space needed for the resource fork. */

	int32				dataFork;		/* refnum for the data fork. */
	int32				rsrcFork;		/* refnum for the resource fork. */
	
	FSSpec *			fileSpec;		/* Full file specification. */

	int16				imageMode;		/* Image mode */
	Point				imageSize;		/* Size of image */
	int16				depth;			/* Bits per sample, currently must be 1 or 8 */
	int16				planes; 		/* Samples per pixel */

	Fixed				imageHRes;		/* Pixels per inch */
	Fixed				imageVRes;		/* Pixels per inch */

	LookUpTable			redLUT; 		/* Red LUT, only used for Indexed Color images */
	LookUpTable			greenLUT;		/* Green LUT, only used for Indexed Color images */
	LookUpTable			blueLUT;		/* Blue LUT, only used for Indexed Color images */

	void *				data;			/* A pointer to the returned image data. The
										   plug-in module is responsible for freeing
										   this buffer. Should be set to NIL when
										   all the image data has been returned. */

	Rect				theRect;		/* Rectangle being returned */
	int16				loPlane;		/* First plane being returned */
	int16				hiPlane;		/* Last plane being returned */
	int16				colBytes;		/* Spacing between columns */
	int32				rowBytes;		/* Spacing between rows */
	int32				planeBytes; 	/* Spacing between planes (ignored if only one
										   plane is returned at a time) */
	PlaneMap			planeMap;		/* Maps plug-in plane numbers to host plane
										   numbers.  The host initializes this is a linear
										   mapping.  The plug-in may change this mapping if
										   it sees the data in a different order. */

	Boolean 			canTranspose;	/* Is the host able to transpose the image? */
	Boolean 			needTranspose;	/* Does the plug-in need the image transposed? */

	OSType				hostSig;		/* Creator code for host application */
	HostProc			hostProc;		/* Host specific callback procedure */

	int16				hostModes;		/* Used by the host to inform the plug-in which
										   imageMode values it supports.  If the corresponding
										   bit (LSB = bit 0) is 1, the mode is supported. */

	Handle				revertInfo; 	/* Information to be kept with the document for reverting or
										   saving.	Where possible this handle should be used to
										   avoid bringing up an options dialog. */

	NewPIHandleProc		hostNewHdl; 	/* Handle allocation and disposal for revert info. */
	DisposePIHandleProc	hostDisposeHdl;
	
	Handle				imageRsrcData;	/* Handle containing the block of resource data. */
	int32				imageRsrcSize;	/* size of image resources. */

	PlugInMonitor		monitor;		/* The host's monitor. */

	void *				platformData;	/* Platform specific information. */

	BufferProcs *		bufferProcs;	/* The procedures for allocating and */
										/* releasing buffers. */
										
	ResourceProcs *		resourceProcs;	/* Plug-in resource procedures. */

	ProcessEventProc	processEvent;	/* Pass event to the application. */
	
	DisplayPixelsProc	displayPixels;	/* Display dithered pixels. */

	HandleProcs			*handleProcs;	/* Platform independent handle manipulation. */
	
	/* New in 3.0. */
	
	OSType				fileType;		/* File-type for filtering */

	ColorServicesProc	colorServices; /* Routine to access color services. */
	
	AdvanceStateProc	advanceState;	/* Valid from continue selectors. */

	/* New in 3.0.4. */
	
	PropertyProcs		*propertyProcs; /* A suite to allow getting and setting image
										   document properties. */

	ImageServicesProcs	*imageServicesProcs;
										/* The suite of image processing services
										   callbacks. */
	
	int16				tileWidth;		/* The width of the tiles. Zero if not set. */
	int16				tileHeight;		/* The height of the tiles. Zero if not set. */
	
	Point				tileOrigin;		/* The origin point for the tiles. */
	
	/* New in 4.0 */
	
	PIDescriptorParameters	*descriptorParameters;	/* For recording and playback */
	Str255					*errorString;			/* For silent and errReportString */

	char				reserved [212]; /* Set to zero */

	}
FormatRecord, *FormatRecordPtr;

#if WIN32
#pragma pack(pop)
#endif

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

/******************************************************************************/
#endif
