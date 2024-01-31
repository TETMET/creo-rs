

/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProObjects.h>
#include <ProMdl.h>
#include <ProMenu.h>
#include <ProSecdim.h>
#include <ProSecerror.h>
#include <ProSection.h>
#include <ProUtil.h>

/*--------------------------------------------------------------------*\
Application includes
\*--------------------------------------------------------------------*/
#include "TestSect.h"
#include "TestFeat.h"
#include "UtilMessage.h"
#include "TestError.h"
#include "UtilMath.h"
#include <PTApplsUnicodeUtils.h>
#include "UtilString.h"
/*====================================================================*\
    FUNCTION :	ProTestSectionMenuAction
    PURPOSE  :	Menu for section action            
\*====================================================================*/
int ProTestSectionMenuAction()
{
    int id;
    ProError status;
    status = ProMenuFileRegister((char *)"TK SECT ACT",(char *)"tk_secact.mnu", &id);
    TEST_CALL_REPORT ("ProMenuFileRegister", "ProTestSectionMenuAction",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char *)"TK SECT ACT",(char *)"-Create",
        (ProMenubuttonAction)ProTestSectionCreate, NULL,0);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionMenuAction",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char *)"TK SECT ACT",(char *)"-Retrieve",
        (ProMenubuttonAction)ProTestSectionRetrieve, NULL,0);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionMenuAction",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char *)"TK SECT ACT",(char *)"-Info",
	(ProMenubuttonAction)ProTestSectionInfoGet, NULL,0);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionMenuAction",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char *)"TK SECT ACT",(char *)"TK SECT ACT",
		      ( ProMenubuttonAction)ProMenuDelete,  NULL,0);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionMenuAction",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenuCreate(PROMENUTYPE_MAIN,(char *)"TK SECT ACT", &id);
    TEST_CALL_REPORT ("ProMenuCreate", "ProTestSectionMenuAction",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenuProcess((char *)"TK SECT ACT", &id);
    TEST_CALL_REPORT ("ProMenuProcess", "ProTestSectionMenuAction",
        status, status != PRO_TK_NO_ERROR);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestSectionSolveRegen
    PURPOSE  :	Solve and regenerate a section     
\*====================================================================*/
ProError ProTestSectionSolveRegen(ProSection section)
{
    ProError status;
    ProWSecerror sec_errors;
    ProLine w_message;

/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/

    ProTestSecerrAlloc(&sec_errors);
    ProStringToWstring(w_message, (char *)"ProSectionSolve() errors.");
    status = ProSecerrorAdd(&sec_errors, 0, w_message, 0, -1, 0);
    status = ProSectionSolve(section,&sec_errors);
    TEST_CALL_REPORT("ProSectionSolve()", "ProTestSectionSolveRegen()",
	          status, status != PRO_TK_NO_ERROR);
    ProTestSecerrShow(sec_errors);
    ProTestSecerrFree(&sec_errors);
    if (status != PRO_TK_NO_ERROR)
	return(PRO_TK_GENERAL_ERROR);

    ProTestSecerrAlloc(&sec_errors);
    status = ProSectionRegenerate(section,&sec_errors);
    TEST_CALL_REPORT("ProSectionRegenerate()", "ProTestSectionSolveRegen()",
		           status, status != PRO_TK_NO_ERROR);
    if (status != PRO_TK_NO_ERROR)
    {
	ProTestSecerrShow(sec_errors);
	return(PRO_TK_GENERAL_ERROR);
    }
    ProTestSecerrFree(&sec_errors);
    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestSectionCreate
    PURPOSE  :	Creates a pre-defined 2D section
\*====================================================================*/
int ProTestSectionCreate()
{
    ProSection section;
    ProError status;
    wchar_t sec_name[PRO_NAME_SIZE];
    Sect_types sect_type;

    status = ProSection2DAlloc(&section);
    TEST_CALL_REPORT("ProSection2DAlloc()", "ProTestSectionCreate()",
		     status, status != PRO_TK_NO_ERROR);

    status = ProTestSectionBuild(section,PRO_2DSECTION,NULL,
						NULL,&sect_type);

    ProTestSectionNameGet(sect_type, sec_name);
    status = ProSectionNameSet(section,sec_name);
    TEST_CALL_REPORT("ProSectionNameSet()", "ProTestSectionCreate()",
		     status, status != PRO_TK_NO_ERROR);

    ProUtilSectionSave(section);
    status = ProSectionFree(&section);
    TEST_CALL_REPORT("ProSectionFree()", "ProTestSectionCreate()",
		     status, status != PRO_TK_NO_ERROR);

    return(0);
}

/*====================================================================*\
    FUNCTION :	ProTestSectionRetrieve
    PURPOSE  :	Retrieves a section   
\*====================================================================*/
int ProTestSectionRetrieve()
{
    ProSection section;
    ProError status;

    status = ProUtilSectionRetrieve(&section);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestSectionInfoGet
    PURPOSE  :	Dumps section entity data to a file
\*====================================================================*/
int ProTestSectionInfoGet()
{
    ProSection section;
    ProError status;
    FILE *fp;
    ProName wname;
    char name[PRO_LINE_SIZE];
    wchar_t wfname[PRO_LINE_SIZE];

    status = ProUtilSectionRetrieve(&section);

    status = ProSectionNameGet(section,wname);
    TEST_CALL_REPORT("ProSectionNameGet()", "ProTestSectionInfoGet()",
		     status, status != PRO_TK_NO_ERROR);
    ProWstringToString(name,wname);
    ProUtilstrcat(name,"_sec.inf");
    fp = PTApplsUnicodeFopen(name,"w");

    status = ProUtilSectionInfoGet(fp,section);

    fclose(fp);

    ProInfoWindowDisplay(ProStringToWstring(wfname,name), NULL, NULL);
    return(0);
}
/*====================================================================*\
    FUNCTION :  ProTestSectionNameGet
    PURPOSE  :  Gets the default name of a 2D section
\*====================================================================*/
void ProTestSectionNameGet(Sect_types sect_type, ProName sec_name)
{
    switch (sect_type)
    {
        case SEC_BOX:
            ProStringToWstring(sec_name, (char*)"BOX");
            break;
        case SEC_CIRCLE:
            ProStringToWstring(sec_name, (char*)"CIRCLE");
            break;
        case SEC_SLOT:
            ProStringToWstring(sec_name, (char*)"SLOT");
            break;
        case SEC_SPLINE:
            ProStringToWstring(sec_name, (char*)"SPLINE");
            break;
        case SEC_WIDGET:
            ProStringToWstring(sec_name, (char*)"WIDGET");
            break;
        case SEC_ANGLE:
            ProStringToWstring(sec_name, (char*)"ANGLE");
            break;
        case SEC_DIAM:
            ProStringToWstring(sec_name, (char*)"DAMETER");
            break;
        default:
            ProStringToWstring(sec_name, (char*)"Unknown");
            break;
    }
}

/*====================================================================*\
    FUNCTION :	ProTestSectionBuild
    PURPOSE  :	Builds a specific section for smoke testing
\*====================================================================*/
ProError ProTestSectionBuild(
    ProSection section, /* In: Pre-Allocated Workspace section handle */
    ProType sec_type, /* 2D or 3D section */
    ProTestSectionInfo *p_sketch_refs, /* In: sketch pln info for 3D sec */
    void *dummy, 
    Sect_types *sect_type)    /* Out: Type of section to create */
{
    ProError status = PRO_TK_GENERAL_ERROR;
    int id;
    int ProUtilUseEdge(ProAppData app_dt, int act);
    
    status = ProMenuFileRegister((char*)"TkSects",(char*)"tksects.mnu", &id);
    TEST_CALL_REPORT ("ProMenuFileRegister", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Box",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_BOX);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Circle",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_CIRCLE);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Slot",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_SLOT);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Spline",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_SPLINE);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Widget",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_WIDGET);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Angle",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_ANGLE);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Diameter",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_DIAM);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Use Existing",
        (ProMenubuttonAction)ProUtilMenuAction, NULL, SEC_EXISTING);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"-Use Edge",
	(ProMenubuttonAction)ProUtilMenuAction,  NULL, SEC_USE_EDGE);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProTestSectionBuild",
	status, status != PRO_TK_NO_ERROR);

    status = ProMenubuttonActionSet((char*)"TkSects",(char*)"TkSects", 
        (ProMenubuttonAction)ProUtilMenuAction,NULL, -1);

    status = ProMenuCreate(PROMENUTYPE_MAIN, (char*)"TkSects", &id);
    TEST_CALL_REPORT ("ProMenuCreate", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenuProcess((char*)"TkSects", (int*)sect_type);
    TEST_CALL_REPORT ("ProMenuProcess", "ProTestSectionBuild",
        status, status != PRO_TK_NO_ERROR);

    status = ProUtilSectionClean(section);

    switch (*sect_type)
    {
         case SEC_BOX:
              status = ProTestBoxSectBuild(section,sec_type,p_sketch_refs);
              break;
         case SEC_CIRCLE:
              status = ProTestCircleSectBuild(section,sec_type,p_sketch_refs);
              break;
         case SEC_SLOT:
              status = ProTestSlotSectBuild(section,sec_type,p_sketch_refs);
              break;
         case SEC_SPLINE:
	      status = ProTestSplineSectBuild(section,sec_type,p_sketch_refs);
	      break;
         case SEC_WIDGET:
	      status = ProTestWidgetSectBuild(section,sec_type,p_sketch_refs);
              break;
         case SEC_ANGLE:
	      status = ProTestAngleSectBuild(section,sec_type,p_sketch_refs);
              break;
         case SEC_DIAM:
              status = ProTestDiamSectBuild(section,sec_type,p_sketch_refs);
              break;
         case SEC_EXISTING:
	      status = ProUtilSectionUseExisting(section,p_sketch_refs);
	      break;
         case SEC_USE_EDGE:
            status = (ProError)ProUtilUseEdge (section,  -1);
            break;
         default:
	      status = PRO_TK_GENERAL_ERROR;
         
    }


    return(status);
}

static int ProUtilMenuAction(
    char *dummy,
    int action)
{
    ProError status;

    status = ProMenuDeleteWithStatus(action);
    TEST_CALL_REPORT ("ProMenuDeleteWithStatus", "ProUtilMenuAction",
       status, status != PRO_TK_NO_ERROR);
    
    return (0);
}

/*====================================================================*\
    FUNCTION :  ProTestBoxSectBuild()
    PURPOSE  :  Create a box section
\*====================================================================*/
ProError ProTestBoxSectBuild(
    ProSection section,
    ProType sec_type,
    ProTestSectionInfo *p_sketch_refs)
{
    ProError status;
    Pro2dLinedef line;
    int ent_ids[7];
    double width,height,offset;
    Pro2dPnt dim_pnt;
    int entdim_ids[2];
    int dim_ids[5];
    double edge1[2][3],edge2[2][3],offsets[2];
    double sk_transf[4][4];
    double transform[4][4];
    double angle = 0.0;
    ProSectionPointType pnt_types[2];
    ProLine w_message;
    ProWSecerror sec_errors;

    width = 12.0;
    height = 10.0;

    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Box width [%0(6.2)f]: ",&width);
    ProUtilDoubleGet(NULL,&width,&width);
    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Box height [%0(6.2)f]: ",&height);
    ProUtilDoubleGet(NULL,&height,&height);

    offset = width/2.0;
    offsets[0] = offsets[1] = offset;

    ProUtilSectEpsModify(section);

    if (p_sketch_refs != NULL)
    {
        status = ProUtilTwoOrthogEdgesOnPlaneGet(section,&ent_ids[5],
                                                 edge1,edge2);
        if ( status != PRO_TK_NO_ERROR )
            return(PRO_TK_GENERAL_ERROR);

        ProUtilXyOffsetsGet(offsets);

        ProUtilSketchCsysGet(p_sketch_refs,sk_transf);

        ProUtilSectCsysTransformGet(p_sketch_refs,sk_transf,edge1,
                                    edge2,transform,&angle);
        if ((fabs(transform[0][0])>1e-3) ^
            (!(fabs(edge1[0][0]-edge1[1][0])>1e-3)))
        {
            offset = offsets[0];offsets[0]=offsets[1];offsets[1]=offset;
        }
    }

/*--------------------------------------------------------------------*\
    Define the lines of the box
\*--------------------------------------------------------------------*/
    line.type = PRO_2D_CENTER_LINE;
    line.end1[0] = 0.0;
    line.end1[1] = 0.0;
    line.end2[0] = 0.0;
    line.end2[1] = 1.0;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);

    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[0]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestBoxSectBuild()",
                           status, status != PRO_TK_NO_ERROR);

    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0];
    line.end1[1] = offsets[1];
    line.end2[0] = offsets[0];
    line.end2[1] = offsets[1] + height;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,  &ent_ids[1]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestBoxSectBuild()",
                          status, status != PRO_TK_NO_ERROR);

    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0];
    line.end1[1] = offsets[1] + height;
    line.end2[0] = offsets[0] + width;
    line.end2[1] = offsets[1] + height;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,  &ent_ids[2]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestBoxSectBuild()",
                          status, status != PRO_TK_NO_ERROR);

    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0] + width;
    line.end1[1] = offsets[1] + height;
    line.end2[0] = offsets[0] + width;
    line.end2[1] = offsets[1];
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,  &ent_ids[3]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestBoxSectBuild()",
                          status, status != PRO_TK_NO_ERROR);
    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0] + width;
    line.end1[1] = offsets[1];
    line.end2[0] = offsets[0];
    line.end2[1] = offsets[1];
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,
                                      &ent_ids[4]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestBoxSectBuild()",
                          status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Dimension the box
\*--------------------------------------------------------------------*/
    dim_pnt[0] = offsets[0] -1;
    dim_pnt[1] = offsets[1] + height/2;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[2];
    entdim_ids[1] = ent_ids[4];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section, entdim_ids, pnt_types, 2,
                        PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[0]);
    TEST_CALL_REPORT("ProSecdimCreate()",
                "ProTestBoxSectBuild()", status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offsets[0] - width/2;
    dim_pnt[1] = offsets[1] + 1.0;
    entdim_ids[0] = ent_ids[2];
    pnt_types[0] = PRO_ENT_WHOLE;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    status = ProSecdimCreate(section,entdim_ids,pnt_types,1,
                                    PRO_TK_DIM_LINE, dim_pnt, &dim_ids[1]);
    TEST_CALL_REPORT("ProSecdimCreate()",
                "ProTestBoxSectBuild()", status, status != PRO_TK_NO_ERROR);
/*--------------------------------------------------------------------*\
    Autodim section
\*--------------------------------------------------------------------*/
    ProTestSecerrAlloc(&sec_errors);
    ProStringToWstring(w_message, (char *)"ProSectionAutodim() errors.");
    status = ProSecerrorAdd(&sec_errors, 0, w_message, 0, -1, 0);
    TEST_CALL_REPORT("ProSecerrorAdd()",
            "ProTestBoxSectBuild()", status, status != PRO_TK_NO_ERROR);
    status = ProSectionAutodim(section, &sec_errors);
    TEST_CALL_REPORT("ProSectionAutodim()",
            "ProTestBoxSectBuild()", status, status != PRO_TK_NO_ERROR);
    if (status != PRO_TK_NO_ERROR)
        ProTestSecerrShow(sec_errors);
    ProTestSecerrFree(&sec_errors);

/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/
    ProTestSecerrAlloc(&sec_errors);
    status = ProSectionRegenerate(section,&sec_errors);
    TEST_CALL_REPORT("ProSectionRegenerate()",
                "ProTestBoxSectBuild()", status, status != PRO_TK_NO_ERROR);
    if (status != PRO_TK_NO_ERROR)
    {
        ProTestSecerrShow(sec_errors);
        return(PRO_TK_GENERAL_ERROR);
    }
    ProTestSecerrFree(&sec_errors);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestDiamSectBuild()
    PURPOSE  :	Test SecdimDiameter functions
\*====================================================================*/
ProError ProTestDiamSectBuild(
    ProSection section,
    ProType sec_type,
    ProTestSectionInfo *p_sketch_refs)
{
    ProError status;
    Pro2dLinedef line;
    int ent_ids[7];
    double width,height,offset;
    Pro2dPnt dim_pnt;
    int entdim_ids[2];
    int dim_ids[5];
    double edge1[2][3],edge2[2][3],offsets[2];
    double sk_transf[4][4];
    double transform[4][4];
    double angle = 0.0;
    ProSectionPointType pnt_types[2];
    ProLine w_message;
    ProWSecerror sec_errors;
    ProBoolean dim_is_diameter;

    width = 12.0;
    height = 10.0;

    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Box width [%0(6.2)f]: ",&width);
    ProUtilDoubleGet(NULL,&width,&width);
    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Box height [%0(6.2)f]: ",&height);
    ProUtilDoubleGet(NULL,&height,&height);

    offset = width/2.0;
    offsets[0] = offsets[1] = offset;

    ProUtilSectEpsModify(section);

    if (p_sketch_refs != NULL)
    {
	status = ProUtilTwoOrthogEdgesOnPlaneGet(section,&ent_ids[5],
						 edge1,edge2);
        if ( status != PRO_TK_NO_ERROR )
	    return(PRO_TK_GENERAL_ERROR);

        ProUtilXyOffsetsGet(offsets);

	ProUtilSketchCsysGet(p_sketch_refs,sk_transf);

	ProUtilSectCsysTransformGet(p_sketch_refs,sk_transf,edge1,
				    edge2,transform,&angle);
	if ((fabs(transform[0][0])>1e-3) ^ 
	    (!(fabs(edge1[0][0]-edge1[1][0])>1e-3)))
	{
	    offset = offsets[0];offsets[0]=offsets[1];offsets[1]=offset;
	}
    }

/*--------------------------------------------------------------------*\
    Define the lines of the box	
\*--------------------------------------------------------------------*/
    line.type = PRO_2D_CENTER_LINE;
    line.end1[0] = 0.0;
    line.end1[1] = 0.0;
    line.end2[0] = 0.0;
    line.end2[1] = 1.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,line.end1,line.end1);
	ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[0]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestDiamSectBuild()",
		           status, status != PRO_TK_NO_ERROR);
       
    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0];
    line.end1[1] = offsets[1];
    line.end2[0] = offsets[0];
    line.end2[1] = offsets[1] + height;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,line.end1,line.end1);
	ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,  &ent_ids[1]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestDiamSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0];
    line.end1[1] = offsets[1] + height;
    line.end2[0] = offsets[0] + width;
    line.end2[1] = offsets[1] + height;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,line.end1,line.end1);
	ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,  &ent_ids[2]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestDiamSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0] + width;
    line.end1[1] = offsets[1] + height;
    line.end2[0] = offsets[0] + width;
    line.end2[1] = offsets[1];
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,line.end1,line.end1);
	ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,  &ent_ids[3]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestDiamSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0] + width;
    line.end1[1] = offsets[1];
    line.end2[0] = offsets[0];
    line.end2[1] = offsets[1];
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,line.end1,line.end1);
	ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,
				      &ent_ids[4]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestDiamSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
/*--------------------------------------------------------------------*\
    Dimension the box
\*--------------------------------------------------------------------*/
    dim_pnt[0] = offsets[0] -1;
    dim_pnt[1] = offsets[1] + height/2;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[2];
    entdim_ids[1] = ent_ids[4];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section, entdim_ids, pnt_types, 2,
			PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[0]);
    TEST_CALL_REPORT("ProSecdimCreate()", 
		"ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offsets[0] - width/2;
    dim_pnt[1] = offsets[1] + 1.0;
    entdim_ids[0] = ent_ids[2];
    pnt_types[0] = PRO_ENT_WHOLE;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    status = ProSecdimCreate(section,entdim_ids,pnt_types,1,
				    PRO_TK_DIM_LINE, dim_pnt, &dim_ids[1]);
    TEST_CALL_REPORT("ProSecdimCreate()", 
		"ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offsets[0] - width/4;
    dim_pnt[1] = offsets[1] + height/2;
    entdim_ids[0] = ent_ids[0];
    entdim_ids[1] = ent_ids[1];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_WHOLE;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
        PRO_TK_DIM_LINE_LINE, dim_pnt, &dim_ids[2]);
    TEST_CALL_REPORT("ProSecdimCreate()",
                "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);

#ifndef PT_PRODUCTS_BUILD
    status = ProSecdimDiameterInquire (section, dim_ids[2], &dim_is_diameter);
    TEST_CALL_REPORT ("ProSecdimDiameterInquire()", 
        "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);
    ProTKPrintf ("dim_is_diameter %d\n", dim_is_diameter);

    if (dim_is_diameter != PRO_B_TRUE)
    {
        status = ProSecdimDiameterSet (section, dim_ids[2]);
        TEST_CALL_REPORT ("ProSecdimDiameterSet()", 
            "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);
        status = ProSecdimDiameterInquire (section, dim_ids[2],
            &dim_is_diameter);
        TEST_CALL_REPORT ("ProSecdimDiameterInquire()",
            "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);
        ProTKPrintf ("dim_is_diameter %d\n", dim_is_diameter);

        if (dim_is_diameter == PRO_B_TRUE) 
        {
            status = ProSecdimDiameterClear (section, dim_ids[2]);
            TEST_CALL_REPORT ("ProSecdimDiameterClear()",
                "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);
            status = ProSecdimDiameterInquire (section, dim_ids[2],
                &dim_is_diameter);
            TEST_CALL_REPORT ("ProSecdimDiameterInquire()",
                "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);
            ProTKPrintf ("dim_is_diameter %d\n", dim_is_diameter);
        }
    }
#endif

/*--------------------------------------------------------------------*\
    Autodim section
\*--------------------------------------------------------------------*/
    ProTestSecerrAlloc(&sec_errors);
    ProStringToWstring(w_message, (char*)"ProSectionAutodim() errors.");
    status = ProSecerrorAdd(&sec_errors, 0, w_message, 0, -1, 0);
    TEST_CALL_REPORT("ProSecerrorAdd()", 
	    "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);
    status = ProSectionAutodim(section, &sec_errors);
    TEST_CALL_REPORT("ProSectionAutodim()", 
	    "ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);

    if (status != PRO_TK_NO_ERROR)
	ProTestSecerrShow(sec_errors);
    ProTestSecerrFree(&sec_errors);

/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/
    ProTestSecerrAlloc(&sec_errors);
    status = ProSectionRegenerate(section,&sec_errors);
    TEST_CALL_REPORT("ProSectionRegenerate()",
		"ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);
    if (status != PRO_TK_NO_ERROR)
    {
	ProTestSecerrShow(sec_errors);
	return(PRO_TK_GENERAL_ERROR);
    }
    ProTestSecerrFree(&sec_errors);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestCircleSectBuild()
    PURPOSE  :	Create a circle section
\*====================================================================*/
ProError ProTestCircleSectBuild(
    ProSection section,
    ProType sec_type,
    ProTestSectionInfo *p_sketch_refs)
{
    ProError status;
    Pro2dClinedef cline;
    Pro2dCircledef circle;
    int ent_ids[4];
    double diameter, offset;
    Pro2dPnt dim_pnt;
    int entdim_ids[2];
    int dim_ids[4];
    double edge1[2][3],edge2[2][3],cln[2][3];
    double sk_transf[4][4];
    double transform[4][4],offsets[2];
    double angle = 0.0;
    ProSectionPointType pnt_types[2];
    int par_idx,oth_idx; 
    int test_dim;

    diameter = 100.0;

    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Circle diameter [%0(6.2)f]: ",&diameter);
    ProUtilDoubleGet(NULL,&diameter,&diameter);

    offset = diameter;
    offsets[0] = offsets[1] = offset;

    ProUtilSectEpsModify(section);

    if (p_sketch_refs != NULL)
    {
	status = ProUtilTwoOrthogEdgesOnPlaneGet(section,&ent_ids[2],
						 edge1,edge2);
        if ( status != PRO_TK_NO_ERROR )
	    return(PRO_TK_GENERAL_ERROR);

        ProUtilXyOffsetsGet(offsets);
	ProUtilSketchCsysGet(p_sketch_refs,sk_transf);
	ProUtilSectCsysTransformGet(p_sketch_refs,sk_transf,edge1,
				    edge2,transform,&angle);
    }

/*--------------------------------------------------------------------*\
    Define the circle sketch    
\*--------------------------------------------------------------------*/

    cline.type = PRO_2D_CENTER_LINE;
    cline.end1[0] = 0.0;
    cline.end1[1] = 0.0;
    cline.end2[0] = 0.0;
    cline.end2[1] = 1.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,cline.end1,cline.end1);
	ProUtil2DPointTrans(transform,cline.end2,cline.end2);
    }
    cln[0][0] = cline.end1[0];
    cln[0][1] = cline.end1[1];
    cln[0][2] = 0.0;
    cln[1][0] = cline.end2[0];
    cln[1][1] = cline.end2[1];
    cln[1][2] = 0.0;
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&cline,&ent_ids[0]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestCircleSectBuild()",
		           status, status != PRO_TK_NO_ERROR);
       
    circle.type = PRO_2D_CIRCLE;
    circle.center[0] = offset;
    circle.center[1] = diameter/2 + 1.0;
    circle.radius = diameter/2;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,circle.center,circle.center);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&circle,&ent_ids[1]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestCircleSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Dimension the circle
\*--------------------------------------------------------------------*/
    dim_pnt[0] = offset/2;
    dim_pnt[1] = diameter/2;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[0];
    entdim_ids[1] = ent_ids[1];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_CENTER;

    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
			PRO_TK_DIM_LINE_POINT,dim_pnt,&dim_ids[0]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestCircleSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    status = ProSecdimValueSet(section,dim_ids[0],offsets[0]);
    TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestCircleSectBuild()",
		       status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = (offset+diameter+1.0)/2;
    dim_pnt[1] = diameter/2.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[1];
    pnt_types[0] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,1,
		             PRO_TK_DIM_DIA,dim_pnt,&dim_ids[1]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestCircleSectBuild()",
		              status, status != PRO_TK_NO_ERROR);

    status = ProSecdimValueSet(section,dim_ids[1],diameter);
    TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestCircleSectBuild()",
		       status, status != PRO_TK_NO_ERROR);

/* Dim not germane to sketch - specifically to test delete func */
    dim_pnt[0] = (offset+diameter+1.0);
    dim_pnt[1] = diameter/2.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[1];
    pnt_types[0] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,1,
		             PRO_TK_DIM_RAD,dim_pnt,&test_dim);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestCircleSectBuild()",
		              status, status != PRO_TK_NO_ERROR);
    status = ProSecdimDelete(section,test_dim);
    TEST_CALL_REPORT("ProSecdimDelete()", "ProTestCircleSectBuild()",
		              status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Position the circle on the sketch plane if 3d section
\*--------------------------------------------------------------------*/
    if (p_sketch_refs != NULL)
    {
        dim_pnt[0] = -offset/2;
        dim_pnt[1] = diameter/2;
        if (p_sketch_refs != NULL)
        {
	    ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }
	entdim_ids[0] = ent_ids[0];
        status = ProUtilParallelLineEntityGet(section,ent_ids[0],
					      &ent_ids[2],2,&par_idx);
        entdim_ids[1] = ent_ids[2+par_idx];
	pnt_types[0] = PRO_ENT_WHOLE;
	pnt_types[1] = PRO_ENT_WHOLE;
	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		                 PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[2]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestCircleSectBuild()",
		      status, status != PRO_TK_NO_ERROR);

        dim_pnt[0] = offset+diameter/2;
        dim_pnt[1] = diameter/2;
        if (p_sketch_refs != NULL)
        {
	    ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }
	entdim_ids[1] = ent_ids[1];
	if ((2+par_idx)==2) oth_idx = 3;
	    else oth_idx =2;
        entdim_ids[0] = ent_ids[oth_idx];
	pnt_types[1] = PRO_ENT_CENTER;
	pnt_types[0] = PRO_ENT_WHOLE;
	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		              PRO_TK_DIM_LINE_POINT,dim_pnt,&dim_ids[3]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestCircleSectBuild()",
		      status, status != PRO_TK_NO_ERROR);

        status = ProSecdimValueSet(section,dim_ids[2],0.0);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestCircleSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
         
        status = ProSecdimValueSet(section,dim_ids[3],offsets[oth_idx-2]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestCircleSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
        status = ProSecdimValueSet(section,dim_ids[0],offsets[par_idx]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestCircleSectBuild()",
		       status, status != PRO_TK_NO_ERROR);
    }
     
/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/

    status = ProTestSectionSolveRegen(section);
    if (status != PRO_TK_NO_ERROR)
	return(PRO_TK_GENERAL_ERROR);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestSlotSectBuild()
    PURPOSE  :	Create a slot section
\*====================================================================*/
ProError ProTestSlotSectBuild(
    ProSection section,
    ProType sec_type,
    ProTestSectionInfo *p_sketch_refs)
{
    ProError status;
    Pro2dClinedef cline;
    Pro2dLinedef line;
    Pro2dArcdef arc;
    int ent_ids[8];
    double offset,width,height;
    Pro2dPnt dim_pnt;
    int entdim_ids[2];
    int dim_ids[5];
    double edge1[2][3],edge2[2][3],cln[2][3];
    double sk_transf[4][4];
    double transform[4][4],offsets[2];
    double angle = 0.0;
    ProSectionPointType pnt_types[2];
    int par_idx,oth_idx; 
    int test_dim;

    height = 50.0;
    width = 100.0;

    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Slot width [%0(6.2)f]: ",&width);
    ProUtilDoubleGet(NULL,&width,&width);
    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Slot height [%0(6.2)f]: ",&height);
    ProUtilDoubleGet(NULL,&height,&height);

    offset = width/2.0 + 1.0;
    offsets[0] = offsets[1] = offset;

    ProUtilSectEpsModify(section);

    if (p_sketch_refs != NULL)
    {
	status = ProUtilTwoOrthogEdgesOnPlaneGet(section,&ent_ids[6],
						 edge1,edge2);
        if ( status != PRO_TK_NO_ERROR )
	    return(PRO_TK_GENERAL_ERROR);

        ProUtilXyOffsetsGet(offsets);

	ProUtilSketchCsysGet(p_sketch_refs,sk_transf);

	ProUtilSectCsysTransformGet(p_sketch_refs,sk_transf,edge1,
			        edge2,transform,&angle);
    }

/*--------------------------------------------------------------------*\
    Define the Slot sketch    
\*--------------------------------------------------------------------*/

    cline.type = PRO_2D_CENTER_LINE;
    cline.end1[0] = 0.0;
    cline.end1[1] = 0.0;
    cline.end2[0] = 0.0;
    cline.end2[1] = 1.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,cline.end1,cline.end1);
	ProUtil2DPointTrans(transform,cline.end2,cline.end2);
    }
    cln[0][0] = cline.end1[0];
    cln[0][1] = cline.end1[1];
    cln[0][2] = 0.0;
    cln[1][0] = cline.end2[0];
    cln[1][1] = cline.end2[1];
    cln[1][2] = 0.0;
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&cline,&ent_ids[0]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSlotSectBuild()",
		           status, status != PRO_TK_NO_ERROR);
       
    cline.type = PRO_2D_CENTER_LINE;
    cline.end1[0] = offset;
    cline.end1[1] = 0.0;
    cline.end2[0] = offset;
    cline.end2[1] = 1.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,cline.end1,cline.end1);
	ProUtil2DPointTrans(transform,cline.end2,cline.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&cline,&ent_ids[1]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSlotSectBuild()",
		           status, status != PRO_TK_NO_ERROR);

    line.type = PRO_2D_LINE;
    line.end1[0] = offset - (width-height)/2.0;
    line.end1[1] = offset;
    line.end2[0] = offset + (width-height)/2.0;
    line.end2[1] = offset;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,line.end1,line.end1);
	ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[2]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSlotSectBuild()",
		           status, status != PRO_TK_NO_ERROR);

    line.type = PRO_2D_LINE;
    line.end1[0] = offset - (width-height)/2.0;
    line.end1[1] = offset+height;
    line.end2[0] = offset + (width-height)/2.0;
    line.end2[1] = offset+height;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,line.end1,line.end1);
	ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[3]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSlotSectBuild()",
		           status, status != PRO_TK_NO_ERROR);

    arc.type = PRO_2D_ARC;
    arc.center[0] = offset - (width-height)/2.0;
    arc.center[1] = offset+height/2.0;
    arc.radius = height/2.0;
    arc.start_angle = PI/2.0;
    arc.end_angle = 3.0*PI/2.0;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,arc.center,arc.center);
	arc.start_angle += angle;
	arc.end_angle += angle;
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&arc,&ent_ids[4]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSlotSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    arc.type = PRO_2D_ARC;
    arc.center[0] = offset + (width-height)/2.0;
    arc.center[1] = offset+height/2.0;
    arc.radius = height/2.0;
    arc.start_angle = 3.0*PI/2.0;
    arc.end_angle = PI/2.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,arc.center,arc.center);
	arc.start_angle += angle;
	arc.end_angle += angle;
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&arc,&ent_ids[5]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSlotSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Dimension the Slot   
\*--------------------------------------------------------------------*/
    dim_pnt[0] = offset/2;
    dim_pnt[1] = offset;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[0];
    entdim_ids[1] = ent_ids[1];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
			PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[0]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSlotSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    status = ProSecdimValueSet(section,dim_ids[0],offsets[0]);
    TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSlotSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offset;
    dim_pnt[1] = offset+height+1.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[3];
    pnt_types[0] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,1,
		       PRO_TK_DIM_LINE,dim_pnt,&dim_ids[1]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSlotSectBuild()",
		              status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offset + height;
    dim_pnt[1] = offset+height/2.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[2];
    entdim_ids[1] = ent_ids[3];
    pnt_types[0] = PRO_ENT_END;
    pnt_types[1] = PRO_ENT_END;

    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		       PRO_TK_DIM_PNT_PNT,dim_pnt,&dim_ids[2]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSlotSectBuild()",
		              status, status != PRO_TK_NO_ERROR);

/* Dim not germane to sketch - specifically to test delete func */
    dim_pnt[0] = offset+width/2;
    dim_pnt[1] = offset-1.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[4];
    entdim_ids[1] = ent_ids[5];
    pnt_types[0] = PRO_ENT_LEFT_TANGENT;
    pnt_types[1] = PRO_ENT_RIGHT_TANGENT;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
			PRO_TK_DIM_AOC_AOC_TAN_HORIZ,dim_pnt,&test_dim);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSlotSectBuild()",
		              status, status != PRO_TK_NO_ERROR);
    status = ProSecdimDelete(section,test_dim);
    TEST_CALL_REPORT("ProSecdimDelete()", "ProTestSlotSectBuild()",
		              status, status != PRO_TK_NO_ERROR);
/*--------------------------------------------------------------------*\
    Position the slot on the sketch plane if 3d section
\*--------------------------------------------------------------------*/
    if (p_sketch_refs != NULL)
    {
	entdim_ids[0] = ent_ids[0];
	status = ProUtilParallelLineEntityGet(section,ent_ids[0],
                                              &ent_ids[6],2,&par_idx);
        entdim_ids[1] = ent_ids[6+par_idx];
	pnt_types[0] = PRO_ENT_WHOLE;
	pnt_types[1] = PRO_ENT_WHOLE;
	dim_pnt[0] = (-offset/2.0);
	dim_pnt[1] = height/2.0;
	if (p_sketch_refs != NULL)
	{
	    ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }
	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		             PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[3]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSlotSectBuild()",
		      status, status != PRO_TK_NO_ERROR);

        if ((6+par_idx) == 6) oth_idx = 7;
	    else oth_idx = 6;
	entdim_ids[0] = ent_ids[oth_idx];
        entdim_ids[1] = ent_ids[4];
	pnt_types[0] = PRO_ENT_WHOLE;
	pnt_types[1] = PRO_ENT_CENTER;
	dim_pnt[0] = offset+width/2.0;
	dim_pnt[1] = -height/2.0;
	if (p_sketch_refs != NULL)
	{
	    ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }
        
	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		             PRO_TK_DIM_LINE_POINT,dim_pnt,&dim_ids[4]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSlotSectBuild()",
		      status, status != PRO_TK_NO_ERROR);

        status = ProSecdimValueSet(section,dim_ids[3],0.0);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSlotSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
         
        status = ProSecdimValueSet(section,dim_ids[4],offsets[oth_idx-6]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSlotSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
        status = ProSecdimValueSet(section,dim_ids[0],offsets[par_idx]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSlotSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    }
     
/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/

    status = ProTestSectionSolveRegen(section);
    if (status != PRO_TK_NO_ERROR)
	return(PRO_TK_GENERAL_ERROR);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestSplineSectBuild()
    PURPOSE  :	Create a spline section
\*====================================================================*/
ProError ProTestSplineSectBuild(
    ProSection section,
    ProType sec_type,
    ProTestSectionInfo *p_sketch_refs)
{
    ProError status;
    Pro2dClinedef cline;
    Pro2dSplinedef spline;
    Pro2dPnt spln_pnts[5];
    Pro2dPointdef point;
    int ent_ids[5];
    double diameter,offset;
    Pro2dPnt dim_pnt;
    int entdim_ids[2];
    int dim_ids[3];
    double edge1[2][3],edge2[2][3],cln[2][3];
    double sk_transf[4][4];
    double transform[4][4],offsets[2];
    double angle = 0.0;
    ProSectionPointType pnt_types[2];
    int par_idx,oth_idx; 
    int test_id;

    diameter = 100.0;

    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Spline diameter [%0(6.2)f]: ",&diameter);
    ProUtilDoubleGet(NULL,&diameter,&diameter);

    offset = diameter;
    offsets[0] = offsets[1] = offset;

    ProUtilSectEpsModify(section);

    if (p_sketch_refs != NULL)
    {
	status = ProUtilTwoOrthogEdgesOnPlaneGet(section,&ent_ids[3],
		                                 edge1,edge2);
        if ( status != PRO_TK_NO_ERROR )
	    return(PRO_TK_GENERAL_ERROR);

        ProUtilXyOffsetsGet(offsets);

        ProUtilSketchCsysGet(p_sketch_refs,sk_transf);
        ProUtilSectCsysTransformGet(p_sketch_refs,sk_transf,edge1,
                                edge2,transform,&angle);
    }

/*--------------------------------------------------------------------*\
    Define the Spline sketch    
\*--------------------------------------------------------------------*/

    cline.type = PRO_2D_CENTER_LINE;
    cline.end1[0] = 1.0;
    cline.end1[1] = 0.0;
    cline.end2[0] = 1.0;
    cline.end2[1] = 1.0;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,cline.end1,cline.end1);
	ProUtil2DPointTrans(transform,cline.end2,cline.end2);
    }
    cln[0][0] = cline.end1[0];
    cln[0][1] = cline.end1[1];
    cln[0][2] = 0.0;
    cln[1][0] = cline.end2[0];
    cln[1][1] = cline.end2[1];
    cln[1][2] = 0.0;
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&cline,&ent_ids[0]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSplineSectBuild()",
		           status, status != PRO_TK_NO_ERROR);
       
    point.type = PRO_2D_POINT;
    point.pnt[0] = offset;
    point.pnt[1] = diameter;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,point.pnt,point.pnt);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&point,&ent_ids[1]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    spline.type = PRO_2D_SPLINE;
    spline.tangency_type = PRO_2D_SPLINE_TAN_NONE;
    spline.n_points = 5;
    spln_pnts[0][0] = offset;
    spln_pnts[0][1] = diameter;
    spln_pnts[1][0] = offset + diameter/2.0;
    spln_pnts[1][1] = diameter/2.0;
    spln_pnts[2][0] = offset;
    spln_pnts[2][1] = 0.0;
    spln_pnts[3][0] = offset - diameter/2.0;
    spln_pnts[3][1] = diameter/2.0;
    spln_pnts[4][0] = offset;
    spln_pnts[4][1] = diameter;
    if (p_sketch_refs != NULL)
    {
	ProUtil2DPointTrans(transform,spln_pnts[0],spln_pnts[0]);
	ProUtil2DPointTrans(transform,spln_pnts[1],spln_pnts[1]);
	ProUtil2DPointTrans(transform,spln_pnts[2],spln_pnts[2]);
	ProUtil2DPointTrans(transform,spln_pnts[3],spln_pnts[3]);
	ProUtil2DPointTrans(transform,spln_pnts[4],spln_pnts[4]);
    }
    spline.point_arr = spln_pnts;
    spline.start_tang_angle = 0.0;
    spline.end_tang_angle = 0.0;
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&spline,&ent_ids[2]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

/* Entity not germane to sketch - specifically to test delete func */
    spline.type = PRO_2D_SPLINE;
    spline.tangency_type = PRO_2D_SPLINE_TAN_NONE;
    spline.n_points = 2;
    spln_pnts[0][0] = -offset;
    spln_pnts[0][1] = -diameter;
    spln_pnts[1][0] = -(offset + diameter/2.0);
    spln_pnts[1][1] = -(diameter/2.0);
    spline.point_arr = spln_pnts;
    spline.start_tang_angle = 0.0;
    spline.end_tang_angle = 0.0;
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&spline,&test_id);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    status = ProSectionEntityDelete(section,test_id);
    TEST_CALL_REPORT("ProSectionEntityDelete()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Dimension the spline
\*--------------------------------------------------------------------*/
    dim_pnt[0] = offset/2;
    dim_pnt[1] = diameter;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[0];
    entdim_ids[1] = ent_ids[2];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_START;

    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
			PRO_TK_DIM_LINE_POINT,dim_pnt,&dim_ids[0]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    status = ProSecdimValueSet(section,dim_ids[0],offsets[0]);
    TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Position the spline on the sketch plane if 3d section
\*--------------------------------------------------------------------*/
    if (p_sketch_refs != NULL)
    {
	entdim_ids[0] = ent_ids[0];
	status = ProUtilParallelLineEntityGet(section,ent_ids[0],
                                              &ent_ids[3],2,&par_idx);
        entdim_ids[1] = ent_ids[3+par_idx];
	pnt_types[0] = PRO_ENT_WHOLE;
	pnt_types[1] = PRO_ENT_WHOLE;
	dim_pnt[0] = (-offset/2.0);
	dim_pnt[1] = diameter/2.0;
        if (p_sketch_refs != NULL)
        {
           ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }
	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		          PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[1]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
         
        if ((3+par_idx) == 3) oth_idx = 4;
	    else oth_idx = 3;
	entdim_ids[1] = ent_ids[2];
        entdim_ids[0] = ent_ids[oth_idx];
	pnt_types[0] = PRO_ENT_WHOLE;
	pnt_types[1] = PRO_ENT_END;
	dim_pnt[1] = offset+diameter/2.0;
	dim_pnt[0] = diameter/2.0;
        if (p_sketch_refs != NULL)
        {
           ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }

	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		    PRO_TK_DIM_LINE_POINT,dim_pnt,&dim_ids[2]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestSplineSectBuild()",
		      status, status != PRO_TK_NO_ERROR);

        status = ProSecdimValueSet(section,dim_ids[1],0.0);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
        status = ProSecdimValueSet(section,dim_ids[2],offsets[oth_idx-3]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
        status = ProSecdimValueSet(section,dim_ids[0],offsets[par_idx]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestSplineSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    }
     
/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/

    status = ProTestSectionSolveRegen(section);
    if (status != PRO_TK_NO_ERROR)
	return(PRO_TK_GENERAL_ERROR);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProTestWidgetSectBuild()
    PURPOSE  :	Create a Widget section
\*====================================================================*/
ProError ProTestWidgetSectBuild(
    ProSection section,
    ProType sec_type,
    ProTestSectionInfo *p_sketch_refs)
{
    ProError status;
    Pro2dLinedef line;
    Pro2dClinedef cline;
    Pro2dArcdef arc;
    int ent_ids[8];
    double width,height,offset,radius;
    Pro2dPnt dim_pnt;
    int entdim_ids[2];
    int dim_ids[7];
    double edge1[2][3],edge2[2][3],cln[2][3];
    double sk_transf[4][4];
    double transform[4][4],offsets[2];
    double angle = 0.0;
    ProSectionPointType pnt_types[2];
    int par_idx,oth_idx; 

    width = 125.0;
    height = 100.0;

    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Widget width [%0(6.2)f]: ",&width);
    ProUtilDoubleGet(NULL,&width,&width);
    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Widget height [%0(6.2)f]: ",&height);
    ProUtilDoubleGet(NULL,&height,&height);

    radius = height/10.0;
    offset = height;
    offsets[0] = offsets[1] = offset;

    ProUtilSectEpsModify(section);

    if (p_sketch_refs != NULL)
    {
	status = ProUtilTwoOrthogEdgesOnPlaneGet(section,&ent_ids[6],
		                                 edge1,edge2);
        if ( status != PRO_TK_NO_ERROR )
	    return(PRO_TK_GENERAL_ERROR);

        ProUtilXyOffsetsGet(offsets);

        ProUtilSketchCsysGet(p_sketch_refs,sk_transf);
        ProUtilSectCsysTransformGet(p_sketch_refs,sk_transf,edge1,
                                   edge2,transform,&angle);
    }

/*--------------------------------------------------------------------*\
    Define the sketch of the widget
\*--------------------------------------------------------------------*/

    cline.type = PRO_2D_CENTER_LINE;
    cline.end1[0] = 0.0;
    cline.end1[1] = 0.0;
    cline.end2[0] = 0.0;
    cline.end2[1] = 1.0;

    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,cline.end1,cline.end1);
        ProUtil2DPointTrans(transform,cline.end2,cline.end2);
    }
    cln[0][0] = cline.end1[0];
    cln[0][1] = cline.end1[1];
    cln[0][2] = 0.0;
    cln[1][0] = cline.end2[0];
    cln[1][1] = cline.end2[1];
    cln[1][2] = 0.0;
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&cline,&ent_ids[0]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestWidgetSectBuild()",
		           status, status != PRO_TK_NO_ERROR);
       
    line.type = PRO_2D_LINE;
    line.end1[0] = offset + width;
    line.end1[1] = offset;
    line.end2[0] = offset;
    line.end2[1] = offset;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[1]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offset;
    line.end1[1] = offset;
    line.end2[0] = offset;
    line.end2[1] = offset + height - radius;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[2]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    arc.type = PRO_2D_ARC;
    arc.center[0] = offset+radius;
    arc.center[1] = offset + height - radius;
    arc.start_angle = PI/2.0;
    arc.end_angle = PI;
    arc.radius = radius;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,arc.center,arc.center);
	arc.start_angle += angle;
	arc.end_angle += angle;
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&arc,&ent_ids[3]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offset+radius;
    line.end1[1] = offset+height;
    line.end2[0] = offset + width/2.0;
    line.end2[1] = offset+height;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[4]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offset+width/2.0;
    line.end1[1] = offset+height;
    line.end2[0] = offset + width;
    line.end2[1] = offset;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[5]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Dimension the widget
\*--------------------------------------------------------------------*/
    dim_pnt[0] = offset/2;
    dim_pnt[1] = offset;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[0];
    entdim_ids[1] = ent_ids[2];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
			PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[0]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    status = ProSecdimValueSet(section,dim_ids[0],offsets[0]);
    TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offset/2.0;
    dim_pnt[1] = offset+height+1.0;;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[3];
    pnt_types[0] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,1,
			PRO_TK_DIM_RAD,dim_pnt,&dim_ids[1]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestWidgetSectBuild()",
		              status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = (offset+width) + 1.0;
    dim_pnt[1] = offset+(height)/2;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[1];
    entdim_ids[1] = ent_ids[4];
    pnt_types[0] = PRO_ENT_START;
    pnt_types[1] = PRO_ENT_END;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
			PRO_TK_DIM_PNT_PNT_VERT,dim_pnt,&dim_ids[2]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestWidgetSectBuild()",
		           status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offset+width/2.0;
    dim_pnt[1] = offset+height/2;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[1];
    entdim_ids[1] = ent_ids[5];
    pnt_types[0] = PRO_ENT_WHOLE;
    pnt_types[1] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
			PRO_TK_DIM_LINES_ANGLE,dim_pnt,&dim_ids[3]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestWidgetSectBuild()",
		           status, status != PRO_TK_NO_ERROR);

    dim_pnt[0] = offset+width/2.0;
    dim_pnt[1] = offset-1.0;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
    }
    entdim_ids[0] = ent_ids[1];
    pnt_types[0] = PRO_ENT_WHOLE;
    status = ProSecdimCreate(section,entdim_ids,pnt_types,1,
			PRO_TK_DIM_LINE,dim_pnt,&dim_ids[4]);
    TEST_CALL_REPORT("ProSecdimCreate()", "ProTestWidgetSectBuild()",
		           status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Position the widget on the sketch plane if 3d section
\*--------------------------------------------------------------------*/
    if (p_sketch_refs != NULL)
    {
	entdim_ids[0] = ent_ids[0];
	status = ProUtilParallelLineEntityGet(section,ent_ids[0],
                                              &ent_ids[6],2,&par_idx);
        entdim_ids[1] = ent_ids[6+par_idx];
	pnt_types[0] = PRO_ENT_WHOLE;
	pnt_types[1] = PRO_ENT_WHOLE;
	dim_pnt[0] = (-offset/2.0);
	dim_pnt[1] = height/2.0;
        if (p_sketch_refs != NULL)
        {
           ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }
	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		    PRO_TK_DIM_LINE_LINE,dim_pnt,&dim_ids[5]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestWidgetSectBuild()",
		      status, status != PRO_TK_NO_ERROR);
         
	entdim_ids[1] = ent_ids[1];
        if ((6+par_idx) == 6) oth_idx = 7;
	    else oth_idx = 6;
        entdim_ids[0] = ent_ids[oth_idx];
	pnt_types[1] = PRO_ENT_START;
	pnt_types[0] = PRO_ENT_WHOLE;
	dim_pnt[1] = offset+width/2.0;
	dim_pnt[0] = -height/2.0;
        if (p_sketch_refs != NULL)
        {
           ProUtil2DPointTrans(transform,dim_pnt,dim_pnt);
        }

	status = ProSecdimCreate(section,entdim_ids,pnt_types,2,
		    PRO_TK_DIM_LINE_POINT,dim_pnt,&dim_ids[6]);
	TEST_CALL_REPORT("ProSecdimCreate()", "ProTestWidgetSectBuild()",
		      status, status != PRO_TK_NO_ERROR);

        status = ProSecdimValueSet(section,dim_ids[5],0.0);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
        status = ProSecdimValueSet(section,dim_ids[6],offsets[oth_idx-6]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
        status = ProSecdimValueSet(section,dim_ids[0],offsets[par_idx]);
        TEST_CALL_REPORT("ProSecdimValueSet()", "ProTestWidgetSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
    }
     
/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/

    status = ProTestSectionSolveRegen(section);
    if (status != PRO_TK_NO_ERROR)
	return(PRO_TK_GENERAL_ERROR);

    return(PRO_TK_NO_ERROR);
}


/*====================================================================*\
    FUNCTION :	ProTestAngleSectBuild()
    PURPOSE  :	Create a Widget section
\*====================================================================*/
ProError ProTestAngleSectBuild(
    ProSection section,
    ProType sec_type,
    ProTestSectionInfo *p_sketch_refs)
{
    ProError status;
    Pro2dLinedef line;
    Pro2dCoordSysdef coord;
    int ent_ids[8];
    double width,height,offset,radius;
    double edge1[2][3],edge2[2][3];
    double sk_transf[4][4];
    double transform[4][4],offsets[2];
    double angle = 0.0;
    ProLine w_message;
    ProWSecerror sec_errors;

    width = 125.0;
    height = 100.0;

    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Box width [%0(6.2)f]: ",&width);
    ProUtilDoubleGet(NULL,&width,&width);
    ProUtilMsgPrint((char*)"sec",(char*)"TEST Enter Box height [%0(6.2)f]: ",&height);
    ProUtilDoubleGet(NULL,&height,&height);

    radius = height/10.0;
    offset = height;
    offsets[0] = offsets[1] = offset;

    ProUtilSectEpsModify(section);

    if (p_sketch_refs != NULL)
    {
	status = ProUtilTwoOrthogEdgesOnPlaneGet(section,&ent_ids[6],
		                                 edge1,edge2);
        if ( status != PRO_TK_NO_ERROR )
	    return(PRO_TK_GENERAL_ERROR);

        ProUtilXyOffsetsGet(offsets);

        ProUtilSketchCsysGet(p_sketch_refs,sk_transf);
        ProUtilSectCsysTransformGet(p_sketch_refs,sk_transf,edge1,
                                   edge2,transform,&angle);
	if ((fabs(transform[0][0])>1e-3) ^ 
	    (!(fabs(edge1[0][0]-edge1[1][0])>1e-3)))
	{
	    /* First edge x */
	    offset = offsets[0];offsets[0]=offsets[1];offsets[1]=offset;
	}
    }

/*--------------------------------------------------------------------*\
    Define the sketch of the widget
\*--------------------------------------------------------------------*/
    coord.type = PRO_2D_COORD_SYS;
    coord.pnt[0] = 0;
    coord.pnt[1] = 0;
    if (p_sketch_refs != NULL)
	ProUtil2DPointTrans(transform, coord.pnt, coord.pnt);
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&coord, &ent_ids[0]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestDiamSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0] + width;
    line.end1[1] = offsets[1];
    line.end2[0] = offsets[0];
    line.end2[1] = offsets[1];
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[1]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestAngleSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0];
    line.end1[1] = offsets[1];
    line.end2[0] = offsets[0];
    line.end2[1] = offsets[1] + height;
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }
    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[2]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestAngleSectBuild()",
		          status, status != PRO_TK_NO_ERROR);
              
    line.type = PRO_2D_LINE;
    line.end1[0] = offsets[0];
    line.end1[1] = offsets[1] + height;
    line.end2[0] = offsets[0] + width;
    line.end2[1] = offsets[1];
    if (p_sketch_refs != NULL)
    {
        ProUtil2DPointTrans(transform,line.end1,line.end1);
        ProUtil2DPointTrans(transform,line.end2,line.end2);
    }

    status = ProSectionEntityAdd(section,(Pro2dEntdef *)&line,&ent_ids[3]);
    TEST_CALL_REPORT("ProSectionEntityAdd()", "ProTestAngleSectBuild()",
		          status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
    Solve and regenerate the section
\*--------------------------------------------------------------------*/
    ProTestSecerrAlloc(&sec_errors);
    ProStringToWstring(w_message, (char*)"ProSectionSolveRigid() errors.");
    status = ProSecerrorAdd(&sec_errors, 0, w_message, 0, -1, 0);
    TEST_CALL_REPORT("ProSecerrorAdd()", 
		"ProTestDiamSectBuild()", status, status != PRO_TK_NO_ERROR);

    status = ProSectionSolveRigid(section, &sec_errors);
    TEST_CALL_REPORT("ProSectionSolveRigid()", 
	    "ProTestAngleSectBuild()", status, status != PRO_TK_NO_ERROR);
    if (status != PRO_TK_NO_ERROR)
    {
	ProTestSecerrShow(sec_errors);
	return(PRO_TK_GENERAL_ERROR);
    }
    ProTestSecerrFree(&sec_errors);

    ProTestSecerrAlloc(&sec_errors);
    status = ProSectionRegenerate(section,&sec_errors);
    TEST_CALL_REPORT("ProSectionRegenerate()", 
	    "ProTestAngleSectBuild()", status, status != PRO_TK_NO_ERROR);
    if (status != PRO_TK_NO_ERROR)
    {
	ProTestSecerrShow(sec_errors);
	return(PRO_TK_GENERAL_ERROR);
    }
    ProTestSecerrFree(&sec_errors);
    return(PRO_TK_NO_ERROR);
}
