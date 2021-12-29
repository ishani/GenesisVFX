/*
	File: PIAbout.h

	Copyright (c) 1992-6, Adobe Systems Incorporated.
	All rights reserved.

	This file describes version 4.0 of Photoshop's plug-in module interface
	for the about box selector.
*/

#ifndef __PIAbout__
#define __PIAbout__

/******************************************************************************/
/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif

/******************************************************************************/
#define plugInSelectorAbout 	 0

typedef struct AboutRecord 
	{

	void *		platformData;		/* Platform specific information. */

	char		reserved [252]; 	/* Set to zero */

	}
AboutRecord, *AboutRecordPtr;

/******************************************************************************/
/* turn off the byte align pragma back to its original state; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a.
#endif

/******************************************************************************/

#endif /* __PIAbout__ */