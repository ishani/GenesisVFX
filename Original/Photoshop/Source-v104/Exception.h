// This makes it virtually impossible for Genesis to crash Lightwave.


#ifndef _EXCEPTION_
#define _EXCEPTION_

#include <eh.h>


//#define ENABLE_EXCEPTIONS  // Undefine this to prevent exception handling.

#define GLOBAL_ABORT 0xFFFF


class exception
{

   private:

       unsigned int errorID;
       char      *exname;

   public:

       exception() { errorID = 0; exname = "Unknown Exeption."; }
       exception( unsigned int id) { errorID = id; exname = "System Exeption."; }
       exception( char *str) { errorID = 0xFFFE; exname = str; }
      ~exception() {}

      unsigned int getID() { return errorID; }
      char *name() { return exname; }

       void handle(void);

};

class exabort : public exception {};

void PPI_ExceptionHandler( unsigned int, _EXCEPTION_POINTERS *);


class exceptionInit
{
  public:

#ifdef ENABLE_EXCEPTIONS      
      exceptionInit() { _set_se_translator(PPI_ExceptionHandler); }
#endif
};

#ifdef ENABLE_EXCEPTIONS
#define EXCEPTION_START		try {
#define EXCEPTION_END		} catch (exception ex)	{ ex.handle(); }
#else
#define EXCEPTION_START		{
#define EXCEPTION_END		}
#endif

#endif _EXCEPTION_