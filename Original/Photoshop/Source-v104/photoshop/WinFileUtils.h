/*
	File: WinFileUtils.h

	Copyright (c) 1996, Adobe Systems Incorporated.
	All rights reserved.

	Header file for Windows file utilities.
*/

#ifndef __WinFileUtils_H__
#define __WinFileUtils_H__

#include "PITypes.h"
#include "PIUtilities.h"

/*
//#include <Types.h>
#include <Errors.h>
#include <Files.h>
#include <Resources.h>
//#include <Quickdraw.h>
#include <Aliases.h>
//#include <String.h>
*/

/***************************************************************/

#if MSWindows
typedef	HFILE	FileHandle;
typedef Handle	AliasHandle;
typedef struct
	{	
	char fileName [256];
	} SFReply;

OSErr FSWrite(HFILE refNum, long *count, void *buffPtr);
#endif

#if Macintosh
#include <StandardFile.h>
#include <LowMem.h>
// On Macintosh systems, FileHandle is a typedef for RefNum
typedef short	FileHandle;
#endif

/***************************************************************/

Boolean PISetSaveDirectory (Handle alias, Str255 s, 
				   Boolean query,
				   short *rVRefNum);
				   
Boolean TestAndStoreResult (short *res, OSErr result);
#define TSR(x) TestAndStoreResult (&gResult, x)

Boolean TestAndStoreCancel (short *res, Boolean tocancel);
#define TSC(x) TestAndStoreCancel (&gResult, x)

Boolean PICreateFile (Str255 filename, 
					  short vRefNum,
					  const ResType creator,
					  const ResType type, 
					  FileHandle *fRefNum,
					  short *result);

Boolean PICloseFile (Str255 filename, 
					 short vRefNum,
					 FileHandle fRefNum, 
					 Boolean sameNames,
					 Boolean *dirty, 
					 AliasHandle *alias,
					 short *result);

void PICloseAndOpenWD(const short vRefNum, const long dirID, short *rVRefNum);

/***************************************************************/

#endif /* __WinFileUtils_H__ */
