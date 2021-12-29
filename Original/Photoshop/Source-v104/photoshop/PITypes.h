/*
	File: PITypes.h

	Copyright (c) 1992-6, Adobe Systems Incorporated.
	All rights reserved.

	This file describes the type definitions used in Photoshop's interface files.
*/

#ifndef __PITypes__
#define __PITypes__

/******************************************************************************/

/* Set up the preprocessor flags to indicate the target platform.	*/

#ifndef	MSWindows
#define	MSWindows	(MSDOS || WIN32)
#endif

#ifndef	Unix
#define	Unix		(__unix)
#endif

#ifndef Macintosh
#if (Unix || defined(macintosh) || defined(__powerc) || \
	defined(__MWERKS__) || defined(THINK_C)) && \
	!MSWindows
#define Macintosh 1
#else
#define Macintosh 0
#endif
#endif /* #ifndef Macintosh */

#ifndef MacintoshOS
#define	MacintoshOS	(Macintosh && !Unix)
#endif

#define	Macintosh68K	(MacintoshOS && m68k)
#define	MacintoshPPC	(MacintoshOS && __powerc)

#if defined (__BORLANDC__)
#pragma option -a-
#endif

/******************************************************************************/

/* Set up the flags for the Adobe Graphics Manager.	*/

#ifdef MAC_ENV
#undef MAC_ENV
#endif

#ifdef WIN_ENV
#undef WIN_ENV
#endif

#if Macintosh
#define MAC_ENV	1
#endif

#if MSWindows
#define WIN_ENV	1
#if WIN32
#define WINNT_ENV   1
#endif
#endif

/******************************************************************************/

#if Macintosh

#ifndef __PSWorld__

#include <c:\msdev\include\sys\Types.h>
#include <Memory.h>
#include <Resources.h>
#include <Quickdraw.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <Packages.h>
#include <Errors.h>
#include <Files.h>
#include <Aliases.h>
#include <AppleEvents.h>

#endif

#ifndef TRUE
#define TRUE	true
#endif

#ifndef FALSE
#define FALSE	false
#endif

#ifndef MACPASCAL
#define MACPASCAL pascal
#endif

#ifndef WINPASCAL
#define WINPASCAL
#endif

#ifndef __MWERKS__
#ifndef far
#define far
#endif
#endif

#ifndef huge
#define huge
#endif

#if !defined(__cplusplus) && !defined(__GNUC__)
#define const	/* nothing */
#endif

#ifndef __PSWorld__

typedef signed long  int32;
typedef signed short int16;
typedef signed char  int8;

typedef unsigned long  unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char  unsigned8;

/* The following types are used by AGM and we arrange to define them so that */
/* we won't have conflicts when working with AGM. */

typedef unsigned8	uint8;
typedef unsigned16	uint16;
typedef unsigned32	uint32;

typedef unsigned char	Bool8;
typedef unsigned long	Bool32;

#ifndef	OTHER_AGM_BASIC_H
#define OTHER_AGM_BASIC_H "PITypes.h"
#endif

#endif /* __PSWorld__ */

typedef struct PlatformData {

	short dummy;                    /* dummy variable required for some compilers. */

	} PlatformData;

#endif /* Macintosh */

/******************************************************************************/

#if MSWindows

#define MACPASCAL
#define WINPASCAL pascal 

#ifndef __PSWorld__
#include <windows.h>

/* Error codes. */
#define noErr 0

#define userCanceledErr (-128)
#define coercedParamErr 2

#define readErr			(-19)
#define writErr			(-20)
#define openErr			(-23)
#define dskFulErr			(-34)
#define ioErr				(-36)
#define eofErr				(-39)		// Also - end of descriptor error.
#define fnfErr				(-43)
#define vLckdErr			(-46)
#define fLckdErr			(-45)
#define paramErr			(-50)
#define memFullErr		(-108)
#define nilHandleErr		(-109)
#define memWZErr			(-111)


#define nil		   NULL
#define true       TRUE
#define false      FALSE

#ifndef __cplusplus
#define const
#endif

typedef signed long  int32;
typedef signed short int16;
typedef signed char  int8;

typedef unsigned long uint32;

typedef unsigned long  unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char  unsigned8;

typedef long Fixed;
typedef long Fract;
typedef long (*ProcPtr)();

#ifndef __TYPES__
#ifndef __GEOMETRY__
typedef struct Point
   {
	  short v;
	  short h;
   } Point;

typedef struct Rect
   {
	  short top;
	  short left;
	  short bottom;
	  short right;
   } Rect;
#endif
#endif

#ifndef __QUICKDRAW__
typedef struct
   {
	  WORD red;        /* Magnitude of red   component, 16 bit significant.*/
	  WORD green;      /* Magnitude of green component, 16 bit significant.*/
	  WORD blue;       /* Magnitude of blue  component, 16 bit significant.*/
   } RGBColor;
#endif

#ifndef __OCE__
typedef unsigned long DescType;
#endif

typedef LPSTR Ptr;
typedef LPSTR *Handle;
typedef BYTE  Boolean;
typedef DWORD OSType;
typedef short OSErr;
typedef unsigned long ResType;

typedef unsigned char Str255[256];  /*  first byte length of string. The string is zero terminated. */
typedef const unsigned char *ConstStr255Param;

typedef struct
	{
	short vRefNum;
	long parID;
	Str255 name;	/* Pascal String, for Windows files. */
	} FSSpec;

#endif

typedef struct PlatformData {

	long hwnd;        /* window to own dialogs. */

	} PlatformData;
	
short ShowAlert (short stringID); /* Developer implements this  */
	
#endif    /* Windows */

/******************************************************************************/

#ifndef __PSWorld__

/* BULLSHIT: Put this in to get BNT* files to compile. */

/* Common datatypes defined in PSWorld */
typedef unsigned8 LookUpTable [256];

typedef struct
	{
	LookUpTable R;
	LookUpTable G;
	LookUpTable B;
	} RGBLookUpTable;
	
/* Structures to hold colors in various spaces. */

typedef struct ShortRGB
	{
	int16 r;
	int16 g;
	int16 b;
	}
ShortRGB;

typedef struct ShortCMYK
	{
	int16 c;
	int16 m;
	int16 y;
	int16 k;
	}
ShortCMYK;

typedef struct ShortXYZ
	{
	int16 x;
	int16 y;
	int16 z;
	}
ShortXYZ;

typedef struct ShortLAB
	{
	int16 L;
	int16 a;
	int16 b;
	}
ShortLAB;


#ifndef __GEOMETRY__

//typedef Rect CRect;

#endif

#ifndef __UGEOMETRY__
typedef struct
	{
	int32 top;
	int32 left;
	int32 bottom;
	int32 right;
	} VRect;
	
typedef struct VPoint
	{
	int32 v;
	int32 h;
	} VPoint;
#endif /* __UGEOMETRY__ */

#if MacintoshOS
typedef char TVolumeName [32];
#else
typedef char TVolumeName [256];
#endif /* MacintoshOS */
	
/******************************************************************************/

/*
 * Display Ordering Values.  Passed to certain bottlenecks to tell them what
 * the data looks like, and used (on non-Macs) in the pmReserved field of a
 * pixmap to tag that data.  For compatibility, the values are designed to
 * match up with some Latitude defined values we created for the Unix 3.0.
 * - SLB
 */

enum DisplayByteOrder {			/* Data is... */
	displayOrderIndexed = 0,	/* 8-bit, or otherwise unspecified. */
	displayOrderURGB = 1,		/* 32-bit, high 8 unused, Mac uses this. */
	displayOrderUBGR = 2,		/* 32-bit, high 8 unused, Sun, SGI like this. */
	displayOrderRGBU = 3,		/* 32-bit, low 8 unused, for completeness. */
	displayOrderBGRU = 4,		/* 32-bit, good for little-endian (PC). */
	displayOrderRGBPacked = 5,	/* 24-bit, unaligned, currently unsupported!! */
	displayOrderBGRPacked = 6	/* 24-bit, unaligned, currently unsupported!! */
};

#endif	/* __PSWorld__ */

/******************************************************************************/

/* BULLSHIT: Temporary hack */

typedef struct {
    unsigned8   c;
    unsigned8   m;
    unsigned8   y;
    unsigned8   k;
} CMYKtuple;

typedef struct {
    unsigned8   alpha;
    unsigned8   r;
    unsigned8   g;
    unsigned8   b;
} RGBtuple;

typedef struct {
    unsigned8   alpha;
    unsigned8   h;
    unsigned8   s;
    unsigned8   l;
} HSLtuple;

typedef struct {
    unsigned8   alpha;
    unsigned8   h;
    unsigned8   s;
    unsigned8   b;
} HSBtuple;

typedef struct {
    unsigned8   alpha;
    unsigned8   L;
    unsigned8   a;
    unsigned8   b;
} LABtuple;

typedef unsigned8   inverseCLUT [32][32][32];
typedef unsigned32  histogram3 [][32][32];

typedef int16            HueSatTable [1536];
typedef LookUpTable      HueSatMaps[4];     /* general form of RGBLookUpTable */

/* hack for BNTLocal.c et al */
#ifndef __UMonitor__
typedef int16 Short3by3 [3] [3];
#endif /* __UMonitor__ */

/******************************************************************************/
#if defined (__BORLANDC__)   
#pragma option -a.
#endif

#endif /* __PITypes__ */
