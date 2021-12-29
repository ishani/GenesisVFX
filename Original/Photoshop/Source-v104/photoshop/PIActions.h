/*
	File: PIActions.h

	Copyright (c) 1996, Adobe Systems Incorporated.
	All rights reserved.

	This file describes Photoshop's scripting interface.
*/

#ifdef	__GNUG__
// Must be placed before _ANY_ #include lines!!!
#pragma interface
#endif	// __GNUG__

#ifndef __PIActions__
#define __PIActions__

#ifndef MSWindows
#define MSWindows (MSDOS || WIN32)
#endif

#ifndef INSIDE_PHOTOSHOP
#define INSIDE_PHOTOSHOP 0
#endif

#ifdef Rez

#ifdef SystemSevenOrLater
#undef SystemSevenOrLater
#endif

#define SystemSevenOrLater 1

#if !MSWindows
#include "Types.r"
#include "AEUserTermTypes.r"
#include "AppleEvents.r"
#endif

#else

#include "PITypes.h"

#if !MSWindows && !INSIDE_PHOTOSHOP
#include "AERegistry.h"
#endif

#endif

/********************************************************************************/

/* Flags for parameters */

#define flagsSingleParameter					\
	required,			singleItem,		notEnumerated,	reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

#define flagsListParameter						\
	required,			listOfItems,	notEnumerated,	reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

#define flagsEnumeratedParameter				\
	required,			singleItem,		enumerated,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

#define flagsEnumeratedListParameter			\
	required,			listOfItems,	enumerated,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

#define flagsOptionalSingleParameter			\
	optional,			singleItem,		notEnumerated,	reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

#define flagsOptionalListParameter				\
	optional,			listOfItems,	notEnumerated,	reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

#define flagsOptionalEnumeratedParameter		\
	optional,			singleItem,		enumerated,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

#define flagsOptionalEnumeratedListParameter	\
	optional,			listOfItems,	enumerated,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	prepositionParam,	notFeminine,	notMasculine,	singular

/* Flags for object propeties */

#define flagsSingleProperty						\
	reserved,			singleItem,		notEnumerated,	readWrite,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	noApostrophe,		notFeminine,	notMasculine,	singular

#define flagsListProperty						\
	reserved,			listOfItems,	notEnumerated,	readWrite,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	noApostrophe,		notFeminine,	notMasculine,	singular

#define flagsEnumeratedProperty					\
	reserved,			singleItem,		enumerated,		readWrite,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	noApostrophe,		notFeminine,	notMasculine,	singular

#define flagsEnumeratedListProperty				\
	reserved,			listOfItems,	enumerated,		readWrite,	\
	reserved,			reserved,		reserved,		reserved,	\
	reserved,			reserved,		reserved,		reserved,	\
	noApostrophe,		notFeminine,	notMasculine,	singular

/* Flags for direct parameters */

#define flagsOptionalEnumeratedChangesDirect	\
	directParamOptional,	singleItem,	enumerated,	changesState,	\
	reserved,				reserved,	reserved,	reserved,		\
	reserved,				reserved,	reserved,	reserved,		\
	reserved,				reserved,	reserved,	reserved

#define flagsEnumeratedChangesDirect	\
	directParamRequired,	singleItem,	enumerated,	changesState,	\
	reserved,				reserved,	reserved,	reserved,		\
	reserved,				reserved,	reserved,	reserved,		\
	reserved,				reserved,	reserved,	reserved

#define flagsEnumeratedDirect	\
	directParamRequired,	singleItem,	enumerated,	doesntChangeState,	\
	reserved,				reserved,	reserved,	reserved,			\
	reserved,				reserved,	reserved,	reserved,			\
	reserved,				reserved,	reserved,	reserved

#define flagsChangesDirect	\
	directParamRequired,	singleItem,	notEnumerated,	changesState,	\
	reserved,				reserved,	reserved,		reserved,		\
	reserved,				reserved,	reserved,		reserved,		\
	reserved,				reserved,	reserved,		reserved

/********************************************************************************/

/* Handy short cuts */

#define NO_REPLY		\
	noReply,			\
	"",					\
	replyRequired,	singleItem,	notEnumerated,	notTightBindingFunction,	\
	reserved,		reserved,	reserved,		reserved,					\
	reserved,		reserved,	reserved,		reserved,					\
	verbEvent,		reserved,	reserved,		reserved

#define IMAGE_DIRECT_PARAMETER	\
	typeImageReference,			\
	"",							\
	flagsOptionalEnumeratedChangesDirect
	
/********************************************************************************/

/* Normalize the AppleScript terminology */

#if MSWindows	// These items come from AppleScript

#define formName				'name'

#define typeFloat				'doub'
#define typeInteger				'long'
#define typeBoolean				'bool'
#define typeAlias				'alis'
#define typeNull				'null'
#define	roman					0
#define english					0
#define japanese				11
#define noReply					typeNull

#ifndef __APPLEEVENTS__
#define typeType				'type'
#define typeChar				'TEXT'
#define typeObjectSpecifier		'obj '
#endif /* __APPLEEVENTS __ */

#ifndef __AEOBJECTS__
#define formAbsolutePosition	'indx'
#define formPropertyID			'prop'
#endif /* __AEOBJECTS__ */

#endif /* MSWindows */

#if INSIDE_PHOTOSHOP
#define keySelection			'fsel'	// keySelection
#endif

#define keyInherits				'c@#^'	/* Hey, Apple defined it! */

#define formIndex				formAbsolutePosition
#define formEnumerated			'Enmr'
#define formProperty			formPropertyID

#define typePath				'Pth '
#define typeClass				typeType
#define typeText				typeChar
#define typeObjectReference		typeObjectSpecifier
#define typeUnitFloat			'UntF'	/* special type for unit values */

#if MSWindows
#define typePlatformFilePath	typePath
#else
#define typePlatformFilePath	typeAlias
#endif

#define unitDistance			'#Rlt'	/* unit value - base 72ppi */
#define unitAngle				'#Ang'	/* unit value - base degrees */
#define unitDensity				'#Rsl'	/* unit value - base per inch */

#define unitPixels				'#Pxl'	/* tagged unit value */
#define unitPercent				'#Prc'	/* tagged unit value */

/* Object types supplied by Photoshop */

#define classColor				'Clr '
#define typeClassColor			'#Clr'

#define classRGBColor			'RGBC'	/* keyRed, keyGreen, keyBlue */
#define classCMYKColor			'CMYC'	/* keyCyan, keyMagenta, keyYellow, keyBlack */
#define classUnspecifiedColor	'UnsC'
#define classGrayscale			'Grsc'	/* keyGray */
#define classBookColor			'BkCl'
#define classLabColor			'LbCl'
#define classHSBColor			'HSBC'

#define classPoint				'Pnt '	/* keyHorizontal, keyVertical */

/* Class types for plug-ins */

#define classImport				'Impr'
	#define keyImport				'Impr'

#define classExport				'Expr'
	#define keyExport				'Expr'

#define classFormat				'Fmt '
	#define kFormatProperty			'Fmt '

/* Enumeration supplied by Photoshop */

#define typeOrientation			'Ornt'
	#define enumHorizontal			'Hrzn'
	#define enumVertical			'Vrtc'

#define typeOrdinal				'Ordn'
	#define enumAll					'Al  '
	#define enumFirst				'Frst'
	#define enumLast				'Lst '
	#define enumNext				'Nxt '
	#define enumPrevious			'Prvs'
	#define enumMiddle				'Mddl'
	#define enumAny					'Any '
	#define enumNone				'None'
	#define enumTarget				'Trgt'
	#define enumForward				'Frwr'
	#define enumBackward			'Bckw'
	#define enumFront				'Frnt'
	#define enumBack				'Back'
	
/* Common properties and parameters */

#define keyA					'A   '
#define keyAdjustment			'Adjs'
#define keyAmount				'Amnt'
#define keyAngle				'Angl'
#define keyAntiAlias			'AntA'
#define keyApply				'Aply'
#define keyArea					'Ar  '
#define keyAs					'As  '
#define keyAt					'At  '
#define keyAuto					'Auto'
#define keyAxis					'Axis'
#define keyB					'B   '
#define keyBackgroundColor		'BckC'
#define keyBlack				'Blck'
#define keyBlue					'Bl  '
#define keyBook					'Bk  '
#define keyBrightness			'Brgh'
#define keyBy					'By  '
#define keyCenter				'Cntr'
#define keyChannel				'Chnl'
#define keyChannels				'Chns'		// Breaks rule for plural
#define keyColor				'Clr '
#define keyColorize				'Clrz'
#define keyCompression			'Cmpr'
#define keyContinue				'Cntn'
#define keyContrast				'Cntr'
#define keyCustom				'Cstm'
#define keyCyan					'Cyn '
#define keyDatum				'Dt  '
#define keyDepth				'Dpth'
#define keyDistance				'Dstn'
#define keyDistribution			'Dstr'
#define keyDither				'Dthr'
#define keyDuplicate			'Dplc'
#define keyEdge					'Edg '
#define keyEncoding				'Encd'
#define keyExtend				'Extd'
#define keyFill					'Fl  '
#define keyFlatness				'Fltn'
#define keyForegroundColor		'FrgC'
#define keyFrequency			'Frqn'
#define keyFrom					'From'
#define keyFuzziness			'Fzns'
#define keyGamma				'Gmm '
#define keyGray					'Gry '
#define keyGreen				'Grn '
#define keyGroup				'Grup'
#define keyHalftoneScreen		'HlfS'
#define keyHeight				'Hght'
#define keyHorizontal			'Hrzn'
#define keyHue					'H   '
#define keyIn					'In  '
#define keyInput				'Inpt'
#define keyInterpolation		'Intr'
#define keyInvert				'Invr'
#define keyKind					'Knd '
#define keyLevel				'Lvl '
#define keyLevels				'Lvls'
#define keyLightness			'Lght'
#define keyLocation				'Lctn'
#define keyLuminance			'Lmnc'
#define keyMagenta				'Mgnt'
#define keyMatrix				'Mtrx'
#define keyMaximum				'Mxm '
#define keyMerged				'Mrgd'
#define keyMessage				'Msge'
#define keyMethod				'Mthd'
#define keyMinimum				'Mnm '
#define keyMode					'Md  '
#define keyMonochromatic		'Mnch'
#define keyName					'Nm  '
#define keyNew					'Nw  '
#define keyOffset				'Ofst'
#define keyOpacity				'Opct'
#define keyOptimized			'Optm'
#define keyOutput				'Otpt'
#define keyPalette				'Plt '
#define keyPosition				'Pstn'
#define keyPreview				'Prvw'
#define keyOrientation			'Ornt'
#define keyQuality				'Qlty'
#define keyRadius				'Rds '
#define keyRatio				'Rt  '
#define keyRed					'Rd  '
#define keyRelative				'Rltv'
#define keyResample				'Rsmp'
#define keyResolution			'Rslt'
#define keyResponse				'Rspn'
#define keySaturation			'Strt'
#define keySavePaths			'SvPt'
#define keyScale				'Scl '
#define keyScans				'Scns'
#define keyShape				'Shp '
#define keySkew					'Skew'
#define keyThreshold			'Thsh'
#define keyTo					'T   '
#define keyType					'Type'
#define keyUntitled				'Untl'
#define keyUsing				'Usng'
#define keyValue				'Vl  '
#define keyVertical				'Vrtc'
#define keyWidth				'Wdth'
#define keyWith					'With'
#define keyYellow				'Ylw '

/* Photoshop object reference types */

#define classChannel			'Chnl'
#define typeChannelReference	'#ChR'
#define typeImageReference		'#ImR'	/* <Enumerated> Used as filter direct paramter */

/********************************************************************************/

#ifndef Rez

/********************************************************************************/

#define PIHasTerminologyProperty	0x6873746DL	/* 'hstm' <PITerminology> Has <aete, 0> resource:
														version number
														classID:		Object or event class
														eventID:		For events (typeNull otherwise)
														terminologyID:	'aete' resource ID (ignored if PITerminologyProperty)
														scopeString:	Unique string present if
																		AppleScript not supported
												*/
#define PITerminologyProperty		0x74726D6EL	/* 'trmn' <aete> Used internaly as terminology cache.
													If present it is used instead of an aete resource.
												*/

/* These are the flags for recordInfo */

enum
	{
	plugInDialogOptional,
	plugInDialogRequired,
	plugInDialogNone
	};

/* These flags are used for playInfo */

enum
	{
	plugInDialogDontDisplay,
	plugInDialogDisplay,
	plugInDialogSilent
	};
	
/********************************************************************************/

/* Flags returned by GetKey (low order word corresponds to AppleEvent flags */

#define actionSimpleParameter		0x00000000L
#define actionEnumeratedParameter	0x00002000L
#define actionListParameter			0x00004000L
#define actionOptionalParameter		0x00008000L

#define actionObjectParameter		0x80000000L
#define actionScopedParameter		0x40000000L
#define actionStringIDParameter		0x20000000L

/********************************************************************************/

typedef Handle PIDescriptorHandle;

typedef struct PIOpaqueWriteDescriptor*	PIWriteDescriptor;
typedef struct PIOpaqueReadDescriptor*	PIReadDescriptor;

typedef unsigned long	DescriptorEventID;
typedef unsigned long	DescriptorClassID;

typedef unsigned long	DescriptorKeyID;
typedef unsigned long	DescriptorTypeID;
typedef unsigned long	DescriptorUnitID;
typedef unsigned long	DescriptorEnumID;

typedef DescriptorKeyID	DescriptorKeyIDArray[];


#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

#if WIN32
#pragma pack(push,1)
#endif

typedef struct PITerminology
	{
	int32	version;
	OSType	classID;
	OSType	eventID;
	int16	terminologyID;
	char	scopeString[1];		// C string
	} PITerminology;

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

#if WIN32
#pragma pack(pop)
#endif

#define PITerminologyMinSize 15

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

typedef struct PIDescriptorSimpleReference
	{
	DescriptorTypeID	desiredClass;
	DescriptorKeyID		keyForm;
	struct _keyData
		{
		Str255				name;
		int32				index;
		DescriptorTypeID	type;
		DescriptorEnumID	value;
		}				keyData;
	} PIDescriptorSimpleReference;
	
#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

/********************************************************************************/

#ifdef __cplusplus
extern "C"
	{
#endif
/********************************************************************************/

typedef MACPASCAL PIWriteDescriptor (*OpenWriteDescriptorProc)(void);
typedef MACPASCAL OSErr (*CloseWriteDescriptorProc)(PIWriteDescriptor, PIDescriptorHandle*);

typedef MACPASCAL OSErr (*PutIntegerProc)(PIWriteDescriptor, DescriptorKeyID, int32);
typedef MACPASCAL OSErr (*PutFloatProc)(PIWriteDescriptor, DescriptorKeyID, const double*);
typedef MACPASCAL OSErr (*PutUnitFloatProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorUnitID, const double*);
typedef MACPASCAL OSErr (*PutBooleanProc)(PIWriteDescriptor, DescriptorKeyID, Boolean);
typedef MACPASCAL OSErr (*PutTextProc)(PIWriteDescriptor, DescriptorKeyID, Handle);
typedef MACPASCAL OSErr (*PutAliasProc)(PIWriteDescriptor, DescriptorKeyID, Handle);

typedef MACPASCAL OSErr (*PutEnumeratedProc)(PIWriteDescriptor, DescriptorKeyID key, DescriptorTypeID type, DescriptorEnumID value);
typedef MACPASCAL OSErr (*PutClassProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID);
typedef MACPASCAL OSErr (*PutSimpleReferenceProc)(PIWriteDescriptor, DescriptorKeyID, const PIDescriptorSimpleReference*);
typedef MACPASCAL OSErr (*PutObjectProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID, PIDescriptorHandle);

typedef MACPASCAL OSErr	(*PutCountProc)(PIWriteDescriptor, DescriptorKeyID, uint32 count);

typedef MACPASCAL OSErr (*PutStringProc)(PIWriteDescriptor, DescriptorKeyID, ConstStr255Param);

/* Scoped classes are not for use by plug-ins in Photoshop 4.0 */

typedef MACPASCAL OSErr (*PutScopedClassProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID);
typedef MACPASCAL OSErr (*PutScopedObjectProc)(PIWriteDescriptor, DescriptorKeyID, DescriptorTypeID, PIDescriptorHandle);

/********************************************************************************/

typedef MACPASCAL PIReadDescriptor (*OpenReadDescriptorProc)(PIDescriptorHandle, DescriptorKeyIDArray);
typedef MACPASCAL OSErr (*CloseReadDescriptorProc)(PIReadDescriptor);
typedef MACPASCAL Boolean (*GetKeyProc)(PIReadDescriptor, DescriptorKeyID* key, DescriptorTypeID* type, int32* flags);

typedef MACPASCAL OSErr (*GetIntegerProc)(PIReadDescriptor, int32*);
typedef MACPASCAL OSErr (*GetFloatProc)(PIReadDescriptor, double*);
typedef MACPASCAL OSErr (*GetUnitFloatProc)(PIReadDescriptor, DescriptorUnitID*, double*);
typedef MACPASCAL OSErr (*GetBooleanProc)(PIReadDescriptor, Boolean*);
typedef MACPASCAL OSErr (*GetTextProc)(PIReadDescriptor, Handle*);
typedef MACPASCAL OSErr (*GetAliasProc)(PIReadDescriptor, Handle*);

typedef MACPASCAL OSErr (*GetEnumeratedProc)(PIReadDescriptor, DescriptorEnumID*);
typedef MACPASCAL OSErr (*GetClassProc)(PIReadDescriptor, DescriptorTypeID*);
typedef MACPASCAL OSErr (*GetSimpleReferenceProc)(PIReadDescriptor, PIDescriptorSimpleReference*);
typedef MACPASCAL OSErr (*GetObjectProc)(PIReadDescriptor, DescriptorTypeID*, PIDescriptorHandle*);

typedef MACPASCAL OSErr (*GetCountProc)(PIReadDescriptor, uint32*);

typedef MACPASCAL OSErr (*GetStringProc)(PIReadDescriptor, Str255*);
typedef MACPASCAL OSErr (*GetPinnedIntegerProc)(PIReadDescriptor, int32 min, int32 max, int32*);
typedef MACPASCAL OSErr (*GetPinnedFloatProc)(PIReadDescriptor, const double* min, const double* max, double*);
typedef MACPASCAL OSErr (*GetPinnedUnitFloatProc)(PIReadDescriptor, const double* min, const double* max, DescriptorUnitID*, double*);

/********************************************************************************/

#ifdef __cplusplus
	}
#endif

/********************************************************************************/

#define kCurrentWriteDescriptorProcsVersion 0

/********************************************************************************/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

typedef struct WriteDescriptorProcs
	{
	int16 writeDescriptorProcsVersion;
	int16 numWriteDescriptorProcs;
	
	OpenWriteDescriptorProc		openWriteDescriptorProc;
	CloseWriteDescriptorProc	closeWriteDescriptorProc;

	PutIntegerProc				putIntegerProc;
	PutFloatProc				putFloatProc;
	PutUnitFloatProc			putUnitFloatProc;
	PutBooleanProc				putBooleanProc;
	PutTextProc					putTextProc;
	PutAliasProc				putAliasProc;

	PutEnumeratedProc			putEnumeratedProc;
	PutClassProc				putClassProc;
	PutSimpleReferenceProc		putSimpleReferenceProc;
	PutObjectProc				putObjectProc;
	
	PutCountProc				putCountProc;
	
	PutStringProc				putStringProc;
	
	/* Scoped classes are not for use by plug-ins in Photoshop 4.0 */
	
	PutScopedClassProc			putScopedClassProc;
	PutScopedObjectProc			putScopedObjectProc;
	
	} WriteDescriptorProcs;

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

/********************************************************************************/

#define kCurrentWriteDescriptorProcsCount \
	((sizeof(WriteDescriptorProcs) - offsetof(WriteDescriptorProcs, openWriteDescriptorProc)) / sizeof(void*))
	
/********************************************************************************/

#define kCurrentReadDescriptorProcsVersion 0

/********************************************************************************/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

typedef struct ReadDescriptorProcs
	{
	int16 readDescriptorProcsVersion;
	int16 numReadDescriptorProcs;
	
	OpenReadDescriptorProc		openReadDescriptorProc;
	CloseReadDescriptorProc		closeReadDescriptorProc;
	GetKeyProc					getKeyProc;

	GetIntegerProc				getIntegerProc;
	GetFloatProc				getFloatProc;
	GetUnitFloatProc			getUnitFloatProc;
	GetBooleanProc				getBooleanProc;
	GetTextProc					getTextProc;
	GetAliasProc				getAliasProc;

	GetEnumeratedProc			getEnumeratedProc;
	GetClassProc				getClassProc;
	GetSimpleReferenceProc		getSimpleReferenceProc;
	GetObjectProc				getObjectProc;

	GetCountProc				getCountProc;
	
	GetStringProc				getStringProc;
	GetPinnedIntegerProc		getPinnedIntegerProc;
	GetPinnedFloatProc			getPinnedFloatProc;
	GetPinnedUnitFloatProc		getPinnedUnitFloatProc;

	} ReadDescriptorProcs;

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

/********************************************************************************/

#define kCurrentReadDescriptorProcsCount \
	((sizeof(ReadDescriptorProcs) - offsetof(ReadDescriptorProcs, openReadDescriptorProc)) / sizeof(void*))
	
/********************************************************************************/

#define kCurrentDescriptorParametersVersion 0

/********************************************************************************/

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=mac68k
#endif

typedef struct PIDescriptorParameters
	{
	int16					descriptorParametersVersion;
	int16					playInfo;
	int16					recordInfo;
	
	PIDescriptorHandle		descriptor;
	
	WriteDescriptorProcs*	writeDescriptorProcs;
	ReadDescriptorProcs*	readDescriptorProcs;
	} PIDescriptorParameters;

#if PRAGMA_ALIGN_SUPPORTED
#pragma options align=reset
#endif

/********************************************************************************/

#endif

/********************************************************************************/

#endif
