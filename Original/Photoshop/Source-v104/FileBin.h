#ifndef _ADJC_FILEBIN_
#define _ADJC_FILEBIN_

#include <memory.h>

typedef unsigned short int WORD;

#define Output(a,b) { FILE *FPtmp=fopen("out.tmp","a"); fprintf(FPtmp,a,b); fclose(FPtmp); }

inline void _FileReadF(FILE *FP,float *a)        { fread(a,sizeof(float),1,FP); }

inline void _FileReadI(FILE *FP,int *a  )        { fread(a,sizeof(int),1,FP);   }

inline void _FileReadW(FILE *FP,WORD *a  )       { fread(a,sizeof(WORD),1,FP);   }

inline void _FileReadC(FILE *FP,char *a )        { fread(a,sizeof(char),1,FP);  }

inline void _FileReadC(FILE *FP,unsigned char *a )     
												 { fread(a,sizeof(char),1,FP);  }

inline void _FileReadS(FILE *FP,char *a,int i)	 { fread(a,1,i,FP);             }

inline void _FileWriteF(FILE *FP,float a)         { fwrite(&a,sizeof(float),1,FP); }

inline void _FileWriteI(FILE *FP,int a  )         { fwrite(&a,sizeof(int),1,FP);   }

inline void _FileWriteW(FILE *FP,WORD a  )        { fwrite(&a,sizeof(WORD),1,FP);   }

inline void _FileWriteC(FILE *FP,char a )         { fwrite(&a,sizeof(char),1,FP);   }

inline void _FileWriteC(FILE *FP,unsigned char a )         
												  { fwrite(&a,sizeof(char),1,FP);   }

inline void _FileWriteS(FILE *FP,char *a,int i)	  { fwrite(a,1,i,FP);              }

inline void _MemReadF(char **FP,float *a)			{ memcpy((char *)a,(*FP),sizeof(float)); 
													  *(FP)+=sizeof(float); 
													}

inline void _MemReadI(char **FP,int *a)				{ memcpy((char *)a,(*FP),sizeof(int)); 
                                                      *(FP)+=sizeof(int); 
													}

inline void _MemReadW(char **FP,WORD *a)			{ memcpy((char *)a,(*FP),sizeof(WORD)); 
                                                      *(FP)+=sizeof(WORD); 
													}

inline void _MemReadC(char **FP,char *a)			{ memcpy((char *)a,(*FP),sizeof(char)); 
                                                      *(FP)+=sizeof(char); 
													}

inline void _MemReadC(char **FP,unsigned char *a)			
													{ memcpy((char *)a,(*FP),sizeof(char)); 
                                                      *(FP)+=sizeof(char); 
													}

inline void _MemReadS(char **FP,char *a,int i)		{ memcpy((char *)a,(*FP),i); 
                                                      *(FP)+=i; 
													}

inline int _MemWriteF(char **FP,float a)				{ if ((*FP)!=NULL) { memcpy((*FP),(char *)&a,sizeof(float)); 
																             *(FP)+=sizeof(float); 
																           }
														  return sizeof(float);
														}

inline int _MemWriteI(char **FP,int a  )				{ if ((*FP)!=NULL) { memcpy((*FP),(char *)&a,sizeof(int)); 
																             *(FP)+=sizeof(int); 
																           }
														  return sizeof(int);
														}

inline int _MemWriteW(char **FP,WORD a  )				{ if ((*FP)!=NULL) { memcpy((*FP),(char *)&a,sizeof(WORD)); 
																             *(FP)+=sizeof(WORD); 
																           }
														  return sizeof(WORD);
														}

inline int _MemWriteC(char **FP,char a )				{ if ((*FP)!=NULL) { memcpy((*FP),(char *)&a,sizeof(char)); 
																             *(FP)+=sizeof(char); 
																           }
														  return sizeof(char);
														}

inline int _MemWriteC(unsigned char **FP,char a )				
														{ if ((*FP)!=NULL) { memcpy((*FP),(char *)&a,sizeof(char)); 
																             *(FP)+=sizeof(char); 
																           }
														  return sizeof(char);
														}

inline int _MemWriteS(char **FP,char *a,int i)			{ if ((*FP)!=NULL) { memcpy((*FP),a,i); *(FP)+=i; }
														  return i;
														}

#endif