///////////////////////////////////////////////////////////
//
// PhotoMain.h:	header file for PhotoMain.cpp
// R. C. Wilson 9/3/97
//

// include photoshop headers
// will not compile as C++

extern "C" {
#include "photoshop\PIFilter.h"
#include "photoshop\PIUtilities.h"
#include "photoshop\PIProperties.h"
}

// structure holding photoshop interface params
typedef struct TParameters
	{
	short				SavedFilter;
	short				queryForParameters;
	ImageFilter_LensF*  filter;
	char				SaveData;
	} TParameters, *PParameters, **HParameters;


// defines for accessing photoshop information block
// defines for accessing photoshop information block
#define gParams				((PParameters) *(Stuff->parameters))
#define gQueryForParameters (gParams->queryForParameters)

// global data structure
typedef struct Globals
{
	short 				result;
	FilterRecord*		filterParamBlock;
	ImageFilter_LensF*	filter;
} Globals, *GPtr, **GHdl;

// more defines for accessing photoshop information block
#define gStuff  		Stuff
