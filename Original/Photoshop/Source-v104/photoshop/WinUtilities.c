/*  (c) Copyright 1996.  Adobe Systems, Incorporated.  All rights reserved. */

#include "WinUtilities.h"

#define signatureSize	4
static char cSig[signatureSize] = {'O', 'T', 'O', 'F'};

static OSErr memError;

void NumToString (const long x, Str255 s)
{
	char c[33] = "";

	s [ (s[0]=0)+1 ] = 0;
	ltoa(x, c, 10);
	AppendString(s, c, 0, (short)strlen(c));
}

Handle NewHandleClear( long size )
{
	return NewHandle( size );	// GlobalAllocPtr( GHND, ... )
					// allocates zeroed memory.
}

Handle NewHandle ( long size )
{
	Handle mHand;
	char *p;

	memError = noErr;

	mHand =	(Handle) GlobalAllocPtr (GHND, (sizeof (Handle) + signatureSize));

	if (mHand)
		*mHand = (Ptr) GlobalAllocPtr (GHND, size);

	if (!mHand || !(*mHand))
		{
		memError = memFullErr;
		return NULL;
		}

	// put the signature after the pointer
	p = (char *) mHand;
	p += sizeof (Handle);
	memcpy (p,cSig, signatureSize);

	return mHand;
		
}

long GetHandleSize(Handle handle)
{
	Ptr p;
	HANDLE h;

	memError = noErr;

	if (handle)
		{
		p = *handle;

		h = GlobalPtrHandle (p);

		if (h)

			return GlobalSize (h);
		}

	memError = nilHandleErr;

	return 0L;

}

void SetHandleSize (Handle handle, long newSize)
{
	Ptr p;
	HANDLE hMem;


	memError = noErr;

	if (handle)
	{
		p = *handle;

		if (p)
		{
			GlobalUnlockPtr (p);
			hMem = GlobalReAlloc (GlobalPtrHandle (p), newSize, GHND);	
			if (hMem)
				p = GlobalLock (hMem);
			else
				p = NULL;
		}
 
		if (p)

			*handle = p;
		else

			memError = memFullErr;
	}

	else

		memError = memWZErr;

}

void DisposHandle (Handle handle)
{

	memError = noErr;

	if (handle)
		{
		Ptr p;

		p = *handle;

		if (p)
			GlobalFreePtr (p);

		GlobalFreePtr ((Ptr)handle);
		}

	else

		memError = nilHandleErr;
}

Handle DupHandle(Handle oldhand)
{
	LPSTR		newptr, oldptr;
	Handle	newhand;
	DWORD		size;

	oldptr = *oldhand;		// ptr to actual data

	size = WSizeBuffer(oldptr);

	newhand = NewHandle(size);
	newptr = *newhand;

#ifdef WIN32
	memcpy(newptr, oldptr, size);
#else
	hmemcpy(newptr, oldptr, size);
#endif
	return newhand;
}

void HLock (Handle h)
{
	GlobalLock(h);
}

void HUnlock (Handle h)
{
	GlobalUnlock(h);
}

void MoveHHi (Handle h)
{

}

void DisposPtr(Ptr p)
{
    WFreeBuffer(p);
}



Ptr NewPtr(long size)
{
    return WAllocBuffer(size);
}



Ptr NewPtrClear( long size )
{
    return WAllocBuffer( size );
}



LPSTR WAllocBuffer(long size)
{
	return GlobalAllocPtr(GHND, size);
}

VOID WFreeBuffer(LPSTR ptr)
{
	GlobalFreePtr(ptr);
}

DWORD WSizeBuffer(LPSTR ptr)
{
	HANDLE	hand;

	hand = GlobalPtrHandle(ptr);
	return GlobalSize(hand);
}

HANDLE HandFromPtr(LPSTR ptr)
{
    return GlobalPtrHandle(ptr);
}

OSErr MemError(void)
{
  return memError;
}

/********************* Rect (Photoshop Rect) Functions **************************/
/* WIN32 RECTs are now made of longs so we no longer can use the Windows RECT   */
/* manipulation funcions. The upswing is that there need no longer be a special */
/* Windows case (swap l-t r-b) if fact these won't work if you do...            */
/* Use PISetRect... for Rects and platform specific functions for Windows RECTs */

Boolean PISetRect(Rect* pRect, short nLeft, short nTop, short nRight, short nBottom)
{
	if (!pRect)
		return false;

	pRect->left = nLeft;
	pRect->top = nTop;
	pRect->right = nRight;
	pRect->bottom = nBottom;

	return true;
}

Boolean PIOffsetRect(Rect* pRect, short xAmt, short yAmt)
{
	if (!pRect)
		return false;

	pRect->left += xAmt;
	pRect->top += yAmt;
	pRect->right += xAmt;
	pRect->bottom += yAmt;

	return true;
}

Boolean PIPtInRect(Point pt, Rect* r)
{

	if (pt.h < r->left || pt.h >= r->right)
		return false;
	if (pt.v < r->top  || pt.v >= r->bottom)
		return false;
	return true;

}

Boolean PIInsetRect(Rect *pRect, short xAmt, short yAmt)
{
	if (!pRect)
		return false;

	pRect->top    += yAmt;
	pRect->bottom -= yAmt;
	pRect->left   += xAmt;
	pRect->right  -= xAmt;

	return true;
}

long TickCount()
{
	return GetTickCount() / 17;	// S.B. 16.6666666; close enough.
}


short Random()
{
	return rand();
}

/******************************************************************/