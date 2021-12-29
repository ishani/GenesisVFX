/*
	File: DialogUtilities.r

	Copyright 1990-91 by Thomas Knoll.
	Copyright 1992-96 by Adobe Systems, Inc.

	Rez source file for Dialog Utilities.
*/

/********************************************************************************/

#define AlertID				16990
#define StringResource		'STR '

/********************************************************************************/

/* Alerts for dialog warnings */

resource 'DITL' (AlertID, purgeable)
{
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{87, 230, 107, 298},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{12, 60, 76, 300},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'ALRT' (AlertID, "Alert", purgeable)
{
	{0, 0, 120, 310},
	AlertID,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	}
};

/********************************************************************************/
