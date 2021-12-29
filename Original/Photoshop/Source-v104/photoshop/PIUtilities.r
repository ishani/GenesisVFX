/*
	File: PIUtilities.r

	Copyright (c) 1990-1, Thomas Knoll.
	Copyright (c) 1992-6, Adobe Systems Incorporated.
	All rights reserved.

	Rez source file for Plug-in Utilities.
*/

/********************************************************************************/
// Version information for SDK releases
#define ReleaseString	"2 (11/96)"

// These are generic.  Override if you need to.
#define StringResource	'STR '
#define ResourceID		16000
#define AboutID			ResourceID
#define uiID			ResourceID+1
#define AlertID			16990
#define kBadNumberID	AlertID
#define kBadDoubleID	kBadNumberID+1
#define kNeedVers		kBadDoubleID+1
#define kWrongHost		kNeedVers+1

/********************************************************************************/
/* Alert strings */

resource StringResource (kBadNumberID, "Bad number", purgeable)
{
	"An integer is required between ^1 and ^2."
};

resource StringResource (kBadDoubleID, "Bad double", purgeable)
{
	"A decimal number is required between ^1 and ^2."
};

resource StringResource (kNeedVers, "Need vers", purgeable)
{
	"This plug-in requires Adobe Photoshop¨ ^1 or later functionality."
};

resource StringResource (kWrongHost, "Wrong host", purgeable)
{
	"This plug-in is incompatible with this version of the host program."
};

//----------------------------------------------------------------------------------------
// Descriptor for allowing filters to animate over time. A structure of this type can be
// added to a 'VFlt', an 'AFlt', or a PhotoShop filter to describe the data structure of
// its data blob. Specify pdOpaque for any non-scalar data in the record, or data that you
// don't want Premiere to interpolate for you. Make the FltD describe all the bytes in the
// data blob. Use ID 1000.
//----------------------------------------------------------------------------------------
type 'FltD' {
	array {
		integer					// Specifies the type of the data
			pdOpaque = 0,			// Opaque - don't interpolate this
			pdChar = 1,				// Interpolate as signed byte
			pdShort = 2,			// Interpolate as signed short
			pdLong = 3,				// Interpolate as signed long
			pdUnsignedChar = 4,		// Interpolate as unsigned byte
			pdUnsignedShort = 5,	// Interpolate as unsigned short
			pdUnsignedLong = 6,		// Interpolate as unsigned long
			pdExtended = 7,			// Interpolate as an extended
			pdDouble = 8,			// Interpolate as a double
			pdFloat = 9;			// Interpolate as a float
		integer;				// Count of bytes to skip with pdOpaque, 0 otherwise.
	};
};
