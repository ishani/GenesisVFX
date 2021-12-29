/*
	File:  PIUtilities.h
	
	Copyright (c) 1993-6, Adobe Systems Incorporated.
	All rights reserved.

	The routines in this module provide a collection of utilities for accessing
 	the plug-in callback procedures and performing other useful tasks within
  	plug-ins.
*/
 
#ifndef __PIUtilities_H__
#define __PIUtilities_H__

#include <stddef.h>
#include <c:\msdev\include\sys\Types.h>
#include "PITypes.h"
#include "PIGeneral.h"
#include "PIActions.h"
#include "PIAbout.h"

/*****************************************************************************/

#if Macintosh
#include "DialogUtilities.h"
#include "LowMem.h"
#define PISetRect	SetRect
#else
#include <stdlib.h>

#include <winver.h>
#include "WinUtilities.h"
#include "WinDialogUtils.h"
Fixed FixRatio(short numer, short denom);
#endif

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* Map to resource strings */

// Some of these are generic.  Override if you need to in your .h files.

#define ResourceID		16000
#define AboutID			ResourceID
#define uiID			ResourceID+1
#define AlertID			16990
#define kBadNumberID	AlertID
#define kBadDoubleID	kBadNumberID+1
#define kNeedVers		kBadDoubleID+1
#define kWrongHost		kNeedVers+1
#define StringResource	'STR '

/*****************************************************************************/
/* Gets the platform window ptr.			 */

Handle HostGetPlatformWindowPtr (AboutRecordPtr aboutPtr);

#define PIPlatform()	\
	HostGetPlatformWindowPtr((AboutRecordPtr)gStuff)

/*****************************************************************************/

/* The following routines provide shells around the buffer procs routines.
   These routines allow us to allocate buffers from Photoshop's memory
   without first telling Photoshop about it in the bufferSpace or maxSpace
   parameter in the interface record.  This can be useful when we need
   different sized buffers at different times. */

/* Are the buffer procs even available?  If not, the plug-in will have to
   put up a warning dialog to indicate that it requires Photoshop 2.5 or
   will have to work around this using the old memory management techniques
   documented in earlier versions of the plug-in kit.  tooNew is set if the
   procs version is newer than the plug-in.  The buffer procs version number
   is unlikely to change, but it is wise to test it anyway.  If tooNew is
   null, it will be ignored. */

Boolean HostBufferProcsAvailable (BufferProcs *procs, Boolean *tooNew);

/* The following dialog takes care of putting up the warning if the appropriate
   version of the buffer procs is not available. */

Boolean WarnHostBufferProcsAvailable (BufferProcs *procs, Handle hDllInstance);

/* How much space is available for buffers?  This space may be fragmented. */

int32 HostBufferSpace (BufferProcs *procs); 

/* Allocate a buffer of the appropriate size setting bufferID to the ID
   for the buffer.  If an error occurs, the error code will be returned
   and bufferID will be set to zero. */

OSErr HostAllocateBuffer (BufferProcs *procs, int32 size, BufferID *bufferID);

/* Free the buffer with the given ID. */

void HostFreeBuffer (BufferProcs *procs, BufferID bufferID);

/* Lock the buffer and return a pointer to its contents. */

Ptr HostLockBuffer (BufferProcs *procs, BufferID bufferID, Boolean moveHigh);

/* Unlock the buffer.  Lock and unlock calls manipulate a counter, so they
   must balance perfectly. */

void HostUnlockBuffer (BufferProcs *procs, BufferID bufferID);

/* The following routine allocates a buffer which is as tall as possible.  It
   takes as parameters, the desired rowBytes, the minimum height, the
   maximum height, and the fraction of the available buffer space to use
   expressed as 1/numBuffers.  It sets the actual height and bufferID
   parameters if successful. */

OSErr HostAllocateStripBuffer (BufferProcs *procs,
							   int32 rowBytes,
							   int16 minHeight,
							   int16 maxHeight,
							   int16 numBuffers,
							   int16 *actualHeight,
							   BufferID *bufferID);

/*****************************************************************************/

/*****************************************************************************/
/* Check for the advance state procedure and warn if not present.			 
   The macros assume that gStuff is defined somewhere as a pointer
   to the current interface record. */

Boolean HostAdvanceStateAvailable (AdvanceStateProc proc);

Boolean WarnHostAdvanceStateAvailable (AdvanceStateProc proc, Handle hDllInstance);
   
#define AdvanceStateAvailable() \
	HostAdvanceStateAvailable (gStuff->advanceState)

#define WarnAdvanceStateAvailable() \
	WarnHostAdvanceStateAvailable (gStuff->advanceState, hDllInstance)
	
#define AdvanceState() \
	(*(gStuff->advanceState)) ()

/*****************************************************************************/

/* Here are the routines for the buffer suite. */

#define BufferProcsAvailable(tooNew) \
	HostBufferProcsAvailable (gStuff->bufferProcs, tooNew)

#define WarnBufferProcsAvailable() \
	WarnHostBufferProcsAvailable (gStuff->bufferProcs, hDllInstance)

#define BufferSpace() HostBufferSpace (gStuff->bufferProcs)

#define AllocateBuffer(size,bufferID) \
	HostAllocateBuffer (gStuff->bufferProcs, size, bufferID)

#define FreeBuffer(bufferID) \
	HostFreeBuffer (gStuff->bufferProcs, bufferID)

#define LockBuffer(bufferID,moveHigh) \
	HostLockBuffer (gStuff->bufferProcs, bufferID, moveHigh)

#define UnlockBuffer(bufferID) \
	HostUnlockBuffer (gStuff->bufferProcs, bufferID)

#define AllocateStripBuffer(rowBytes,minHeight,maxHeight,numBuffers,actualHeight,bufferID) \
	HostAllocateStripBuffer (gStuff->bufferProcs,\
							 rowBytes,\
							 minHeight,\
							 maxHeight,\
							 numBuffers,\
							 actualHeight,\
							 bufferID)

/****************************************************************************
 * Similarly assuming gStuff to be defined, we define macros for testing
 * for abort and for updating the progress bar.
 */
 
#define TestAbort() ((*gStuff->abortProc) ())

#define UpdateProgress(done,total) ((*gStuff->progressProc) (done, total))

/*****************************************************************************/

/* Here is a corresponding set of routines and macros for the pseudo-resource
 * callbacks.
 * These macros also make the additional assumption that hDllInstance is
 * defined as a global variable (NULL on Mac, value for Windows).
 */
 
Boolean HostResourceProcsAvailable (ResourceProcs *procs, Boolean *tooNew);

Boolean WarnHostResourceProcsAvailable (ResourceProcs *procs, Handle hDllInstance);

int16 HostCountPIResources (ResourceProcs *procs, ResType type);

Handle HostGetPIResource (ResourceProcs *procs, ResType type, int16 index);

void HostDeletePIResource (ResourceProcs *procs, ResType type, int16 index);

OSErr HostAddPIResource (ResourceProcs *procs, ResType type, Handle data);

Ptr HostGetResource (Handle hDllInstance, ResType type, int32 index, int32 *size);

// int32 resource to 4-chars
void PIType2Char (const ResType type, char *name);

// 4-chars to int32 resource
ResType PIChar2Type (char *name);

void PIReleaseResource(Ptr p);

void HostGetString (Handle hDllInstance, Str255 s, int32 index);

#define ResourceProcsAvailable(tooNew)										\
	HostResourceProcsAvailable (gStuff->resourceProcs, tooNew)
	
#define WarnResourceProcsAvailable()										\
	WarnHostResourceProcsAvailable (gStuff->resourceProcs, hDllInstance)
	
#define CountPIResources(type)												\
	HostCountPIResources (gStuff->resourceProcs, type)
	
#define GetPIResource(type,index)											\
	HostGetPIResource (gStuff->resourceProcs, type, index)
	
#define DeletePIResource(type,index)										\
	HostDeletePIResource (gStuff->resourceProcs, type, index)
	
#define AddPIResource(type,data)											\
	HostAddPIResource (gStuff->resourceProcs, type, data)

#define PIGetResource(type, index, size)									\
	HostGetResource(hDllInstance, type, index, size)

#define PIGetString(s, index)												\
	HostGetString(hDllInstance, s, index)

/*****************************************************************************/

/* And a set for the handle routines. */
 
Boolean HostHandleProcsAvailable (HandleProcs *procs, Boolean *tooNew);

Boolean WarnHostHandleProcsAvailable (HandleProcs *procs, Handle hDllInstance);

Handle HostNewHandle (HandleProcs *procs, int32 size);

void HostDisposeHandle (HandleProcs *procs, Handle h);

void HostBlockMove (Ptr p1, Ptr p2, const int32 amount);

void HostCopy (void *s1, const void *s2, const int32 amount);

int32 HostGetHandleSize (HandleProcs *procs, Handle h);

OSErr HostSetHandleSize (HandleProcs *procs, Handle h, int32 newSize);

Ptr HostLockHandle (HandleProcs *procs, Handle h, Boolean moveHigh);

void HostUnlockHandle (HandleProcs *procs, Handle h);

void HostHandle2String (HandleProcs *procs, Handle h, Str255 s);

Handle HostString2Handle (HandleProcs *procs, Str255 s);

Handle HostHandleCat (HandleProcs *procs, Handle h1, Handle h2);

int16 HostMatch(char *s1, const char *s2, Boolean ignoreCase);

int16 HostStringMatch(Str255 s1, char *s2, Boolean ignoreCase);

char UpperToLower(const char c1);

int16 HostStringCat(char *s1, const char *s2);

#define HandleProcsAvailable(tooNew)										\
	HostHandleProcsAvailable (gStuff->handleProcs, tooNew)
	
#define WarnHandleProcsAvailable()											\
	WarnHostHandleProcsAvailable (gStuff->handleProcs, hDllInstance);
	
#define PINewHandle(size)													\
	HostNewHandle (gStuff->handleProcs, size)
	
#define PIDisposeHandle(h)													\
	HostDisposeHandle (gStuff->handleProcs, h)
	
#define PIGetHandleSize(h)													\
	HostGetHandleSize (gStuff->handleProcs, h)
	
#define PISetHandleSize(h,size)												\
	HostSetHandleSize (gStuff->handleProcs, h, size)
	
#define PILockHandle(h,moveHigh)											\
	HostLockHandle (gStuff->handleProcs, h, moveHigh)
	
#define PIUnlockHandle(h)													\
	HostUnlockHandle (gStuff->handleProcs, h)

#define PIString2Handle(s)													\
	HostString2Handle (gStuff->handleProcs, s)
	
#define PIHandle2String(h, s)												\
	HostHandle2String (gStuff->handleProcs, h, s)

#define PIHandleCat(h1, h2)													\
	HostHandleCat(gStuff->handleProcs, h1, h2)
	
#define PICloneHandle(h)													\
	HostHandleCat(gStuff->handleProcs, NULL, h)
	
#define PICopy(s1, s2, size)												\
	HostCopy(s1, s2, size)

#define PIBlockMove(p1, p2, size)											\
	HostBlockMove (p1, p2, size)
	
#define noMatch	-1
#define gotMatch 0
// any other number is exact match at offset #

#define PIMatch(s1, s2)														\
	HostMatch(s1, s2, true)
	
#define PISMatch(s1, s2)													\
	HostStringMatch(s1, s2, true)

#define PIStringCat(s1, s2)													\
	HostStringCat(s1, s2)
 
/*****************************************************************************/

/* Routines for Display Pixels procs */

Boolean HostDisplayPixelsAvailable (DisplayPixelsProc proc);

Boolean WarnHostDisplayPixelsAvailable (DisplayPixelsProc proc,
										Handle hDllInstance);

#define DisplayPixelsAvailable() \
	HostDisplayPixelsAvailable (gStuff->displayPixels)

#define	WarnDisplayPixelsAvailable() \
	WarnHostDisplayPixelsAvailable (gStuff->displayPixels, hDllInstance)

/*****************************************************************************/

/* Routines for Property procs */

Boolean HostPropertyAvailable (PropertyProcs *procs, Boolean *tooNew);

Boolean WarnHostPropertyAvailable (PropertyProcs *procs, Handle hDllInstance);

#define PropertyAvailable() \
	HostPropertyAvailable (gStuff->propertyProcs)

#define WarnPropertyAvailable()	\
	WarnHostPropertyAvailable (gStuff->propertyProcs, hDllInstance)

#define PIGetProp		gStuff->propertyProcs->getPropertyProc
#define PISetProp		gStuff->propertyProcs->setPropertyProc

#define GetSimple(key, simple)	\
	PIGetProp('8BIM', key, 0, simple, nil)

#define PutSimple(key, simple)  \
	PISetProp('8BIM', key, 0, simple, nil)
	
#define GetComplex(key, index, complex)	\
	PIGetProp('8BIM', key, index, 0, complex)
	
#define PutComplex(key, index, complex)	\
	PISetProp('8BIM', key, index, 0, complex)
	
/*****************************************************************************/

/* Routines for the Channel Port procs */
	
Boolean HostChannelPortAvailable (ChannelPortProcs *procs, Boolean *tooNew);

Boolean WarnHostChannelPortAvailable (ChannelPortProcs *procs, Handle hDllInstance);

#define ChannelPort \
	(gStuff->channelPortProcs)
	
#define ReadPixels \
	(ChannelPort->readPixelsProc)

#define WritePixels \
    (ChannelPort->writeBasePixelsProc)

#define ReadFromWritePort \
	(ChannelPort->readPortForWritePortProc)

#define ChannelPortAvailable(tooNew) \
	HostChannelPortAvailable (ChannelPort, tooNew)

#define WarnChannelPortAvailable() \
	WarnHostChannelPortAvailable (ChannelPort, hDllInstance)
	
/*****************************************************************************/

short HostReportError(Str255 s1, Str255 s2);

#define PIReportError(errString) \
	HostReportError(*(gStuff->errorString), errString)

/*****************************************************************************/

/* Here are the routines for the scripting system */
	
#define NULLID				0 // for ID routines needing null terminator

Boolean HostDescriptorAvailable (PIDescriptorParameters *procs, Boolean *tooNew);

Boolean WarnHostDescriptorAvailable (PIDescriptorParameters *procs, Handle hDllInstance);

/* Closes read descriptor and disposes handle. */
OSErr	HostCloseReader (PIDescriptorParameters *procs, HandleProcs *hProcs, PIReadDescriptor *token);

/* Closes write descriptor.  Disposes old handle, if present, and sets
   recordInfo to plugInDialogOptional. */
OSErr	HostCloseWriter (PIDescriptorParameters *procs,
						 HandleProcs *hProcs,
						 PIWriteDescriptor *token);

/* Puts an object and disposes its handle, returning any error. */
OSErr HostPutObj (PIDescriptorParameters *procs, HandleProcs *hProcs, 
				  PIWriteDescriptor desc, DescriptorKeyID key, DescriptorTypeID type,
				  PIDescriptorHandle *h);
				  
/* Returns whether playInfo is plugInDialogDisplay, meaning to absolutely pop
   the plug-in dialog. */
Boolean HostPlayDialog (PIDescriptorParameters *procs);

#define DescParams 	gStuff->descriptorParameters

#define Reader 		DescParams->readDescriptorProcs

#define Writer 		DescParams->writeDescriptorProcs

#define PlayInfo	DescParams->playInfo

#define RecordInfo	DescParams->recordInfo

#define	PlayDialog() \
	HostPlayDialog (DescParams)

#define DescriptorAvailable() \
	HostDescriptorAvailable(DescParams, false)

#define WarnDescriptorAvailable() \
	WarnHostDescriptorAvailable(DescParams, hDllInstance)

#define OpenReadDesc(desc, array) \
	Reader->openReadDescriptorProc(desc, array)

#define	OpenReader(array) \
	OpenReadDesc(DescParams->descriptor, array)
	
#define CloseReadDesc(token) \
	Reader->closeReadDescriptorProc(token)

#define CloseReader(token) \
	HostCloseReader(DescParams, gStuff->handleProcs, token)
	
#define OpenWriter() \
	Writer->openWriteDescriptorProc()

#define CloseWriteDesc(token, handle) \
	Writer->closeWriteDescriptorProc(token, handle)

#define CloseWriter(token) \
	HostCloseWriter(DescParams, gStuff->handleProcs, token)

#define PIGetKey(token, key, type, flags) \
	Reader->getKeyProc(token, key, type, flags)
	
#define PIGetEnum(token, value)	\
	Reader->getEnumeratedProc(token, value)
				
#define PIPutEnum(token, key, type, value) \
	Writer->putEnumeratedProc(token, key, type, value)
	
#define PIGetInt(token, value) \
	Reader->getIntegerProc(token, value)

#define PIGetPinInt(token, min, max, value) \
	Reader->getPinnedIntegerProc(token, min, max, value)

#define PIPutInt(token, key, value) \
	Writer->putIntegerProc(token, key, value)
	
#define PIGetFloat(token, value) \
	Reader->getFloatProc(token, value)
	
#define PIGetPinFloat(token, min, max, value) \
	Reader->getPinnedFloatProc(token, min, max, value)

#define PIPutFloat(token, key, value) \
	Writer->putFloatProc(token, key, value)
	
#define PIGetUnitFloat(token, unit, value) \
	Reader->getUnitFloatProc(token, unit, value)

#define PIGetPinUnitFloat(token, min, max, unit, value) \
	Reader->getPinnedUnitFloatProc(token, min, max, unit, value)

#define PIPutUnitFloat(token, key, unit, value) \
	Writer->putUnitFloatProc(token, key, unit, value)
	
#define PIGetBool(token, value) \
	Reader->getBooleanProc(token, value)

#define PIPutBool(token, key, value) \
	Writer->putBooleanProc(token, key, value)

#define PIGetText(token, value) \
	Reader->getTextProc(token, value)
	
#define PIPutText(token, key, value) \
	Writer->putTextProc(token, key, value)
	
#define PIGetAlias(token, value) \
	Reader->getAliasProc(token, value)
	
#define PIPutAlias(token, key, value) \
	Writer->putAliasProc(token, key, value)

#define PIGetEnum(token, value) \
	Reader->getEnumeratedProc(token, value)

#define PIPutEnum(token, key, type, value) \
	Writer->putEnumeratedProc(token, key, type, value)

#define PIGetClass(token, value) \
	Reader->getClassProc(token, value)
	
#define PIPutClass(token, key, value) \
	Writer->putClassProc(token, key, value)
	
#define PIGetRef(token, value) \
	Reader->getSimpleReferenceProc(token,value)
	
#define PIPutRef(token, key, value) \
	Writer->putSimpleReferenceProc(token, key, value)
	
#define PIGetObj(token, type, value) \
	Reader->getObjectProc(token, type, value)

#define PIPutObj(token, key, type, value) \
	HostPutObj(DescParams, gStuff->handleProcs, token, key, type, value)

#define PIPutObjProc(token, key, type, value) \
	Writer->putObjectProc(token, key, type, value)
	
#define PIGetCount(token, value) \
	Reader->getCountProc(token, value)
	
#define PIPutCount(token, key, value) \
	Writer->putCountProc(token, key, value)
	
#define PIGetStr(token, value) \
	Reader->getStringProc(token, value)
	
#define PIPutStr(token, key, value) \
	Writer->putStringProc(token, key, value)

/*****************************************************************************/

/* The following macros assume that gStuff is defined somewhere as a pointer
   to the current interface record. */
   
Boolean HostColorServicesAvailable (ColorServicesProc proc);

Boolean WarnHostColorServicesAvailable (ColorServicesProc proc, Handle hDLLinstance);

/* Sets a color array from four int16 colors */
void CSSetColor (int16 *color, int16 color1, int16 color2, int16 color3, int16 color4);

/* Copies one color2 array to color1 */
void CSCopyColor (int16 *color1, int16 *color2);

OSErr HostCSConvertColor (ColorServicesProc proc,
						 int16 sourceSpace,
						 int16 resultSpace,
						 int16 *color);

/* Returns number of expected planes for imageMode */
int16 CSPlanesFromMode (int16 imageMode, int16 currPlanes);

/* Maps imageMode to color services space.  Returns -1 if notSupported. */
int16 CSModeToSpace (int16 imageMode);

#define ColorServicesAvailable() \
	HostColorServicesAvailable (gStuff->colorServices)

#define WarnColorServicesAvailable() \
	WarnHostColorServicesAvailable (gStuff->colorServices, hDllInstance)
	
#define ColorServices(info) \
	(*(gStuff->colorServices)) (info)

/* Converts a color array from sourceSpace to targetSpace, returning in array */
#define CSConvertColor(source, result, color) \
	HostCSConvertColor (gStuff->colorServices, source, result, color)

/*****************************************************************************/
/* Gestalt-like OS functions */

typedef struct OSInfo
{
	ResType		signature;
	unsigned8	majorVersion;
	unsigned8	minorVersion;
	unsigned8	subVersion;
	unsigned8	stage;
	unsigned8	stageVersion;
	unsigned8	os;
} OSInfo, *POSInfo, **HOSInfo;

enum
{ // "stage" will be one of these:
	OSStageDevelopment,
	OSStageAlpha,
	OSStageBeta,
	OSStageRelease
};

enum
{ // "os" will be one of these:
	OSMacOS6 = 1,
	OSMacOS7,
	OSMacOS8,
	OSWinNT = 10,
	OSWin16,
	OSWin32
};

#define OSMacOS1	OSMacOS6
#define OSMacOSEnd	OSMacOS8

#define OSWin1		OSWinNT
#define OSWinEnd	OSWin32

// Macintosh ResTypes and Windows file names for popular Adobe apps
#define rtAfterEffects		'FXTC'
#define fnAfterEffects		"Adobe After Effects"

#define rtIllustrator		'ART5'
#define fnIllustrator		"Adobe Illustrator"

#define rtPageMaker5		'ALD5'
#define fnPageMaker5		"PM5"

#define rtPageMaker6		'ALD6'
#define fnPageMaker6		"PM6"

#define rtPageMaker65		'AD65'
#define fnPageMaker65		"PM65"

#define rtPhotoDeluxe		'PHUT'
#define fnPhotoDeluxe		"PhotoDeluxe"

#define rtPhotoshop			kPhotoshopSignature // '8BIM'
#define fnPhotoshop			"Photoshp"

#define rtPremiere			'PrMr'
#define fnPremiere			"Adobe Premiere"

void GetOSInfo (HandleProcs *procs, OSInfo *osInfo);

unsigned32 AppMatch (HandleProcs *procs, ResType *type);

unsigned32 HostHostIsApp (HandleProcs *procs, const ResType type);

unsigned8 MapSystem (const unsigned32 os);

unsigned8 MapStage (const unsigned32 stageMask, const unsigned32 stage);

unsigned32 MapSignature (char *s1);

Boolean		IsWindows (const unsigned32 flag);

Boolean		IsMacOS (const unsigned32 flag);

#define HostIsApp(type) \
	HostHostIsApp (gStuff->handleProcs, type)

#define HostIsAfterEffects() \
	HostIsApp(rtAfterEffects)
	
#define HostIsIllustrator() \
	HostIsApp(rtIllustrator)
	
#define HostIsPageMaker() \
	HostIsApp (rtPageMaker5)
	
#define HostIsPhotoDeluxe() \
	HostIsApp (rtPhotoDeluxe)

#define HostIsPhotoshop() \
	HostIsApp (rtPhotoshop)

#define HostIsPremiere() \
	HostIsApp (rtPremiere)
				
/*****************************************************************************/
/* String and Number functions */

/* Convert a string to its long value */
Boolean StringToNumber (Str255 s, long *value);

/* Append one string to a pascal string */
void AppendString (Str255 s1, Str255 s2, short start, short length);

/* Add a character to a pascal string */
void AppendCharToString (Str255 s1, const unsigned char c);

/* Change a double value to a string, with a minimal amount of 
   decimal points (precision) */
void DoubleToString (double value, Str255 s, short precision);

/* Takes source string and moves everything after a decimal to target,
   removing extra from source */
void DivideAtDecimal(Str255 s1, Str255 s2);

/* Faux version of Macintosh ParamText: Takes a string and replaces
   occurences of "^0" with r1, "^1" with r2, and "^2" with r3. */
void PIParamText(unsigned char *s, unsigned char *r1, unsigned char *r2, unsigned char *r3);

/* Raises a base to a power */
double power (long base, short raise);

/* Multipliers to move long to fixed */
#define kHard16				(1L << 16) /* 16.16 */
#define kSoft16				kHard16 // use for "other" number systems

/* 16.16 (or other) -> double and double -> 16.16 (or other) */
#define PIFixedToDouble(x, shift)	(x / (double) shift)
#define PIDoubleToFixed(x, shift)	((int32)(x * shift))
#define Fixed16ToDouble(x) 	(x / (double) 65536.0)
#define DoubleToFixed16(x) 	((int32)(x * 65536))

/* 16.16 -> long and long -> 16.16 */
#define long2fixed(value)	(value << 16)
#define fixed2long(value)	(value >> 16)

/*****************************************************************************/

#ifdef __cplusplus
} /* End of extern "C" block. */
#endif

/*****************************************************************************/

#endif /* __PIUtilities__ */
