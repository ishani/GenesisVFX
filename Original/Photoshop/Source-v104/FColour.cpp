#include "FColour.h"

void fColour::FilterOver(float &or,float &og,float &ob,float &oa,fColour &C)
	{ or=C.r*(1-a)+r*a; og=C.g*(1-a)+g*a;
	  ob=C.b*(1-a)+b*a; oa=1.0f-(1.0f-C.a)*(1.0f-a);
	}

void fColour::FilterOver(fColour &o,fColour &C)
	{ o.r=C.r*(1-a)+r*a; o.g=C.g*(1-a)+g*a;
	  o.b=C.b*(1-a)+b*a; o.a=1.0f-(1.0f-C.a)*(1.0f-a);
	}

void fColour::FilterOver(float &or,float &og,float &ob,float &oa,float &Cr,float &Cg,float &Cb,float &Ca)
	{ or=Cr*(1-a)+r*a; og=Cg*(1-a)+g*a;
	  ob=Cb*(1-a)+b*a; oa=1.0f-(1.0f-Ca)*(1.0f-a);
	}

void fColour::FilterOver(float &Cr,float &Cg,float &Cb,float &Ca)
	{ Cr=Cr*(1-a)+r*a; Cg=Cg*(1-a)+g*a;
	  Cb=Cb*(1-a)+b*a; Ca=1.0f-(1.0f-Ca)*(1.0f-a);
	}