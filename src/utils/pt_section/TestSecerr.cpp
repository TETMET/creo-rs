

/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include "ProToolkit.h"
#include "ProObjects.h"
#include "ProSecerror.h"
#include "ProUtil.h"

/*--------------------------------------------------------------------*\
C System includes
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application includes
\*--------------------------------------------------------------------*/
#include "TestError.h"
#include "UtilMessage.h"
#include "TestSect.h"
#include "PTApplsUnicodeUtils.h"
/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application data types
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application global/external data
\*--------------------------------------------------------------------*/

/*====================================================================*\
    FUNCTION :  ProTestSecerrAlloc()
    PURPOSE  :  Allocates a work-space table of section errors
\*====================================================================*/
ProError ProTestSecerrAlloc(ProWSecerror *error_tbl)
{
    ProError status;

    status = ProSecerrorAlloc(error_tbl);
    TEST_CALL_REPORT("ProSecerrorAlloc()", "ProTestSecerrAlloc()",
                               status, status != PRO_TK_NO_ERROR);
    return (status);
}

/*====================================================================*\
    FUNCTION :  ProTestSecerrFree()
    PURPOSE  :  Free the work-space table of section errors
\*====================================================================*/
ProError ProTestSecerrFree(ProWSecerror *error_tbl)
{
    ProError status;

    status = ProSecerrorFree(error_tbl);
    TEST_CALL_REPORT("ProSecerrorFree()", "ProTestSecerrFree()",
                              status, status != PRO_TK_NO_ERROR);
    return (status);
}

/*====================================================================*\
    FUNCTION :  ProTestSecerrCount()
    PURPOSE  :  Counts the number of entries in the errors table
\*====================================================================*/
ProError ProTestSecerrCount(ProWSecerror *error_tbl, int *r_count)
{
    ProError status;

    status = ProSecerrorCount(error_tbl, r_count);
    TEST_CALL_REPORT("ProSecerrorCount()", "ProTestSecerrCount()",
                                status, status != PRO_TK_NO_ERROR);
    return (status);
}

/*====================================================================*\
    FUNCTION :  ProTestSecerrAdd()
    PURPOSE  :  Add a user-composed error to the errors table
\*====================================================================*/
ProError ProTestSecerrAdd(ProWSecerror *error_tbl, ProSecerrCode user_error,
		    wchar_t *msg, ProSecerrSever severity, int ent_id,
		    ProSecerrAttr attribute)
{
    ProError status;

    status = ProSecerrorAdd(error_tbl, user_error, msg, severity, ent_id,
			    attribute);
    TEST_CALL_REPORT("ProSecerrorAdd()", "ProTestSecerrAdd()",
                            status, status != PRO_TK_NO_ERROR);
    return (status);
}

/*====================================================================*\
    FUNCTION :  ProTestSecerrMsgGet()
    PURPOSE  :  Get the error message from ith error table entry
\*====================================================================*/
ProError ProTestSecerrMsgGet(ProWSecerror error_tbl, int ith, ProMsg PMsg)
{
    ProError status;

    status = ProSecerrorMsgGet(error_tbl, ith, PMsg);
    TEST_CALL_REPORT("ProSecerrorMsgGet()", "ProTestSecerrMsgGet()",
                              status, status != PRO_TK_NO_ERROR);
    return (status);
}

/*====================================================================*\
    FUNCTION :  ProTestSecerrItemGet()
    PURPOSE  :  Get the entity-in-trouble from ith error table entry
\*====================================================================*/
ProError ProTestSecerrItemGet(ProWSecerror error_tbl, int ith, int *r_item)
{
    ProError status;

    status = ProSecerrorItemGet(error_tbl, ith, r_item);
    TEST_CALL_REPORT("ProSecerrorItemGet()", "ProTestSecerrItemGet()",
                                   status, status != PRO_TK_NO_ERROR);
    return (status);
}

/*====================================================================*\
    FUNCTION :  ProTestSecerrShow()
    PURPOSE  :  Show the errors given a handle to error table
\*====================================================================*/
ProError ProTestSecerrShow(ProWSecerror error_tbl)
{
    ProError status;
    int i, err_count, entity_id;
    ProMsg err_msg;
    FILE *fp;
    char fname[] = "sec_err.txt";
    char astr[PRO_PATH_SIZE];
    wchar_t wstr[PRO_PATH_SIZE];

    if (error_tbl == NULL)
	return (PRO_TK_GENERAL_ERROR);

    status = ProTestSecerrCount(&error_tbl, &err_count);
    if (err_count <= 0)
        return (PRO_TK_GENERAL_ERROR);

    if ((fp = PTApplsUnicodeFopen(fname, "w")) == NULL)
    {
	ProTKFprintf(stderr, "Error, failed to open file %s.\n", fname);
	return (PRO_TK_GENERAL_ERROR);
    }

    ProTKFprintf(fp, "\n*** Errors of section solving and regenerating ***\n");
    ProTKFprintf(fp, "__________________________________________________\n\n");
    ProTKFprintf(fp, "Index   Entity ID       Error Message\n\n");
    for (i = 0; i < err_count; i++)
    {
	ProTestSecerrMsgGet(error_tbl, i, err_msg);
	ProTestSecerrItemGet(error_tbl, i, &entity_id);
	ProTKFprintf(fp, "%3d    %10d       %s\n", i, entity_id, 
		ProWstringToString(astr, err_msg));
    }
    fclose(fp);
    status = ProInfoWindowDisplay(ProStringToWstring(wstr, fname), NULL, NULL);
    TEST_CALL_REPORT ("ProInfoWindowDisplay", "ProTestSecerrShow",
        status, status != PRO_TK_NO_ERROR);
    return (PRO_TK_NO_ERROR);
}
