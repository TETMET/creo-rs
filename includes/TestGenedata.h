
#ifndef TESTGENEDATA_DOT_H
#define TESTGENEDATA_DOT_H

/*--------------------------------------------------------------------*\
Pro/DEVELOP includes
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProExtdata.h>
#include <ProObjects.h>
#include <TestError.h>
#include <UtilFiles.h>
#include <UtilMessage.h>

/*--------------------------------------------------------------------*\
Application data structures
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Function prototypes
\*--------------------------------------------------------------------*/
extern int ProTestRegClass(
        ProMdl p_model);
 
extern int ProTestCreateSlot(
        ProMdl p_model);
 
extern int ProTestWrCheck(
        ProMdl p_model);
 
extern int ProTestRandom(
        ProMdl p_model);
 
extern int ProTestDump(
        ProMdl p_model);
 
extern int ProTestDelete(
        ProMdl p_model);

extern int ProTestDeleteC(
        ProMdl p_model);

extern int ProTestRefresh(
        ProMdl p_model);
 
extern char *ProTestProAppdataErr(
        ProExtdataErr err);

extern ProError ProTestSvRdmData(
        ProExtdataClass *p_class,
        int             maxsize);

#endif /* TESTGENEDATA_DOT_H */
