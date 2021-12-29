/*
	File: PIUtilities.c
	
	Copyright (c) 1993-6, Adobe Systems Incorporated.
	All rights reserved.
	
	C source file for plug-in callback utilities.
*/
#include <stdio.h>
#include <stdlib.h>
#include "PIUtilities.h"

/*****************************************************************************/
/* Returns a handle to the current window or NULL. */

Handle HostGetPlatformWindowPtr (AboutRecordPtr aboutPtr)
{
	PlatformData *platform = (PlatformData *) (aboutPtr->platformData);
	#if MSWindows
	return (Handle)platform->hwnd;
	#else
	return NULL;
	#endif
}
/*****************************************************************************/

Boolean HostDisplayPixelsAvailable (DisplayPixelsProc proc)
{
	Boolean	available = TRUE;
	
	if (!proc)
		available = FALSE;
	
	return available;
}
/*****************************************************************************/

Boolean WarnHostDisplayPixelsAvailable (DisplayPixelsProc proc,
										Handle hDllInstance)
{
	
	Boolean available = HostDisplayPixelsAvailable(proc);
	Str255 s = "";
	
	if (!available)
	{
		DoubleToString(2.502, s, 3);
		s[ s[0]-1 ] = '.'; // 2.5.2
		
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  kNeedVers,
						  s,
						  NULL);
	}
	return available;
	
}
	
/*****************************************************************************/

Boolean HostAdvanceStateAvailable (AdvanceStateProc proc)
{
	Boolean	available = TRUE;
	
	if (!proc)
		available = FALSE;
	
	return available;
}
/*****************************************************************************/

Boolean WarnHostAdvanceStateAvailable (AdvanceStateProc proc,
									   Handle hDllInstance)
{
	Boolean available = HostAdvanceStateAvailable(proc);
	Str255	s = "";
	
	if (!available)
	{
		DoubleToString(3.0, s, 1);
	
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  kNeedVers,
						  s,
						  NULL);
	}
	return available;
}
/*****************************************************************************/

Boolean HostColorServicesAvailable (ColorServicesProc proc)
{
	Boolean	available = TRUE;
	
	if (!proc)
		available = FALSE;
	
	return available;
}
/*****************************************************************************/

Boolean WarnHostColorServicesAvailable (ColorServicesProc proc,
									    Handle hDllInstance)
{
	Boolean available = HostColorServicesAvailable(proc);
	Str255	s = "";
	
	if (!available)
	{
		DoubleToString(2.5, s, 1);
	
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  kNeedVers,
						  s,
						  NULL);
	}
	return available;
}
/*******************************************************************/
/* Sets a standard color array */
void CSSetColor (int16 *color, int16 color1, int16 color2, int16 color3, int16 color4)
{
	color[0] = color1;
	color[1] = color2;
	color[2] = color3;
	color[3] = color4;
}
/*******************************************************************/
/* Copys a color array from color2 -> color1 */
void CSCopyColor (int16 *color1, int16 *color2)
{
	short loop;
	
	for (loop = 0; loop < 4; loop++)
	{
		color1[loop] = color2[loop];
	}
}
/*******************************************************************/
/* Returns number of expected planes for imageMode */
int16 CSPlanesFromMode (int16 imageMode, int16 currPlanes)
{
	switch (imageMode)
	{
		case plugInModeBitmap:
		case plugInModeGrayScale:
		case plugInModeGray16:
		case plugInModeDuotone:
			return 1;
			break;
		case plugInModeIndexedColor:
		case plugInModeRGBColor:
		case plugInModeHSLColor:
		case plugInModeHSBColor:
		case plugInModeLabColor:
		case plugInModeRGB48:
			return 3;
			break;
		case plugInModeCMYKColor:
			return 4;
			break;
		case plugInModeMultichannel:
			return currPlanes;
			break;
	}
}
/*******************************************************************/
/* Maps imageMode to color services space */
int16 CSModeToSpace (int16 imageMode)
{
	switch (imageMode)
	{
		case plugInModeBitmap:
		case plugInModeIndexedColor:
		case plugInModeMultichannel:
			return plugIncolorServicesChosenSpace; // unsupported space
			break;
		case plugInModeGrayScale:
		case plugInModeGray16:
		case plugInModeDuotone:
			return plugIncolorServicesGraySpace;
			break;
		case plugInModeRGBColor:
		case plugInModeRGB48:
			return plugIncolorServicesRGBSpace;
			break;
		case plugInModeHSLColor:
			return plugIncolorServicesHSLSpace;
			break;
		case plugInModeHSBColor:
			return plugIncolorServicesHSBSpace;
			break;
		case plugInModeLabColor:
			return plugIncolorServicesLabSpace;
			break;
		case plugInModeCMYKColor:
			return plugIncolorServicesCMYKSpace;
			break;
	}
}
/*******************************************************************/
OSErr HostCSConvertColor (ColorServicesProc proc,
						 int16 sourceSpace,
						 int16 resultSpace,
						 int16 *color)
{
	OSErr	gotErr = noErr;
	ColorServicesInfo	csinfo;
	
	if (HostColorServicesAvailable(proc))
	{ // now populate color services info with stuff
		csinfo.selector = plugIncolorServicesConvertColor;
		csinfo.sourceSpace = sourceSpace;
		csinfo.resultSpace = resultSpace;
		csinfo.reservedSourceSpaceInfo = NULL; // must be null
		csinfo.reservedResultSpaceInfo = NULL; // must be null
		CSCopyColor(&csinfo.colorComponents[0], color);
		csinfo.reserved = NULL; // must be null
		csinfo.selectorParameter.pickerPrompt = NULL; // not needed
		csinfo.infoSize = sizeof(csinfo);
		gotErr = (*(proc)) (&csinfo);
		CSCopyColor (color, &csinfo.colorComponents[0]);
	} // otherwise return noErr with original components intact
	return gotErr;
}
/*****************************************************************************/
Boolean HostBufferProcsAvailable (BufferProcs *procs, Boolean *tooNew)
	{
	
	Boolean available = TRUE;
	Boolean myTooNew = FALSE;
	
	if (!procs)
		available = FALSE;
		
	else if (procs->bufferProcsVersion < kCurrentBufferProcsVersion)
		available = FALSE;
		
	else if (procs->bufferProcsVersion > kCurrentBufferProcsVersion)
	{	
		available = FALSE;
		myTooNew = TRUE;	
	}
		
	else if (procs->numBufferProcs < kCurrentBufferProcsCount ||
			 !procs->allocateProc ||
			 !procs->lockProc ||
			 !procs->unlockProc ||
			 !procs->freeProc ||
			 !procs->spaceProc)
		available = FALSE;
		
	if (tooNew)
		*tooNew = myTooNew;
		
	return available;
	
	}
/*****************************************************************************/
Boolean WarnHostBufferProcsAvailable (BufferProcs *procs,
									  Handle hDllInstance)
{
	
	Boolean available, tooNew;
	Str255 s = "";
	
	available = HostBufferProcsAvailable (procs, &tooNew);
	
	if (!available)
	{
		DoubleToString(2.5, s, 1);
		
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  (short)(tooNew ? kWrongHost : kNeedVers),
						  s,
						  NULL);
	}		
	return available;
}
/*****************************************************************************/
int32 HostBufferSpace (BufferProcs *procs)
	{
	
	if (!HostBufferProcsAvailable (procs, NULL))
		return 0;	
	else
		return (*procs->spaceProc) ();
	
	}
/*****************************************************************************/
OSErr HostAllocateBuffer (BufferProcs *procs, int32 size, BufferID *bufferID)
	{
	
	*bufferID = 0;
	
	if (!HostBufferProcsAvailable (procs, NULL))
		return memFullErr;
		
	else
		return (*procs->allocateProc) (size, bufferID);
	
	}
/*****************************************************************************/
void HostFreeBuffer (BufferProcs *procs, BufferID bufferID)
	{
	
	if (HostBufferProcsAvailable (procs, NULL))
		(*procs->freeProc) (bufferID);
	
	}
/*****************************************************************************/
Ptr HostLockBuffer (BufferProcs *procs, BufferID bufferID, Boolean moveHigh)
	{
	
	if (HostBufferProcsAvailable (procs, NULL))
		return (*procs->lockProc) (bufferID, moveHigh);
	else
		return NULL;
	
	}
/*****************************************************************************/
void HostUnlockBuffer (BufferProcs *procs, BufferID bufferID)
	{
	
	if (HostBufferProcsAvailable (procs, NULL))
		(*procs->unlockProc) (bufferID);
	
	}
/*****************************************************************************/
OSErr HostAllocateStripBuffer (BufferProcs *procs,
							   int32 rowBytes,
							   int16 minHeight,
							   int16 maxHeight,
							   int16 numBuffers,
							   int16 *actualHeight,
							   BufferID *bufferID)
	{
	
	int16 curHeight;
	int32 curSize;
	OSErr curResult = noErr;
	int32 availableSpace = HostBufferSpace (procs) / numBuffers;
	
	int32 availableHeight = availableSpace / rowBytes;
	
	curHeight = (availableHeight < maxHeight) ? (int16) availableHeight
											  : maxHeight;
											  
	++curHeight;
	
	curSize = rowBytes * (int32) curHeight;
	
	*bufferID = 0;
	
	while (!*bufferID && curHeight > minHeight)
		{
		
		--curHeight;
		curSize -= rowBytes;
		
		curResult = HostAllocateBuffer (procs, curSize, bufferID);
		
		}
		
	if (*bufferID)
		*actualHeight = curHeight;
		
	return curResult;
	
	}
/*****************************************************************************/
Boolean HostResourceProcsAvailable (ResourceProcs *procs, Boolean *tooNew)
	{
	
	Boolean available = TRUE;
	Boolean myTooNew = FALSE;
	
	if (!procs)
		available = FALSE;
		
	else if (procs->resourceProcsVersion < kCurrentResourceProcsVersion)
		available = FALSE;
		
	else if (procs->resourceProcsVersion > kCurrentResourceProcsVersion)
		{
		
		available = FALSE;
		
		myTooNew = TRUE;
		
		}
		
	else if (procs->numResourceProcs < kCurrentResourceProcsCount ||
			 !procs->countProc ||
			 !procs->getProc ||
			 !procs->deleteProc ||
			 !procs->addProc)
		available = FALSE;
		
	if (tooNew)
		*tooNew = myTooNew;
		
	return available;
	
	}
/*****************************************************************************/
Boolean WarnHostResourceProcsAvailable (ResourceProcs *procs,
										Handle hDllInstance)
{
	
	Boolean available, tooNew;
	Str255 s = "";
	available = HostResourceProcsAvailable (procs, &tooNew);
	
	if (!available)
	{
		DoubleToString(3.0, s, 1);
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  (short)(tooNew ? kWrongHost : kNeedVers),
						  s,
						  NULL);
	}	
	return available;
}
/*****************************************************************************/
int16 HostCountPIResources (ResourceProcs *procs, ResType type)
	{
	
	if (!HostResourceProcsAvailable (procs, NULL))
		return 0;
		
	else
		return (*procs->countProc) (type);
	
	}
/*****************************************************************************/
Handle HostGetPIResource (ResourceProcs *procs, ResType type, int16 index)
	{
	
	if (!HostResourceProcsAvailable (procs, NULL))
		return NULL;
		
	else
		return (*procs->getProc) (type, index);
	
	}
/*****************************************************************************/
void HostDeletePIResource (ResourceProcs *procs, ResType type, int16 index)
	{
	
	if (HostResourceProcsAvailable (procs, NULL))
		(*procs->deleteProc) (type, index);
	
	}
/*****************************************************************************/
OSErr HostAddPIResource (ResourceProcs *procs, ResType type, Handle data)
	{
	
	if (!HostResourceProcsAvailable (procs, NULL))
		return memFullErr;
		
	else
		return (*procs->addProc) (type, data);
	
	}
/*****************************************************************************/
Boolean HostHandleProcsAvailable (HandleProcs *procs, Boolean *tooNew)
	{
	
	/* We only require the new, dispose, set size, and get size procs. */
	/* Lock and unlock can usually be gotten around without resorting  */
	/* to callbacks.												   */
	
	#define kCoreHandleProcsCount 4
	
	Boolean available = TRUE;
	Boolean myTooNew = FALSE;
	
	if (!procs)
		available = FALSE;
		
	else if (procs->handleProcsVersion < kCurrentHandleProcsVersion)
		available = FALSE;
		
	else if (procs->handleProcsVersion > kCurrentHandleProcsVersion)
		{
		
		available = FALSE;
		
		myTooNew = TRUE;
		
		}
		
	else if (procs->numHandleProcs < kCoreHandleProcsCount ||
			 !procs->newProc ||
			 !procs->disposeProc ||
			 !procs->getSizeProc ||
			 !procs->setSizeProc)
		available = FALSE;
		
	if (tooNew)
		*tooNew = myTooNew;
		
	return available;
	
	}
/*****************************************************************************/
Boolean WarnHostHandleProcsAvailable (HandleProcs *procs,
									  Handle hDllInstance)
{
	Boolean available, tooNew;
	Str255 s = "";
	
	available = HostHandleProcsAvailable (procs, &tooNew);
	
	if (!available)
	{
		DoubleToString(3.0, s, 1);
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  (short)(tooNew ? kWrongHost : kNeedVers),
						  s,
						  NULL);
	}
		
	return available;
}
/*****************************************************************************/
Handle HostNewHandle (HandleProcs *procs, int32 size)
	{
	
	if (!HostHandleProcsAvailable (procs, NULL))
	{
#if Macintosh
		return (Handle)(NewHandle (size));
#else
		return NULL;
#endif
	}
	else
		return (*procs->newProc) (size);
	
	}
/*****************************************************************************/
void HostDisposeHandle (HandleProcs *procs, Handle h)
	{
	
	if (!HostHandleProcsAvailable (procs, NULL))
	{
#if Macintosh
		DisposHandle (h);
#endif
	}
	else
		(*procs->disposeProc) (h);
	
	}
/*****************************************************************************/
int32 HostGetHandleSize (HandleProcs *procs, Handle h)
	{
	
	if (!HostHandleProcsAvailable (procs, NULL))
	{
#if Macintosh
		return GetHandleSize (h);
#else
		return 0;
#endif
	}
	else
		return (*procs->getSizeProc) (h);
	
	}
/*****************************************************************************/
OSErr HostSetHandleSize (HandleProcs *procs, Handle h, int32 newSize)
	{
	
	if (!HostHandleProcsAvailable (procs, NULL))
		{	
#if Macintosh
		SetHandleSize (h, newSize);
		return MemError ();
#else
		return -1;
#endif
		}
		
	else
		return (*procs->setSizeProc) (h, newSize);
	
	}
/*****************************************************************************/
Ptr HostLockHandle (HandleProcs *procs, Handle h, Boolean moveHigh)
	{
	
	if (procs &&
		procs->handleProcsVersion == kCurrentHandleProcsVersion &&
		procs->numHandleProcs >= 6 &&
		procs->lockProc &&
		procs->unlockProc)
		{
		
		return (*procs->lockProc) (h, moveHigh);
		
		}
		
	else
		{
		
#if Macintosh
		if (moveHigh)
			MoveHHi (h);
	
		HLock (h);
		
		return *h;
#else
		return GlobalLock (h);
#endif
		}
	
	}
/*****************************************************************************/
void HostUnlockHandle (HandleProcs *procs, Handle h)
	{
	
	if (procs &&
		procs->handleProcsVersion == kCurrentHandleProcsVersion &&
		procs->numHandleProcs >= 6 &&
		procs->lockProc &&
		procs->unlockProc)
		{
		
		(*procs->unlockProc) (h);
		
		}
		
	else
		{
#if Macintosh	
		HUnlock (h);
#else
		GlobalUnlock (h);
#endif
		}
	
	}
/*****************************************************************************/
void HostHandle2String(HandleProcs *procs, Handle h, Str255 s)
{
	Ptr p = NULL;
	
	s[ (s[0]=0)+1 ] = 0;
	if (h != NULL)
	{
		p = HostLockHandle(procs, h, FALSE);
		if (p != NULL)
		{
			AppendString(s, (unsigned char *)p, 0, (short)HostGetHandleSize(procs, h));
		}
		HostUnlockHandle(procs, h);
	}
	s[ 1+((s[0] > 254) ? 254 : s[0]) ] = 0; // null terminate that puppy!
}
	
/*****************************************************************************/
Handle HostString2Handle(HandleProcs *procs, Str255 s)
{
	Handle	h = NULL;
	Ptr		p = NULL;
	
	h = HostNewHandle(procs, s[0]);
	if (h != NULL)
	{
		p = HostLockHandle(procs, h, FALSE);
		if (p != NULL)
			HostCopy(p, &s[1], s[0]);
		HostUnlockHandle(procs, h);
	}
	return h;
}
/*****************************************************************************/
Handle HostHandleCat(HandleProcs *procs, Handle h1, Handle h2)
{
	int32	len1 = HostGetHandleSize(procs, h1);
	int32	len2 = HostGetHandleSize(procs, h2);
	Ptr		p1 = NULL;
	Ptr		p2 = NULL;
	
	if (h1 == NULL)
		h1 = HostNewHandle(procs, len2);
	if (h1 != NULL && len2 > 0)
	{
		HostSetHandleSize(procs, h1, len1 + len2);
		
		p1 = HostLockHandle(procs, h1, FALSE);
		p2 = HostLockHandle(procs, h2, FALSE);
		
		if (p1 != NULL && p2 != NULL)
			HostCopy(&p1[len1], p2, len2);
		
		HostUnlockHandle(procs, h1);
		HostUnlockHandle(procs, h2);
	}
	return h1;
}
/*****************************************************************************
 * This routine uses the generic PIGetResource to grab a resource and stuff
 * it into a string.  The resource will be truncated if over 255 characters.
 */
 
void HostGetString (Handle hDllInstance, Str255 s, int32 index)
{
	int32	size = 0;
	Ptr		p = HostGetResource(hDllInstance, StringResource, index, &size);
	
	s[ (s[0] = 0)+1 ] = 0;
	
	if (p != NULL)
	{
		if (size > 1) AppendString(s, (unsigned char *)p, 1, (short)(size - 1));
		PIReleaseResource(p);
	}
}
/*****************************************************************************/
// Convert int32 restype to 4 characters
void PIType2Char (const ResType type, char *name)
{
	short x;
	
	for (x = 3; x >= 0; x--)
	{
		name[x] = (char) (type & 0x000000FF);
		type >>= 8;
	}
}
/*****************************************************************************/
// Convert 4 characters to int32 restype
ResType PIChar2Type (char *name)
{
	short x;
	ResType type = name[0];
	
	for (x = 1; x < 4; x++)
	{
		type <<= 8;
		type += name[x];
	}
	return type;
}
/*****************************************************************************/
Ptr	HostGetResource (Handle hDllInstance, ResType type, int32 index, int32 *size)
{
	Handle			h2 = NULL; // HGLOBAL Handle h;
	Handle			h1 = NULL;	// HRSRC Handle hr;
	Handle			h = NULL;
	short			x = 3;
	Ptr				p = NULL; // LPVOID Ptr hPtr;
	char			*id = "#\0";
	char			name[5] = "\0\0\0\0";
	char			*s = "";
	
	*size = 0;
#if MSWindows
	if (type == StringResource)
	{
		char	buff[256];
		
		*size = LoadString(hDllInstance, index, buff, 256);
		if (*size > 0)
		{ // got something
			(*size)++; // add one for pascal length byte
			h = NewHandle(*size);
			if (h != NULL)
			{
				HLock(h);
				memcpy(*h+1, buff, (*size)-1);
				*h[0] = (char)(*size);
				p = *h;
			}
		}
	}
	else
	{
//		itoa ((int)index, s, 10);
		strcat (id, s);
		PIType2Char(type, &name[0]);
		h1 = FindResource(hDllInstance, id, name);
		if (h1 != NULL)
		{
			h2 = LoadResource(hDllInstance, h1);
			
			if (h2 != NULL)
				{
				p = LockResource(h2);
	
				if (p != NULL)
				{
					*size = SizeofResource(hDllInstance, h1);
	
				}
			}
		}
	}
#else // Mac
	h = GetResource(type, index);
	HNoPurge(h);
	HLockHi(h); // lock and move high
	*size = GetHandleSize(h);
	p = *h; // want to return a ptr
#endif
	return p;
}
/*****************************************************************************/
void PIReleaseResource(Ptr p)
{
#if MSWindows
	// Win32 does not require FreeResource nor UnlockResource
#else // Mac
	if (p != NULL)
	{
		Handle h = RecoverHandle(p); // get that handle back
		if (h != NULL)
		{
			HUnlock(h);
			HPurge(h);
			ReleaseResource(h);
			h = NULL;
		}
	}
	p = NULL;
#endif
}
/*****************************************************************************/
Boolean HostPropertyAvailable (PropertyProcs *procs, Boolean *tooNew)
	{
	
	Boolean available = TRUE;
	Boolean myTooNew = FALSE;
	
	if (!procs)
		available = FALSE;
		
	else if (procs->propertyProcsVersion < kCurrentPropertyProcsVersion)
		available = FALSE;
		
	else if (procs->propertyProcsVersion > kCurrentPropertyProcsVersion)
		{
		available = FALSE;
		myTooNew = TRUE;
		}
		
	else if (!procs->getPropertyProc ||
			 !procs->setPropertyProc)
		available = FALSE;
		
	if (tooNew)
		*tooNew = myTooNew;
		
	return available;
	
	}
/*****************************************************************************/
Boolean WarnHostPropertyAvailable (PropertyProcs *procs,
								   Handle hDllInstance)
{
	
	Boolean available, tooNew;
	Str255 s = "";
	
	available = HostPropertyAvailable (procs, &tooNew);
	
	if (!available)
	{
		DoubleToString(2.5, s, 1);
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  (short)(tooNew ? kWrongHost : kNeedVers),
						  s,
						  NULL);
	}
	return available;	
}
/*****************************************************************************/
Boolean HostChannelPortAvailable (ChannelPortProcs *procs, Boolean *tooNew)
	{
	
	Boolean available = TRUE;
	Boolean myTooNew = FALSE;
	
	if (!procs)
		available = FALSE;
		
	else if (procs->channelPortProcsVersion < kCurrentChannelPortProcsVersion)
		available = FALSE;
		
	else if (procs->channelPortProcsVersion > kCurrentChannelPortProcsVersion)
		{
		
		available = FALSE;
		
		myTooNew = TRUE;
		
		}
		
	else if (procs->numChannelPortProcs < kCurrentChannelPortProcsCount ||
			 !procs->readPixelsProc ||
			 !procs->writeBasePixelsProc ||
			 !procs->readPortForWritePortProc)
		available = FALSE;
		
	if (tooNew)
		*tooNew = myTooNew;
		
	return available;
	
	}
/*****************************************************************************/
Boolean WarnHostChannelPortAvailable (ChannelPortProcs *procs,
									  Handle hDllInstance)
{
	
	Boolean available, tooNew;
	Str255 s = "";
	
	available = HostChannelPortAvailable (procs, &tooNew);
	
	if (!available)
	{
		DoubleToString(2.5, s, 1);
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  (short)(tooNew ? kWrongHost : kNeedVers),
						  s,
						  NULL);
	}
	return available;	
}
/*****************************************************************************/
Boolean HostDescriptorAvailable (PIDescriptorParameters *procs, Boolean *tooNew)
	{
	
	Boolean available = TRUE;
	Boolean myTooNew = FALSE;
	
	if (!procs)
		available = FALSE;
		
	else if (procs->descriptorParametersVersion < 
			 kCurrentDescriptorParametersVersion)
		available = FALSE;
		
	else if (procs->descriptorParametersVersion >
			 kCurrentDescriptorParametersVersion)
		{
		
		available = FALSE;
		
		myTooNew = TRUE;
		
		}
		
	else if (!procs->readDescriptorProcs || 
			 !procs->writeDescriptorProcs)
		available = FALSE;
		
	else if (procs->readDescriptorProcs->readDescriptorProcsVersion
			 < kCurrentReadDescriptorProcsVersion ||
			 procs->readDescriptorProcs->numReadDescriptorProcs
			 < kCurrentReadDescriptorProcsCount ||
			 procs->writeDescriptorProcs->writeDescriptorProcsVersion
			 < kCurrentWriteDescriptorProcsVersion ||
			 procs->writeDescriptorProcs->numWriteDescriptorProcs
			 < kCurrentWriteDescriptorProcsCount)
		available = FALSE;
		
	if (tooNew)
		*tooNew = myTooNew;
		
	return available;
	
	}
/*****************************************************************************/
Boolean WarnHostDescriptorAvailable (PIDescriptorParameters *procs,
									 Handle hDllInstance)
{
	
	Boolean available, tooNew;
	Str255 s = "";
	
	available = HostDescriptorAvailable (procs, &tooNew);
	
	if (!available)
	{
		DoubleToString(2.5, s, 1);
		ShowVersionAlert (hDllInstance,
						  NULL,
						  AlertID,
						  (short)(tooNew ? kWrongHost : kNeedVers),
						  s,
						  NULL);
	}
	return available;
}
/*****************************************************************************/
/* Closes read descriptor and disposes handle. */
OSErr HostCloseReader (PIDescriptorParameters *procs,
					   HandleProcs *hProcs,
					   PIReadDescriptor *token)
{
	OSErr		err = noErr;
	
	err = procs->readDescriptorProcs->closeReadDescriptorProc(*token);
	HostDisposeHandle(hProcs, procs->descriptor);
	procs->descriptor = NULL;
	*token = NULL;
	
	return err;
}
/*****************************************************************************/
/* Closes write descriptor.  Disposes old handle, if present, and sets
   recordInfo to plugInDialogOptional. */
OSErr	HostCloseWriter(PIDescriptorParameters *procs,
						HandleProcs *hProcs,
						PIWriteDescriptor *token)
{
	OSErr				err = noErr;
	PIDescriptorHandle	h = NULL;
	
	if (procs->descriptor) // don't need descriptor passed to us
		HostDisposeHandle(hProcs, procs->descriptor); // dispose.
	procs->writeDescriptorProcs->closeWriteDescriptorProc(*token, &h);
	procs->descriptor = h;
	procs->recordInfo = plugInDialogOptional;
		/* most likely.  Otherwise:  plugInDialogOptional ||
									 plugInDialogRequire ||
									 plugInDialogNone */
	*token = NULL;
	
	return err;
}
/*****************************************************************************/
/* Puts an object and disposes its handle, returning any error. */
OSErr HostPutObj (PIDescriptorParameters *procs, HandleProcs *hProcs, 
				  PIWriteDescriptor desc, DescriptorKeyID key, DescriptorTypeID type,
				  PIDescriptorHandle *h)
{
	OSErr		err = noErr;
	
	err = procs->writeDescriptorProcs->putObjectProc(desc, key, type, *h);
	HostDisposeHandle(hProcs, *h);
	*h = NULL;
	return err;
}
/*****************************************************************************/
/* Returns whether playInfo is plugInDialogDisplay, meaning to absolutely pop
   the plug-in dialog. */
   
Boolean	HostPlayDialog(PIDescriptorParameters *procs)
{
	return procs->playInfo == plugInDialogDisplay;
	/* plugInDialogDontDisplay = display if necessary,
	   plugInDialogDisplay = display,	
	   plugInDialogSilent = never display */
}
/*****************************************************************************/
/* This routine divides a string at its decimal point. */
void DivideAtDecimal(Str255 s1, Str255 s2)
{ 
	short	loop = s1[0];
	
	do
	{
		if (s1[loop] == '.')
		{
			AppendString(s2, s1, (short)(loop+1), (short)(s1[0] - loop));
			s1[ 1+ (s1[0]=loop-1) ] = 0;
			loop = 1;
		}
		loop--;
	}
	while (loop > 0);
}
	
/*****************************************************************************/
/* The following routine appends one string to another. */
void AppendString (Str255 s1, Str255 s2, short start, short length)
{
	if ( (s1[0] + (length - start)) > 255) length = (255 + start) - s1[0];
	for ( ; length > 0; length--)
		s1[ ++s1[0] ] = s2[start++];
}
void AppendCharToString (Str255 s1, const unsigned char c)
{
	if (s1[0] < 256) s1[ ++s1[0] ] = c;
}
/*****************************************************************************/
void HostBlockMove (Ptr p1, Ptr p2, const int32 amount)
{
	if (p1 != NULL && p2 != NULL)
	{
		for ( ; amount > 0; amount--)
			*(p1++) = *(p2++);
	}
}
/*****************************************************************************/
void HostCopy (void *s1, const void *s2, const int32 amount)
{
	HostBlockMove((Ptr)s1, (Ptr)s2, amount);
}		
/*****************************************************************************/
int16 HostStringCat (char *s1, const char *s2)
{
	int16	x = HostMatch(s1, "\0", false);
	int16	loop = 0;
	
	if (x < 0) x = 0; // no match, blast at beginning
	while (x < 256 && s2[loop] != 0)
		s1[x++] = s2[loop++];
	s1[x] = 0;
	return x;
}
/*****************************************************************************/
int16 HostStringMatch(Str255 s1, const char *s2, Boolean ignoreCase)
{
	if (s1[0] < 255)
	{ // null terminate
		s1[ s1[0]+1 ] = 0;
		// s2[ s1[0]+1 ] = 0;
	}
	return HostMatch((char *)s1, s2, ignoreCase);
}
/*****************************************************************************/
char UpperToLower(const char c1)
{
	char	retn = c1;
	
	if (c1 >= 'A' && c1 <= 'Z')
		retn = (c1-'A') + 'a';
	return retn;
}
	
/*****************************************************************************/
int16 HostMatch(char *s1, const char *s2, Boolean ignoreCase)
{
	int16	loop = 0;
	int16	start = 0;
	int16	retn = noMatch; // -1 = no match
	
	do
	{
		if (s1[loop] == s2[start] || 
			(ignoreCase && (UpperToLower(s1[loop]) == UpperToLower(s2[start]))))
		{
			if (retn < 0) retn = loop;
			start++;
		}
		
		else
		{
			retn = noMatch;
			start = 0; // reset
		}
	} while (s1[loop++] > 0 && loop < 256);
	
	if (s1[loop-1] > 0) retn = noMatch; // didn't match exactly
	return retn;
}		
/*****************************************************************************/
/* Turns a double into a string. */
void DoubleToString (double value, Str255 s, short precision)
{
	unsigned32	x = 0;
	double		y = 0;
	
	s[ (s[0]=0)+1 ] = 0;
	
	// determine if we need to round:
	y = value * power(10, precision);
	x = (unsigned32)y; // whole portion;
	y -= x; // subtract whole portion
	y *= 10; // move over one more place
	if (y >= 5) value += (5.0 / (double)power(10, (short)(precision+1)));
	
	if (precision > 1)
	{ // now check to see if we can lose a decimal place
		y = value * power(10, (short)(precision-1));
		x = (unsigned32)y; // whole portion
		y -= x; // subtract whole portion
		y *= 10; // number of interest
		if (y < 1) precision--;
		// our number of interest is zero, so lose a point of precision
	}
	y = 0; // done with this
	
	x = (unsigned32)value;
	
	NumToString((long)x, s);
	s[++s[0]] = '.';
	value -= x; // get rid of whole-number portion
	do
	{
		value *= 10;
		x = (unsigned32)value; // whole number
		s[++s[0]] = '0' + (char)(x);
		value -= x;
	}
	while (value > 0 && --precision > 0);
}
/*****************************************************************************/
/* Raises a base to a power */
double power (long base, short raise)
{
	double p = 1;
	
	for ( ; raise > 0; raise--)
		p *= base;
	return p;
}
/*****************************************************************************/
Boolean StringToNumber (Str255 s, long *value)
	{
	
	short i;
	short j;
	long x;
	Boolean negative = FALSE;
	Boolean isNumber = TRUE;
	Boolean trailingBlanks = FALSE;
	
	for (i = 1, j = 0; i <= s[0] && isNumber; ++i)
		{
		
		if (j == 0 && s [i] == ' ')
			; /* Do nothing: Leading blanks */
			
		else if (j > 0 && s [i] == ' ')
			trailingBlanks = TRUE;
			
		else if (trailingBlanks && s [i] != ' ')
			isNumber = FALSE;
			
		else if (j == 0 && !negative && s [i] == '-')
			negative = TRUE;
			
		else if (s [i] < '0' || s [i] > '9')
			isNumber = FALSE;
			
		else
			s [++j] = s [i];
		
		}
		
	if (j == 0)
		isNumber = FALSE;
	else
		s[ (s[0] = (char) j)+1] = 0;
	
	if (isNumber)
		{
		
		if (j <= 9)
#if MSWindows
			x = atol(&s[1]);
#else // Mac
			StringToNum (s, &x);
#endif
			else
			x = 0x7FFFFFFF;
		
		if (negative)
			x = -x;
			
		*value = x;
			
		}
		
	return isNumber;
	
	}
	
/*********************************************************************/
void PIParamText(unsigned char *s, unsigned char *r0, unsigned char *r1, unsigned char *r2)
{
	short	loop = 0;
	Boolean	flag = false;
	Str255 s1 = "";
	Str255 rx = "";
	
	while (loop < s[0])
	{
		loop++;
		
		if (s[loop] == '^') flag = true;
		else if (s[loop] >= '0' && s[loop] <= '2' && flag)
		{
			flag = false;
			switch(s[loop])
			{
				case '0':
					if (r0) AppendString(rx, r0, 1, (short)(r0[0]));
					else rx[ (rx[0] = 0)+1 ] = 0;
					break;
				case '1':
					if (r1) AppendString(rx, r1, 1, (short)(r1[0]));
					else rx[ (rx[0] = 0)+1 ] = 0;
					break;
				case '2':
					if (r2) AppendString(rx, r2, 1, (short)(r2[0]));
					else rx[ (rx[0] = 0)+1 ] = 0;
					break;
			}
			
			if (rx)
			{
				AppendString(s1, s, 1, (short)(loop-2));
				AppendString(s1, rx, 1, (short)rx[0]);
				AppendString(s1, s, (short)(loop+1), (short)(s[0]-loop));
				s[0] = 0;
				AppendString(s, s1, 1, s1[0]);
				s1[ (s1[0]=0)+1 ] = 0;
				rx[ (rx[0]=0)+1 ] = 0;
				loop--; // stay here to check for other ^x's.
			}
		}
		else flag = false;
	}
}
	  	
/*********************************************************************/
short HostReportError(Str255 s1, Str255 s2)
{
	if (s1)
	{
		s1[ (s1[0]=0)+1 ] = 0;
		AppendString(s1, s2, 1, s2[0]);
	}
	else s1 = s2;
	return errReportString;
}
/*********************************************************************/
void GetOSInfo(HandleProcs *procs, OSInfo *osInfo)
{ /*
#if MSWindows

	DWORD		lpdwHandle; 	// pointer to variable to receive zero
    char		*lpBuffer[80];	// address of buffer for version pointer
    char		lpFilename[256];
	int32		len = 0;
	char		lpData[1024]; // enough for what we need
	VS_FIXEDFILEINFO	*ffi;
	DWORD		x = 0;
	Ptr			p = 0;
	Str255		s1 = "", s2 = "";
	len = GetModuleFileName(NULL , lpFilename, 255);
	len = GetFileVersionInfoSize(lpFilename, &lpdwHandle);
	GetFileVersionInfo(lpFilename, lpdwHandle, 1024, lpData);

	VerQueryValue(lpData, TEXT("\\"), lpBuffer, &len); // ask for basic info
	p = *(lpBuffer);
	ffi = (VS_FIXEDFILEINFO *)p;
	osInfo->stage = MapStage(ffi->dwFileFlagsMask, ffi->dwFileFlags); // release, prerelease, etc.
	osInfo->stageVersion = 0;
	osInfo->os = MapSystem(ffi->dwFileOS);
  
	VerQueryValue(lpData, 
				  TEXT("\\StringFileInfo\\040904E4\\InternalName"),
				  lpBuffer,
				  &len);
	p = *lpBuffer; // returns a ptr to the string
	osInfo->signature = MapSignature(p);

	VerQueryValue(lpData,
				  TEXT("\\StringFileInfo\\040904E4\\FileVersion"),
				  lpBuffer,
				  &len);
	p = *lpBuffer; // returns a ptr to the string
	AppendString(s1, (unsigned char *)p, 0, (short)strlen(p)); // copy string to pascal
	DivideAtDecimal(s1, s2);
	StringToNumber(s1, &x);
	osInfo->majorVersion = (unsigned8)x;

	x = 0; // just in case
	StringToNumber(s2, &x);
	osInfo->minorVersion = (unsigned8)x;

	osInfo->subVersion = 0;

#else // Macintosh/Unix
	short	rID;
	Handle	h = NULL;
	Ptr		p = NULL;
	long	x;
		
	rID = CurResFile(); // get the current resource file
	
	UseResFile( LMGetCurApRefNum() ); // sets the res file to the application (not the plug-in)
	
	h = Get1IndResource('BNDL', 1); // want the 1st bndl resource of the app
	
	if (!ResError())
	{ // work with bndl
		p = HostLockHandle(procs, h, FALSE);
		if (p != NULL)
		{
			osInfo->signature = PIChar2Type(p);
		}
		
		HostUnlockHandle(procs, h);
		ReleaseResource(h);
		h = NULL;
		p = NULL;
	}
		
	h = Get1IndResource('vers', 1); // want the 1st vers resource of the app
		
	if (!ResError())
	{
		
		p = HostLockHandle(procs, h, FALSE);
		if (p != NULL)
		{
			osInfo->majorVersion = (unsigned8)*p++;
			osInfo->minorVersion = osInfo->subVersion = (unsigned8)*p++;
			osInfo->minorVersion >>= 4; // top 4 bits = vers2
			osInfo->subVersion &= 0x0F; // bottom 4 bits = vers3
			osInfo->stage = MapStage(NULLID,(unsigned32)*p++); // 0x20 = dev,
							  // 0x40 = alpha,
							  // 0x60 = beta,
							  // 0x80 = release
			osInfo->stageVersion = (unsigned8)*p;
		}
		HostUnlockHandle(procs, h);
		ReleaseResource(h);
		h = NULL;
		p = NULL;
	}
	
	if (Gestalt(gestaltSystemVersion, &x) == noErr)
	{ // got the system version.
		osInfo->os = MapSystem((unsigned32)(x >> 8));
	}
	UseResFile (rID);
#endif // Mac/Unix */
} 
/*********************************************************************/
unsigned32 AppMatch (HandleProcs *procs, ResType *type)
{
	ResType		*p	= type;
	Boolean		gotit = false;
	unsigned32 	retn = 0;
	OSInfo		osInfo;
	
	GetOSInfo(procs, &osInfo);
	
	while (*p != NULLID && gotit == false)
		gotit = (*p++ == osInfo.signature);
		
	if (gotit)
		retn = (osInfo.majorVersion << 24) |
			   (osInfo.minorVersion << 16) |
			   (osInfo.os);
	return retn;
}
/*********************************************************************/
unsigned32 HostHostIsApp (HandleProcs *procs, const ResType type)
{
	const ResType types[] = { rtAfterEffects, NULLID,
							 rtIllustrator, NULLID,
							 rtPageMaker5, rtPageMaker6, rtPageMaker65, NULLID,
							 rtPhotoDeluxe, NULLID,
							 rtPhotoshop, NULLID,
							 rtPremiere, NULLID };
	
	const short	numResTypes = sizeof(types) / sizeof(ResType);
	
	short loop = 0;
	unsigned32	retn = 0;
	
	do
	{
		if (types[loop] == type && types[loop] != NULLID)
		{ // got a match
			retn = AppMatch(procs, &types[loop]);
			loop = numResTypes; // done looping
		}
	} while (++loop < numResTypes);
	
	return retn;
}

/*********************************************************************/
unsigned8	MapStage (const unsigned32 stageMask, const unsigned32 stage)

{
#if MSWindows
	if (stageMask & 0x01L && stage & 0x01L)
		return OSStageDevelopment; // 0x01L = VS_FF_DEBUG
	else if (stageMask & 0x02L && stage & 0x02L)
		return OSStageBeta; // 0x02L = VS_FF_PRERELEASE
	else return OSStageRelease;
	
#else // Mac/Unix

	switch (stage)
	{
		case 0x20:
			return OSStageDevelopment;
			break;
		case 0x40:
			return OSStageAlpha;
			break;
		case 0x60:
			return OSStageBeta;
			break;
		case 0x80:
		default:
			return OSStageRelease;
			break;
	}
#endif
}

/*********************************************************************/
unsigned8	MapSystem (const unsigned32 os)
{
#if MSWindows
	switch (os)
	{
		case 0x00040000L:
			return OSWinNT;
			break;
		case 0x01L:
			return OSWin16;
			break;
		case 0x04L:
		default:
			return OSWin32;
			break;
	}
#else // Mac/Unix
	switch (os)
	{
		case 6:
			return OSMacOS6;
			break;
		case 8:
			return OSMacOS8;
			break;
		case 7:
		default:
			return OSMacOS7;
			break;
	}
#endif
}
/*********************************************************************/

unsigned32	MapSignature (char *s1)
{
	const char   *fileNames[] = { fnAfterEffects,
								  fnIllustrator,
								  fnPageMaker5,
								  fnPageMaker6,
								  fnPageMaker65,
								  fnPhotoDeluxe,
								  fnPhotoshop,
								  fnPremiere };
	const ResType fileTypes[] = { rtAfterEffects,
								  rtIllustrator,
								  rtPageMaker5,
								  rtPageMaker6,
								  rtPageMaker65,
								  rtPhotoDeluxe,
								  rtPhotoshop,
								  rtPremiere };

	const short numResTypes = sizeof(fileTypes) / sizeof(ResType);

	unsigned32 retn = NULLID;

	short loop = 0;

	do
	{
		if (HostMatch(s1, fileNames[loop], true) == gotMatch)
		{ // got a match
			retn = fileTypes[loop];
			loop = numResTypes;
		}
	} while (++loop < numResTypes);

	return retn;

}

/*********************************************************************/

Boolean		IsWindows (const unsigned32 flag)
{
	unsigned8 smallFlag = (unsigned8)(flag & 0xFF); // take lowest byte
	
	if (smallFlag >= OSWin1 && smallFlag <= OSWinEnd)
		return true;
	else return false;
}
/*********************************************************************/

Boolean		IsMacOS (const unsigned32 flag)
{
	unsigned8 smallFlag = (unsigned8)(flag & 0xFF); // take lowest byte
	
	if (smallFlag >= OSMacOS1 && smallFlag <= OSMacOSEnd)
		return true;
	else return false;
}
/*********************************************************************/
