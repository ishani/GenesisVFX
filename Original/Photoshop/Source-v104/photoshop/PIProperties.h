/*
	File: PIProperties.h

	Copyright (c) 1994-6, Adobe Systems Incorporated.
	All rights reserved.

	This file describes the selectors for the properties callback suite.
*/

/* Get the number of channels.  Returns the number of channels in the simple
   property field. */
   
#define propNumberOfChannels  'nuch'

/* Returns a handle containing the characters for the name of the channel. */

#define propChannelName		  'nmch'

/* Returns the image mode using the plugInMode constants.  (Simple) */

#define propImageMode		  'mode'

/* Returns the number of channels including the work path. (Simple) */

#define propNumberOfPaths	  'nupa'

/* Returns the name of the indexed path (zero-based). (Complex) */

#define propPathName          'nmpa'

/* Returns the contents of the indexed path (zero-based). (Complex) */

#define propPathContents      'path'

/* The index of the work path.  -1 if no work path. (Simple) */

#define propWorkPathIndex     'wkpa'

/* The index of the clipping path.  -1 if none. (Simple) */

#define propClippingPathIndex 'clpa'

/* The index of the target path.  -1 if none. (Simple) */

#define propTargetPathIndex   'tgpa'

/* The file meta information is an IPTC-NAA record (Complex, Modifiable) */

#define propCaption			  'capt'

/* The big nudge distance. Horizontal and vertical components. These are
   represented as a 16.16 values. (Simple, Modifiable) */

#define propBigNudgeH		  'bndH'
#define propBigNudgeV		  'bndV'

/* The current interpolation method: 1 = point sample, 2 = bilinear, 3 = bicubic
   (Simple) */
   
#define propInterpolationMethod 'intp'

/* The current ruler units. (Simple) */

#define propRulerUnits		    'rulr'

/* The current ruler origin.  Horizontal and vertical components.  These are
	represented as 16.16 values. (Simple, Modifiable) */

#define propRulerOriginH		'rorH'
#define propRulerOriginV		'rorV'

/* The current major grid rules, in inches, unless propRulerUnits is pixels, and
   then pixels.  Represented as 16.16 value. (Simple, Modifiable) */
   
#define propGridMajor			'grmj'

/* The current number of grid subdivisions per major rule (Simple, Modifiable) */

#define propGridMinor			'grmn'

/* The serial number string shown to the user. (Complex) */

#define propSerialString		'sstr'

/* The hardware gamma table. (PC Only) (Complex) */

#define propHardwareGammaTable	'hgam'

/* The interface Color scheme */

#define propInterfaceColor		'iclr'

/* The watch suspension level. When non-zero, you can make callbacks to the host
without fear that the watch will start spinning. It is reset to zero at the beginning
of each call from the host to the plug-in. (Simple, modifiable) */

#define propWatchSuspension		'wtch'

/* Whether the current image is considered copywritten (Simple, Modifiable) */

#define propCopyright			'cpyr'

/* The URL for the current image (Complex, Modifiable) */

#define propURL					'URL '

/* The title of the current document (Complex) */

#define propTitle				'titl'
