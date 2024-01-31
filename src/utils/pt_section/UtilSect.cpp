

/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProObjects.h>
#include <ProSecdim.h>
#include <ProSection.h>
#include <ProMdl.h>
#include <ProMenu.h>
#include <ProEdge.h>
#include <ProGeomitem.h>
#include <ProFeature.h>
#include <ProSelection.h>
#include <ProSurface.h>
#include <ProArray.h>
#include <ProUtil.h>

/*--------------------------------------------------------------------*\
C System includes
\*--------------------------------------------------------------------*/
#include <math.h>

/*--------------------------------------------------------------------*\
Application includes
\*--------------------------------------------------------------------*/
#include "UtilMenu.h"
#include "UtilMessage.h"
#include "TestSect.h"
#include "TestSelect.h"
#include "UtilMessage.h"
#include "TestError.h"
#include "UtilMath.h"
#include "UtilMatrix.h"
#include "UtilCollect.h"
#include "UtilTree.h"
#include "ProDtmCrv.h"
#include "TestFeattype.h"
#include "UtilString.h"
/*--------------------------------------------------------------------*\
Application global/external data
\*--------------------------------------------------------------------*/
#define mnUSE_EDGE          0
#define mnUSE_EDGE_CHAIN    1
#define mnUSE_EDGE_LOOP     2
#define mnUSE_CURVE_CHAIN   3
#define mnUSE_CURVE_LOOP    4
#define mnUSE_DONE          -1

/*
Used functions
*/        
int ProUtilSecEntityUseEdge (ProSection section);
int ProUtilSecEntityUseEdgeChain (ProSection section);
int ProUtilSecEntityUseEdgeLoop (ProSection section);
int ProUtilSecEntityUseCurveLoop (ProSection section);
int ProUtilSecEntityUseCurveChain (ProSection section);

ProError AddProjectedEdgeSectionEntities(ProSolid solid, ProSection section, ProEdge *edges);
ProError CreateSketchFeatures(SketchInfo *sketch_info, ProFeature *feature);
ProError CreateSketchReferences (SketchInfo *sketch_info, ProSelection **proj_refs_i);


/*====================================================================*\
    FUNCTION :	ProUtilSectionRetrieve
    PURPOSE  :	Retrieves a section from disk, memory or from feature
\*====================================================================*/
ProError ProUtilSectionRetrieve(ProSection *section)
{
    int by_name = 0;
    ProError status;
    int id;

    status = ProMenuFileRegister((char*)"TK SECT RETR",(char*)"tk_secretr.mnu", &id);
    TEST_CALL_REPORT ("ProMenuFileRegister", "ProUtilSectionRetrieve",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TK SECT RETR",(char*)"-By Name",(ProMenubuttonAction)ProUtilAssign,
                      &by_name,1);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProUtilSectionRetrieve",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TK SECT RETR",(char*)"-From Feature",
        (ProMenubuttonAction)ProUtilAssign, &by_name,0);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProUtilSectionRetrieve",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenubuttonActionSet((char*)"TK SECT RETR",(char*)"TK SECT RETR",
         (ProMenubuttonAction)ProMenuDelete, NULL,0);
    TEST_CALL_REPORT ("ProMenubuttonActionSet", "ProUtilSectionRetrieve",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenuCreate(PROMENUTYPE_MAIN,(char*)"TK SECT RETR", &id);
    TEST_CALL_REPORT ("ProMenuCreate", "ProUtilSectionRetrieve",
        status, status != PRO_TK_NO_ERROR);
    status = ProMenuProcess((char*)"TK SECT RETR", &id);
    TEST_CALL_REPORT ("ProMenuProcess", "ProUtilSectionRetrieve",
        status, status != PRO_TK_NO_ERROR);

    if (by_name)
    {
	status = ProUtilSectionRetr(section);
    }
    else
    {
	status = ProUtilSectionFromFeatGet(section);
    }

    return(status);
}
/*====================================================================*\
    FUNCTION :	ProUtilSectionFromFeatGet
    PURPOSE  :	Gets a section from a feature
\*====================================================================*/
ProError ProUtilSectionFromFeatGet(ProSection *section)
{
    ProSelection *p_sels;
    int n_sels;
    ProFeature feat;
    int n_sections;
    int range[2],n;
    wchar_t wname[PRO_NAME_SIZE];
    ProError err;

    *section = NULL;

    ProUtilMsgPrint((char*)"gen",(char*)"TEST %0s",
		    (char*)"Select a feature containing a sketch.");
    err = ProSelect((char*)"feature", 1,
		    NULL,NULL,NULL,NULL, &p_sels, &n_sels);
    TEST_CALL_REPORT("ProSelect", "ProUtilSectionFromFeatGet",
		      err , err != PRO_TK_NO_ERROR);

    if (err != PRO_TK_NO_ERROR || n_sels != 1)
	return(PRO_TK_USER_ABORT);

    err = ProSelectionModelitemGet(p_sels[0],&feat);
    TEST_CALL_REPORT("ProSelectionModelitemGet",
		     "ProUtilSectionFromFeatGet",
		      err , err != PRO_TK_NO_ERROR);

    err = ProFeatureNumSectionsGet(&feat,&n_sections);
    TEST_CALL_REPORT("ProFeatureNumSectionsGet",
		     "ProUtilSectionFromFeatGet",
		      err , err != PRO_TK_NO_ERROR);

    if (n_sections < 1)
    {
	ProUtilMsgPrint((char*)"gen",(char*)"TEST %0s",
			(char*)"Feature does not contain any sections.");
        return(PRO_TK_GENERAL_ERROR);
    }

    range[0] = 0; range[1] = n_sections-1;
    n = 0;
    if ( range[0] != range[1] )
    {
	ProUtilMsgPrint((char*)"gen",(char*)"TEST %0s",
			(char*)"Specify section number to retrieve.");
        ProUtilIntGet(range,&range[0],&n);
    }

    err = ProFeatureSectionCopy(&feat,n,section);
    TEST_CALL_REPORT("ProFeatureSectionCopy",
		     "ProUtilSectionFromFeatGet",
		      err , err != PRO_TK_NO_ERROR);
    ProUtilMsgPrint((char*)"gen",(char*)"TEST %0s",
		(char*)"Enter section name [NONE]: ");
    if (ProUtilStringGet(wname,NULL, PRO_NAME_SIZE-1))
    {
	err = ProSectionNameSet(*section,wname);
	/*  The function returns -2 for feature section (3D sec)
	    but successfully sets the name. ProSection3dTo2d (doesn't 
	    exist yet))needs to be used before calling to ProSectionNameSet().
        TEST_CALL_REPORT("ProSectionNameSet",
		         "ProUtilSectionFromFeatGet",
		         err , err != PRO_TK_NO_ERROR);
	*/
    }

     return(err);
}

/*====================================================================*\
    FUNCTION :	ProUtilSectionPointTypeStr
    PURPOSE  :	Dumps section entity info to a file
\*====================================================================*/
char* ProUtilSectionPointTypeStr(
    ProSectionPointType type)
{
    switch(type)
    {
	case PRO_ENT_WHOLE:
	     return((char*)"PRO_ENT_WHOLE");
	case PRO_ENT_START:
	     return((char*)"PRO_ENT_START");
	case PRO_ENT_END:
	     return((char*)"PRO_ENT_END");
	case PRO_ENT_CENTER:
	     return((char*)"PRO_ENT_CENTER");
	case PRO_ENT_LEFT_TANGENT:
	     return((char*)"PRO_ENT_LEFT_TANGENT");
	case PRO_ENT_RIGHT_TANGENT:
	     return((char*)"PRO_ENT_RIGHT_TANGENT");
	case PRO_ENT_TOP_TANGENT:
	     return((char*)"PRO_ENT_TOP_TANGENT");
	case PRO_ENT_BOTTOM_TANGENT:
	     return((char*)"PRO_ENT_BOTTOM_TANGENT");
    }
    return((char*)"UNKNOWN");
}



/*====================================================================*\
    FUNCTION :	ProUtilSectionInfoGet
    PURPOSE  :	Dumps section entity info to a file

    WARNING:	Do not call to this function during feature redefinition. 
		The function calls to ProSectionDimensionIdsGet which 
		should be used only for completely redefined feature.
\*====================================================================*/
ProError ProUtilSectionInfoGet(FILE *fp,ProSection section)
{
    ProIntlist ent_ids,dim_ids,con_ids;
    int n_ids,n_dimids, n_conids, i,j;
    ProError status;
    Pro2dEntdef *p_ent;
    Pro2dPointdef *p_pnt;
    Pro2dLinedef *p_line;
    Pro2dArcdef *p_arc;
    Pro2dCircledef *p_circle;
    Pro2dSplinedef *p_spline;
    Pro2dTextdef *p_text;
    Pro2dBlendVertexdef *p_bvertex;
    Pro2dEllipsedef *p_ellipse;
    Pro2dConicdef *p_conic;
    ProVector point,deriv,deriv2;
    double value;
    ProSecdimType dim_type;
    char d_type[50];
    char *p_type, *c_type, *c_status;
    ProSectionPointType *p_types;
    int *ref_ids;
    int n_ref_ids,k;
    ProBoolean is_projection;
    ProIntlist secdim_ids,mdldim_ids;
    int n_mdldim_ids,l;
    ProConstraintType con_type;
    ProConstraintStatus con_status;
    char c_string [PRO_COMMENT_SIZE];
    char c_font_name [PRO_COMMENT_SIZE];
    ProWSecerror sec_errors; 

    ProTestSecerrAlloc(&sec_errors);

    status = ProSectionRegenerate( section, &sec_errors);
    TEST_CALL_REPORT("ProSectionRegenerate()",
		     "ProUtilSectionInfoGet()",
		     status, status != PRO_TK_NO_ERROR);

    ProTestSecerrFree(&sec_errors);

    ProTKFprintf(fp,"Section Entity and Dimension Info\n");
    ProTKFprintf(fp,"---------------------------------\n\n");
 
    status = ProSectionEntityIdsGet(section,&ent_ids,&n_ids);
    TEST_CALL_REPORT("ProSectionEntityIdsGet()", "ProUtilSectionInfoGet()",
		     status, status != PRO_TK_NO_ERROR);

    if ( status != PRO_TK_NO_ERROR ) 
      {
	ProTKFprintf(fp,"ProSectionEntityIdsGet returned an error = %d \n", status );
	ProTKFprintf(fp,"Not able to access section information \n" );
	ProTKFprintf(fp,"---------------------------------\n\n");

	return status; 
      }

    for (i=0; i<n_ids; i++)
    {
	status = ProSectionEntityGet(section,ent_ids[i],&p_ent);
        TEST_CALL_REPORT("ProSectionEntityGet()",
			 "ProUtilSectionInfoGet()",
		         status, status != PRO_TK_NO_ERROR);

	ProTKFprintf(fp,"Entity Id: [%d]\n",ent_ids[i]);
        if (status != PRO_TK_NO_ERROR) continue;

        status = ProSectionEntityIsProjection(section,ent_ids[i],
					       &is_projection);
        TEST_CALL_REPORT("ProSectionEntityIsProjection()",
			 "ProUtilSectionInfoGet()",
		         status, status != PRO_TK_NO_ERROR);
        if (status != PRO_TK_NO_ERROR) continue;

        if (is_projection == PRO_B_TRUE)
	     ProTKFprintf(fp,"\tEntity Is Projection\n");

        switch(p_ent->type)
	{
	    case PRO_2D_POINT:
	    case PRO_2D_COORD_SYS:
		if (p_ent->type == PRO_2D_POINT)
		    ProTKFprintf(fp,"\tEntity Type: PRO_2D_POINT\n");
                else
		    ProTKFprintf(fp,"\tEntity Type: PRO_2D_COORD_SYS\n");
                p_pnt = (Pro2dPointdef *)p_ent;
                ProTKFprintf(fp,"\tLocation: [ %4.2lf, %4.2lf ]\n",
			p_pnt->pnt[0],p_pnt->pnt[1]);;
		break;
            case PRO_2D_LINE:
	    case PRO_2D_CENTER_LINE:
		if (p_ent->type == PRO_2D_LINE)
		    ProTKFprintf(fp,"\tEntity Type: PRO_2D_LINE\n");
                else
		    ProTKFprintf(fp,"\tEntity Type: PRO_2D_CENTER_LINE\n");
                p_line = (Pro2dLinedef *)p_ent;
                ProTKFprintf(fp,"\tEnd1: [ %4.2lf, %4.2lf ]\n",p_line->end1[0],
						  p_line->end1[1]);
                ProTKFprintf(fp,"\tEnd2: [ %4.2lf, %4.2lf ]\n",p_line->end2[0],
						  p_line->end2[1]);
                break;
            case PRO_2D_ARC:
		ProTKFprintf(fp,"\tEntity Type: PRO_2D_ARC\n");
		p_arc = (Pro2dArcdef *)p_ent;
		ProTKFprintf(fp,"\tCenter: [ %4.2lf, %4.2lf ]\n",p_arc->center[0],
						         p_arc->center[1]);
		ProTKFprintf(fp,"\tRadius: [ %4.2lf ]\n",p_arc->radius);
		ProTKFprintf(fp,"\tStart Angle: [ %4.2lf ]\n",
				      180/PI*p_arc->start_angle);
		ProTKFprintf(fp,"\tEnd Angle: [ %4.2lf ]\n",
				      180/PI*p_arc->end_angle);
		break;
	    case PRO_2D_CIRCLE:
	    case PRO_2D_CONSTR_CIRCLE:
	      if (p_ent -> type == PRO_2D_CIRCLE)
		ProTKFprintf(fp,"\tEntity Type: PRO_2D_CIRCLE\n");
	      else
		ProTKFprintf(fp,"\tEntity Type: PRO_2D_CONSTR_CIRCLE\n");
		p_circle = (Pro2dCircledef *)p_ent;
		ProTKFprintf(fp,"\tCenter: [ %4.2lf ]\n",p_circle->center[0],
						  p_circle->center[1]);
		ProTKFprintf(fp,"\tRadius: [ %4.2lf ]\n",p_circle->radius);
		break;
            case PRO_2D_SPLINE:
	        ProTKFprintf(fp,"\tEntity Type: PRO_2D_SPLINE\n");
		p_spline = (Pro2dSplinedef *)p_ent;
		ProTKFprintf(fp,"\tTangency Type: [%d]\n",
			p_spline->tangency_type);
                ProTKFprintf(fp,"\tStart Tangent Angle: [ %4.2lf ]\n",
			180/PI*p_spline->start_tang_angle);
                ProTKFprintf(fp,"\tEnd Tangent Angle: [ %4.2lf ]\n",
			180/PI*p_spline->end_tang_angle);
                ProTKFprintf(fp,"\tNum Points: [%d]\n",p_spline->n_points);
		for (j=0; j<(int)p_spline->n_points; j++)
		    ProTKFprintf(fp,"\tPoint%d: [ %4.2lf, %4.2lf ]\n",j,
			    p_spline->point_arr[j][0],
			    p_spline->point_arr[j][1]);
                break;

	    case PRO_2D_TEXT:
	      ProTKFprintf (fp, "\tEntity Type: PRO_2D_TEXT\n");
	      p_text = (Pro2dTextdef *)p_ent;
	      ProTKFprintf (fp, "\tFirst corner: [ %4.2lf , %4.2lf ]\n",
		       p_text->first_corner[0], p_text->first_corner[1]);
	      ProTKFprintf (fp, "\tSecond corner: [ %4.2lf , %4.2lf ]\n",
		       p_text->second_corner[0], p_text->second_corner[1]);
	      ProWstringToString (c_string, p_text->string);
	      ProTKFprintf (fp, "\tText: %s\n", c_string);
	      ProWstringToString (c_font_name, p_text->font_name);
	      ProTKFprintf (fp, "\tFont name: %s\n", c_font_name);
	      break;
	      
	    case PRO_2D_BLEND_VERTEX:
	      ProTKFprintf (fp, "\tEntity Type: PRO_2D_BLEND_VERTEX\n");
	      p_bvertex = (Pro2dBlendVertexdef *)p_ent;
	      ProTKFprintf (fp, "\tPoint: [ %4.2lf , %4.2lf ]\n",
		       p_bvertex->pnt[0], p_bvertex->pnt[1]);
	      ProTKFprintf (fp, "\tDepth level: %d\n",
		       p_bvertex->depth_level);
	      break;

	    case PRO_2D_ELLIPSE:
	      ProTKFprintf (fp, "\tEntity Type: PRO_2D_ELLIPSE\n");
	      p_ellipse = (Pro2dEllipsedef *)p_ent;
	      ProTKFprintf (fp, "\tOrigin: [ %4.2lf , %4.2lf ]\n",
		       p_ellipse->origin[0], p_ellipse->origin [1]);
	      ProTKFprintf (fp, "\tX radius: %4.2lf\n", p_ellipse->x_radius);
	      ProTKFprintf (fp, "\tY radius: %4.2lf\n", p_ellipse->y_radius);	
	      break;

	    case PRO_2D_CONIC:
	      ProTKFprintf (fp, "\tEntity Type: PRO_2D_CONIC\n");
	      p_conic = (Pro2dConicdef *)p_ent;
	      ProTKFprintf (fp, "\tFirst end point: [ %4.2lf , %4.2lf ]\n",
		       p_conic->first_end_point[0], p_conic->first_end_point[1]);
	      ProTKFprintf (fp, "\tSecond end point: [ %4.2lf , %4.2lf ]\n",
		       p_conic->second_end_point[0], p_conic->second_end_point[1]);
	      ProTKFprintf (fp, "\tShoulder point: [ %4.2lf , %4.2lf ]\n",
		       p_conic->shoulder_point[0], p_conic->shoulder_point[1]);
	      ProTKFprintf (fp, "\tParameter: %4.2lf\n", p_conic->parameter);
	      
	      break;

            default:
		ProTKFprintf(fp,"\tEntity Type: UNKNOWN\n");
		break;
         }

         if (p_ent->type != PRO_2D_POINT &&
	     p_ent->type != PRO_2D_COORD_SYS )
         {
             ProTKFprintf(fp,"\tPoints Evaluated on Entity\n");
             for (j=0; j<=4; j++)
	     {
	        status = ProSectionEntityEval(section,ent_ids[i],j*.25,
					  point,deriv,deriv2);
                TEST_CALL_REPORT("ProSectionEntityEval()",
			     "ProUtilSectionInfoGet()",
		              status, status != PRO_TK_NO_ERROR);
                ProTKFprintf(fp,"\t\tParameter: [%4.2lf]\n",j*.25);
	        ProTKFprintf(fp,"\t\t\tPoint: [ %4.2lf, %4.2lf ]\n",
				     point[0],point[1]);
	        ProTKFprintf(fp,"\t\t\tFirst Derivative: [ %4.2lf, %4.2lf ]\n",
				     deriv[0],deriv[1]);
	        ProTKFprintf(fp,"\t\t\tSecond Derivative: [ %4.2lf, %4.2lf ]\n",
				     deriv2[0],deriv2[1]);
             }
         }

	 ProTKFprintf(fp,"\n");
    }

    status = ProArrayFree((ProArray*)&ent_ids);
    TEST_CALL_REPORT("ProArrayFree()",
	    "ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);

/*------------------------------------------------*\
    Section dimensions
\*------------------------------------------------*/

    status = ProSecdimIdsGet(section,&dim_ids,&n_dimids);
    TEST_CALL_REPORT("ProSecdimIdsGet()", "ProUtilSectionInfoGet()",
		     status, status != PRO_TK_NO_ERROR);
    n_mdldim_ids = 0;
    status = ProSectionDimensionIdsGet(section,&secdim_ids,
				       &mdldim_ids, &n_mdldim_ids);
    TEST_CALL_REPORT("ProSectionDimensionIdsGet()",
		     "ProUtilSectionInfoGet()",
		     status, (status != PRO_TK_NO_ERROR) && 
                     (status != PRO_TK_NOT_EXIST));
    if( status != PRO_TK_NO_ERROR )
	n_mdldim_ids = 0;
    ProTKFprintf(fp,"\n");
    for (j=0; j<n_dimids; j++)
    {
	ProTKFprintf(fp,"\tSecDim Id: [%d]\n",dim_ids[j]);

        for (l=0; l<n_mdldim_ids; l++)
	    if (dim_ids[j] == secdim_ids[l])
	    {
		ProTKFprintf(fp,"\tDimension Id: [%d]\n",mdldim_ids[l]);
		break;
            }

        status = ProSecdimValueGet(section,dim_ids[j],&value);
        TEST_CALL_REPORT("ProSecdimValueGet()",
			 "ProUtilSectionInfoGet()",
		         status, 
                         status != PRO_TK_NO_ERROR);

        if (status != PRO_TK_NO_ERROR) continue;
	ProTKFprintf(fp,"\t\tValue: [%4.2lf]\n",value);

	status = ProSecdimTypeGet(section,dim_ids[j],&dim_type);
        TEST_CALL_REPORT("ProSecdimTypeGet()",
			 "ProUtilSectionInfoGet()",
		         status, status != PRO_TK_NO_ERROR);
        if (status != PRO_TK_NO_ERROR) continue;

        switch(dim_type)
	{
	    case PRO_TK_DIM_LINE:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_LINE");
		 break;
            case PRO_TK_DIM_LINE_POINT:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_LINE_POINT");
		 break;
            case PRO_TK_DIM_RAD:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_RAD");
		 break;
            case PRO_TK_DIM_DIA:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_DIA");
		 break;
            case PRO_TK_DIM_LINE_LINE:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_LINE_LINE");
		 break;
            case PRO_TK_DIM_PNT_PNT:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_PNT_PNT");
		 break;
            case PRO_TK_DIM_PNT_PNT_HORIZ:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_PNT_PNT_HORIZ");
		 break;
            case PRO_TK_DIM_PNT_PNT_VERT:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_PNT_PNT_VERT");
		 break;
            case PRO_TK_DIM_AOC_AOC_TAN_HORIZ:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_AOC_AOC_TAN_HORIZ");
		 break;
            case PRO_TK_DIM_AOC_AOC_TAN_VERT:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_AOC_AOC_TAN_VERT");
		 break;
            case PRO_TK_DIM_ARC_ANGLE:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_ARC_ANGLE");
		 break;
            case PRO_TK_DIM_LINES_ANGLE:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_LINES_ANGLE");
		 break;
            case PRO_TK_DIM_LINE_AOC:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_LINE_AOC");
		 break;
            case PRO_TK_DIM_LINE_CURVE_ANGLE:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_LINE_CURVE_ANGLE");
		 break;
            case PRO_TK_DIM_CONIC_PARAM:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_CONIC_PARAM");
		 break;
            case PRO_TK_DIM_NONE:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_NONE");
		 break;
            default:
		 ProUtilstrcpy(d_type,"PRO_TK_DIM_TYPE_UNKNOWN");
		 break;
        }
	ProTKFprintf(fp,"\t\tType: %s\n",d_type);

        status = ProSecdimReferencesGet(section,dim_ids[j],&ref_ids,
					&p_types,&n_ref_ids);
        TEST_CALL_REPORT("ProSecdimReferencesGet()",
			 "ProUtilSectionInfoGet()",
		         status, status != PRO_TK_NO_ERROR);
        if (status != PRO_TK_NO_ERROR) continue;

        ProTKFprintf(fp,"\t\tDimension References:\n");
        for (k=0; k<n_ref_ids; k++)
	{
	    p_type = ProUtilSectionPointTypeStr(p_types[k]);
            ProTKFprintf(fp,"\t\t\tRef%d: Entity Id [%d], Point Type %s\n",
		    k,ref_ids[k],p_type);
        }

	status = ProArrayFree((ProArray*)&p_types);
        TEST_CALL_REPORT("ProArrayFree()",
		"ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);
	status = ProArrayFree((ProArray*)&ref_ids);
        TEST_CALL_REPORT("ProArrayFree()",
		"ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);
	ProTKFprintf(fp,"\n");
    }

    status = ProArrayFree((ProArray*)&dim_ids);
    TEST_CALL_REPORT("ProArrayFree()",
	    "ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);

/*------------------------------------------------*\
    Section constarins
\*------------------------------------------------*/
#if 1
    status = ProSectionConstraintsIdsGet(section, &con_ids,&n_conids);
    TEST_CALL_REPORT("ProSectionConstraintsIdsGet()",
		"ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);

    ProTKFprintf(fp,"\n");
    for (j=0; j<n_conids; j++)
    {
	ProTKFprintf(fp,"\tConstraint Id: [%d]\n",con_ids[j]);

        status = ProSectionConstraintsGet(section, con_ids[j],
	    &con_type, &con_status, &n_ref_ids, &ref_ids, &p_types);
	TEST_CALL_REPORT("ProSectionConstraintsGet()",
		"ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);

	if (status != PRO_TK_NO_ERROR)
	    continue;

        switch(con_type)
	{
	    case PRO_CONSTRAINT_TYPE_UNKNOWN:
		 c_type = (char*)"PRO_CONSTRAINT_TYPE_UNKNOWN";
		 break;
            case PRO_CONSTRAINT_SAME_POINT:
		 c_type = (char*)"PRO_CONSTRAINT_SAME_POINT";
		 break;
            case PRO_CONSTRAINT_HORIZONTAL_ENT:
		 c_type = (char*)"PRO_CONSTRAINT_HORIZONTAL_ENT";
		 break;
            case PRO_CONSTRAINT_VERTICAL_ENT:
		 c_type = (char*)"PRO_CONSTRAINT_VERTICAL_ENT";
		 break;
            case PRO_CONSTRAINT_PNT_ON_ENT:
		 c_type = (char*)"PRO_CONSTRAINT_PNT_ON_ENT";
		 break;
            case PRO_CONSTRAINT_TANGENT_ENTS:
		 c_type = (char*)"PRO_CONSTRAINT_TANGENT_ENTS";
		 break;
            case PRO_CONSTRAINT_PARALLEL_ENTS:
		 c_type = (char*)"PRO_CONSTRAINT_PARALLEL_ENTS";
		 break;
            case PRO_CONSTRAINT_ORTHOG_ENTS:
		 c_type = (char*)"PRO_CONSTRAINT_ORTHOG_ENTS";
		 break;
            case PRO_CONSTRAINT_EQUAL_SEGMENTS:
		 c_type = (char*)"PRO_CONSTRAINT_EQUAL_SEGMENTS";
		 break;
            case PRO_CONSTRAINT_EQUAL_RADII:
		 c_type = (char*)"PRO_CONSTRAINT_EQUAL_RADII";
		 break;
            case PRO_CONSTRAINT_SYMMETRY:
		 c_type = (char*)"PRO_CONSTRAINT_SYMMETRY";
		 break;
            case PRO_CONSTRAINT_SAME_COORD:
		 c_type = (char*)"PRO_CONSTRAINT_SAME_COORD";
		 break;
            default:
		 c_type = (char*)"Unknown";
		 break;
        }

        switch(con_status)
	{
	    case PRO_TK_CONSTRAINT_DENIED:
		c_status = (char*)"PRO_TK_CONSTRAINT_DENIED";
		break;
	    case PRO_TK_CONSTRAINT_ENABLED:
		c_status = (char*)"PRO_TK_CONSTRAINT_ENABLED";
		break;
            default:
		c_status = (char*)"Unknown";
		break;
	}

	ProTKFprintf(fp,"\t\tType: %-30s Status: %s\n", c_type, c_status);

        ProTKFprintf(fp,"\t\tConstraint References:\n");
        for (k=0; k<n_ref_ids; k++)
	{
	    p_type = ProUtilSectionPointTypeStr(p_types[k]);
            ProTKFprintf(fp,"\t\t\tRef%d: Entity Id [%d], Point Type %s\n",
		    k,ref_ids[k],p_type);
        }

	status = ProArrayFree((ProArray*)&p_types);
        TEST_CALL_REPORT("ProArrayFree()",
		"ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);
	status = ProArrayFree((ProArray*)&ref_ids);
        TEST_CALL_REPORT("ProArrayFree()",
		"ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);
	ProTKFprintf(fp,"\n");
    }

    status = ProArrayFree((ProArray*)&con_ids);
    TEST_CALL_REPORT("ProArrayFree()",
	    "ProUtilSectionInfoGet()", status, status != PRO_TK_NO_ERROR);
#endif

    return(status);
}

/*====================================================================*\
    FUNCTION :	ProUtilSectionRetr
    PURPOSE  :	Retrieves a 2D section
\*====================================================================*/
ProError ProUtilSectionRetr(ProSection *section)
{
    ProFamilyMdlName sec_name;
    ProError status;
    char str[PRO_FAMILY_MDLNAME_SIZE];
    char *p;

    ProUtilMsgPrint((char*)"gen",(char*)"TEST %0s",(char*)"Enter section name to retrieve: ");
    if (!ProUtilStringGet(sec_name,NULL,PRO_NAME_SIZE-1))
	 return(PRO_TK_GENERAL_ERROR);

    ProWstringToString( str, sec_name );
    if( (p = strchr( str, '.')) != NULL )
	*p = '\0';
    ProTKPrintf("Section name %s.sec\n",str);
    ProStringToWstring( sec_name, str );
    status = ProMdlnameRetrieve(sec_name,PRO_MDLFILE_2DSECTION,(ProMdl *)section);
    TEST_CALL_REPORT("ProMdlnameRetrieve()", "ProUtilSectionRetr()",
		     status, status != PRO_TK_NO_ERROR);
    return(status);
}

/*====================================================================*\
    FUNCTION :  ProUtilSectionSave
    PURPOSE  :  Saves a 2D section
\*====================================================================*/
ProError ProUtilSectionSave(ProSection section)
{
    ProError status;

    status = ProMdlSave((ProMdl) section);
    TEST_CALL_REPORT("ProMdlSave()", "ProUtilSectionSave()",
                     status, status != PRO_TK_NO_ERROR);
    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :  ProUtilSectionErase
    PURPOSE  :  Erases a 2D section
\*====================================================================*/
ProError ProUtilSectionErase(ProSection section)
{
    ProError status;

    status = ProMdlErase((ProMdl) section);
    TEST_CALL_REPORT("ProMdlErase()", "ProUtilSectionErase()",
                     status, status != PRO_TK_NO_ERROR);
    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :  ProUtilSectionDelete
    PURPOSE  :  Delete a 2D section
\*====================================================================*/
ProError ProUtilSectionDelete(ProSection section)
{
    ProError status;

    status = ProMdlDelete((ProMdl) section);
    TEST_CALL_REPORT("ProMdlDelete()", "ProUtilSectionDelete()",
                     status, status != PRO_TK_NO_ERROR);
    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProUtilLinesParallel
    PURPOSE  :	Check if two lines are parallel (returns 1) to each other
\*====================================================================*/
int ProUtilLinesParallel(double line1[2][3],double line2[2][3])
{
     double vec1[3], vec2[3];

     ProUtilVectorDiff(line1[0],line1[1],vec1);
     ProUtilVectorDiff(line2[0],line2[1],vec2);

     ProUtilVectorNormalize(vec1,vec1);
     ProUtilVectorNormalize(vec2,vec2);

     if ( (1.0-fabs(ProUtilVectorDot(vec1,vec2))) <= EPSM6 )
	  return(1);

     return(0);
}

/*====================================================================*\
    FUNCTION :	ProUtilLinesPerpend
    PURPOSE  :	Check if two lines are perpendicular (returns 1)
\*====================================================================*/
int ProUtilLinesPerpend(double line1[2][3],double line2[2][3])
{
     double vec1[3], vec2[3];

     ProUtilVectorDiff(line1[0],line1[1],vec1);
     ProUtilVectorDiff(line2[0],line2[1],vec2);

     ProUtilVectorNormalize(vec1,vec1);
     ProUtilVectorNormalize(vec2,vec2);

     if ( fabs(ProUtilVectorDot(vec1,vec2) ) <= EPSM6 )
	  return(1);

     return(0);
}

/*====================================================================*\
    FUNCTION :	ProUtilLineLineX
    PURPOSE  :	Get the intersection point of two lines
\*====================================================================*/
double *ProUtilLineLineX(
     double l1[2][3],
     double l2[2][3],
     double int_pnt[3])
{
     double v1[3], v2[3], v3[3], v4[4];
     double n[3],n1[3], n2[3];

     /* if lines parallel return NULL */
     if ( ProUtilLinesParallel(l1,l2) )
	 return(NULL);

     ProUtilVectorDiff(l1[0],l1[1],v1);
     ProUtilVectorDiff(l2[0],l2[1],v2);

     if ((fabs(l1[0][0]-l2[0][0])<= EPSM6) &&
         (fabs(l1[0][1]-l2[0][1])<= EPSM6) &&
         (fabs(l1[0][2]-l2[0][2])<= EPSM6))
     {
	 memcpy(int_pnt,l1[0],3*sizeof(double));
	 return(int_pnt);
     }

     if ((fabs(l1[1][0]-l2[1][0])<= EPSM6) &&
         (fabs(l1[1][1]-l2[1][1])<= EPSM6) &&
         (fabs(l1[1][2]-l2[1][2])<= EPSM6))
     {
	 memcpy(int_pnt,l1[1],3*sizeof(double));
	 return(int_pnt);
     }

     ProUtilVectorDiff(l1[0],l2[0],v3);
     ProUtilVectorDiff(l1[1],l2[1],v4);
     ProUtilVectorCross(v1,v2,n1);
     ProUtilVectorCross(v3,v4,n2);
     ProUtilVectorNormalize(n1,n1);
     ProUtilVectorNormalize(n2,n2);

     /* if lines not on same plane return NULL */
     if ( (1.0 - fabs(ProUtilVectorDot(n1,n2))) >= EPSM6 )
	  return(NULL);

     ProUtilVectorNormalize(v1,v1);
     ProUtilVectorNormalize(v2,v2);
     ProUtilVectorCross(n1,v1,n);
     ProUtilPlaneLineX(l1[0],n,l2[0],v2,int_pnt);

     return(int_pnt);

}

/*====================================================================*\
    FUNCTION :	ProUtilParallelLineEntityGet
    PURPOSE  :	Given sk line ent and array of line ents - finds first
		parallel entity
\*====================================================================*/
ProError ProUtilParallelLineEntityGet(
    ProSection section,
    int ent_id,
    int ent_ids[],
    int n_ids,
    int *parallel_index)
{
    ProError status;
    Pro2dLinedef *line;
    int i;
    double l1[2][3];
    double l2[2][3];

    status = ProSectionEntityGet(section,ent_id,(Pro2dEntdef **)&line);
    TEST_CALL_REPORT("ProSectionEntityGet()",
		     "ProUtilParallelLineEntityGet()",
                     status, status != PRO_TK_NO_ERROR);

    if (!(line->type == PRO_2D_LINE || line->type == PRO_2D_CENTER_LINE))
	return(PRO_TK_GENERAL_ERROR);

    l1[0][0] = line->end1[0];
    l1[0][1] = line->end1[1];
    l1[1][0] = line->end2[0];
    l1[1][1] = line->end2[1];
    l1[0][2] = l1[1][2] = 0.0;

    for (i=0; i<n_ids; i++)
    {
	status = ProSectionEntityGet(section,ent_ids[i],
				    (Pro2dEntdef **)&line);
        TEST_CALL_REPORT("ProSectionEntityGet()",
		         "ProUtilParallelLineEntityGet()",
                         status, status != PRO_TK_NO_ERROR);

        if (!(line->type == PRO_2D_LINE || line->type == PRO_2D_CENTER_LINE))
	    return(PRO_TK_GENERAL_ERROR);

        l2[0][0] = line->end1[0];
        l2[0][1] = line->end1[1];
        l2[1][0] = line->end2[0];
        l2[1][1] = line->end2[1];
        l2[0][2] = l2[1][2] = 0.0;

        if (ProUtilLinesParallel(l1,l2))
	{
	    *parallel_index = i;
	    return(PRO_TK_NO_ERROR);
        }
    }

    return(PRO_TK_E_NOT_FOUND);
}

/*====================================================================*\
    FUNCTION :  ProUtilSketchCsysGet
    PURPOSE  :  Gets the sketch csys transform
\*====================================================================*/
ProError ProUtilSketchCsysGet(
    ProTestSectionInfo *p_sketch_refs,
    double sk_transf[4][4])
{
    ProError status;

    status = ProSectionLocationGet(p_sketch_refs->section,sk_transf);
    TEST_CALL_REPORT("ProSectionLocationGet()","ProUtilSketchCsysGet()",
		     status, status != PRO_TK_NO_ERROR);

    return (PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :  UserCreateSecTrfMatrix
    PURPOSE  :  Set the basic section transformation matrix
\*====================================================================*/
ProError UserCreateSecTrfMatrix(SectionMatrix_data *m_data)
{

  ProPoint3d origin; 
  double sel_pnt[3], proj_sel_pnt[3], sel_vect[3]; 
  double x_plane[3];
  ProVector x, y, sk_plane_norm;
  ProVector d1[2], d2[2], norm;
  ProError status;
  ProMatrix inv_sk_mtrx;
  ProModelitem sk_modelitem;
  ProSurface sk_surf;
  ProUvParam sk_param;
  /* =============================================================== *\
   	Project point selected on sketch plane onto section
\* =============================================================== */

  ProUtilMatrixInvert(m_data->sk_mtrx, inv_sk_mtrx);

  status = ProSelectionUvParamGet(m_data->sk_plane, sk_param);
  ERROR_CHECK((char*)"ProSelectionUvParamGet",(char*) "UserCreateSecTrfMatrix", status);
  status = ProSelectionModelitemGet(m_data->sk_plane, &sk_modelitem);
  ERROR_CHECK((char*)"ProSelectionModelitemGet", (char*)"UserCreateSecTrfMatrix", status);
  status = ProGeomitemToSurface((ProGeomitem *)&sk_modelitem, &sk_surf);
  ERROR_CHECK((char*)"ProGeomitemToSurface", (char*)"UserCreateSecTrfMatrix", status);
  status = ProSurfaceXyzdataEval(sk_surf, sk_param, sel_pnt, d1, d2, norm);
  ERROR_CHECK((char*)"ProSurfaceXyzdataEval", (char*)"UserCreateSecTrfMatrix", status);

  ProUtilPointTrans(inv_sk_mtrx, sel_pnt, proj_sel_pnt);
  proj_sel_pnt[2] = 0.0;

  /* =============================================================== *\
  	Create x and y vectors of transformation matrix such that 
	selected point on sketch plane is in first quadrant
\* =============================================================== */

  ProUtilLineLineX(m_data->x_axis, m_data->y_axis, origin);

  ProUtilVectorDiff(proj_sel_pnt, origin, sel_vect);
  sel_vect[2] = 0.0;
  ProUtilVectorNormalize(sel_vect, sel_vect);

  ProUtilVectorDiff(m_data->x_axis[0], m_data->x_axis[1], x);
  ProUtilVectorNormalize(x, x);

  ProUtilVectorDiff(m_data->y_axis[0], m_data->y_axis[1], y);
  ProUtilVectorNormalize(y, y);	
	
/* =========  Selected point should be in first quadrant  ======= */

  if ((ProUtilVectorDot(x,sel_vect)) < 0.0)
    ProUtilVectorScale(-1.0, x, x);

  if ((ProUtilVectorDot(y,sel_vect)) < 0.0)
    ProUtilVectorScale(-1.0, y, y);

/* ========= Make sure surface normal is properly oriented   ======= */
/* ========= with respect to x and y   ============================= */


  ProUtilVectorCross(x, y, sk_plane_norm);
  ProUtilVectorTrans(m_data->sk_mtrx, sk_plane_norm, sk_plane_norm);
  ProUtilMatrixCopy(NULL, m_data->sec_trf);

  if ((ProUtilVectorDot(sk_plane_norm, m_data->sk_mtrx[2])) < 0.0)
    {
      ProUtilVectorCopy(y, m_data->sec_trf[0]);
      ProUtilVectorCopy(x, m_data->sec_trf[1]);
      ProUtilVectorCopy(NULL, m_data->sec_trf[2]);
      ProUtilVectorCopy(origin, m_data->sec_trf[3]);
    }

  else
    {
      ProUtilVectorCopy(x, m_data->sec_trf[0]);
      ProUtilVectorCopy(y, m_data->sec_trf[1]);
      ProUtilVectorCopy(NULL, m_data->sec_trf[2]);
      ProUtilVectorCopy(origin, m_data->sec_trf[3]);
    }
		
  m_data->sec_trf[2][2] = m_data->sec_trf[3][3] = 1.0;

/* =============================================================== *\
		Calculate rotation angle  	
\* =============================================================== */
  x_plane[0] = 1.0;
  x_plane[1] = x_plane[2] = 0.0;
  ProUtilVectorCross(x_plane, m_data->sk_mtrx[0], norm);
  ProUtilVectorCross(m_data->sec_trf[0], m_data->sec_trf[1], sk_plane_norm);
  m_data->angle = fabs(acos(ProUtilVectorDot(x_plane, m_data->sec_trf[0])));
  if (ProUtilVectorDot(norm, sk_plane_norm) < 0.0)
    m_data->angle *= -1.0;

  return(status);

}


/*====================================================================*\
    FUNCTION :	ProUtilSectCsysTransformGet
    PURPOSE  :	Given a plane selection and two orthogonal lines on
		the plane defining a csys, output transform from this
		csys to plane csys (selection point is used to determine
		x and y vectors such that the point is in the first
		quadrant) (currently works for part mode)
\*====================================================================*/
ProError ProUtilSectCsysTransformGet(
     ProTestSectionInfo *p_sketch_refs,
     double sk_transf[4][4],
     double l1[2][3],
     double l2[2][3],
     double transform[4][4],
     double *angle)
{

     ProMatrix inv_sk_transf;
     Pro3dPnt origin, sel_pnt, proj_sel_pnt;
     ProVector n, v, n1, plane_x;
     ProVector x, y, z_vec={0,0,1};
     double dot;
     ProSurface p_plane;
     ProModelitem mdlitem;
     ProError status;

     ProUtilLineLineX(l1,l2,origin);

     status = ProSelectionModelitemGet(p_sketch_refs->sketch_pln,&mdlitem);
     TEST_CALL_REPORT("ProSelectionModelitemGet()",
		      "ProUtilSectCsysTransformGet()",
		      status, status != PRO_TK_NO_ERROR);

     status = ProSurfaceInit(mdlitem.owner,mdlitem.id,&p_plane);
     TEST_CALL_REPORT("ProSurfaceInit()",
		      "ProUtilSectCsysTransformGet()",
		      status, status != PRO_TK_NO_ERROR);

     status = ProSurfaceXyzdataEval(p_plane,p_sketch_refs->sketch_uv,
				    sel_pnt,NULL,NULL,NULL);
     TEST_CALL_REPORT("ProSurfaceXyzdataEval()",
		      "ProUtilSectCsysTransformGet()",
		      status, status != PRO_TK_NO_ERROR);

     ProUtilVectorDiff(l1[0],l1[1],x);
     ProUtilVectorNormalize(x,x);
     ProUtilVectorDiff(l2[0],l2[1],y);
     ProUtilVectorNormalize(y,y);

     ProUtilMatrixInvert(sk_transf,inv_sk_transf);
     ProUtilPointTrans(inv_sk_transf,sel_pnt,proj_sel_pnt);
     proj_sel_pnt[2] = 0.0;

     ProUtilVectorDiff(proj_sel_pnt,origin,v);
     v[2] = 0.0;
     ProUtilVectorNormalize(v,v);

     /* make sure sel pnt is in first quadrant */

     dot = ProUtilVectorDot(v,x);
     if ( dot < 0.0 )
	  ProUtilVectorScale(-1.0,x,x);
     dot = ProUtilVectorDot(v,y);
     if ( dot < 0.0 )
	  ProUtilVectorScale(-1.0,y,y);

     /* make sure x and y are correct relative to plane normal */
     ProUtilVectorCross(x,y,n);
     if ( n[2] < 0.0 )
     {
	ProUtilVectorCopy(x, n);
	ProUtilVectorCopy(y, x);
	ProUtilVectorCopy(n, y);
     }

     status = ProMatrixInit(x, y, z_vec, origin, transform);
     TEST_CALL_REPORT("ProMatrixInit()",
	"ProUtilSectCsysTransformGet()",  status, status != PRO_TK_NO_ERROR);

     /* calculate rotation angle */
     plane_x[0] = 1.0;
     plane_x[1] = plane_x[2] = 0.0;
     ProUtilVectorCross(x,y,n);
     ProUtilVectorCross(plane_x,x,n1);
     *angle = fabs(acos(ProUtilVectorDot(plane_x,x)));
     if (ProUtilVectorDot(n,n1) < 0.0)
	  *angle *= -1.0;

     return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProUtilTwoOrthogEdgesOnPlaneGet
    PURPOSE  :	Gets the selection of two orthogonal edges on sketch
		plane from the user
\*====================================================================*/
ProError ProUtilTwoOrthogEdgesOnPlaneGet(
    ProSection section,
    int ent_ids[2],
    double edge1[2][3],
    double edge2[2][3])
{

    ProError status;
    double edges[2][2][3];
    ProSelection *p_sels;
    int n_edges,i;
    int error = 0, cont = 1;
    Pro2dLinedef *linedef;

    while (cont)
    {
         ProUtilMsgPrint((char*)"sec",(char*)"TEST Select two orthogonal edges.");
	 status = ProSelect((char*)"edge",2,NULL,NULL,NULL,NULL,&p_sels,&n_edges);
	 TEST_CALL_REPORT("ProSelect()", "ProUtilTwoOrthogEdgesOnPlaneGet()",
	                   status, status != PRO_TK_NO_ERROR);

	 if (n_edges < 2) return(PRO_TK_GENERAL_ERROR);

	 for (i=0; i<2; i++)
	 {
              status = ProSectionEntityFromProjection(section,p_sels[i],
							   &ent_ids[i]);
                  TEST_CALL_REPORT("ProSectionEntityFromProjection()",
                      "ProUtilTwoOrthogEdgesOnPlaneGet()",
                      status, status != PRO_TK_NO_ERROR);

	      if (status == PRO_TK_NO_ERROR)
	      {
                   status = ProSectionEntityGet(section,ent_ids[i],
						(Pro2dEntdef **)&linedef);
                   TEST_CALL_REPORT("ProSectionEntityGet()",
				    "ProUtilTwoOrthogEdgesOnPlaneGet()",
				    status, status != PRO_TK_NO_ERROR);
              }
	      else
	      {
                   ProUtilMsgPrint("sec",
			"TEST One of the edges is perpendicular to sketch plane.");
		   error = 1;
		   break;
              }

	      edges[i][0][0] = linedef->end1[0];
	      edges[i][0][1] = linedef->end1[1];
	      edges[i][0][2] = 0.0;
	      edges[i][1][0] = linedef->end2[0];
	      edges[i][1][1] = linedef->end2[1];
	      edges[i][1][2] = 0.0;
         }
	 if (error)
	 {
	      error = 0;
	      continue;
         }

         if (!ProUtilLinesPerpend(edges[0],edges[1]))
	 {
	      ProUtilMsgPrint((char *)"sec", (char *)"TEST Edges are not orthogonal.");
              continue;
         }
	 else
	 {
	      ProUtilVectorCopy(edges[0][0],edge1[0]);
	      ProUtilVectorCopy(edges[0][1],edge1[1]);
	      ProUtilVectorCopy(edges[1][0],edge2[0]);
	      ProUtilVectorCopy(edges[1][1],edge2[1]);
              break;
         }
    }

    return(PRO_TK_NO_ERROR);
}

void ProUtil2DPointTrans(
    double m[4][4],
    double p2D[2],
    double out2D[2])
{
    double p3D[3];
    double out3D[3];

    p3D[0] = p2D[0];
    p3D[1] = p2D[1];
    p3D[2] = 0.0;

    ProUtilPointTrans(m,p3D,out3D);

    out2D[0] = out3D[0];
    out2D[1] = out3D[1];
}

/*====================================================================*\
    FUNCTION :	ProUtilXyOffsetsGet
    PURPOSE  :	Gets xy offsets from the user
\*====================================================================*/
ProError ProUtilXyOffsetsGet(
    double offsets[2])
{

    ProUtilMsgPrint((char *)"sec",
	(char *)"TEST Enter offset dimension from first reference [%0(6.4)f]: ",&offsets[0]);
    ProUtilDoubleGet(NULL,&offsets[0],&offsets[0]);
    ProUtilMsgPrint((char *)"sec",
     (char *)"TEST Enter offset dimension from second reference [%0(6.4)f]: ",&offsets[1]);
    ProUtilDoubleGet(NULL,&offsets[1],&offsets[1]);

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
    FUNCTION :	ProUtilSectEpsModify
    PURPOSE  :	Allows interactive section epsilon modification
\*====================================================================*/
ProError ProUtilSectEpsModify(
    ProSection section)
{
    double eps;
    ProError status;

    status = ProSectionEpsilonGet(section,&eps);
    TEST_CALL_REPORT("ProSectionEpsilonGet()", "ProUtilSectEpsModify()",
		     status, status != PRO_TK_NO_ERROR);

    ProUtilMsgPrint("sec","TEST Enter section solver epsilon [%0(6.4)f]: ",&eps);
    ProUtilDoubleGet(NULL,&eps,&eps);

    status = ProSectionEpsilonSet(section,eps);
    TEST_CALL_REPORT("ProSectionEpsilonSet()", "ProUtilSectEpsModify()",
		     status, status != PRO_TK_NO_ERROR);

    return(PRO_TK_NO_ERROR);
}
/*====================================================================*\
    FUNCTION :	ProUtilSectionUseExisting
    PURPOSE  :	Top level function for extracting existing section
		definition and applying it to a new section
\*====================================================================*/
ProError ProUtilSectionUseExisting(ProSection new_section,
				   ProTestSectionInfo *secinfo)
{
    ProSection exist_section;
    ProError status;
    ProSelection *sel = NULL;
    int proj_ent, n;
    double transf[4][4];
    double inv_transf[4][4];
    ProVector d1,d2;
    ProSectionPointType pnt_types[2];
    ProIntlist ids;
    int n_ids;
    double orig_3d[3],origin[3];
    double ent_origin[3];
    double ref_pnt3d[3],ref_pnt[3];
    int ent_ids[2], id,i;
    Pro2dPnt dim_pnt;
    ProVector trans_vector;
    ProModelitem mdlitem;
    ProSurface p_plane;
    ProBoolean is_proj;
    Pro2dEntdef *entity;

/*--------------------------------------------------------------------*\
Retrieve existing section from
\*--------------------------------------------------------------------*/
    status = ProUtilSectionRetrieve(&exist_section);

    if (status != PRO_TK_NO_ERROR)
	return(PRO_TK_GENERAL_ERROR);
#if 0
    status = ProSectionCheck(exist_section, &sec_error, &sec_info)
    TEST_CALL_REPORT("ProSectionEntityIdsGet()",
		     "ProUtilSectionUseExisting()",
	             status, status != PRO_TK_NO_ERROR);
#endif

/*--------------------------------------------------------------------*\
If 2D section copy the section as is
\*--------------------------------------------------------------------*/
    if (secinfo == NULL)
    {
        ProUtilVectorCopy(NULL,trans_vector);
        status = ProUtilSectionInfoCopy(exist_section,new_section,
					trans_vector);
    }
/*--------------------------------------------------------------------*\
Else copy, translate and locate 3D section as necessary
\*--------------------------------------------------------------------*/
    else
    {
/*--------------------------------------------------------------------*\
Get tranform to new section coordinates
\*--------------------------------------------------------------------*/
	status = ProUtilSketchCsysGet(secinfo, transf);
        ProUtilMatrixInvert(transf, inv_transf);

/*--------------------------------------------------------------------*\
Find start vertex of first section-defining entity to use to locate sect
\*--------------------------------------------------------------------*/
        status = ProSectionEntityIdsGet(exist_section,&ids,&n_ids);
	TEST_CALL_REPORT("ProSectionEntityIdsGet()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);

        for (i=0; i<n_ids; i++)
	{
	    status = ProSectionEntityGet(exist_section,ids[i],
					          &entity);
	    TEST_CALL_REPORT("ProSectionEntityGet()",
			     "ProUtilSectionUseExisting()",
	                     status, status != PRO_TK_NO_ERROR);
            if (entity->type != PRO_2D_CENTER_LINE)
	    {
                status = ProSectionEntityEval(exist_section,ids[i],0.0,
				          ent_origin,d1,d2);
	        TEST_CALL_REPORT("ProSectionEntityEval()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);
                break;
            }
        }

        status = ProArrayFree( (ProArray *) &ids);
	TEST_CALL_REPORT("ProArrayFree()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);

     if(secinfo->sketch_pln != NULL)
      {
/*--------------------------------------------------------------------*\
Get sect location point from selection (new origin)
\*--------------------------------------------------------------------*/
        status = ProSelectionModelitemGet(secinfo->sketch_pln,&mdlitem);
        TEST_CALL_REPORT("ProSelectionModelitemGet()",
		      "ProUtilSectionUseExisting()",
		      status, status != PRO_TK_NO_ERROR);
        status = ProSurfaceInit(mdlitem.owner,mdlitem.id,&p_plane);
        TEST_CALL_REPORT("ProSurfaceInit()",
		      "ProUtilSectionUseExisting()",
		      status, status != PRO_TK_NO_ERROR);
        status = ProSurfaceXyzdataEval(p_plane,secinfo->sketch_uv,
				    orig_3d,NULL,NULL,NULL);
        TEST_CALL_REPORT("ProSurfaceXyzdataEval()",
		      "ProUtilSectionUseExisting()",
		      status, status != PRO_TK_NO_ERROR);

/*--------------------------------------------------------------------*\
Calc trans vector to move first ent pnt to location pnt
\*--------------------------------------------------------------------*/
        ProUtilPointTrans(inv_transf,orig_3d,origin);
	origin[2] = 0.0;
        ProUtilVectorDiff(origin,ent_origin,trans_vector);
       }
      else
       {
         origin[0] = 0.0;
         origin[1] = 0.0;
         origin[2] = 0.0;
         ProUtilVectorCopy(NULL,trans_vector);
       }
/*--------------------------------------------------------------------*\
Copy and translate existing section entities to new section
\*--------------------------------------------------------------------*/
        status = ProUtilSectionInfoCopy(exist_section,new_section,
					trans_vector);

/*--------------------------------------------------------------------*\
Choose a model vertex to dimension section (first ent start pnt) to
\*--------------------------------------------------------------------*/
        ProUtilMsgPrint((char *)"gen",(char *)"TEST %0s",
			(char *)"Select a vertex as the section location reference.");
	status = ProSelect((char *)"edge_end",1,NULL,NULL,NULL,NULL,&sel,&n);
	TEST_CALL_REPORT("ProSelect()", "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);

        if (status != PRO_TK_NO_ERROR) return(PRO_TK_GENERAL_ERROR);

        status = ProSectionEntityFromProjection(new_section,sel[0],
						&proj_ent);
            TEST_CALL_REPORT("ProSectionEntityFromProjection()",
                "ProUtilSectionUseExisting()",
                status, status != PRO_TK_NO_ERROR);

        status = ProSelectionPoint3dGet(sel[0],ref_pnt3d);
	TEST_CALL_REPORT("ProSelectionPoint3dGet()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);
        ProUtilPointTrans(inv_transf,ref_pnt3d,ref_pnt);
	ref_pnt[2] = 0;

/*--------------------------------------------------------------------*\
Find the first section defining entity in the new section
\*--------------------------------------------------------------------*/
        status = ProSectionEntityIdsGet(new_section,&ids,&n_ids);
	TEST_CALL_REPORT("ProSectionEntityIdsGet()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);

        for (i=0; i<n_ids; i++)
	{
	    status = ProSectionEntityGet(new_section,ids[i],
					          &entity);
	    TEST_CALL_REPORT("ProSectionEntityGet()",
			     "ProUtilSectionUseExisting()",
	                     status, status != PRO_TK_NO_ERROR);
	    status = ProSectionEntityIsProjection(new_section,ids[i],
					          &is_proj);
	    TEST_CALL_REPORT("ProSectionEntityIsProjection()",
			     "ProUtilSectionUseExisting()",
	                     status, status != PRO_TK_NO_ERROR);

            if (entity->type != PRO_2D_CENTER_LINE &&
		is_proj == PRO_B_FALSE)
	    {
		ent_ids[1] = ids[i];
                break;
            }
        }


/*--------------------------------------------------------------------*\
Create horiz and vertical pnt to pnt dims from first ent start pnt
to projected vertex pnt to locate the section
\*--------------------------------------------------------------------*/
	ent_ids[0] = proj_ent;
	pnt_types[0] = PRO_ENT_CENTER;
	pnt_types[1] = PRO_ENT_START;
	dim_pnt[0] = origin[0] + (ref_pnt[0] - origin[0])/2.0;
	dim_pnt[1] = origin[1];
	status = ProSecdimCreate(new_section,ent_ids,pnt_types,
			 2,PRO_TK_DIM_PNT_PNT_HORIZ,dim_pnt,&id);
	TEST_CALL_REPORT("ProSectionEntityEval()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);
	dim_pnt[0] = origin[0];
	dim_pnt[1] = origin[1] + (ref_pnt[1] - origin[1])/2.0;
	status = ProSecdimCreate(new_section,ent_ids,pnt_types,
			 2,PRO_TK_DIM_PNT_PNT_VERT,dim_pnt,&id);
	TEST_CALL_REPORT("ProSectionEntityEval()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);

        ProArrayFree( (ProArray *) &ids);
	TEST_CALL_REPORT("ProArrayFree()",
			 "ProUtilSectionUseExisting()",
	                 status, status != PRO_TK_NO_ERROR);

    }

    status = ProTestSectionSolveRegen(new_section);

    return(status);
}
/*====================================================================*\
    FUNCTION :	ProUtilSectionInfoCopy
    PURPOSE  :	Copies section defining entities to a new section
		applying translation vector
\*====================================================================*/
ProError ProUtilSectionInfoCopy(ProSection exist_section,
				ProSection new_section,
				ProVector trans_vector)
{
    ProError err;
    ProIntlist ent_ids, new_ent_ids;
    int n_ids,i,j,k;
    ProBoolean is_proj;
    Pro2dEntdef *p_entity;
    ProIntlist dim_ids;
    int n_dim_ids,*p_ent_refs;
    ProSecdimType d_type;
    ProSectionPointType *p_pnt_types;
    int n_refs,new_ent_refs[5],dim_id, tmp_id;
    ProBoolean ref_proj;
    ProVector mid_pnt, d1, d2;
    Pro2dPnt dim_pnt;
    Pro2dPointdef dim_loc;

    err = ProSectionEntityIdsGet(exist_section,&ent_ids,&n_ids);
    TEST_CALL_REPORT("ProSectionEntityIdsGet()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

    err = ProArrayAlloc(n_ids,sizeof(ProIntlist),n_ids,
			(ProArray *)&new_ent_ids);
    TEST_CALL_REPORT("ProArrayAlloc()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

    for (i=0; i<n_ids; i++)
    {
	err = ProSectionEntityIsProjection(exist_section,ent_ids[i],
					   &is_proj);
        TEST_CALL_REPORT("ProSectionEntityIsProjection()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        if (is_proj == PRO_B_TRUE)
	{
	    new_ent_ids[i] = -1;
	    continue;
        }

        err = ProSectionEntityGet(exist_section,ent_ids[i],&p_entity);
        TEST_CALL_REPORT("ProSectionEntityGet()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        if (err != PRO_TK_NO_ERROR)
	{
	    ProTKPrintf(
        "ProUtilSectionInfoCopy Section Entity Id: %d not suppported\n",
		   ent_ids[i]);
	    continue;
        }


        err = ProSectionEntityAdd(new_section,p_entity,
				  &new_ent_ids[i]);
        TEST_CALL_REPORT("ProSectionEntityAdd()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        err = ProSectionEntityTranslate(new_section,new_ent_ids[i],
					trans_vector);
        TEST_CALL_REPORT("ProSectionEntityTranslate()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);
    }

    err = ProSecdimIdsGet(exist_section,&dim_ids,&n_dim_ids);
    TEST_CALL_REPORT("ProSecdimIdsGet()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

    for (i=0; i<n_dim_ids; i++)
    {
	err = ProSecdimTypeGet(exist_section,dim_ids[i],&d_type);
        TEST_CALL_REPORT("ProSecdimTypeGet()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        err = ProSecdimReferencesGet(exist_section,dim_ids[i],
			      &p_ent_refs,&p_pnt_types,&n_refs);
        TEST_CALL_REPORT("ProSecdimReferencesGet()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        ref_proj = PRO_B_FALSE;
        for (j=0; j<n_refs; j++)
	{
	    err = ProSectionEntityIsProjection(exist_section,
				   p_ent_refs[j],&is_proj);
            TEST_CALL_REPORT("ProSectionEntityIsProjection()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);
	    if ( is_proj == PRO_B_TRUE )
	    {
		ref_proj = PRO_B_TRUE;
		break;
            }
        }

        if (ref_proj == PRO_B_TRUE)
	{
	    ref_proj = PRO_B_FALSE;
	    continue;
        }

        for (j=0; j<n_refs; j++)
	{
	    for (k=0; k<n_ids; k++)
		if ( ent_ids[k] == p_ent_refs[j] )
		    break;

	    new_ent_refs[j] = new_ent_ids[k];
        }

	err = ProSecdimLocationGet(exist_section, dim_ids[i], dim_loc.pnt);
        TEST_CALL_REPORT("ProSecdimLocationGet",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

	dim_loc.type = PRO_2D_POINT;
	err = ProSectionEntityAdd(new_section, (Pro2dEntdef*)&dim_loc, &tmp_id);
        TEST_CALL_REPORT("ProSectionEntityAdd",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        err = ProSectionEntityTranslate(new_section, tmp_id, trans_vector);
        TEST_CALL_REPORT("ProSectionEntityTranslate()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        err = ProSectionEntityEval(new_section, tmp_id, 0, mid_pnt, d1, d2);
        TEST_CALL_REPORT("ProSectionEntityEval()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);

        err = ProSectionEntityDelete(new_section, tmp_id);
        TEST_CALL_REPORT("ProSectionEntityDelete()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);
        dim_pnt[0] = mid_pnt[0];
        dim_pnt[1] = mid_pnt[1];

        err = ProSecdimCreate(new_section,new_ent_refs,
		   p_pnt_types,n_refs,d_type,dim_pnt,&dim_id);
        TEST_CALL_REPORT("ProSecdimCreate()",
			  "ProUtilSectionCopy()", err, err != PRO_TK_NO_ERROR);
    }

    return(err);
}

/*====================================================================*\
    FUNCTION :	ProUtilSectionClean
    PURPOSE  :	Deletes all dimensions and entities from a section
\*====================================================================*/
ProError ProUtilSectionClean(ProSection section)
{
    ProError err;
    ProIntlist ent_ids, dim_ids, constr_ids;
    int n_dim_ids, n_ids, i, n_constr_ids;

#if 1
    err = ProSectionConstraintsIdsGet(section, &constr_ids, &n_constr_ids);
    TEST_CALL_REPORT("ProSectionConstraintsIdsGet()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);

    for (i=0; i<n_constr_ids; i++)
    {
	err = ProSectionConstraintDeny(section, constr_ids[i]);
        TEST_CALL_REPORT("ProSectionConstraintDeny()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);
    }

    err = ProArrayFree( (ProArray *) &constr_ids);
    TEST_CALL_REPORT("ProArrayFree()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);
#endif
    err = ProSecdimIdsGet(section,&dim_ids,&n_dim_ids);
    TEST_CALL_REPORT("ProSecdimIdsGet()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);

    for (i=0; i<n_dim_ids; i++)
    {
	err = ProSecdimDelete(section,dim_ids[i]);
        TEST_CALL_REPORT("ProSecdimDelete()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);
    }

    err = ProArrayFree( (ProArray *) &dim_ids);
    TEST_CALL_REPORT("ProArrayFree()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);

    err = ProSectionEntityIdsGet(section,&ent_ids,&n_ids);
    TEST_CALL_REPORT("ProSectionEntityIdsGet()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);

    for (i=0; i<n_ids; i++)
    {
	err = ProSectionEntityDelete(section,ent_ids[i]);
        TEST_CALL_REPORT("ProSectionEntityDelete()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);
    }

    err = ProArrayFree( (ProArray *) &dim_ids);
    TEST_CALL_REPORT("ProArrayFree()",
		     "ProUtilSectionClean()", err, err != PRO_TK_NO_ERROR);

    return(err);
}



/*====================================================================*\
FUNCTION : ProUtilStdSectInfoFromElemTreeGet
PURPOSE  : Nicely gets the pertinent section info from feat elem tree;
\*====================================================================*/
ProError ProUtilStdSectInfoFromElemTreeGet(ProElement std_sect_elem,
                                        ProTestSectionInfo *secinfo)
{
    ProError status;
    ElemtreeElement *elements;
    int elements_num, i;


    status = ProUtilCollectElemtreeElements (std_sect_elem, NULL, &elements);
    if (status == PRO_TK_NO_ERROR)
    {
        status = ProArraySizeGet ((ProArray)elements, &elements_num);
        TEST_CALL_REPORT( "ProArraySizeGet()",
            "ProUtilStdSectInfoFromElemTreeGet()",
            status, status != PRO_TK_NO_ERROR );
        for (i = 0; i < elements_num; i++)
        {
            status = ProStdSectVisitAction (std_sect_elem,
                elements[i].p_element,
                elements[i].p_elempath, (ProAppData)secinfo);
        }
        status = ProUtilElemtreeElementArrayFree (&elements);
    }
    return(status);
}



/*====================================================================*\
FUNCTION : ProStdSectVisitAction
PURPOSE  : Visit function for getting std section information
\*====================================================================*/
ProError ProStdSectVisitAction(ProElement tree,ProElement element,
                               ProElempath elem_path,
                               ProAppData data)
{
    ProElemId elem_id;
    ProError err;
    ProTestSectionInfo *secinfo;


    err = ProElementIdGet(element,&elem_id);
    TEST_CALL_REPORT("ProElementIdGet",
                     "ProStdSectVisitAction",
                     err, err != PRO_TK_NO_ERROR);

    secinfo = (ProTestSectionInfo *)data;

    switch(elem_id)
    {
         case PRO_E_SKETCHER:
              err = ProUtilElementDataGet(element,NULL,0,
                                          &secinfo->section);
              break;
         case PRO_E_STD_SEC_PLANE:
              err = ProUtilElementDataGet(element,NULL,0,
                                          &secinfo->sketch_pln);
              break;
         case PRO_E_STD_SEC_PLANE_VIEW_DIR:
              err = ProUtilElementDataGet(element,NULL,0,
                                          &secinfo->view_dir);
              break;
         case PRO_E_STD_SEC_PLANE_ORIENT_DIR:
              err = ProUtilElementDataGet(element,NULL,0,
                                          &secinfo->orient_dir);
              break;
         case PRO_E_STD_SEC_PLANE_ORIENT_REF:
              err = ProUtilElementDataGet(element,NULL,0,
                                          &secinfo->orient_pln);
              break;
     }

     return(err);
}



/*====================================================================*\
FUNCTION : ProUtilElementDataGet
PURPOSE  : Gets element single value data given tree and path
\*====================================================================*/
ProError ProUtilElementDataGet(ProElement tree,ProElempathItem *path_items,
                               int path_size,void *data)
{
    ProElempath elem_path;
    ProElement element;
    ProError err;
    ProValue value;
    ProValueData vdata;
    int *i;
    double *d;
    void **p;
    char **s;
    wchar_t **w;
    ProSelection *r;
    ProValueDataType data_type;

    if (path_items != NULL)
    {
        err = ProElempathAlloc(&elem_path);
        TEST_CALL_REPORT("ProElempathAlloc",
                     "ProUtilElementDataGet",
                     err,err != PRO_TK_NO_ERROR);
        err = ProElempathDataSet(elem_path,path_items,path_size);
        TEST_CALL_REPORT("ProElempathDataSet",
                     "ProUtilElementDataGet",
                     err,err != PRO_TK_NO_ERROR);
        err = ProElemtreeElementGet(tree,elem_path,
                                &element);
        TEST_CALL_REPORT("ProElemtreeElementGet",
                     "ProUtilElementDataGet",err,
                     err != PRO_TK_NO_ERROR);
    }
    else element = tree;

    err = ProElementValuetypeGet(element,&data_type);
    TEST_CALL_REPORT("ProElementValuetypeGet",
                     "ProUtilElementDataGet",err,
                     err != PRO_TK_NO_ERROR);

    if (err != PRO_TK_NO_ERROR)
    {
        *(char*)data = (char)NULL;
        return(PRO_TK_GENERAL_ERROR);
    }

    switch(data_type)
    {
        case PRO_VALUE_TYPE_INT:
	  ProElementIntegerGet(element, NULL, (int*)data);
                break;
        case PRO_VALUE_TYPE_DOUBLE:
	  ProElementDoubleGet(element, NULL, (double*)data);
                break;
        case PRO_VALUE_TYPE_POINTER:
                ProElementSpecialvalueGet(element, NULL, (ProAppData*)data);
                break;
        case PRO_VALUE_TYPE_STRING:
	  ProElementStringGet(element, NULL, (char**)data);
                break;
        case PRO_VALUE_TYPE_WSTRING:
	  ProElementWstringGet(element, NULL, (wchar_t**)data);
                break;
        case PRO_VALUE_TYPE_SELECTION:
	  ProElementReferenceGet(element, NULL, (ProReference*)data);
                break;
    }

    if (path_items != NULL)
    {
        err = ProElempathFree(&elem_path);
        TEST_CALL_REPORT("ProElempathFree","ProUtilElementDataGet",
                      err, err != PRO_TK_NO_ERROR);
    }

    return(err);
}

/*====================================================================*\
FUNCTION : ProUtilUseEdge
PURPOSE  : Create "TkSect/-Use Edge/" menu
\*====================================================================*/
int ProUtilUseEdge(ProAppData app_dt, int act)
{   
    int                 ret;
    ProError            status;
    ProUtilMenuButtons  menu [] = 
    {
        {"UseEdge", -1, 0},
            {"-Use Edge",       mnUSE_EDGE,         0},
            {"-SelEdgeChain",   mnUSE_EDGE_CHAIN,   0},
            {"-SelEdgeLoop",    mnUSE_EDGE_LOOP,    0},
            {"-SelCurveChain",  mnUSE_CURVE_CHAIN,  0},
            {"-SelCurveLoop",   mnUSE_CURVE_LOOP,   0},
            {"-Done",           mnUSE_DONE,         0},
        {"",-1, -1}
    };
    ProSection          section = (ProSection)app_dt;

    while(1==1)
    {
        status = ProUtilMenuIntValueSelect (menu, &ret);
    
        if (status != PRO_TK_NO_ERROR)
            break;
    
        switch (ret)
        {
        case mnUSE_EDGE : 
            ret = ProUtilSecEntityUseEdge(section);
            break;
        case mnUSE_EDGE_CHAIN: 
            ret = ProUtilSecEntityUseEdgeChain(section);
            break;
        case mnUSE_EDGE_LOOP : 
            ret = ProUtilSecEntityUseEdgeLoop(section);
            break;
        case mnUSE_CURVE_LOOP:
            ret = ProUtilSecEntityUseCurveLoop (section);
            break;
        case mnUSE_CURVE_CHAIN:
            ret = ProUtilSecEntityUseCurveChain (section);
            break;
        case mnUSE_DONE:
            ProTestSectionSolveRegen (section);
            return 0;
        }
        
        if (ret == -1)
            break;
    }
    return (-1);
}

/*====================================================================*\
FUNCTION : ProUtilSecEntityUseEdge
PURPOSE  : Simulates behavior of Sketcer/Geom Tools/Use Edge/Sel Edge
\*====================================================================*/
int ProUtilSecEntityUseEdge(ProSection section)

{
    int             r_ent_id, sel_count, i;
    ProSelection    *ref_edge, sel;
    ProError        status;

    ProUtilMsgPrint ((char *)"sec", (char *)"TEST %0s",(char *) "Select edge.");
    
    status = ProSelect((char *)"edge", -1,NULL,NULL,NULL,NULL, &ref_edge, &sel_count);
    TEST_CALL_REPORT("ProSelection","ProUtilSecEntityUseEdge",
        status, status != PRO_TK_NO_ERROR);
    if (status!=PRO_TK_NO_ERROR || sel_count < 1)
        return -1;
    
    for (i=0; i<sel_count; i++)
    {
        status = ProSelectionCopy (ref_edge[i], &sel);
        TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseEdge",
            status, status != PRO_TK_NO_ERROR);

        status = ProSectionEntityUseEdge(section, sel, &r_ent_id);
        TEST_CALL_REPORT("ProSectionEntityUseEdge","ProUtilSecEntityUseEdge",
                status, status != PRO_TK_NO_ERROR);
        }

    return(0);
}

/*====================================================================*\
FUNCTION : ProUtilSecEntityUseEdgeLoop
PURPOSE  : Simulates behavior of Sketcer/Geom Tools/Use Edge/Sel Loop
\*====================================================================*/
int ProUtilSecEntityUseEdgeLoop(ProSection section)
{
    int             r_ent_id, sel_count, i;
    ProSelection    *sel_ptr = NULL, sel1, sel2;
    ProError        status;
    ProIntlist      p_id_list;

    ProUtilMsgPrint ((char *)"sec", (char *)"TEST %0s", (char *)"Select surface.");
    
    status = ProSelect((char *)"surface",1,NULL,NULL,NULL,NULL, &sel_ptr, &sel_count);
    TEST_CALL_REPORT("ProSelection","ProUtilSecEntityUseEdgeLoop",
        status, status != PRO_TK_NO_ERROR);

    if (status!=PRO_TK_NO_ERROR || sel_count < 1)
        return -1;
        
    status = ProSelectionCopy (*sel_ptr, &sel1);
    TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseEdgeLoop",
        status, status != PRO_TK_NO_ERROR);
    
    ProUtilMsgPrint ((char *)"sec", (char *)"TEST %0s",
        (char *)"Select edge belonging to the contour of the surface.");
    
    status = ProSelect((char*)"edge", 1,NULL,NULL,NULL,NULL, &sel_ptr, &sel_count);
    TEST_CALL_REPORT("ProSelection","ProUtilSecEntityUseEdgeLoop",
        status, status != PRO_TK_NO_ERROR);

    if (status!=PRO_TK_NO_ERROR || sel_count < 1)
        return -1;
    
    status = ProSelectionCopy (sel_ptr[0], &sel2);
    TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseEdgeLoop",
        status, status != PRO_TK_NO_ERROR);

    status = ProSectionEntityUseEdgeLoop (
        section, sel1, sel2, &p_id_list, &r_ent_id);
        TEST_CALL_REPORT("ProSectionEntityUseEdgeLoop",
            "ProUtilSecEntityUseEdgeLoop",
            status, status != PRO_TK_NO_ERROR);
    if (status == PRO_TK_NO_ERROR)
    {
        ProTKPrintf("Edge loop IDs:");
        for (i = 0; i < r_ent_id; i++)
        {
            ProTKPrintf("%d\n", p_id_list[i]);
        }
        status = ProArrayFree((ProArray*)&p_id_list);
        TEST_CALL_REPORT("ProArrayFree", "ProUtilSecEntityUseEdgeLoop",
            status, status != PRO_TK_NO_ERROR);
    }
    return(0);
}

/*====================================================================*\
FUNCTION : ProUtilSecEntityUseEdge
PURPOSE  : Simulates behavior of Sketcer/Geom Tools/Use Edge/Sel Chain
\*====================================================================*/
int ProUtilSecEntityUseEdgeChain(ProSection section)
{   
    int             i, num, sel_count;
    char            *msg[4] = 
    {
    "Select surface from whose contour the chain of entities should be created.",
    "Select first edge delimiting the chain.",
    "Select second edge delimiting the chain.",
    "Select vertex at the end of the chain."
    };
    char            geomTypes[4][9]={"surface", "edge", "edge", "edge_end"};
    ProSelection    *sel_ptr, sels[4];
    ProError        status;
    ProIntlist       p_id_list;

    for(i=0; i<4; i++)
    {
        ProUtilMsgPrint ((char *)"sec", (char *)"TEST %0s", msg[i]);
        
        status = ProSelect (geomTypes[i], 1, NULL,NULL,NULL,NULL,
            &sel_ptr, &sel_count);
        TEST_CALL_REPORT("ProSelection",
            "ProUtilSecEntityUseEdgeChain",
            status, status != PRO_TK_NO_ERROR);

        if (status!=PRO_TK_NO_ERROR || sel_count < 1)
            return -1;
        
        status = ProSelectionCopy (*sel_ptr, &sels[i]);
        TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseEdgeChain",
            status, status != PRO_TK_NO_ERROR);

    }

    status = ProSectionEntityUseEdgeChain (section, sels[0], sels[1],
        sels[2], sels[3], &p_id_list, &num);
    TEST_CALL_REPORT("ProSectionEntityUseEdgeChain",
     "ProUtilSecEntityUseEdgeChain", status, status != PRO_TK_NO_ERROR);
         
    if (status == PRO_TK_NO_ERROR) // API is for 3D section
    {
        ProTKPrintf("Edge chain IDs:");
        for (i = 0; i < num; i++)
        {
            ProTKPrintf("%d\n", p_id_list[i]);
        }
        status = ProArrayFree((ProArray*)&p_id_list);
        TEST_CALL_REPORT("ProArrayFree", "ProUtilSecEntityUseEdgeLoop",
            status, status != PRO_TK_NO_ERROR);
    }

    return(0);
}

/*====================================================================*\
FUNCTION : ProUtilSecEntityUseCurveLoop
PURPOSE  : Adds loop of entities to the specified section creating them
   from projections of 3D curve segments (datum curve, IGES, or pipe
   segments can be selected) .
\*====================================================================*/
int ProUtilSecEntityUseCurveLoop (ProSection section)
{
    int             num_ids, sel_count;
    ProIntlist      id_list;
    ProSelection    *sel_ptr, sel;
    ProError        status;
    
    ProUtilMsgPrint ("sec", "TEST %0s",
        "Select curve belonging to the contour from which" 
        "the loop of entities should be created.");
    
    status = ProSelect((char*)"curve", 1,NULL,NULL,NULL,NULL, &sel_ptr, &sel_count);
    TEST_CALL_REPORT("ProSelection","ProUtilSecEntityUseCurveLoop",
        status, status != PRO_TK_NO_ERROR);
    
    if (status!=PRO_TK_NO_ERROR || sel_count < 1)
        return -1;
    
    status = ProSelectionCopy (sel_ptr[0], &sel);
    TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseCurveLoop",
        status, status != PRO_TK_NO_ERROR);
    
    status = ProSectionEntityUseCurveLoop (
        section, sel, &id_list, &num_ids);
        TEST_CALL_REPORT("ProSectionEntityUseCurveLoop",
            "ProUtilSecEntityUseCurveLoop", status, status != PRO_TK_NO_ERROR);
    return (0);
}

/*====================================================================*\
FUNCTION : ProUtilSecEntityUseCurveChain
PURPOSE  : AAdds chain of entities to the specified section creating them
   from projections of 3D curve segments (datum curve, IGES, or pipe
   segments can be selected) .
\*====================================================================*/
int ProUtilSecEntityUseCurveChain (ProSection section)
{
    int             num_ids, sel_count;
    ProIntlist      id_list;
    ProSelection    *sel_ptr,sel1, sel2, sel3;
    ProError        status;
    
    ProUtilMsgPrint ((char *)"sec", (char *)"TEST %0s", (char *)"Select first curve.");
    status = ProSelect((char *)"curve",1,NULL,NULL,NULL,NULL, &sel_ptr, &sel_count);
    TEST_CALL_REPORT("ProSelection", "ProUtilSecEntityUseCurveChain",
        status, status != PRO_TK_NO_ERROR);
    
    if (status!=PRO_TK_NO_ERROR || sel_count < 1)
        return -1;
    
    status = ProSelectionCopy (*sel_ptr, &sel1);
    TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseCurveChain",
        status, status != PRO_TK_NO_ERROR);
        
    ProUtilMsgPrint ("sec", "TEST %0s", "Select second curve.");
    status = ProSelect((char*)"curve",1,NULL,NULL,NULL,NULL, &sel_ptr, &sel_count);
    TEST_CALL_REPORT("ProSelection", "ProUtilSecEntityUseCurveChain",
        status, status != PRO_TK_NO_ERROR);

    if (status!=PRO_TK_NO_ERROR || sel_count < 1)
        return -1;
    
    status = ProSelectionCopy (*sel_ptr, &sel2);
    TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseCurveChain",
        status, status != PRO_TK_NO_ERROR);

    ProUtilMsgPrint ("sec", "TEST %0s", "Select vertex at end of the chain.");
    status = ProSelect((char*)"curve_end",1,NULL,NULL,NULL,NULL, &sel_ptr, &sel_count);
    TEST_CALL_REPORT("ProSelection", "ProUtilSecEntityUseCurveChain",
        status, status != PRO_TK_NO_ERROR);
    
    if (status!=PRO_TK_NO_ERROR || sel_count < 1)
        return -1;

    status = ProSelectionCopy (*sel_ptr, &sel3);
    TEST_CALL_REPORT("ProSelectionCopy","ProUtilSecEntityUseCurveChain",
        status, status != PRO_TK_NO_ERROR);

    status = ProSectionEntityUseCurveChain (
        section, sel1, sel2, sel3, &id_list, &num_ids);
        TEST_CALL_REPORT("ProSectionEntityUseCurveChain",
            "ProUtilSecEntityUseCurveChain", status, status != PRO_TK_NO_ERROR);
    return (0);
}

/*====================================================================*\
    FUNCTION :  ProUtilInitializeSketch
    PURPOSE  :  Initialize Sketchch information structure
\*====================================================================*/
ProError ProUtilInitializeSketch(SketchInfo *sketch_info)
{
     ProError status; 
     sketch_info->sket_feature.id = PRO_VALUE_UNUSED; 
     sketch_info->solid = NULL;
     sketch_info->prim_surface = NULL; 
     sketch_info->ref1.type = GEOM_UNUSED; 
     sketch_info->ref2.type = GEOM_UNUSED; 
     sketch_info->orient_dir = PRO_VALUE_UNUSED;
     sketch_info->orient_view_dir = PRO_VALUE_UNUSED;

     status = ProArrayAlloc(0, sizeof(SectionConstraintInfo), 1, 
        (ProArray *)&sketch_info->constraints_info);     
     status = ProArrayAlloc(0, sizeof(ProEdge), 1, 
        (ProArray *)&sketch_info->edges);
             
     status = ProArrayAlloc(0, sizeof(Pro2dLinedef), 1, 
        (ProArray *)&sketch_info->line_entity);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->line_ids);
        
     status = ProArrayAlloc(0, sizeof(Pro2dClinedef), 1, 
        (ProArray *)&sketch_info->cline_entity);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->cline_ids);
        
     status = ProArrayAlloc(0, sizeof(Pro2dArcdef), 1, 
        (ProArray *)&sketch_info->arc_entity);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->arc_ids);
        
     status = ProArrayAlloc(0, sizeof(Pro2dConicdef), 1, 
        (ProArray *)&sketch_info->conic_entity);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->conic_ids);
        
     status = ProArrayAlloc(0, sizeof(Pro2dCircledef), 1,
        (ProArray *)&sketch_info->circle_entity);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->circle_ids);

     status = ProArrayAlloc(0, sizeof(Pro2dPolylinedef), 1,
        (ProArray *)&sketch_info->poly_line_ent);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->poly_line_ids);

     status = ProArrayAlloc(0, sizeof(Pro2dSplinedef), 1, 
        (ProArray *)&sketch_info->spline_ent);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->spline_ids);
        
     status = ProArrayAlloc(0, sizeof(Pro2dEllipsedef), 1, 
        (ProArray *)&sketch_info->ellipse_ent);
     status = ProArrayAlloc(0, sizeof(int), 1, 
        (ProArray *)&sketch_info->ellipse_ids);
     return status;
}

/*====================================================================*\
    FUNCTION :  ProUtilFreeSketchInfo
    PURPOSE  :  Frees the Sketchch information structure
\*====================================================================*/
ProError ProUtilFreeSketchInfo(SketchInfo *sketch_info)
{
     ProError status; 
     status = ProArrayFree((ProArray *)&sketch_info->constraints_info);     
     status = ProArrayFree((ProArray *)&sketch_info->edges);
             
     status = ProArrayFree((ProArray *)&sketch_info->line_entity);
     status = ProArrayFree((ProArray *)&sketch_info->line_ids);
        
     status = ProArrayFree((ProArray *)&sketch_info->cline_entity);
     status = ProArrayFree((ProArray *)&sketch_info->cline_ids);
        
     status = ProArrayFree((ProArray *)&sketch_info->arc_entity);
     status = ProArrayFree((ProArray *)&sketch_info->arc_ids);
        
     status = ProArrayFree((ProArray *)&sketch_info->conic_entity);
     status = ProArrayFree((ProArray *)&sketch_info->conic_ids);
        
     status = ProArrayFree((ProArray *)&sketch_info->circle_entity);
     status = ProArrayFree((ProArray *)&sketch_info->circle_ids);

     status = ProArrayFree((ProArray *)&sketch_info->poly_line_ent);
     status = ProArrayFree((ProArray *)&sketch_info->poly_line_ids);

     status = ProArrayFree((ProArray *)&sketch_info->spline_ent);
     status = ProArrayFree((ProArray *)&sketch_info->spline_ids);
        
     status = ProArrayFree((ProArray *)&sketch_info->ellipse_ent);
     status = ProArrayFree((ProArray *)&sketch_info->ellipse_ids);        
     return status;
}

/*====================================================================*\
    FUNCTION :  ProUtilCopySectionEntities
    PURPOSE  :  Copies all the section information from a section
                and stores in the Sketch information structure 
\*====================================================================*/
ProError ProUtilCopySectionEntities(ProSection *section, SketchInfo *sketch_info)
{
     ProError status;
     int i = 0, n_ids = 0;
     ProIntlist      p_id_list;
     Pro2dEntdef *entity;
           
     Pro2dLinedef   line_entity;
     Pro2dArcdef    arc_entity;
     Pro2dCircledef circle_entity;
     Pro2dConicdef conic_entity;
     Pro2dPolylinedef poly_line_ent; 
     Pro2dSplinedef   spline_ent;
     Pro2dEllipsedef  ellipse_ent; 
     Pro2dPnt	*point_arr;
     ProEdge *test_edges;

     status = ProSectionEntityIdsGet(*section, &p_id_list, &n_ids);
     TEST_CALL_REPORT("ProSectionEntityIdsGet", "ProUtilCopySectionEntities",  
			    status, status != PRO_TK_NO_ERROR );                      
     if(status != PRO_TK_NO_ERROR)
            return status;
     /* 
        First two entities are the 2D-Entities
        created from the references
     */
     
     for(i=0; i<n_ids; i++) 
     {
     status = ProSectionEntityGet(*section, p_id_list[i],
        (Pro2dEntdef **)&entity);
     TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilCopySectionEntities",  
			    status, status != PRO_TK_NO_ERROR );                      
     switch (entity ->type) 
        {
        case  PRO_2D_LINE :
           status = ProUtilCopy2dEntity(entity, (Pro2dEntdef *)&line_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->line_entity,
              PRO_VALUE_UNUSED, 1, &line_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->line_ids,
              PRO_VALUE_UNUSED, 1, &p_id_list[i]);
           break; 
        case  PRO_2D_ARC  :
           status = ProUtilCopy2dEntity(entity, (Pro2dEntdef *)&arc_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->arc_entity,
              PRO_VALUE_UNUSED, 1, &arc_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->arc_ids,
              PRO_VALUE_UNUSED, 1, &p_id_list[i]);
           break;
        case  PRO_2D_CONIC  :
           status = ProUtilCopy2dEntity(entity, (Pro2dEntdef *)&conic_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->conic_entity,
              PRO_VALUE_UNUSED, 1, &conic_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->conic_ids,
              PRO_VALUE_UNUSED, 1, &p_id_list[i]);              
           break;
        case  PRO_2D_CIRCLE  :
           status = ProUtilCopy2dEntity(entity, (Pro2dEntdef *)&circle_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->circle_entity,
              PRO_VALUE_UNUSED, 1, &circle_entity);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->circle_ids,
              PRO_VALUE_UNUSED, 1, &p_id_list[i]);              
           break;
        case  PRO_2D_SPLINE :
           status = ProUtilCopy2dEntity(entity, (Pro2dEntdef *)&spline_ent);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->spline_ent, 
              PRO_VALUE_UNUSED, 1, &spline_ent);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->spline_ids,
              PRO_VALUE_UNUSED, 1, &p_id_list[i]);              
           break;
        case  PRO_2D_ELLIPSE :
           status = ProUtilCopy2dEntity(entity, (Pro2dEntdef *)&ellipse_ent);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->ellipse_ent,
              PRO_VALUE_UNUSED, 1, &ellipse_ent);
           status = ProArrayObjectAdd(
              (ProArray*)&sketch_info->ellipse_ids,
              PRO_VALUE_UNUSED, 1, &p_id_list[i]);              
           break;
        default :              
           break;
        }
    }
    return status;
}

/*===============================================================*\
FUNCTION: ProUtilAddSectionEntities
PURPOSE:  Add section entities from the sketch information
\*===============================================================*/
ProError ProUtilAddSectionEntities(ProSection section, SketchInfo *sketch_info)   
{ 	
    ProError status;
    ProSelection sel_1, sel_2, cpy_sel;
    ProMdl model;
    int ent_id, edge_num, ent_id1;
    Pro2dEntdef *entity;
    int          i, r_ent_id;
    ProIntlist      p_id_list;

    Pro2dLinedef   line_entity;
    Pro2dArcdef    arc_entity;
    Pro2dCircledef circle_entity;
    Pro2dConicdef conic_entity;
    Pro2dPolylinedef poly_line_ent; 
    Pro2dSplinedef   spline_ent;
    Pro2dEllipsedef  ellipse_ent; 
    Pro2dPnt	*point_arr;
    ProEdge *test_edges;
    int item_nums;

    status = ProArraySizeGet ((ProArray)sketch_info->line_entity,
       &item_nums);
    for(i = 0; i < item_nums; i++)
    {
        status = ProSectionEntityAdd(section,
          (Pro2dEntdef *)&sketch_info->line_entity[i], &ent_id1); 
    }
    status = ProArraySizeGet ((ProArray)sketch_info->arc_entity,
       &item_nums);
    for(i = 0; i < item_nums; i++)
    {
        status = ProSectionEntityAdd(section,
          (Pro2dEntdef *)&sketch_info->arc_entity[i], &ent_id1); 
    }
    status = ProArraySizeGet ((ProArray)sketch_info->conic_entity,
       &item_nums);
    for(i = 0; i < item_nums; i++)
    {
        status = ProSectionEntityAdd(section, 
          (Pro2dEntdef *)&sketch_info->conic_entity[i], &ent_id1); 
    }
    status = ProArraySizeGet ((ProArray)sketch_info->circle_entity,
       &item_nums);
    for(i = 0; i < item_nums; i++)
    {
        status = ProSectionEntityAdd(section, 
          (Pro2dEntdef *)&sketch_info->circle_entity[i], &ent_id1); 
    }
    status = ProArraySizeGet ((ProArray)sketch_info->ellipse_ent, 
       &item_nums);
    for(i = 0; i < item_nums; i++)
    {
        status = ProSectionEntityAdd(section, 
          (Pro2dEntdef *)&sketch_info->ellipse_ent[i], &ent_id1); 
    }
    status = ProArraySizeGet ((ProArray)sketch_info->spline_ent,
       &item_nums);
    for(i = 0; i < item_nums; i++)
    {
        status = ProSectionEntityAdd(section,
          (Pro2dEntdef *)&sketch_info->spline_ent[i], &ent_id1); 
    }
    status = ProArraySizeGet ((ProArray)sketch_info->poly_line_ent,
       &item_nums);
    for(i = 0; i < item_nums; i++)
    {
        status = ProSectionEntityAdd(section,
          (Pro2dEntdef *)&sketch_info->poly_line_ent[i], &ent_id1); 
    }
    return status;
}

/*====================================================================*\
    FUNCTION :  ProUtilCopy2dEntity
    PURPOSE  :  Utility to copy the 2d entity
\*====================================================================*/
ProError ProUtilCopy2dEntity(Pro2dEntdef *entity, Pro2dEntdef *copy_entity )
{
    int       j,no_points;
    ProError  status;

    Pro2dLinedef *line_entity;
    Pro2dLinedef *o_line_entity;

    Pro2dClinedef *cline_entity;
    Pro2dClinedef *o_cline_entity;

    Pro2dArcdef *arc_entity;
    Pro2dArcdef *o_arc_entity;

    Pro2dConicdef *conic_entity;
    Pro2dConicdef *o_conic_entity;

    Pro2dCircledef *circle_entity;
    Pro2dCircledef *o_circle_entity;

    Pro2dSplinedef *spline_entity;
    Pro2dSplinedef *o_spline_entity;

    Pro2dEllipsedef *ellipse_entity;
    Pro2dEllipsedef *o_ellipse_entity;

    switch (entity ->type) 
    {
        case PRO_2D_LINE:
           line_entity = (Pro2dLinedef *)copy_entity;
           o_line_entity = (Pro2dLinedef *)entity;
           memcpy( line_entity, o_line_entity, sizeof(Pro2dLinedef) );
           break;
        case PRO_2D_CENTER_LINE:
           cline_entity = (Pro2dClinedef *)copy_entity;
           o_cline_entity = (Pro2dClinedef *)entity;
           memcpy( cline_entity, o_cline_entity, sizeof(Pro2dClinedef) );
           break;
        case PRO_2D_ARC:
           arc_entity = (Pro2dArcdef *)copy_entity;
           o_arc_entity = (Pro2dArcdef *)entity;
           memcpy( arc_entity, o_arc_entity, sizeof(Pro2dArcdef) );
           break;
        case PRO_2D_CONIC:
           conic_entity = (Pro2dConicdef *)copy_entity;
           o_conic_entity = (Pro2dConicdef *)entity;
           memcpy( conic_entity, o_conic_entity, sizeof(Pro2dConicdef) );
           break;
  
        case PRO_2D_CIRCLE:
           circle_entity = (Pro2dCircledef *)copy_entity;
           o_circle_entity = (Pro2dCircledef *)entity;
           memcpy( circle_entity, o_circle_entity, sizeof(Pro2dCircledef) );
           break;
  
        case PRO_2D_ELLIPSE:
           ellipse_entity = (Pro2dEllipsedef *)copy_entity;
           o_ellipse_entity = (Pro2dEllipsedef *)entity;
           memcpy( ellipse_entity, o_ellipse_entity, sizeof(Pro2dEllipsedef) );
           break;
  
        case PRO_2D_SPLINE:
           spline_entity = (Pro2dSplinedef *)copy_entity;
           o_spline_entity = (Pro2dSplinedef *)entity;
           //memcpy( spline_entity, o_spline_entity, sizeof(Pro2dSplinedef) );
           no_points = o_spline_entity->n_points;
           status = ProArrayAlloc( no_points, sizeof(Pro2dPnt), no_points, 
              (ProArray*)&spline_entity->point_arr );
           spline_entity->type = PRO_2D_SPLINE;
           spline_entity->tangency_type = o_spline_entity->tangency_type;
           spline_entity->start_tang_angle = o_spline_entity->start_tang_angle;
           spline_entity->end_tang_angle = o_spline_entity->end_tang_angle;
           spline_entity->n_points = o_spline_entity->n_points;
           for (j=0; j< no_points; j++)
              {
              spline_entity->point_arr[j][0] = o_spline_entity->point_arr[j][0];
              spline_entity->point_arr[j][1] = o_spline_entity->point_arr[j][1];
              }
           break;
        default :
           break;
    }
    return PRO_TK_NO_ERROR;
}

/*====================================================================*\
    FUNCTION :  ProUtilGetGeomItemToRef
    PURPOSE  :  Utility to set the geometry reference
                (It can be expanded further)     
\*====================================================================*/
ProError ProUtilGetGeomItemToRef(ProGeomitem *geom_item, FeatRef *ref)
{
    ProError status = PRO_TK_NO_ERROR;
    ProSurface surface;
    ProEdge edge;
    int id;

    if(geom_item->type == PRO_SURFACE)
    {
        status = ProGeomitemToSurface(geom_item, &surface);
        status = ProSurfaceIdGet(surface, &id);       
        TEST_CALL_REPORT("ProSurfaceIdGet", "ProUtilGetGeomItemToRef",  
			    status, status != PRO_TK_NO_ERROR );                      
        ref->type = GEOM_SURFACE;
    }
    else if(geom_item->type == PRO_EDGE)
    {
        status = ProGeomitemToEdge(geom_item, &edge);
        status = ProEdgeIdGet(edge, &id);       
        TEST_CALL_REPORT("ProEdgeIdGet", "ProUtilGetGeomItemToRef",  
			    status, status != PRO_TK_NO_ERROR );                      
        ref->type = GEOM_EDGE;
    }
    else
        return PRO_TK_NOT_IMPLEMENTED;
  
    if(PRO_TK_NO_ERROR == status)
        ref->id = id;
    return status;  
}

/*====================================================================*\
    FUNCTION :  ProUtilSectionEntityPropCheck
    PURPOSE  :  Verify the two section entities have same
                geometry or not 
\*====================================================================*/
ProError ProUtilSectionEntityPropCheck(Pro2dEntdef *entity_1,
                                 Pro2dEntdef *entity_2,
                                 ProBoolean *isSame)
{
    ProError status = PRO_TK_NO_ERROR;
    int i = 0, j, no_points = 0;
    ProIntlist      p_id_list;
    Pro2dEntdef *entity;
           
    Pro2dLinedef *line_entity;
    Pro2dLinedef *o_line_entity;

    Pro2dClinedef *cline_entity;
    Pro2dClinedef *o_cline_entity;

    Pro2dArcdef *arc_entity;
    Pro2dArcdef *o_arc_entity;

    Pro2dConicdef *conic_entity;
    Pro2dConicdef *o_conic_entity;

    Pro2dCircledef *circle_entity;
    Pro2dCircledef *o_circle_entity;

    Pro2dSplinedef *spline_entity;
    Pro2dSplinedef *o_spline_entity;

    Pro2dEllipsedef *ellipse_entity;
    Pro2dEllipsedef *o_ellipse_entity;

    Pro2dPnt	*point_arr;
    ProEdge *test_edges;

    *isSame = PRO_B_FALSE;
    if((entity_1->type) != (entity_2->type))
        return PRO_TK_NO_ERROR;
         
     switch (entity_1->type) 
        {
        case PRO_2D_LINE:
           line_entity = (Pro2dLinedef *)entity_1;
           o_line_entity = (Pro2dLinedef *)entity_2;
           memcpy( line_entity, o_line_entity, sizeof(Pro2dLinedef) );           
           if(
              (PRO_B_TRUE == POINT_CMP(line_entity->end1, o_line_entity->end1)) &&
              (PRO_B_TRUE == POINT_CMP(line_entity->end2, o_line_entity->end2))
             )
             {
               *isSame = PRO_B_TRUE;
             }
           break;
        case PRO_2D_CENTER_LINE:
           cline_entity = (Pro2dClinedef *)entity_1;
           o_cline_entity = (Pro2dClinedef *)entity_2;
           if(
              (PRO_B_TRUE == POINT_CMP(cline_entity->end1, o_cline_entity->end1)) &&
              (PRO_B_TRUE == POINT_CMP(cline_entity->end2, o_cline_entity->end2))
             )
             {
               *isSame = PRO_B_TRUE;
             }
             break;
        case PRO_2D_ARC:
           arc_entity = (Pro2dArcdef *)entity_1;
           o_arc_entity = (Pro2dArcdef *)entity_2;
           if(
              (PRO_B_TRUE == POINT_CMP(arc_entity->center, o_arc_entity->center)) &&
              (PRO_B_TRUE == FLOAT_CMP(arc_entity->start_angle, o_arc_entity->start_angle)) &&
              (PRO_B_TRUE == FLOAT_CMP(arc_entity->end_angle, o_arc_entity->end_angle)) &&
              (PRO_B_TRUE == FLOAT_CMP(arc_entity->radius, o_arc_entity->radius))
             )                            
             {
               *isSame = PRO_B_TRUE;
             }
           break;
        case PRO_2D_CONIC:
           conic_entity = (Pro2dConicdef *)entity_1;
           o_conic_entity = (Pro2dConicdef *)entity_2;
           if(
              (PRO_B_TRUE == POINT_CMP(conic_entity->first_end_point, o_conic_entity->first_end_point)) &&
              (PRO_B_TRUE == POINT_CMP(conic_entity->second_end_point, o_conic_entity->second_end_point)) &&
              (PRO_B_TRUE == POINT_CMP(conic_entity->shoulder_point, o_conic_entity->shoulder_point)) &&
              (PRO_B_TRUE == FLOAT_CMP(conic_entity->parameter, o_conic_entity->parameter)) 
             )
             {
               *isSame = PRO_B_TRUE;
             }
           break;
        case PRO_2D_CIRCLE:
           circle_entity = (Pro2dCircledef *)entity_1;
           o_circle_entity = (Pro2dCircledef *)entity_2;
           if(
              (PRO_B_TRUE == POINT_CMP(circle_entity->center, o_circle_entity->center)) &&
              (PRO_B_TRUE == FLOAT_CMP(circle_entity->radius, o_circle_entity->radius)) 
             )
             {
               *isSame = PRO_B_TRUE;
             }
           break;
        case PRO_2D_ELLIPSE:
           ellipse_entity = (Pro2dEllipsedef *)entity_1;
           o_ellipse_entity = (Pro2dEllipsedef *)entity_2;
           if(
              (PRO_B_TRUE == POINT_CMP(ellipse_entity->origin, o_ellipse_entity->origin)) &&
              (PRO_B_TRUE == FLOAT_CMP(ellipse_entity->x_radius, o_ellipse_entity->x_radius)) &&
              (PRO_B_TRUE == FLOAT_CMP(ellipse_entity->y_radius, o_ellipse_entity->y_radius))                           
             )
             {
               *isSame = PRO_B_TRUE;
             }
           break;
  
        case PRO_2D_SPLINE:
           spline_entity = (Pro2dSplinedef *)entity_1;
           o_spline_entity = (Pro2dSplinedef *)entity_2;
           //memcpy( spline_entity, o_spline_entity, sizeof(Pro2dSplinedef) );
           if(
              (PRO_B_TRUE ==
FLOAT_CMP((double)spline_entity->tangency_type,(double) o_spline_entity->tangency_type)) &&
              (PRO_B_TRUE == FLOAT_CMP(spline_entity->start_tang_angle, o_spline_entity->start_tang_angle)) &&
              (PRO_B_TRUE == FLOAT_CMP(spline_entity->end_tang_angle, o_spline_entity->end_tang_angle)) &&
              (PRO_B_TRUE == FLOAT_CMP((double)spline_entity->n_points, (double)o_spline_entity->n_points)) &&
              (PRO_B_TRUE == FLOAT_CMP(spline_entity->point_arr[0][0], o_spline_entity->point_arr[0][0]))
             ) 
             {
              /*for (j=0; j< no_points; j++)
              {
              spline_entity->point_arr[j][0] = o_spline_entity->point_arr[j][0];
              spline_entity->point_arr[j][1] = o_spline_entity->point_arr[j][1];
              }*/
               *isSame = PRO_B_TRUE;                  
             }
           break;
        default :
           break;
        }
    return status;
}

/*====================================================================*\
    FUNCTION :  ProUtilFindSketchEntity
    PURPOSE  :  Find the section entity id which has 
                same geometry info in a section  
\*====================================================================*/
ProError ProUtilFindSketchEntity(SketchInfo *sketch_info, ProSection *section, int id, int *sec_id)
{
    ProError status;
    Pro2dEntdef *old_entity;
    ProBoolean isSame = PRO_B_FALSE;
    Pro2dEntdef *new_entity;
    int i, item_nums;

    status = ProSectionEntityGet(*section, id, (Pro2dEntdef **)&old_entity);    
    TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilFindSketchEntity",  
			    status, status != PRO_TK_NO_ERROR );                      
    switch (old_entity->type)
    {
        case  PRO_2D_LINE :
            status = ProArraySizeGet ((ProArray)sketch_info->line_entity,
               &item_nums);
            for(i = 0; i<item_nums; i++)
            {
               status = ProSectionEntityGet(sketch_info->section,
                      sketch_info->line_ids[i], (Pro2dEntdef **)&new_entity);
               TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilFindSketchEntity",  
			          status, status != PRO_TK_NO_ERROR );                      
               if(status != PRO_TK_NO_ERROR)                       
                   continue;
               status = ProUtilSectionEntityPropCheck(new_entity, old_entity, &isSame);
               if((PRO_B_TRUE == isSame) && (PRO_TK_NO_ERROR == status))
                   break;
            }
           break; 
        case  PRO_2D_ARC  :
            status = ProArraySizeGet ((ProArray)sketch_info->arc_entity,
               &item_nums);
            for(i = 0; i<item_nums; i++)
            {
               status = ProSectionEntityGet(sketch_info->section,
                      sketch_info->arc_ids[i], (Pro2dEntdef **)&new_entity);
               if(status != PRO_TK_NO_ERROR)                       
                   continue;
               TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilFindSketchEntity",  
			          status, status != PRO_TK_NO_ERROR );                      
               status = ProUtilSectionEntityPropCheck(new_entity, old_entity, &isSame);
               if((PRO_B_TRUE == isSame) && (PRO_TK_NO_ERROR == status))
                   break;
            }
           break; 
        case  PRO_2D_CONIC  :
            status = ProArraySizeGet ((ProArray)sketch_info->conic_entity,
               &item_nums);
            for(i = 0; i<item_nums; i++)
            {
               status = ProSectionEntityGet(sketch_info->section,
                      sketch_info->conic_ids[i], (Pro2dEntdef **)&new_entity);
               if(status != PRO_TK_NO_ERROR)                       
                   continue;                      
               TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilFindSketchEntity",  
		        	    status, status != PRO_TK_NO_ERROR );                                          
               status = ProUtilSectionEntityPropCheck(new_entity, old_entity, &isSame);
               if((PRO_B_TRUE == isSame) && (PRO_TK_NO_ERROR == status))
                   break;
            }
           break; 
        case  PRO_2D_CIRCLE  :
            status = ProArraySizeGet ((ProArray)sketch_info->circle_entity,
               &item_nums);
            for(i = 0; i<item_nums; i++)
            {
               status = ProSectionEntityGet(sketch_info->section,
                      sketch_info->circle_ids[i], (Pro2dEntdef **)&new_entity);
               if(status != PRO_TK_NO_ERROR)                       
                   continue;                      
               TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilFindSketchEntity",  
		    	    status, status != PRO_TK_NO_ERROR );                                          
               status = ProUtilSectionEntityPropCheck(new_entity, old_entity, &isSame);
               if((PRO_B_TRUE == isSame) && (PRO_TK_NO_ERROR == status))
                   break;
            }
           break; 
        case  PRO_2D_SPLINE :
            status = ProArraySizeGet ((ProArray)sketch_info->spline_ent,
               &item_nums);
            for(i = 0; i<item_nums; i++)
            {
               status = ProSectionEntityGet(sketch_info->section,
                      sketch_info->spline_ids[i], (Pro2dEntdef **)&new_entity);
               if(status != PRO_TK_NO_ERROR)                       
                   continue;                      
               TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilFindSketchEntity",  
	    		    status, status != PRO_TK_NO_ERROR );                                        
               status = ProUtilSectionEntityPropCheck(new_entity, old_entity, &isSame);
               if((PRO_B_TRUE == isSame) && (PRO_TK_NO_ERROR == status))
                   break;
            }
           break; 
        case  PRO_2D_ELLIPSE :
            status = ProArraySizeGet ((ProArray)sketch_info->ellipse_ent,
               &item_nums);
            for(i = 0; i<item_nums; i++)
            {
               status = ProSectionEntityGet(sketch_info->section,
                      sketch_info->ellipse_ids[i], (Pro2dEntdef **)&new_entity);
               if(status != PRO_TK_NO_ERROR)                       
                   continue;                      
               TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilFindSketchEntity",  
	    		    status, status != PRO_TK_NO_ERROR );                                       
               status = ProUtilSectionEntityPropCheck(new_entity, old_entity, &isSame);
               if((PRO_B_TRUE == isSame) && (PRO_TK_NO_ERROR == status))
                   break;
            }
           break; 
        default :              
           break;
    }    
    if(PRO_B_TRUE == isSame)
        return status;
    return PRO_TK_E_NOT_FOUND;
}

/*====================================================================*\
    FUNCTION :  ProUtilCopySectionConstraints
    PURPOSE  :  Utility to fill the constraints information from the 
                section  
\*====================================================================*/
ProError ProUtilCopySectionConstraints(SketchInfo *sketch_info, 
                                ProFeature *sk_feat, ProSection *section)
{
    ProError status;
    ProIntlist con_ids;
    int n_ids, n_conids, j, k, num_constraints;   
    int sec_id, constr_id; 
    int *ref_ids;
    int *new_ref_ids;   
    ProWSecerror  sec_error;    
    ProElement elem_tree_redef;
     
    // Get the section constraints info of the section
    status = ProSectionConstraintsIdsGet(*section, &con_ids, &n_conids);
    TEST_CALL_REPORT("ProSectionConstraintsIdsGet", "ProUtilCopySectionConstraints",  
			    status, status != PRO_TK_NO_ERROR );                      
    for (j=0; j<n_conids; j++)
    {
        SectionConstraintInfo constraint_info;
        ProBoolean isFound = PRO_B_TRUE;
        status = ProSectionConstraintsGet(*section, con_ids[j],
            &constraint_info.type, &constraint_info.status, &constraint_info.num_refs,
            &ref_ids , &constraint_info.p_senses);
        TEST_CALL_REPORT("ProSectionConstraintsGet", "ProUtilCopySectionConstraints",  
			    status, status != PRO_TK_NO_ERROR );                      
        if (status != PRO_TK_NO_ERROR)
            continue;
        status = ProArrayAlloc( 0, sizeof(int),
                      1, (ProArray *)&constraint_info.p_ent_ids );            
        for(k=0; k<constraint_info.num_refs; k++) 
        {
            status = ProUtilFindSketchEntity(sketch_info, section, ref_ids[k], &sec_id );
            if(status != PRO_TK_NO_ERROR)
            {
                isFound = PRO_B_FALSE;
                break;
            }
            status = ProArrayObjectAdd ((ProArray*)&constraint_info.p_ent_ids,
                   PRO_VALUE_UNUSED, 1, &sec_id);
            TEST_CALL_REPORT("ProArrayObjectAdd", "ProUtilCopySectionConstraints",  
			    status, status != PRO_TK_NO_ERROR );                      
        }
        if(isFound == PRO_B_FALSE)
        {
            /*
            status = ProArrayFree((ProArray*)&p_types);
            status = ProArrayFree((ProArray*)&ref_ids);
            status = ProArrayFree((ProArray*)&new_ref_ids);
            */
            continue;
        }
        status = ProArrayObjectAdd ((ProArray*)&sketch_info->constraints_info, 
                     -1, 1, &constraint_info);
        TEST_CALL_REPORT("ProArrayObjectAdd+", "ProUtilCopySectionConstraints",  
			    status, status != PRO_TK_NO_ERROR );                                          
        /*constraint_info.p_ent_ids*/
        /*status = ProArrayFree((ProArray*)&p_types);
        status = ProArrayFree((ProArray*)&ref_ids);*/
    }
    /*status = ProArrayFree((ProArray*)&con_ids);*/

    /* Redefing the feature to add the section entities */
    status = ProFeatureElemtreeExtract(sk_feat, NULL, PRO_FEAT_EXTRACT_NO_OPTS, &elem_tree_redef);
    status = ProArraySizeGet((ProArray)sketch_info->constraints_info, &num_constraints);
    for (j=0; j<n_conids; j++)
    {
        ProSelection *sel_array = NULL;  
        ProSelection sel_sec_ent;
        SectionConstraintInfo constraint_info = sketch_info->constraints_info[j];
        status = ProArrayAlloc(0, sizeof(ProSelection), 1,
                         (ProArray*)&sel_array);
        for(k=0; k<constraint_info.num_refs; k++)                          
        { 
            status = ProSectionEntityGetSelected (sketch_info->section, 
                          constraint_info.p_ent_ids[k], constraint_info.p_senses[k],
                          NULL, 
                          PRO_VALUE_UNUSED, &sel_sec_ent);
            if(status != PRO_TK_NO_ERROR)
                break;
            status = ProArrayObjectAdd ((ProArray*)&sel_array,
                     PRO_VALUE_UNUSED, 1, &sel_sec_ent);                          
        }                
        
        if(status == PRO_TK_NO_ERROR)
        {
            status = ProSectionConstraintCreate(sketch_info->section, 
                    sel_array, 2, constraint_info.type, &constr_id);                    
            TEST_CALL_REPORT("ProSectionConstraintCreate", "ProUtilCopySectionConstraints",  
			    status, status != PRO_TK_NO_ERROR );                      
        }
    }
  /* Free sel_array */    
  status = ProSecerrorAlloc(&sec_error);
  status = ProSectionSolve(sketch_info->section, &sec_error);
  status = ProSecerrorFree(&sec_error);
  status = ProSecerrorAlloc(&sec_error);
  status = ProSectionRegenerate(sketch_info->section, &sec_error);
  status = ProSecerrorFree(&sec_error);
  /*Set the updated section pointer back into the feature*/
  status = ProUtilSketchFeatureReset(sk_feat, &sketch_info->section);
  return status;
}

/*===============================================================*\
FUNCTION: ProUtilSketchSectionBuild
PURPOSE:  Creates a 3D section from the sketch ref, orientation 
                            and entity information
\*===============================================================*/
ProError ProUtilSketchSectionBuild (ProSection section, SketchInfo *sketch_info, 
                             ProSelection *sketch_refs)
{
    ProSelection          *proj_ents;
    ProError  			   status;	
    int                    i, num_errors, err_counter, proj_ids[2];
    ProWSecerror           sec_errors;
    Pro2dLinedef          *left_linedef, *btm_linedef;
    ProSectionPointType    pt_type[1], proj_pt_type[2];
    Pro2dArcdef            arc;
    Pro2dPnt               place_pnt;
    ProMsg                 wmsg;
    char                   msg[PRO_PATH_SIZE];
    double	   eps;
    SectionMatrix_data            matrix_data;

    /*---------------------------------------------------------------*\
    Get the projection entity handles as ProSelection structures.
    \*---------------------------------------------------------------*/
    status = CreateSketchReferences (sketch_info, &proj_ents);
    if(status != PRO_TK_NO_ERROR)
        return status;

    /*---------------------------------------------------------------*\
    Project the reference edges onto the section.
    \*---------------------------------------------------------------*/
    for (i = 0; i < 2; i++)
    {
        status = ProSectionEntityFromProjection (section, proj_ents[i],
         &proj_ids[i]);
            TEST_CALL_REPORT("ProSectionEntityFromProjection", "ProUtilSketchSectionBuild",
			    status, status != PRO_TK_NO_ERROR );                       
        if (status != PRO_TK_NO_ERROR)
            return (status);
    }

    /*---------------------------------------------------------------*\
    Create the section coordinate system from the edges. Get the 
    transformation matrix between the sketch plane coordinate system
    and the section coordinate system.
    \*---------------------------------------------------------------*/
    status = ProSectionEntityGet (section, proj_ids[0],
          (Pro2dEntdef**)&btm_linedef);
    TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilSketchSectionBuild",  
			    status, status != PRO_TK_NO_ERROR );                      
    for (i = 0; i < 2; i++)
    {
        matrix_data.x_axis[0][i] = btm_linedef->end1[i];
        matrix_data.x_axis[1][i] = btm_linedef->end2[i];
        matrix_data.x_axis[i][2] = 0.0; 
    }

    status = ProSectionEntityGet (section, proj_ids[1],
            (Pro2dEntdef**)&left_linedef);
    TEST_CALL_REPORT("ProSectionEntityGet", "ProUtilSketchSectionBuild",  
			    status, status != PRO_TK_NO_ERROR );                      
    for (i = 0; i < 2; i++)
    {
        matrix_data.y_axis[0][i] = left_linedef->end1[i]; 
        matrix_data.y_axis[1][i] = left_linedef->end2[i];
        matrix_data.y_axis[i][2] = 0.0; 
    }

    status = ProSectionLocationGet (section, matrix_data.sk_mtrx);
    TEST_CALL_REPORT("ProSectionLocationGet", "ProUtilSketchSectionBuild", 
			    status, status != PRO_TK_NO_ERROR );                      
    status = ProSelectionCopy (sketch_refs[0], &(matrix_data.sk_plane));
    TEST_CALL_REPORT("ProSelectionCopy", "ProUtilSketchSectionBuild",  
			    status, status != PRO_TK_NO_ERROR );                       
    status = UserCreateSecTrfMatrix(&matrix_data);

    status = AddProjectedEdgeSectionEntities(sketch_info->solid,
                                section, sketch_info->edges);
    
    status = ProUtilAddSectionEntities(section, sketch_info);
    if(PRO_TK_NO_ERROR != status)
        return status;

        
    /*status = ProSectionEpsilonGet(section, &eps);*/
    status = ProSectionEpsilonSet(section, sketch_info->eps_val); 
    TEST_CALL_REPORT("ProSectionEpsilonSet", "ProUtilSketchSectionBuild",  
			    status, status != PRO_TK_NO_ERROR );                        
    
    status = ProSecerrorAlloc (&sec_errors);
    TEST_CALL_REPORT("ProSectionEpsilonSet", "ProUtilSketchSectionBuild", 
			    status, status != PRO_TK_NO_ERROR );                            
    /* status = ProSectionSolve (section, &sec_errors); */ 
    status = ProSectionAutodim (section, &sec_errors);
   
    if (status != 0)
    {
        status = ProSecerrorCount (&sec_errors, &num_errors);
        for (err_counter = 0; err_counter < num_errors; err_counter++)
        {
            status = ProSecerrorMsgGet (sec_errors, err_counter, wmsg);
            ProWstringToString (msg, wmsg);
        }
    return status;
    }
    status = ProSecerrorFree (&sec_errors);
    TEST_CALL_REPORT("ProSecerrorFree", "ProUtilSketchSectionBuild",  
			    status, status != PRO_TK_NO_ERROR );                           
    status = ProSecerrorAlloc (&sec_errors);
    TEST_CALL_REPORT("ProSecerrorAlloc", "ProUtilSketchSectionBuild",  
			    status, status != PRO_TK_NO_ERROR );                              
    status = ProSectionRegenerate (section, &sec_errors);
    if (status != 0)
    {
        status = ProSecerrorCount (&sec_errors, &num_errors);
        for (err_counter = 0; err_counter < num_errors; err_counter++)
        {
            status = ProSecerrorMsgGet (sec_errors, err_counter, wmsg);
            ProWstringToString (msg, wmsg);
        }
        return status;
     }
    ProSecerrorFree (&sec_errors);
    status = ProSelectionFree (&(matrix_data.sk_plane));
    TEST_CALL_REPORT("ProSelectionFree", "ProUtilSketchSectionBuild",  
			    status, status != PRO_TK_NO_ERROR );                      
    return (status);
}

/*===============================================================*\
FUNCTION: AddProjectedEdgeSectionEntities
PURPOSE:  Add section entities from the sketch information
\*===============================================================*/
ProError AddProjectedEdgeSectionEntities(ProSolid solid, ProSection section, ProEdge *edges)   
{ 	
    ProError status;
    int num_edges;
    ProSelection sel_edge;
    int i, ent_id;

    status = ProArraySizeGet ((ProArray)edges, &num_edges);
    for(i = 0; i < num_edges; i++)
    {
        status = UtilSelectionFromEdge(solid,
             edges[i], &sel_edge);   
        status = ProSectionEntityUseEdge(section, sel_edge, &ent_id); 
    }
    return status;
}

/*===============================================================*\
FUNCTION: CreateSketchReferences
PURPOSE:  Create the selection references from the sketch information
\*===============================================================*/
ProError CreateSketchReferences (SketchInfo *sketch_info, ProSelection **proj_refs_i)
{
   ProSelection  sel_entity = NULL;
   ProError status;
   ProSelection *temp2 = NULL; 

   temp2 = (ProSelection *)calloc(2, sizeof (ProSelection));

   if(sketch_info->ref1.type == GEOM_EDGE)
      {      
      status = UtilSelectionFromEdgeId ((ProSolid)sketch_info->solid,
         sketch_info->ref1.id, &sel_entity);
      }
   else if(sketch_info->ref1.type == GEOM_SURFACE)
      {
      status = UtilSelectionFromSurfaceId ((ProSolid)sketch_info->solid,
         sketch_info->ref1.id, &sel_entity);
      }
   status = ProSelectionCopy (sel_entity, &temp2[0]);  
   status = 	ProSelectionFree( &sel_entity );

   if(sketch_info->ref2.type == GEOM_EDGE)
      {
      status = UtilSelectionFromEdgeId ((ProSolid)sketch_info->solid,
         sketch_info->ref2.id, &sel_entity);      
      }
   if(sketch_info->ref2.type == GEOM_SURFACE)
      {
      status = UtilSelectionFromSurfaceId ((ProSolid)sketch_info->solid,
         sketch_info->ref2.id, &sel_entity);
      }
   status = ProSelectionCopy (sel_entity, &temp2[1]);
   TEST_CALL_REPORT("CreateSketchReferences", "PromptSketchProjectionIds",  
			    status, status != PRO_TK_NO_ERROR );                      
   
   status = 	ProSelectionFree( &sel_entity );
   TEST_CALL_REPORT("CreateSketchReferences", "PromptSketchProjectionIds",  
			    status, status != PRO_TK_NO_ERROR );                      

   *proj_refs_i = temp2;
   return (status);
}


ProError ProUtilFeatureSectionGet(ProFeature feature, ProSection *section)
{
    ProError status;
    ProElempathItem path_items [] = {{ PRO_ELEM_PATH_ITEM_TYPE_ID, 
                                       PRO_E_STD_SECTION},
                                     { PRO_ELEM_PATH_ITEM_TYPE_ID, 
                                       PRO_E_SKETCHER}};
    ProElement   created_elemtree;  
    ProElement   elem_sketch;
    ProElempath             path;
    ProAsmcomppath          *p_comp_path = NULL;

    status = ProFeatureElemtreeExtract ( &feature, p_comp_path, 
        PRO_FEAT_EXTRACT_NO_OPTS, &created_elemtree ); 
    TEST_CALL_REPORT("ProFeatureElemtreeExtract", "ProUtilFeatureSectionGet",  
			    status, status != PRO_TK_NO_ERROR );
    status = ProElempathAlloc (&path);
    TEST_CALL_REPORT("ProElempathAlloc", "ProUtilFeatureSectionGet",  
			    status, status != PRO_TK_NO_ERROR );                                  
    status = ProElempathDataSet (path, path_items, 2);
    TEST_CALL_REPORT("ProElempathDataSet", "ProUtilFeatureSectionGet",  
			    status, status != PRO_TK_NO_ERROR );                          
    status = ProElemtreeElementGet ( created_elemtree, path, &elem_sketch);
    TEST_CALL_REPORT("ProElemtreeElementGet", "ProUtilFeatureSectionGet",  
			    status, status != PRO_TK_NO_ERROR );                        
    status = ProElementSpecialvalueGet (elem_sketch, NULL,
                                                   (ProAppData*)section);
    TEST_CALL_REPORT("ProElementSpecialvalueGet", "ProUtilFeatureSectionGet",  
			    status, status != PRO_TK_NO_ERROR );                         
    status = ProElempathFree (&path);
    return status;
}
