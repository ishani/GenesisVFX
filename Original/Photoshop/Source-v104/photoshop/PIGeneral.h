/*
	File: PIGeneral.h

	Copyright (c) 1992-6, Adobe Systems Incorporated.
	All rights reserved.

	This file describes the elements common to all plug-in interfaces.
*/

#ifndef __PIGeneral__
#define __PIGeneral__

#include "PITypes.h"

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

#if defined(__BORLANDC__)
#pragma option -a-
#endif

#if MSWindows
/* The following macros are part of PIGeneral.r on the Mac. We need to have access  */
/* to those macros in a header file. This file should be updated to reflect the     */
/* changes made to PIGeneral.r.														*/

#define PIVersion "4.0"
#define ProductVersion  3,0,0x86,4

#define supportsBitmap          1
#define supportsGrayScale       2
#define supportsIndexedColor    4
#define supportsRGBColor        8
#define supportsCMYKColor      16
#define supportsHSLColor       32
#define supportsHSBColor       64
#define supportsMultichannel  128
#define supportsDuotone       256
#define supportsLABColor      512

#define latestAcquireVersion        4
#define latestAcquireSubVersion     0
#define latestExportVersion         4
#define latestExportSubVersion      0
#define latestExtensionVersion      1
#define latestExtensionSubVersion   0
#define latestFilterVersion         4
#define latestFilterSubVersion      0
#define latestFormatVersion         1
#define latestFormatSubVersion      0
#define latestParserVersion			1
#define latestParserSubVersion		0
#define latestSelectionVersion		1
#define latestSelectionSubVersion	0
#define latestPickerVersion			1
#define latestPickerSubVersion		0

#endif

typedef unsigned long PIType;  /* this originally came from OSType on the Mac */

/********************************************************************************/

#define kPhotoshopSignature		0x3842494dL	/* '8BIM' <OSType. Photoshop application signature */

/********************************************************************************/

/* The following error codes are available for all plug-ins. */

#define errPlugInHostInsufficient			-30900
		/* "the plug-in requires services not provided by this host." */

#define errPlugInPropertyUndefined			-30901
		/* "a requested property could not be found. */

/*	
	The following error code is specifically used for some of the image
	processing operations to reflect whether the host supports colBytes values
	other than 1.
*/
		
#define errHostDoesNotSupportColStep		-30902
		/* "the plug-in requires services not provided by this host." */

#define errInvalidSamplePoint				-30903
		/*	Returned from plugIncolorServicesSamplePoint to indicate point
			is not in an image window.
		*/
		 
#define errReportString						-30904
		/*	Plug-in can return this to notify the host to use the error
		 	string for reporting.
		*/
		 
/*	The following error numbers are used with the routines for working with
	channel ports.
*/
		 
#define errUnknownPort						-30910

#define errUnsupportedRowBits				-30911
#define errUnsupportedColBits				-30912
#define errUnsupportedBitOffset				-30913
#define errUnsupportedDepth					-30914
	/* We've never heard of this depth. */
#define errUnsupportedDepthConversion 		-30915
	/* We've heard of this depth, but we aren't prepared to do the conversion. */

/* Errors used by the scripting system */
#define errWrongPlatformFilePath	 		-30916
#define errMissingParameter					-1715		/* errAEParamMissed */

/********************************************************************************/

#define PIKindProperty			0x6b696e64L /* 'kind' <PIType> giving the plug-in's kind */
#define PIVersionProperty		0x76657273L /* 'vers' <int32> Major<int16>.Minor<int16> version number. */
#define PIPriorityProperty		0x70727479L /* 'prty' <int32> Load order priority */
#define PIImageModesProperty	0x6d6f6465L /* 'mode' <FlagSet> Image modes supported flags. (bitmask) */
#define PIEnableInfoProperty	0x656e626cL /* 'enbl' <CString> Enabling expression. */
#define PIRequiredHostProperty	0x686f7374L /* 'host' <PIType> giving host required if any. */
#define PICategoryProperty		0x63617467L /* 'catg' <PString> Category name that appears on top level menu. */
#define PINameProperty			0x6e616d65L /* 'name' <PString> Menu name. */
#define PIPickerIDProperty		0x706e6d65L	/* 'pnme' <CString> picker ID */
#define PI68KCodeProperty		0x6d36386bL /* 'm68k' <PI68KCodeDesc> 68k code descriptor. See type below. */
#define PI68KFPUCodeProperty	0x36386670L	/* '68fp' <PI68KCodeDesc> 68k w/FPU code descriptor. See type below. */
#define PIPowerPCCodeProperty	0x70777063L /* 'pwpc' <PICFMCodeDesc> PowerPC code descriptor. See type below. */
#define PIWin32X86CodeProperty  0x77783836L /* 'wx86' <PIWin32X86CodeDesc> Win32 Intel code descriptor. See type below */
#define PIWin16X86CodeProperty  0x66783836L /* 'fx86' <PIWin16X86CodeDesc> Win16 Intel code descriptor. See type below */

#define PIUnixSPARCV7CodeProperty	0x75737037	/* 'usp7' SPARC version 7 */
#define PIUnixSPARCV8CodeProperty	0x75737038	/* 'usp8' SPARC version 8 */
#define PIUnixSPARCV9CodeProperty	0x75737039	/* 'usp9' SPARC version 9 */
#define PIUnixSPARCCodeProperty	PIUnixSPARCV7CodeProperty

#define PIUnixMIPSR3000CodeProperty	0x756d7233	/* 'umr3' MIPS R3000 */
#define PIUnixMIPSR4000CodeProperty	0x756d7234	/* 'umr4' MIPS R4000 */
#define PIUnixMIPSR400064BitCodeProperty	0x756d3477	/* 'um4w' MIPS R4000 (can be 64 bit) */
#define PIUnixMIPSR8000CodeProperty	0x756d7238	/* 'umr8' MIPS R8000 (always 64 bit) */
#define PIUnixMIPSCodeProperty	PIUnixMIPSR3000CodeProperty

/********************************************************************************/

/* Structures that directly represent resources. */

#define kCurrentPiPLVersion 0

typedef struct PIProperty
	{
	PIType vendorID;		/* Vendor specific identifier. */
	PIType propertyKey;		/* Identification key for this property type. */
	int32  propertyID;		/* Index within this property type. Must be unique for properties of a given type in a PiPL. */
	int32  propertyLength;	/* Length of following data array. Will be rounded to a multiple of 4. */
	char   propertyData [1]; /* propertyLength bytes long. Must be a multiple of 4. Can be zero? */
	} PIProperty;

typedef struct PIPropertyList
	{
	int32 version;			/* Properties datastructure version number. */
	int32 count;			/* Number of properties contained. */
	PIProperty properties[1];	/* The actual properties. */
	} PIPropertyList;

typedef PIPropertyList **PIPropertyListHandle;

/* Below are structures describing types used in plug-in property lists.
 * These structure obey Macintosh 68k alignment and padding rules though
 * generally they are laid out so fields have natural alignment and any
 * needed padding is explicit in the structure.
 *
 * In addition to these structure types, simple types such as int32, int16,
 * etc. are used. Two other examples are PString and FlagSet. PString is like a
 * rez pstring or a Str255 except that storage maynot be present for all
 * 255 characters. The initial length byte is present in the property.
 * FlagSet is a variable length bitstring in which the first member is
 * represented by the most significant bit of the first byte, the eighth
 * member is in the least significant bit of the first byte, etc. The length
 * of the set is kept in the property's length field of course.
 *
 * Arrays are also used in places with bounds either taken from the
 * property length or explicitly represented within the property.
 */

typedef struct PI68KCodeDesc
	{
	PIType fType;
	int16 fID;
	} PI68KCodeDesc;

typedef struct PICFMCodeDesc
	{
	long fContainerOffset;
	long fContainerLength;
	unsigned char fEntryName[1];
	} PICFMCodeDesc;
	
typedef struct PIWin32X86CodeDesc
{
	char fEntryName[1];
} PIWin32X86CodeDesc;

typedef struct PIWin16X86CodeDesc
{
	char fEntryName[1];
} PIWin16X86CodeDesc;

typedef struct TypeCreatorPair
	{
	PIType type;
	PIType creator;
	} TypeCreatorPair;

/********************************************************************************/

typedef struct PlugInInfo
	{

	int16	version;			/* The version number of the interface supported. */
	int16	subVersion; 		/* The sub-version number.						  */
	int16	priority;			/* The plug-in's priority.                        */
	int16	generalInfoSize;	/* The size of the general info.				  */
	int16	typeInfoSize;		/* The size of the type specific info.			  */
	int16	supportsMode;		/* A bit mask indicating supported image modes.   */
	PIType	requireHost;		/* A required host if any.						  */

	}
PlugInInfo;

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

typedef struct PSBuffer *BufferID;

#define kPIGeneralInfoSize 16

/* Image modes */

#define plugInModeBitmap	   0
#define plugInModeGrayScale    1
#define plugInModeIndexedColor 2
#define plugInModeRGBColor	   3
#define plugInModeCMYKColor    4
#define plugInModeHSLColor	   5
#define plugInModeHSBColor	   6
#define plugInModeMultichannel 7
#define plugInModeDuotone	   8
#define plugInModeLabColor	   9
#define plugInModeGray16	  10
#define plugInModeRGB48		  11

/* The Window procID for a Moveable Modal Dialog window that can be used
 *	under System 6.  WDEF(128) is part of Photoshop.
 */
 
#define PSmovableDBoxProc	(128*16+5)

/* Other commonly used structures */

typedef int16 PlaneMap [16];

typedef struct PlugInMonitor
	{
	Fixed gamma;
	Fixed redX;
	Fixed redY;
	Fixed greenX;
	Fixed greenY;
	Fixed blueX;
	Fixed blueY;
	Fixed whiteX;
	Fixed whiteY;
	Fixed ambient;
	}
PlugInMonitor;

/* The following structures reflect the version 1 structure for PSPixelMaps. */

typedef struct PSPixelMask
	{
	struct PSPixelMask *next;
	void *maskData;
	int32 rowBytes;
	int32 colBytes;
	int32 maskDescription;
	}
PSPixelMask;

#define kSimplePSMask   0
#define kBlackMatPSMask 1
#define kGrayMatPSMask  2
#define kWhiteMatPSMask 3
#define kInvertPSMask   4

typedef struct PSPixelMap
	{
	int32 version;
	VRect bounds;
	int32 imageMode;
	int32 rowBytes;
	int32 colBytes;
	int32 planeBytes;
	void *baseAddr;
	
	/* Fields new in version 1. */
	
	PSPixelMask *mat;
	PSPixelMask *masks;
	int32 maskPhaseRow;	/* Used for setting the phase of the checkerboard. */
	int32 maskPhaseCol;
	}
PSPixelMap;

typedef struct ColorServicesInfo
	{
	int32 infoSize;
	int16 selector;
	int16 sourceSpace;
	int16 resultSpace;
	Boolean resultGamutInfoValid;
	Boolean resultInGamut;
	void *reservedSourceSpaceInfo;	 /* Must be NULL. (Will return paramErr if not.) */
	void *reservedResultSpaceInfo;	 /* Must be NULL. (Will return paramErr if not.) */
	int16 colorComponents[4];
	void *reserved;					 /* Must be NULL. (Will return paramErr if not.) */
	union
		{
		Str255 *pickerPrompt;
		Point *globalSamplePoint;
		int32 specialColorID;
		} selectorParameter;
	}
ColorServicesInfo;

/* Constants for resultSpace and sourceSpace fields. */
#define plugIncolorServicesRGBSpace		0
#define plugIncolorServicesHSBSpace		1
#define plugIncolorServicesCMYKSpace	2
#define plugIncolorServicesLabSpace		3
#define plugIncolorServicesGraySpace	4
#define plugIncolorServicesHSLSpace		5
#define plugIncolorServicesXYZSpace		6

/* Only valid in the resultSpace field. Says leave the color in the space the user chose.
 * Custom colors will be returned in an arbitrary space possibly depending on the book.
 */
#define plugIncolorServicesChosenSpace	-1

/* Used with the plugIncolorServicesGetSpecialColor selector. */
#define plugIncolorServicesForegroundColor 0
#define plugIncolorServicesBackgroundColor 1

/* Color Service Selectors. */
#define plugIncolorServicesChooseColor		0
#define plugIncolorServicesConvertColor		1		
#define plugIncolorServicesSamplePoint		2
#define plugIncolorServicesGetSpecialColor	3

/* The following images are used for the resampling callbacks. */

typedef struct PSImagePlane
	{
	void *data;
	Rect bounds;
	int32 rowBytes;
	int32 colBytes;
	}
PSImagePlane;

/* Sorry about this, but to support more general pixel formats, here is yet
another structure for describing areas of memory as pixels. The actual
rectangle is expected to be given separately. */

typedef struct PixelMemoryDesc
	{
	void *data;
	int32 rowBits;	/* Note bits not bytes! */
	int32 colBits;	/* Note bits not bytes! */
	int32 bitOffset;
	int32 depth;
	}
PixelMemoryDesc;

/* We describe a scaling by providing two rectangles. */

typedef struct PSScaling
	{
	VRect sourceRect;
	VRect destinationRect;
	}
PSScaling;

/* We provide access to internal channels via the following abstract types. */

typedef struct ChannelReadPortDesc *ChannelReadPort;
typedef struct ChannelWritePortDesc *ChannelWritePort;

/* We describe a channel for reading with the following data structure. */

#define kCurrentMinVersReadChannelDesc	0
#define kCurrentMaxVersReadChannelDesc	0

typedef struct ReadChannelDesc
	{
	
	int32 minVersion;		/* The minimum and maximum version which */
	int32 maxVersion;		/* can be used to interpret this record. */
	
	struct ReadChannelDesc *next;	/* The next descriptor in the list. */
	
	ChannelReadPort port;	/* The port to use for reading. */
	
	VRect bounds;			/* The bounds of the channel data. */
	int32 depth;			/* The depth of the data. */
	
	VPoint tileSize;		/* The size of the tiles. */
	VPoint tileOrigin;		/* The origin for the tiles. */
	
	Boolean target;			/* Is this a target channel. */
	Boolean shown;			/* Is this channel shown? */
	
	int16 channelType;		/* The channel type. */
	
	void *contextInfo;		/* A pointer to additional info dependent on context. */
	
	const char *name;		/* The name of the channel. */
	
	}
ReadChannelDesc;

/* We describe a document as a collection of lists of read channels. */

#define kCurrentMinVersReadImageDocDesc	0
#define kCurrentMaxVersReadImageDocDesc	0

typedef struct ReadImageDocumentDesc
	{
	
	int32 minVersion;		/* The minimum and maximum version which */
	int32 maxVersion;		/* can be used to interpret this record. */
	
	/* Version 0 fields. */
	
	int32 imageMode;		/* The color mode for the document. */
	
	int32 depth;			/* The default depth for the document. */
	
	VRect bounds;			/* The bounds for the document. */
	
	Fixed hResolution;		/* The resolution for the document. */
	Fixed vResolution;
	
	LookUpTable redLUT;		/* Color table for indexed color and duotone. */
	LookUpTable greenLUT;
	LookUpTable blueLUT;
	
	ReadChannelDesc *targetCompositeChannels;
							/* The composite channels in the target layer. */
							
	ReadChannelDesc *targetTransparency;
							/* The transparency channel for the target layer. */
							
	ReadChannelDesc *targetLayerMask;
							/* The layer mask for the target layer. */
							
	ReadChannelDesc *mergedCompositeChannels;
							/* The composite channels in the merged data. */
							
	ReadChannelDesc *mergedTransparency;
							/* The transparency channel for the merged data. */
							
	ReadChannelDesc *alphaChannels;
							/* The alpha channels. */
							
	ReadChannelDesc *selection;
							/* The selection mask if any. */
	
	}
ReadImageDocumentDesc;

/* We provide a similar structure for accessing write channels. */

#define kCurrentMinVersWriteChannelDesc	0
#define kCurrentMaxVersWriteChannelDesc	0

typedef struct WriteChannelDesc
	{
	
	int32 minVersion;		/* The minimum and maximum version which */
	int32 maxVersion;		/* can be used to interpret this record. */
	
	struct WriteChannelDesc *next;	/* The next descriptor in the list. */
	
	ChannelWritePort port;	/* The port to use for reading. */
	
	VRect bounds;			/* The bounds of the channel data. */
	int32 depth;			/* The depth of the data. */
	
	VPoint tileSize;		/* The size of the tiles. */
	VPoint tileOrigin;		/* The origin for the tiles. */
	
	int16 channelType;		/* The channel type. */
	
	int16 padding;			/* Reserved. Defaults to zero. */
	
	void *contextInfo;		/* A pointer to additional info dependent on context. */
	
	const char *name;		/* The name of the channel. */
	
	}
WriteChannelDesc;

/* The channel types are covered by the following values. */

#define ctUnspecified		 0
#define ctRed				 1
#define ctGreen				 2
#define ctBlue				 3
#define ctCyan				 4
#define ctMagenta			 5
#define ctYellow			 6
#define ctBlack				 7
#define ctL					 8
#define ctA					 9
#define ctB					10
#define ctDuotone			11
#define ctIndex				12
#define ctBitmap			13
#define ctColorSelected		14
#define ctColorProtected	15
#define ctTransparency      16
#define ctLayerMask         17
#define ctInvertedLayerMask 18
#define ctSelectionMask     19

#ifdef __cplusplus

extern "C"
	{
#endif

	typedef MACPASCAL Boolean (*TestAbortProc) (void);

	typedef MACPASCAL void (*ProgressProc) (int32 done, int32 total);

	typedef MACPASCAL void (*HostProc) (int16 selector, int32 *data);

	typedef MACPASCAL int32 (*SpaceProc) (void);

	typedef MACPASCAL Handle (*NewPIHandleProc) (int32 size);

	typedef MACPASCAL void (*DisposePIHandleProc) (Handle h);
	
	typedef MACPASCAL int32 (*GetPIHandleSizeProc) (Handle h);
	
	typedef MACPASCAL OSErr (*SetPIHandleSizeProc) (Handle h, int32 newSize);

	typedef MACPASCAL Ptr (*LockPIHandleProc) (Handle h, Boolean moveHigh);
	
	typedef MACPASCAL void (*UnlockPIHandleProc) (Handle h);

	typedef MACPASCAL void (*RecoverSpaceProc) (int32 size);

	typedef MACPASCAL OSErr (*AllocateBufferProc) (int32 size, BufferID *bufferID);

	typedef MACPASCAL Ptr (*LockBufferProc) (BufferID bufferID, Boolean moveHigh);

	typedef MACPASCAL void (*UnlockBufferProc) (BufferID bufferID);

	typedef MACPASCAL void (*FreeBufferProc) (BufferID bufferID);
	
	typedef MACPASCAL int32 (*BufferSpaceProc) (void);
	
	typedef MACPASCAL int16 (*CountPIResourcesProc) (ResType type);
	
	typedef MACPASCAL Handle (*GetPIResourceProc) (ResType type, int16 index);
	
	typedef MACPASCAL void (*DeletePIResourceProc) (ResType type, int16 index);
	
	typedef MACPASCAL OSErr (*AddPIResourceProc) (ResType type, Handle data);
	
	typedef MACPASCAL void (*ProcessEventProc) (void *event);
	
	typedef MACPASCAL OSErr (*DisplayPixelsProc) (const PSPixelMap *source,
												  const VRect *srcRect,
												  int32 dstRow,
												  int32 dstCol,
												  void *platformContext);
											
	typedef MACPASCAL OSErr (*AdvanceStateProc) (void);
	
	typedef MACPASCAL OSErr (*GetPropertyProc) (PIType signature,
												PIType key,
												int32 index,
												int32 *simpleProperty,
												Handle *complexProperty);

	typedef MACPASCAL OSErr (*SetPropertyProc) (PIType signature,
												PIType key,
												int32 index,
												int32 simpleProperty,
												Handle complexProperty);

	typedef MACPASCAL OSErr (*ColorServicesProc) (ColorServicesInfo *info);
	
	typedef MACPASCAL OSErr (*PIResampleProc) (PSImagePlane *source,
											   PSImagePlane *destination,
											   Rect *area,
											   Fixed *coords,
											   int16 method);
											   
	typedef MACPASCAL OSErr (*ReadPixelsProc) (ChannelReadPort port,
											   const PSScaling *scaling,
											   const VRect *writeRect,
											   		/* Rectangle in destination coordinates. */
											   const PixelMemoryDesc *destination,
											   VRect *wroteRect);
											   
	typedef MACPASCAL OSErr (*WriteBasePixelsProc) (ChannelWritePort port,
													const VRect *writeRect,
													const PixelMemoryDesc *source);
													
	typedef MACPASCAL OSErr (*ReadPortForWritePortProc) (ChannelReadPort *readPort,
														 ChannelWritePort writePort);
	
#ifdef __cplusplus
	}
#endif

#define kCurrentBufferProcsVersion 2

typedef struct BufferProcs
	{

	int16 bufferProcsVersion;
	
	int16 numBufferProcs;

	AllocateBufferProc allocateProc;

	LockBufferProc lockProc;

	UnlockBufferProc unlockProc;

	FreeBufferProc freeProc;
	
	BufferSpaceProc spaceProc;

	}
BufferProcs;

#define kCurrentBufferProcsCount \
	((sizeof(BufferProcs) - offsetof(BufferProcs, allocateProc)) / sizeof(void *)) 

#define kCurrentResourceProcsVersion 3

typedef struct ResourceProcs
	{
	
	int16 resourceProcsVersion;
	
	int16 numResourceProcs;
	
	CountPIResourcesProc countProc;
	
	GetPIResourceProc getProc;
	
	DeletePIResourceProc deleteProc;
	
	AddPIResourceProc addProc;
	
	}
ResourceProcs;

#define kCurrentResourceProcsCount \
	((sizeof(ResourceProcs) - offsetof(ResourceProcs, countProc)) / sizeof(void *)) 

/* Here are the resource types that have been reserved. */

#define kDayStarColorMatchingResource 'DCSR'


#define kCurrentHandleProcsVersion 1

typedef struct HandleProcs
	{
	
	int16 handleProcsVersion;
	
	int16 numHandleProcs;
	
	NewPIHandleProc newProc;
	
	DisposePIHandleProc disposeProc;
	
	GetPIHandleSizeProc getSizeProc;
	
	SetPIHandleSizeProc setSizeProc;
	
	LockPIHandleProc lockProc;
	
	UnlockPIHandleProc unlockProc;
	
	RecoverSpaceProc recoverSpaceProc;
	
	}
HandleProcs;

#define kCurrentHandleProcsCount \
	((sizeof(HandleProcs) - offsetof(HandleProcs, newProc)) / sizeof(void *)) 

#define kCurrentImageServicesProcsVersion 1
	
typedef struct ImageServicesProcs
	{
	
	int16 imageServicesProcsVersion;
	int16 numImageServicesProcs;
	
	PIResampleProc interpolate1DProc;
	PIResampleProc interpolate2DProc;
	
	}
ImageServicesProcs;

#define kCurrentImageServicesProcsCount \
	((sizeof(ImageServicesProcs) - offsetof(ImageServicesProcs, interpolate1DProc)) / sizeof(void *)) 
	

#define kCurrentPropertyProcsVersion 1

typedef struct PropertyProcs
	{
	
	int16 propertyProcsVersion;
	int16 numPropertyProcs;
	
	GetPropertyProc	getPropertyProc;
	SetPropertyProc setPropertyProc;
	
	}
PropertyProcs;

#define kCurrentPropertyProcsCount \
	((sizeof(PropertyProcs) - offsetof(PropertyProcs, getPropertyProc)) / sizeof(void *))

#define kCurrentChannelPortProcsVersion 1
	
typedef struct ChannelPortProcs
	{
	
	int16 channelPortProcsVersion;
	int16 numChannelPortProcs;
	
	ReadPixelsProc readPixelsProc;
	WriteBasePixelsProc writeBasePixelsProc;
	ReadPortForWritePortProc readPortForWritePortProc;
	
	}
ChannelPortProcs;

#define kCurrentChannelPortProcsCount \
	((sizeof(ChannelPortProcs) - offsetof(ChannelPortProcs, readPixelsProc)) / sizeof(void *)) 
	

/*********************************************************************************/

/* The following constants indicate what sort of padding to use.  Values 0
   through 255 specify a constant value.  Negative values imply special
   operations. */
   
#define plugInWantsEdgeReplication			-1
#define plugInDoesNotWantPadding			-2
#define plugInWantsErrorOnBoundsException	-3

/* If the plug-in does not want padding and does not want errors, then
   exterior areas will contain arbitrary values. */

/*********************************************************************************/

/* The sampling flag indicates the level of sampling support. */

#define hostDoesNotSupportSampling		0
#define hostSupportsIntegralSampling	1
#define hostSupportsFractionalSampling	2

/*********************************************************************************/

/* Layout values for filters and export modules. */

#define piLayoutTraditional				0	/* Rows, columns, planes with colbytes = # planes */
#define piLayoutRowsColumnsPlanes		1
#define piLayoutRowsPlanesColumns		2
#define piLayoutColumnsRowsPlanes		3
#define piLayoutColumnsPlanesRows		4
#define piLayoutPlanesRowsColumns		5
#define piLayoutPlanesColumnsRows		6

/*********************************************************************************/
typedef struct PIInterfaceColor
	{
	RGBtuple color32;	 // interface color for full color depth
	RGBtuple color2;	 // interface color for B/W display
	}
PIInterfaceColor;

/*********************************************************************************/

// macros for interface color selectors
#define kPIInterfaceButtonUpFill	1
#define kPIInterfaceBevelShadow 	2
#define kPIInterfaceIconFillActive  3
#define	kPIInterfaceIconFillDimmed  4
#define	kPIInterfacePaletteFill 	5
#define	kPIInterfaceIconFrameDimmed 6
#define	kPIInterfaceIconFrameActive 7
#define	kPIInterfaceBevelHighlight  8
#define	kPIInterfaceButtonDownFill  9
#define	kPIInterfaceIconFillSelected	10
#define kPIInterfaceBorder			11
#define	kPIInterfaceButtonDarkShadow 	12
#define	kPIInterfaceIconFrameSelected 	13
#define	kPIInterfaceRed				14

/*********************************************************************************/


#if defined(__BORLANDC__)
#pragma option -a.
#endif

#endif

