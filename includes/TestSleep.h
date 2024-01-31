
#ifndef TESTSLEEP_H
#define TESTSLEEP_H

#include <ProANSI.h>

#if (PRO_OS == 3)    /* UNIX */
#include <unistd.h>
#elif(PRO_OS == 4)   /* WINDOWS */
#include <windows.h>
#endif

PRO_BEGIN_C_DECLS
    
int ProTestSleep (int seconds);

PRO_END_C_DECLS

#endif /* ifdef TESTERROR_H */
