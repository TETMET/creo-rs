

/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include        "ProToolkit.h"
#include        "ProObjects.h"
#include        "ProMdl.h"
#include        <ProUtil.h>
/*--------------------------------------------------------------------*\
Application includes
\*--------------------------------------------------------------------*/
#include	"UtilString.h"
#include        "TestError.h"

/*====================================================================*\
    FUNCTION :	ProUtilModelnameGet()
    PURPOSE  :	Get the name and type of a model (in lower case)
\*====================================================================*/
char *ProUtilModelnameGet(
    ProMdl *model,
    char *name,
    char *type)
{
    ProError status;
    ProMdldata mdata;
	ProMdlName modelName;
	ProMdlExtension modelExtension;
    char str[80];

/*--------------------------------------------------------------------*\
    Get the current model
\*--------------------------------------------------------------------*/
    if(*model == NULL)
    {
	status = ProMdlCurrentGet(model);
	TEST_CALL_REPORT("ProMdlCurrentGet()", "ProUtilModelnameGet()",
			    status, ((status != PRO_TK_NO_ERROR) &&
				     (status != PRO_TK_E_NOT_FOUND)));
        if(status == PRO_TK_E_NOT_FOUND)
            return(NULL);				
    }

/*--------------------------------------------------------------------*\
    Get the current model name and type
\*--------------------------------------------------------------------*/

    status = ProMdlMdlnameGet(*model, modelName);         
    TEST_CALL_REPORT ("ProMdlMdlnameGet()",
            "TestGtolPrintInfo()", status, status != PRO_TK_NO_ERROR);

	if(status != PRO_TK_NO_ERROR)
	return(NULL);

    status = ProMdlExtensionGet(*model, modelExtension);         
    TEST_CALL_REPORT ("ProMdlExtensionGet()",
            "TestGtolPrintInfo()", status, status != PRO_TK_NO_ERROR);

    if(status != PRO_TK_NO_ERROR)
	return(NULL);

/*--------------------------------------------------------------------*\
    Convert to C string and lower case.
\*--------------------------------------------------------------------*/
    if(name != NULL )
	ProUtilStringLower(ProWstringToString(str, modelName), name);
    if(type != NULL )
	ProUtilStringLower(ProWstringToString(str, modelExtension), type);

    return(name);
}
