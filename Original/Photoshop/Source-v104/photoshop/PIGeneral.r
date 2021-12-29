/*
	File: PIGeneral.r

	Copyright (c) 1991-6, Adobe Systems Incorporated.
	All rights reserved.

	Rez file describing Plug-in resources.
*/

#ifndef __PIGeneral_r__
#define __PIGeneral_r__

#ifndef kPIPropertiesVersion
#define kPIPropertiesVersion 0
#endif

// Dont include this file on MSWindows resources

#define qIncludeMe 1

#ifdef MSWindows
#undef qIncludeMe
#define qIncludeMe !MSWindows
#endif

#if qIncludeMe

/* First, temporarily define Pipl types for easier rezing */
#define PIPiplTypes Filter = '8BFM',			\
					Parser = '8BYM',			\
					ImageFormat='8BIF',			\
					Extension = '8BXM',			\
					Acquire = '8BAM',			\
					Export = '8BEM',			\
					Selection = '8BSM',			\
					Picker = '8BCM'

type 'PiPL'
	{
	longint = kPIPropertiesVersion;
	longint = $$CountOf(properties);
	array properties
		{
		switch
			{

/* Properties for all types of plug-ins. */

			case Kind:
				longint = '8BIM';
				key longint = 'kind';
				longint = 0;
				longint = 4;
				literal longint PIPiplTypes;

			case Version:
				longint = '8BIM';
				key longint = 'vers';
				longint = 0;
				longint = 4;
				longint;

			case Priority:
				longint = '8BIM';
				key longint = 'prty';
				longint = 0;
				longint = 4;
				longint;

			case RequiredHost:
				longint = '8BIM';
				key longint = 'host';
				longint = 0;
				longint = 4;
				literal longint;

			case Name:
				longint = '8BIM';
				key longint = 'name';
				longint = 0;
#if DeRez
				fill long;
#else
				longint = (nameEnd[$$ArrayIndex(properties)] - nameStart[$$ArrayIndex(properties)]) / 8;
#endif
			  nameStart:
				pstring;
			  nameEnd:
				align long;

			case Category:
				longint = '8BIM';
				key longint = 'catg';
				longint = 0;
#if DeRez
				fill long;
#else
				longint = (catgEnd[$$ArrayIndex(properties)] - catgStart[$$ArrayIndex(properties)]) / 8;
#endif
			  catgStart:
				pstring;
			  catgEnd:
				align long;

			case Code68k:
				longint = '8BIM';
				key longint = 'm68k';
				longint = 0;
				longint = 6;
				literal longint PIPiplTypes;
				integer;
				align long;

			case Code68kFPU:
				longint = '8BIM';
				key longint = '68fp';
				longint = 0;
				longint = 6;
				literal longint PIPiplTypes;
				integer;
				align long;

			case CodePowerPC:
				longint = '8BIM';
				key longint = 'pwpc';
				longint = 0;
#if DeRez
				fill long;
#else
				longint = (pwpcEnd[$$ArrayIndex(properties)] - pwpcStart[$$ArrayIndex(properties)]) / 8;
#endif
			  pwpcStart:
				longint;
				longint;
				pstring;
			  pwpcEnd:
				align long;

#ifdef MSWindows /* For documentation purposes only. */
			case CodeWin32X86:
				longint = '8BIM';
				key longint = 'wx86';
				longint = 0;
				longint = (win32x86End[$$ArrayIndex(properties)] - win32x86Start[$$ArrayIndex(properties)]) / 8;
			  win32x86Start:
				cstring;
			  win32x86End:
				align long;
#endif

			case SupportedModes:
				longint = '8BIM';
				key longint = 'mode';
				longint = 0;
#if DeRez
				fill long;
#else
				longint = (modeEnd[$$ArrayIndex(properties)] - modeStart[$$ArrayIndex(properties)]) / 8;
#endif
			  modeStart:
				boolean noBitmap, doesSupportBitmap;
				boolean noGrayScale, doesSupportGrayScale;
				boolean noIndexedColor, doesSupportIndexedColor;
				boolean noRGBColor, doesSupportRGBColor;
				boolean noCMYKColor, doesSupportCMYKColor;
				boolean noHSLColor, doesSupportHSLColor;
				boolean noHSBColor, doesSupportHSBColor;
				boolean noMultichannel, doesSupportMultichannel;
				boolean noDuotone, doesSupportDuotone;
				boolean noLABColor, doesSupportLABColor;
				fill bit[6];
			  modeEnd:
				align long;

			case EnableInfo:
				longint = '8BIM';
				key longint = 'enbl';
				longint = 0;
#if DeRez
				fill long;
#else
				longint = (EnableInfoEnd[$$ArrayIndex(properties)] - EnableInfoStart[$$ArrayIndex(properties)]) / 8;
#endif
			  EnableInfoStart:
				cstring;
			  EnableInfoEnd:
				align long;

/* Filter plug-in properties. */

			case FilterCaseInfo:
				longint = '8BIM';
				key longint = 'fici';
				longint = 0;
				longint = 28;
				array [7]
					{
					byte inCantFilter = 0,
						 inStraightData = 1,
						 inBlackMat = 2,
						 inGrayMat = 3,
						 inWhiteMat = 4,
						 inDefringe = 5,
						 inBlackZap = 6,
						 inGrayZap = 7,
						 inWhiteZap = 8,
						 inBackgroundZap = 10,
						 inForegroundZap = 11;
					byte outCantFilter = 0,
						 outStraightData = 1,
						 outBlackMat = 2,
						 outGrayMat = 3,
						 outWhiteMat = 4,
						 outFillMask = 9;
					fill bit [4];
					boolean doNotWriteOutsideSelection, writeOutsideSelection;
					boolean doesNotFilterLayerMasks, filtersLayerMasks;
					boolean doesNotWorkWithBlankData, worksWithBlankData;
					boolean copySourceToDestination, doNotCopySourceToDestination;
					fill byte;
					} ;

/* Export plug-in properties. */

			case ExportFlags:
				longint = '8BIM';
				key longint = 'expf';
				longint = 0;
#if DeRez
				fill long;
#else
				longint = (expFlagsEnd[$$ArrayIndex(properties)] - expFlagsStart[$$ArrayIndex(properties)]) / 8;
#endif
			 expFlagsStart:
				boolean expDoesNotSupportTransparency, expSupportsTransparency;
				fill bit[7];
			  expFlagsEnd:
				align long;

/* Format plug-in properties. */

			case FmtFileType:
				longint = '8BIM';
				key longint = 'fmTC';
				longint = 0;
				longint = 8;
				literal longint; /* Default file type. */
				literal longint; /* Default file creator. */

			case ReadTypes:
				longint = '8BIM';
				key longint = 'RdTy';
				longint = 0;
				longint = $$CountOf(ReadableTypes) * 8;
				wide array ReadableTypes { literal longint; literal longint; } ;

			case WriteTypes:
				longint = '8BIM';
				key longint = 'WrTy';
				longint = 0;
				longint = $$CountOf(WritableTypes) * 8;
				wide array WritableTypes { literal longint; literal longint; } ;

			case FilteredTypes:
				longint = '8BIM';
				key longint = 'fftT';
				longint = 0;
				longint = $$CountOf(FilteredTypes) * 8;
				wide array FilteredTypes { literal longint; literal longint; } ;

			case ReadExtensions:
				longint = '8BIM';
				key longint = 'RdEx';
				longint = 0;
				longint = $$CountOf(ReadableExtensions) * 4;
				wide array ReadableExtensions { literal longint; } ;

			case WriteExtensions:
				longint = '8BIM';
				key longint = 'WrEx';
				longint = 0;
				longint = $$CountOf(WriteableExtensions) * 4;
				wide array WriteableExtensions { literal longint; } ;

			case FilteredExtensions:
				longint = '8BIM';
				key longint = 'fftE';
				longint = 0;
				longint = $$CountOf(FilteredExtensions) * 4;
				wide array FilteredExtensions { literal longint; } ;

			case FormatFlags:
				longint = '8BIM';
				key longint = 'fmtf';
				longint = 0;
				longint = (fmtFlagsEnd[$$ArrayIndex(properties)] - fmtFlagsStart[$$ArrayIndex(properties)]) / 8;
			 fmtFlagsStart:
				boolean = false; /* Obsolete flag. */
				boolean fmtDoesNotSaveImageResources, fmtSavesImageResources;
				boolean fmtCannotRead, fmtCanRead;
				boolean fmtCannotWrite, fmtCanWrite;
				boolean fmtWritesAll, fmtCanWriteIfRead;
				fill bit[3];
			  fmtFlagsEnd:
				align long;

			case FormatMaxSize:
				longint = '8BIM';
				key longint = 'mxsz';
				longint = 0;
				longint = 4;
				Point;

			case FormatMaxChannels:
				longint = '8BIM';
				key longint = 'mxch';
				longint = 0;
				longint = $$CountOf(ChannelsSupported) * 2;
				wide array ChannelsSupported { integer; } ;
				align long;

/* Parser Properties. */

			case ParsableTypes:
				longint = '8BIM';
				key longint = 'psTY';
				longint = 0;
				longint = $$CountOf(ParsableTypes) * 8;
				wide array ParsableTypes { literal longint; literal longint; } ;

			case ParsableExtensions:
				longint = '8BIM';
				key longint = 'psEX';
				longint = 0;
				longint = $$CountOf(ParsableExtensions) * 4;
				wide array ParsableExtensions { literal longint; };

			case FilteredParsableTypes:
				longint = '8BIM';
				key longint = 'psTy';
				longint = 0;
				longint = $$CountOf(ParsableTypes) * 8;
				wide array ParsableTypes { literal longint; literal longint; } ;

			case FilteredParsableExtensions:
				longint = '8BIM';
				key longint = 'psEx';
				longint = 0;
				longint = $$CountOf(ParsableExtensions) * 4;
				wide array ParsableExtensions { literal longint; };

			case ParsableClipboardTypes:
				longint = '8BIM';
				key longint = 'psCB';
				longint = 0;
				longint = $$CountOf(ParsableClipboardTypes) * 4;
				wide array ParsableClipboardTypes { literal longint; };

/* Picker Properties. */

			case PickerID:
				longint = '8BIM';
				key longint = 'pnme';
				longint = 0;
#if DeRez
				fill long;
#else
				longint = (PickerIDEnd[$$ArrayIndex(properties)] - PickerIDStart[$$ArrayIndex(properties)]) / 8;
#endif
			  PickerIDStart:
				pstring;			// The ID string
			  PickerIDEnd:
				align long;

/* New in 4.0 */
				
			case HasTerminology:
				longint = '8BIM';
				key longint = 'hstm';
				longint = 0;
				longint = (hasTermEnd[$$ArrayIndex(properties)] - hasTermStart[$$ArrayIndex(properties)]) / 8;
			hasTermStart:
				longint = 0;	// version
				longint;		// classID
				longint;		// eventID
				integer;		// terminologyID
				cstring;		// scopeString
			hasTermEnd:
				align long;
			};
		};
	};

#undef PIPiplTypes

/********************************************************************************/

#ifndef PiMIVersion
#define PiMIVersion 0
#endif

type 'PiMI'
{
Start:				/* The following is common to all Photoshop 2.5 Plugin modules	  */
	integer;						/* The version number of the interface supported. */
	integer;						/* The sub-version number.						  */
	integer;						/* The plug-in's priority.                        */
	integer = (General-Start)/8;	/* The size of the general info.				  */
	integer = (TypeInfo-General)/8;	/* The size of the type specific info.			  */
	integer;						/* A bit mask indicating supported image modes.   */
	literal longint;				/* A required host if any.						  */
General:

	array {			/* The following is used only for Format Plugin modules			  */
		integer = $$Countof(TypeArray);				/* type count */
		integer = $$Countof(ExtensionArray);		/* extension count */
		byte  cannotRead, canRead;					/* Can we read using this format?				  */
		byte  cannotReadAll, canReadAll;			/* Can this plug-in read from all files?		  */
		byte  cannotWrite, canWrite;				/* Can we write using this format?				  */
		byte  cannotWriteIfRead, canWriteIfRead;	/* Can we write if we read using this format?	  */
		byte  doesntSaveResources, savesResources;	/* Does this file format save the resource data?  */
		#if PiMIVersion > 0
		byte  doesntSupportsFilterCalls, supportsFilterCalls; /* Supports filter calls for formats. */
		#else
		fill byte;					/* Padding */
		#endif
		wide array [16]
			{ integer; };			/* Maximum # of channels with each plug-in mode.  */
		integer;					/* Maximum rows allowed in document.			  */
		integer;					/* Maximum columns allowed in document.			  */
		literal longint;			/* The file type if we create a file.			  */
		literal longint;			/* The creator type if we create a file.		  */
		array TypeArray
		{
			literal longint;		/* The type-creator pairs supported.			  */
			literal longint;
		};
		array ExtensionArray
		{
			literal longint;		/* The extensions supported.					  */
		};
	};
TypeInfo:
};
	
#define supportsBitmap 			1
#define supportsGrayScale		2
#define supportsIndexedColor	4
#define supportsRGBColor		8
#define supportsCMYKColor	   16
#define supportsHSLColor	   32
#define supportsHSBColor	   64
#define supportsMultichannel  128
#define supportsDuotone		  256
#define supportsLABColor	  512

#define flagSupportsBitmap	 	 128
#define flagSupportsGrayScale	  64
#define flagSupportsIndexedColor  32
#define flagSupportsRGBColor	  16
#define flagSupportsCMYKColor	   8
#define flagSupportsHSLColor	   4
#define flagSupportsHSBColor	   2
#define flagSupportsMultichannel   1
#define flagSupportsDuotone		 128
#define flagSupportsLABColor	  64

#define latestAcquireVersion		4
#define latestAcquireSubVersion		0
#define latestExportVersion			4
#define latestExportSubVersion		0
#define latestExtensionVersion		1
#define latestExtensionSubVersion	0
#define latestFilterVersion			4
#define latestFilterSubVersion		0
#define latestFormatVersion			1
#define latestFormatSubVersion		0
#define latestParserVersion			1
#define latestParserSubVersion		0
#define latestSelectionVersion		1
#define latestSelectionSubVersion	0
#define latestPickerVersion			1
#define latestPickerSubVersion		0


#if 0

/* Example of Format plugin resource */

resource 'PiMI' (16000, purgeable)
{
	latestFormatVersion, 	/* Version, subVersion, and priority of the interface */
	latestFormatSubVersion,
	0,
	supportsGrayScale+supportsRGBColor+supportsCMYKColor,
	'8BIM',
	
	{
		canRead,
		cannotReadAll,
		canWrite,
		canWriteIfRead,
		doesntSaveResources,
		{ 0, 1, 0, 3,
		  4, 0, 0, 0,
		  0, 0, 0, 0,
		  0, 0, 0, 0 },
		32767,
		32767,
		'JPEG',
		'8BIM',
		{
			'JPEG', '8BIM'
		},
		{
			'JPG '
		}
	}
};

/* Example of Acquire/Export resource */

resource 'PiMI' (16001, purgeable)
{
	latestAcquireVersion, 	/* Version, subVersion, and priority of the interface */ 
	latestAcquireSubVersion, 
	0,
	supportsGrayScale+supportsRGBColor+supportsCMYKColor,
	'8BIM',
	{}			/* Null Format extension field */
};

#endif /* Examples */

#endif	/* qIncludeMe */

#endif	/* PIGeneral.r */
