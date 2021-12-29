/*//////////////////////////////////////////////////////////
//
// File: genesis.r
//
// resources for pipl
// Copyright (c) 1997, R.C. Wilson
// All rights reserved
///////////////////////////////////////////////////////////*/

/*
To include this file in the program, type from the command line
CL /P /EP genesis.r				generates genesis.i
cnvtpipl genesis.i genesis.pipl

then make sure the line #include "genesis.pipl" is in the .rc file
you may need to hack out the old resources from the .rc file
*/

#define MSWindows 1

/**********************************************************************************/
/* Defines needed in other rez files */

#include "version.h"

/********************************************************************************/

#if MSWindows
#include "photoshop\PIGeneral.h"
#include "photoshop\PIUtilities.r"
#include "photoshop\WinDialogUtils.r"
#endif

#include "photoshop\PIActions.h"

/********************************************************************************/



#define ourSuiteID			'sdKB'
#define ourClassID			ourSuiteID
#define ourEventID			'clrM'

#define typeColorName		'tyCN'
#define cnRGB				'cRGB'
#define cnHSB				'cHSB'
#define cnCMYK				'cmyK'
#define cnLab				'cLab'
#define cnGray				'graY'
#define cnHSL				'cHSL'
#define cnXYZ				'cXYZ'

/********************************************************************************/

/* Photoshop v3.0 and above resource */

resource 'PiPL' (ResourceID, purgeable)
	{
		{
		Kind { Filter },
		Name { plugInName "..." },
		Category { category },
		Version { (latestFilterVersion << 16) | latestFilterSubVersion },


		#if MSWindows
		CodeWin32X86 { "ENTRYPOINT" },
		#endif
		
		HasTerminology { ourClassID, ourEventID, ResourceID, "" },
		/* classID, eventID, aete ID, uniqueString */
		
		SupportedModes
			{
			noBitmap, doesSupportGrayScale,
			noIndexedColor, doesSupportRGBColor,
			noCMYKColor, noHSLColor,
			noHSBColor, noMultichannel,
			noDuotone, noLABColor
			},

		EnableInfo { "in (PSHOP_ImageMode, GrayScaleMode, RGBMode)" },
					     
		FilterCaseInfo
			{
				{
				/* Flat data, no selection */
				inStraightData, outStraightData,
				writeOutsideSelection,
				doesNotFilterLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Flat data with selection */
				inStraightData, outStraightData,
				writeOutsideSelection,
				doesNotFilterLayerMasks, worksWithBlankData,
				copySourceToDestination,
				
				/* Floating selection */
				inStraightData, outStraightData,
				writeOutsideSelection,
				doesNotFilterLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Editable transparency, no selection */
				inBlackMat, outBlackMat,
				writeOutsideSelection,
				doesNotFilterLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Editable transparency, with selection */
				inBlackMat, outBlackMat,
				writeOutsideSelection,
				doesNotFilterLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Preserved transparency, no selection */
				inBlackMat, outBlackMat,
				writeOutsideSelection,
				doesNotFilterLayerMasks, worksWithBlankData,
				copySourceToDestination,
					
				/* Preserved transparency, with selection */
				inBlackMat, outBlackMat,
				writeOutsideSelection,
				doesNotFilterLayerMasks, worksWithBlankData,
				copySourceToDestination
				}
			}
		}
	};

/* support for version <2.0 Photoshop */

resource 'PiMI' (ResourceID, purgeable)
{
	latestFilterVersion,
	latestFilterSubVersion,
	0,
	supportsGrayScale +
	supportsRGBColor,
	'    ', /* No required host */
	{},
};

/* About box resources */

resource StringResource (AboutID, "About Text", purgeable)
{
	plugInName "\n\n"
	"Version " VersionString " "
	"Release " ReleaseString "\n"
	"Copyright © 1996, R. C. Wilson and A. D. J. Cross.\n"
	"All rights reserved.\n\n"
	"Genesis¨."
};

