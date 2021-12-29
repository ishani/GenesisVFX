#ifndef _ADJCFCCLASS_
#define _ADJCFCCLASS_

#include <stdio.h>
#include <stdlib.h>
#include "FileBin.h"

class fColour
	{ public:
		float r,g,b,a; 
		
		//fColour() {}
	    //~fColour() {}
		
		void SetColourRGBA(float _r,float _g,float _b,float _a) { r=_r; g=_g; b=_b; a=_a;  }
		void SetColourRGBA(float _r,float _g,float _b)          { r=_r; g=_g; b=_b; a=1.0f;}
		void SetColourRGBA(float _G,float _a)                   { r=_G; g=_G; b=_G; a=_a;  }
		void SetColourRGBA(float _a)							{ a=_a;  }
		
		void FilterOver(float &,float &,float &,float &,fColour &);
		void FilterOver(fColour &,fColour &);
		void FilterOver(float &,float &,float &,float &,float &,float &,float &,float &);
		void FilterOver(float &,float &,float &,float &);

		void Save(FILE *FP) { _FileWriteF(FP,r); 
		                      _FileWriteF(FP,g); 
							  _FileWriteF(FP,b); 
							  _FileWriteF(FP,a);
		                    }

		void Load(FILE *FP) { _FileReadF(FP,&r); 
		                      _FileReadF(FP,&g); 
							  _FileReadF(FP,&b); 
							  _FileReadF(FP,&a);
		                    }

		int MemSave(char **FP) { int Ret=0;
		                         Ret+=_MemWriteF(FP,r); 
								 Ret+=_MemWriteF(FP,g); 
								 Ret+=_MemWriteF(FP,b); 
								 Ret+=_MemWriteF(FP,a);

								 return Ret;
							   }

		void MemLoad(char **FP) { _MemReadF(FP,&r); 
								  _MemReadF(FP,&g); 
								  _MemReadF(FP,&b); 
								  _MemReadF(FP,&a);
							    }
	};

class cColour
	{ public:
		unsigned char r,g,b,a; 
		
		//fColour() {}
	    //~fColour() {}
		
		void Save(FILE *FP) { _FileWriteC(FP,r); 
		                      _FileWriteC(FP,g); 
							  _FileWriteC(FP,b); 
							  _FileWriteC(FP,a);
		                    }

		void Load(FILE *FP) { _FileReadC(FP,&r); 
		                      _FileReadC(FP,&g); 
							  _FileReadC(FP,&b); 
							  _FileReadC(FP,&a);
		                    }

		int MemSave(char **FP) { int Ret=0;
		                         Ret+=_MemWriteC(FP,r); 
								 Ret+=_MemWriteC(FP,g); 
								 Ret+=_MemWriteC(FP,b); 
								 Ret+=_MemWriteC(FP,a);

								 return Ret;
							   }

		void MemLoad(char **FP) { _MemReadC(FP,&r); 
								  _MemReadC(FP,&g); 
								  _MemReadC(FP,&b); 
								  _MemReadC(FP,&a);
							    }
	};

#endif
