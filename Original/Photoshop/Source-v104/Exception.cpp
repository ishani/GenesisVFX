#include "exception.h"
#include <stdio.h>
#include <windows.h>

exceptionInit initialiseExceptionHandler;  // System based exeption handler install


void PPI_ExceptionHandler( unsigned int id, _EXCEPTION_POINTERS *pExp)
{
    throw exception(id);
}



void exception::handle(void)
{
    char buf[255];

    if(errorID == GLOBAL_ABORT) return;

    sprintf(buf,"ERROR 0x%X: %s.\n Genesis will protect application environment.",errorID,exname);

    MessageBox(NULL,buf,"Genesis Exeption Handler",MB_ICONSTOP | MB_OK);
}
