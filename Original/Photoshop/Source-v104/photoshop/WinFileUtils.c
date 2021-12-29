/*
	File: WinFileUtils.c

	Copyright (c) 1996, Adobe Systems Incorporated.
	All rights reserved.

	C source file for file utilities.
*/

#include "WinFileUtils.h"

/*****************************************************************************/

Boolean PISetSaveDirectory (Handle alias, Str255 s, 
		   			   		Boolean query, short *rVRefNum)
{

#if 0
	CInfoPBRec			pb;
	Boolean				wasChanged;
	FSSpec				spec;
	int32				gotErr = 0;
	
	if (alias != NULL)
	{
	
		ResolveAlias(nil, (AliasHandle) alias, &spec, &wasChanged);
		
		/* set up file param structure so we can get whether we've been handed
		   a file or a directory */
		
		pb.dirInfo.ioCompletion = nil;
		pb.dirInfo.ioNamePtr = spec.name;
		pb.dirInfo.ioVRefNum = spec.vRefNum;
		pb.dirInfo.ioFRefNum = 0;
		pb.dirInfo.ioFDirIndex = 0;
		pb.dirInfo.ioDrDirID = spec.parID;
		
		gotErr = PBGetCatInfoSync( &pb );
		
		/* now check for NO error and this is NOT a directory */
		// nah, don't need to check for error.
		/* even if we got an error, these fields will be valid (enough) */
		
		if (spec.name[0] > 0)
		/* the scripting system passed us something.  Figure out what to do with it. */
		{
			if (pb.dirInfo.ioFlAttrib & ioDirMask)
			{ // the scripting system passed us a directory.  Add filename to it
		 	  // if we're not popping a dialog.
									
				if (!query)
				{ // if we were going to show this, we could not
				  // make it :dir:name since that would be bad for the file dialog
					spec.name[++spec.name[0]] = ':';
					PICopy(&spec.name[ (spec.name[0]+1) ], &s[1], s[0]);
					spec.name[0] += s[0];
					s[0] = spec.name[0]+1;
					s[1] = ':'; // since we're a directory
					PICopy (&s[2], &spec.name[1], spec.name[0]); // copy name to output string
				}
				else
				{ // querying so set dir
					LMSetSFSaveDisk(-pb.dirInfo.ioVRefNum);
					LMSetCurDirStore(pb.dirInfo.ioDrDirID);
				}
			}
			else
			{
				/* we were given a full pathname. Copy the name. */
				PICopy (s, spec.name, (spec.name[0])+1);
				
				if (query)
				{ // we're showing our dialog, set set directory from alias
					LMSetSFSaveDisk(-spec.vRefNum);
					LMSetCurDirStore(spec.parID);
				}
			}
			/* otherwise we'll just use whatever the export parameter block
			   has passed us */
		}

		/* set gStuff->vRefNum as working directory (alias) from vRefNum
		   from hard vRefNum/parID */
		PICloseAndOpenWD(spec.vRefNum, spec.parID, rVRefNum);
		return TRUE;
	}
	else return FALSE; // aliashandle was null
#endif
	return FALSE;
}

/*****************************************************************************/

Boolean PICreateFile (Str255 filename, short vRefNum, const ResType creator, 
					  const ResType type, FileHandle *fRefNum, short *result)
{
#if 0
	/* Mark the file as not being open. */
	
	*fRefNum = 0;
	
	/* Create the file. */

	(void) FSDelete (filename, vRefNum);

	if (!TestAndStoreResult (result, Create (filename, vRefNum, creator, type)))
		return FALSE;
		
	/* Open the file. */

	if (!TestAndStoreResult (result, FSOpen (filename, vRefNum, fRefNum)))
	{
		/* failure */
		(void) FSDelete (filename, vRefNum);
		return FALSE;
	}
	else
		return TRUE; // succeeded
#endif
	return FALSE; //failed
}

/*****************************************************************************/

Boolean PICloseFile (Str255 filename, short vRefNum,
					 FileHandle fRefNum, Boolean sameNames,
					 Boolean *dirty, AliasHandle *alias, short *result)
{

	OSErr			gotErr = noErr;
#if 0
	FSSpec			spec;
	
	// Close the file if it is open.
	if (fRefNum != 0)
		(void) TestAndStoreResult (result, FSClose (fRefNum));
		
	// Delete the file if we had an error.	
	if (*result != noErr)
		(void) FSDelete (filename, vRefNum);
			
	// Flush the volume anyway.
	if (!TestAndStoreResult (result, FlushVol (NULL, vRefNum))) return FALSE;
	
	// Mark the file as clean.
	*dirty = FALSE;
	
	/* create alias for scripting system */
	
	/* first, make FSSpec record */
	FSMakeFSSpec(vRefNum, 0, filename, &spec);

	if (sameNames)
	{ // didn't enter new filename, so set filename to nothing
		spec.name[ (spec.name[0] = 0)+1 ] = 0;
	} // otherwise use filename and store entire reference

	gotErr = NewAlias(nil, &spec, alias);
#endif
	return (gotErr == noErr);
	
}
	
/*****************************************************************************/

Boolean TestAndStoreResult (short *res, OSErr result)
{
	if (result != noErr)
		{
		if (*res == noErr)
			*res = result;
		return FALSE;
		}
	else
		return TRUE;	
}
	
/*****************************************************************************/

Boolean TestAndStoreCancel (short *res, Boolean tocancel)
{	
	if (tocancel)
		{
		if (*res == noErr)
			*res = userCanceledErr;
		return FALSE;
		}
	else
		return TRUE;	
}

/*****************************************************************************/
// Creates new working directory and deletes old one if it was valid

void PICloseAndOpenWD(const short vRefNum, const long dirID, short *rVRefNum)
{
	short oldVRefNum = *rVRefNum;
#if 0	
	OpenWD(vRefNum, dirID, 0, rVRefNum);
	if (*rVRefNum != oldVRefNum && oldVRefNum < -100)
	{ // small negative numbers are actual volumes (-1, etc.)
	  // large negative numbers (-32766, etc.) are WD's to be closed
		CloseWD(oldVRefNum); // close our old vRefNum
	}
#endif
}

#if 0 /* I haven't had a chance to fix these up and test them
       * so that's why they're commented out.
	   */
/************************* File I/O Function ****************************/
/* Normal Order */

long    eofPos = -1L;

OSErr SetFPos (int fHand, short posMode, long posOff)
{
    long    newPos;


    if (eofPos < 0) {   /* Initialize eofPos */
        newPos = _llseek(fHand, 0, fsFromMark);
        eofPos = _llseek(fHand, 0, fsFromEnd);
        if (posMode == fsFromMark)
            newPos = _llseek(fHand, newPos, fsFromStart);
    }

    newPos = _llseek(fHand, posOff, posMode);
    if (newPos == -1L || newPos >= eofPos)
        return eofErr;
    else
        return noErr;
}

OSErr GetFPos (int fHand, long *filePos)
{
    if ((*f = _llseek(fHand, 0, fsFromMark)) == -1L)
        return -1;
    else
        return noErr;
}


OSErr GetEOF (int fHand,long *logEOF)
{
    if ((*logEOF = _llseek(fHand, 0, fsFromEnd)) == -1L)
        return -1;
    else
        return noErr;
}


unsigned8 readBYTE(GPtr data)
{
    int       fHand = data->gStuff->dataFork;
    unsigned8   cdata;

    if (_lread(fHand, (LPSTR)&cdata, sizeof(unsigned8)) != sizeof(unsigned8))
        data->gResult = readErr;

    return (cdata);
}


unsigned16 readWORD(GPtr data)
{
    int       fHand = data->gStuff->dataFork;
    unsigned16  cdata;

    if (_lread(fHand, (LPSTR)&cdata, sizeof(unsigned16)) != sizeof(unsigned16))
        data->gResult = readErr;

    return (cdata);
}


unsigned32 readDWORD(GPtr data)
{
    int       fHand = data->gStuff->dataFork;
    unsigned16  cdata;

    if (_lread(fHand, (LPSTR)&cdata, sizeof(unsigned32)) != sizeof(unsigned32))
        data->gResult = readErr;

    return (cdata);
}


void readCount(GPtr data, Ptr buf, unsigned32 count)
{
    int       fHand = data->gStuff->dataFork;

    if (_lread(file, (LPSTR)buf, (short)count) != count)
        data->gResult = readErr;
}

void writeBYTE(GPtr data, BYTE cdata)
{
    short       file = (short) data->gStuff->dataFork;

    if (_lwrite(file, (LPSTR)&cdata, sizeof(BYTE)) != sizeof(BYTE))
        data->gResult = writErr;
}

void writeWORD(GPtr data, WORD cdata)
{
    short       file = (short) data->gStuff->dataFork;

    if (_lwrite(file, (LPSTR)&cdata, sizeof(WORD)) != sizeof(WORD))
        data->gResult = writErr;
}

void writeDWORD(GPtr data, DWORD cdata)
{
    short       file = (short) data->gStuff->dataFork;

    if (_lwrite(file, (LPSTR)&cdata, sizeof(DWORD)) != sizeof(DWORD))
        data->gResult = writErr;
}

void writeCount(GPtr data, Ptr buf, unsigned32 count)
{
    short       file = (short) data->gStuff->dataFork;

    if (_lwrite(file, (LPSTR)buf, (short)count) != count)
        data->gResult = writErr;
}

#endif