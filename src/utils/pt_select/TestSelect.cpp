


/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include "ProToolkit.h"
#include "ProFeature.h"
#include "ProMdl.h"
#include "ProModelitem.h"
#include "ProParameter.h"
#include "ProSelection.h"
#include "ProCsys.h"
#include "ProEdge.h"
#include "ProCurve.h"
#include "ProSurface.h"
#include "ProQuilt.h"
#include "ProArray.h"
#include "ProUtil.h"
#include "ProMenu.h"

#include "TestError.h"

/*--------------------------------------------------------------------*\
C System includes
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application includes
\*--------------------------------------------------------------------*/
#include "TestSelect.h"
#include "UtilFiles.h"
#include "TestFiletypes.h"
#include "UtilMessage.h"
#include "PTApplsUnicodeUtils.h"
#include "UtilString.h"
/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application data types
\*--------------------------------------------------------------------*/

struct sel_to_opt
{
    char option[40];
    ProType sel_type;
    char sel_str[40];
};

/*--------------------------------------------------------------------*\
Application global/external data
\*--------------------------------------------------------------------*/

#define TRUE 1
#define FALSE 0

static struct sel_to_opt selopt_table[] = 
    {
        {"point",    PRO_POINT,      "PRO_POINT"},
        {"axis",     PRO_AXIS,       "PRO_AXIS"},
        {"csys",     PRO_CSYS,       "PRO_CSYS"},
        {"feature",  PRO_FEATURE,    "PRO_FEATURE"},
        {"edge",     PRO_EDGE,       "PRO_EDGE"},
        {"edge_end", PRO_EDGE_END,   "PRO_EDGE_END"},
        {"curve",    PRO_CURVE,      "PRO_CURVE"},
        {"curve_end",PRO_CRV_END,    "PRO_CRV_END"},
        {"surface",  PRO_SURFACE,    "PRO_SURFACE"},
        {"dtmqlt",   PRO_QUILT,      "PRO_QUILT"},
        {"part",     PRO_PART,       "PRO_PART"},
        {"prt_or_asm", PRO_ASSEMBLY, "PRO_ASSEMBLY"},
        {"parameter",PRO_PARAMETER,  "PRO_PARAMETER"},
        {"dimension",PRO_DIMENSION,  "PRO_DIMENSION"},
        {"ref_dim",  PRO_REF_DIMENSION, "PRO_REF_DIMENSION"},
        {"datum",    PRO_DATUM_PLANE,"PRO_DATUM_PLANE"},
        {"comp_crv", PRO_COMP_CRV,   "PRO_COMP_CRV"},
        {"qltedge",  PRO_EDGE,       "PRO_EDGE"},
        {"qltface",  PRO_SURFACE,    "PRO_SURFACE"},
        {"sldedge",  PRO_EDGE,       "PRO_EDGE"},
        {"sldface",  PRO_SURFACE,    "PRO_SURFACE"},
        {"gtol",     PRO_GTOL,       "PRO_GTOL"}, 
        {"NoEntry",  PRO_TYPE_UNUSED,"PRO_TYPE_UNUSED"}
    };

/*====================================================================*\
    FUNCTION :  OptionToType
    PURPOSE  :  Return the ProType corresponding to an option
\*====================================================================*/
static ProType OptionToType(char *option)
{
    int ntype = sizeof(selopt_table) / sizeof(struct sel_to_opt);
    int i;

    ProTKPrintf("option (%s)\n", option);
    for( i = 0; i < ntype; i++ )
    {
        if(strcmp( option, selopt_table[i].option ) == 0)
            return( selopt_table[i].sel_type );
    }

    return( selopt_table[ntype - 1].sel_type );
}

/*====================================================================*\
    FUNCTION :  TypeToTypeStr
    PURPOSE  :  Return the type string corresponding to a type
\*====================================================================*/
static char *TypeToTypeStr(ProType type)
{
    int ntype = sizeof(selopt_table) / sizeof(struct sel_to_opt);
    int i;

    for( i = 0; i < ntype; i++ )
    {
        if( type == selopt_table[i].sel_type )
            return( selopt_table[i].sel_str );
    }

    return( selopt_table[ntype - 1].sel_str );
}

/*====================================================================*\
    FUNCTION :  ProTestSelection()
    PURPOSE  :  Create a menu for testing selection functions
\*====================================================================*/
int ProTestSelection(char *a, int b)
{
    ProError		status;
    int			menu_id;    /* The identifier of the created menu */
    int			action;

    /*-----------------------------*\
	Create new menu TkEntity
    \*-----------------------------*/

    /* Load base menu from file */
    status = ProMenuFileRegister( (char*)"TkSelect", (char*)"tkselect.mnu", 
				    &menu_id );
    TEST_CALL_REPORT( "ProMenuFileRegister()", "ProTestSelection()", 
			status, status != PRO_TK_NO_ERROR );

    /* Define menu buttons */
    ProMenubuttonActionSet( (char*)"TkSelect", (char*)"Highlight", 
	(ProMenubuttonAction)HighlightItem, NULL, 0 );
    ProMenubuttonActionSet((char*) "TkSelect", (char*)"UnHighlight", 
	(ProMenubuttonAction)UnHighlightItem, NULL, 0 );
    ProMenubuttonActionSet((char*) "TkSelect",(char*) "Display", 
	(ProMenubuttonAction)DisplayItem, NULL, 0 );
    ProMenubuttonActionSet( (char*)"TkSelect",(char*) "UnDisplay", 
	(ProMenubuttonAction)UnDisplayItem, NULL, 0 );
    ProMenubuttonActionSet( (char*)"TkSelect",(char*) "OptInfo", 
	(ProMenubuttonAction)ProTestOptionalInfo, NULL, 0 );
    ProMenubuttonActionSet( (char*)"TkSelect", (char*)"SelParams", 
	(ProMenubuttonAction)SelParams, NULL, 0 );
    ProMenubuttonActionSet( (char*)"TkSelect", (char*)"TkSelect", 
	(ProMenubuttonAction)ProMenuDelete, NULL, 0 );

    /*-----------------------*\
	Run menu TkSelect
    \*-----------------------*/
    status = ProMenuCreate( PROMENUTYPE_MAIN, (char*)"TkSelect", &menu_id );
    TEST_CALL_REPORT( "ProMenuCreate()", "ProTestSelection()", 
			status, status != PRO_TK_NO_ERROR );
    if( status == PRO_TK_NO_ERROR )
    {
	status = ProMenuProcess((char*) "TkSelect", &action );
	TEST_CALL_REPORT( "ProMenuProcess()", "ProTestSelection()", 
			    status, status != PRO_TK_NO_ERROR );
    }


    return(0);
}

/*====================================================================*\
    FUNCTION :  SelParams()
    PURPOSE  :  Select a parameter and show info
\*====================================================================*/
int SelParams(char *a, int b)
{
    ProSelection *p_sel_list;
    int p_sel_num;
    ProMdl p_handle;
    ProError status;
    ProType type[2];
    ProParameter param;
    char fname[PRO_FILE_NAME_SIZE];
    ProModelitem sel_obj;
    FILE *fp;
    char astr[80], option[40], output[80];

/*--------------------------------------------------------------------*\
Get the current model and open an output file
\*--------------------------------------------------------------------*/
    status = ProMdlCurrentGet(&p_handle);
    TEST_CALL_REPORT("ProMdlCurrentGet()", "SelParams()", status,
                    status != PRO_TK_NO_ERROR);
    ProTestQcrName(&p_handle, (char*)SELECTIONS, (char*)fname);
    fp = PTApplsUnicodeFopen(fname, "a");

    if( fp == NULL )
    return(-1);

    ProTKFprintf(fp, "Filename: %s\n", fname);

    ProUtilstrcpy(option, "prt_or_asm");
        type[1] = OptionToType(option);
    ProUtilstrcpy(option, "part");
        type[0] = OptionToType(option);

    ProUtilMsgPrint("gen", "TEST %0s", "Make a selection");

    ProUtilstrcpy(option, "prt_or_asm,part");
    status = ProSelect(option, 1, NULL,
                           NULL, NULL, NULL, &p_sel_list, &p_sel_num);
    TEST_CALL_REPORT("ProSelect()", "SelParams()",
                      status, status == PRO_TK_COMM_ERROR);

    if( status != PRO_TK_NO_ERROR )
        return( -1 );

    ProUtilstrcpy(option, "parameter");
    type[0] = OptionToType(option);

/*--------------------------------------------------------------------*\
Select a parameter from the selected model item
\*--------------------------------------------------------------------*/

    ProSelectionModelitemGet(p_sel_list[0], &sel_obj);
    TEST_CALL_REPORT("ProSelectionModelitemGet()", "UnDisplayItem()",
                      status, status != PRO_TK_NO_ERROR);

    status = ProParameterSelect(&sel_obj, p_handle, PRO_PARAMSELECT_ANY, PRO_B_FALSE, NULL, &param, NULL);
    TEST_CALL_REPORT("ProParameterSelect()", "SelParams()", status,
                      status == PRO_TK_COMM_ERROR);

    ProWstringToString(astr, param.id);
    ProTKSprintf(output, "%-15s (%-15s):   %s", option, TypeToTypeStr(type[0]),astr);

    ProTKFprintf(fp, "%s\n", output);
    ProUtilMsgPrint("gen", "TEST %0s", output);

    ProTKSprintf(astr, "Filename: %s", fname);
    ProUtilMsgPrint("gen", "TEST %0s", astr);
    fclose(fp);

    return(0);
}

/*====================================================================*\
    FUNCTION :  UnDisplayItem()
    PURPOSE  :  UnDisplay different types of items and show info
\*====================================================================*/
int UnDisplayItem(char *a, int b)
{
    ProSelection *p_sel_list;
    ProSelection artifical_sel;
    ProSelection copy_sel;
    int p_sel_num;
    ProMdl p_handle;
    ProError status;
    ProType type;
    char fname[PRO_FILE_NAME_SIZE];
    ProAsmcomppath comp_path;
    ProModelitem sel_obj;
    ProUvParam uv_point;
    FILE *fp;
    char astr[80], option[40], output[80];
    wchar_t wstr1[40];
    ProPoint3d point;
    ProView view;

/*--------------------------------------------------------------------*\
Get the current model and open an output file
\*--------------------------------------------------------------------*/
    status = ProMdlCurrentGet(&p_handle);
    TEST_CALL_REPORT("ProMdlCurrentGet()", "UnDisplayItem()", status,
                    status != PRO_TK_NO_ERROR);
    ProTestQcrName(&p_handle, (char*) SELECTIONS, (char*)fname);
    fp = PTApplsUnicodeFopen(fname, "a");

    if( fp == NULL )
    return(-1);

    ProTKFprintf(fp, "Filename: %s\n", fname);

    while( 1 )
    {
    ProUtilMsgPrint("gen", "TEST %0s", "Enter the option mask: ");
    if( !ProUtilStringGet( wstr1, NULL, 40 ) )
        break;
    ProWstringToString(option, wstr1);
    ProUtilMsgPrint("gen", "TEST %0s", option);

    type = OptionToType(option);

    status = PRO_TK_NO_ERROR;

    while( status == PRO_TK_NO_ERROR )
    {
        status = ProSelect(option, 1, NULL, 
                       NULL, NULL, NULL, &p_sel_list, &p_sel_num);
        TEST_CALL_REPORT("ProSelect()", "UnDisplayItem()",
                          status, status == PRO_TK_COMM_ERROR);
	 
        if( status != PRO_TK_NO_ERROR || p_sel_num <= 0 )
           break; 


	status = ProSelectionAsmcomppathGet(p_sel_list[0], &comp_path);
	TEST_CALL_REPORT("ProSelectionAsmcomppathGet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);

	status = ProSelectionModelitemGet(p_sel_list[0], &sel_obj);
	TEST_CALL_REPORT("ProSelectionModelitemGet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);

	status = ProSelectionUvParamGet(p_sel_list[0], uv_point);
	TEST_CALL_REPORT("ProSelectionUvParamGet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);
                         
        status = ProSelectionPoint3dGet (p_sel_list[0], point);
	TEST_CALL_REPORT("ProSelectionPoint3dGet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);
        
        status = ProSelectionViewGet (p_sel_list[0], &view);
	TEST_CALL_REPORT("ProSelectionPointViewGet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);

	status = ProSelectionAlloc(&comp_path, &sel_obj, &artifical_sel );
	TEST_CALL_REPORT("ProSelectionAlloc()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);

	status = ProSelectionUvParamSet( artifical_sel, uv_point);
	TEST_CALL_REPORT("ProSelectionUvParamSet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);
        
        status = ProSelectionViewSet (view, &artifical_sel);
	TEST_CALL_REPORT("ProSelectionViewSet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);
        
        status = ProSelectionPoint3dSet (point, &artifical_sel);
 	TEST_CALL_REPORT("ProSelectionPoint3dSet()", "UnDisplayItem()",
			 status, status != PRO_TK_NO_ERROR);
       
/*--------------------------------------------------------------------*\
Check if the selection is valid (should be valid here)
\*--------------------------------------------------------------------*/
	status = ProSelectionVerify( artifical_sel );
	TEST_CALL_REPORT("ProSelectionVerify()", "UnDisplayItem()",
			 status, status == PRO_TK_COMM_ERROR);

	if( status == PRO_TK_NO_ERROR )
	{
	    ProTKSprintf(output, "%-15s (%-15s):   %d", option, 
		    TypeToTypeStr(type), sel_obj.id );

	    ProTKFprintf(fp, "%s\n", output);
	    ProUtilMsgPrint("gen", "TEST %0s", output);

	    ProSelectionUnhighlight(p_sel_list[0]);
	    TEST_CALL_REPORT("ProSelectionUnhighlight()", "UnHighlightItem()",
			      status, status != PRO_TK_NO_ERROR);
	}

	/* Copy created selection and check the copy */
        status = ProSelectionCopy( artifical_sel, &copy_sel );
        TEST_CALL_REPORT( "ProSelectionCopy()", "UnDisplayItem()",
			    status, status != PRO_TK_NO_ERROR);
	status = ProSelectionVerify( copy_sel );
	TEST_CALL_REPORT("ProSelectionVerify()", "UnDisplayItem()",
			 status, status == PRO_TK_COMM_ERROR);

/*--------------------------------------------------------------------*\
Free the created selection
\*--------------------------------------------------------------------*/
        status = ProSelectionFree( &artifical_sel );
        TEST_CALL_REPORT( "ProSelectionFree()", "UnDisplayItem()",
			    status, status != PRO_TK_NO_ERROR);
    }
    }

    ProTKSprintf(astr, "Filename: %s", fname);
    ProUtilMsgPrint("gen", "TEST %0s", astr);
    fclose(fp);

    return( 0 );
}

/*====================================================================*\
    FUNCTION :  DisplayItem()
    PURPOSE  :  Display different types of items and show info
\*====================================================================*/
int DisplayItem(char *a, int b)
{
    ProSelection *p_sel_list;
    int p_sel_num;
    ProMdl p_handle;
    ProError status;
    ProType type;
    char fname[PRO_FILE_NAME_SIZE];
    FILE *fp;
    char astr[80], option[40], output[80];
    wchar_t wstr1[40];
    ProModelitem sel_obj;


/*--------------------------------------------------------------------*\
Get the current model and open an output file
\*--------------------------------------------------------------------*/
    status = ProMdlCurrentGet(&p_handle);
    TEST_CALL_REPORT("ProMdlCurrentGet()", "DisplayItem()", status,
                    status != PRO_TK_NO_ERROR);
    ProTestQcrName(&p_handle, (char*)SELECTIONS, (char*)fname);
    fp = PTApplsUnicodeFopen(fname, "a");

    if( fp == NULL )
        return(-1);

    ProTKFprintf(fp, "Filename: %s\n", fname);

    while( 1 )
    {
    ProUtilMsgPrint("gen", "TEST %0s", "Enter the option mask: ");
    if( !ProUtilStringGet( wstr1, NULL, 40 ) )
        break;

    ProWstringToString(option, wstr1);
    ProUtilMsgPrint("gen", "TEST %0s", option);

/*--------------------------------------------------------------------*\
Set the type of selection required
\*--------------------------------------------------------------------*/
    type = OptionToType(option);

    status = PRO_TK_NO_ERROR;

    while( status == PRO_TK_NO_ERROR )
    {
        status = ProSelect(option, 1, NULL,
                        NULL, NULL, NULL, &p_sel_list, &p_sel_num);
        TEST_CALL_REPORT("ProSelect()", "UnDisplayItem()",
                          status, status == PRO_TK_COMM_ERROR);

 
        if( status == PRO_TK_NO_ERROR && p_sel_num > 0 )
        {
            ProSelectionModelitemGet(p_sel_list[0], &sel_obj);
            TEST_CALL_REPORT("ProSelectionModelitemGet()", "UnDisplayItem()",
                              status, status != PRO_TK_NO_ERROR);

            ProTKSprintf(output, "%-15s (%-15s):   %d", option, 
                    TypeToTypeStr(type), sel_obj.id );

            ProTKFprintf(fp, "%s\n", output);
            ProUtilMsgPrint("gen", "TEST %0s", output);

            ProSelectionDisplay(p_sel_list[0]);

            TEST_CALL_REPORT("ProSelectionDisplay()", "DisplayItem()",
                    status, status != PRO_TK_NO_ERROR);
        }
        else
           break;

       }
    }

    ProTKSprintf(astr, "Filename: %s", fname);
    ProUtilMsgPrint("gen", "TEST %0s", astr);
    fclose(fp);

    return( 0 );
}

/*====================================================================*\
    FUNCTION :  UnHighlightItem()
    PURPOSE  :  Unhighlight different types of items and show info
\*====================================================================*/
int UnHighlightItem(char *a, int b)
{
    ProSelection *p_sel_list;
    int p_sel_num;
    ProMdl p_handle;
    ProError status;
    ProType type;
    char fname[PRO_FILE_NAME_SIZE];
    FILE *fp;
    ProModelitem sel_obj;
    char astr[80], option[40], output[80];
    wchar_t wstr1[40];


/*--------------------------------------------------------------------*\
Get the current model and open an output file
\*--------------------------------------------------------------------*/
    status = ProMdlCurrentGet(&p_handle);
    TEST_CALL_REPORT("ProMdlCurrentGet()", "UnHighlightItem()", status,
                    status != PRO_TK_NO_ERROR);
    ProTestQcrName(&p_handle, (char*)SELECTIONS, (char*)fname);
    fp = PTApplsUnicodeFopen(fname, "a");

    if( fp == NULL )
    return(-1);

    ProTKFprintf(fp, "Filename: %s\n", fname);

    while( 1 )
    {
    ProUtilMsgPrint("gen", "TEST %0s", "Enter the option mask: ");
    if( !ProUtilStringGet( wstr1, NULL, 40 ) )
        break;

    ProWstringToString(option, wstr1);
    ProUtilMsgPrint("gen", "TEST %0s", option);

/*--------------------------------------------------------------------*\
Set the type of selection required
\*--------------------------------------------------------------------*/
    type = OptionToType(option);

    status = PRO_TK_NO_ERROR;

    while( status == PRO_TK_NO_ERROR )
    {
        status = ProSelect(option, 1, NULL,
                                  NULL, NULL, NULL, &p_sel_list, &p_sel_num);
        TEST_CALL_REPORT("ProSelect()", "UnHighlightItem()",
                          status, status == PRO_TK_COMM_ERROR);

        if( status == PRO_TK_NO_ERROR && p_sel_num > 0 )
        {
            if (type != PRO_TYPE_UNUSED)
            {
                ProSelectionModelitemGet(p_sel_list[0], &sel_obj);
                TEST_CALL_REPORT("ProSelectionModelitemGet()", 
                    "UnHighlightItem()", status, status != PRO_TK_NO_ERROR);

                ProTKSprintf(output, "%-15s (%-15s):   %d", option,
                    TypeToTypeStr(type), sel_obj.id );
            }
            else
                ProTKSprintf(output, "%-15s (%-15s)", option, TypeToTypeStr(type));

            ProTKFprintf(fp, "%s\n", output);
            ProUtilMsgPrint("gen", "TEST %0s", output);

            status = ProSelectionUnhighlight(p_sel_list[0]);
            TEST_CALL_REPORT("ProSelectionUnhighlight()", "UnHighlightItem()",
                              status, status != PRO_TK_NO_ERROR);
            
        }
        else
           break;
    }
    }

    ProTKSprintf(astr, "Filename: %s", fname);
    ProUtilMsgPrint("gen", "TEST %0s", astr);
    fclose(fp);

    return( 0 );
}

/*====================================================================*\
    FUNCTION :  HighlightItem()
    PURPOSE  :  Highlight different types of items and show info
\*====================================================================*/
int HighlightItem(char *a, int b)
{
    ProSelection *p_sel_list;
    int p_sel_num;
    wchar_t wstr1[40];
    char option[40], output[80], astr[80];
    ProType type;
    ProMdl p_handle;
    char fname[PRO_FILE_NAME_SIZE];
    FILE *fp;
    ProError status;
    ProModelitem sel_obj;


/*--------------------------------------------------------------------*\
Get the current model and open an output file
\*--------------------------------------------------------------------*/
    status = ProMdlCurrentGet(&p_handle);
    TEST_CALL_REPORT("ProMdlCurrentGet()", "SelectDemo()",
                    status, status != PRO_TK_NO_ERROR);
    ProTestQcrName(&p_handle, (char*)SELECTIONS,(char*) fname);
    fp = PTApplsUnicodeFopen(fname, "a");

    if( fp == NULL )
    return(-1);

    ProTKFprintf(fp, "Filename: %s\n", fname);

    while( 1 )
    {
    ProUtilMsgPrint("gen", "TEST %0s", "Enter the option mask: ");
    if( !ProUtilStringGet( wstr1, NULL, 40 ) )
        break;

    ProWstringToString(option, wstr1);
    ProUtilMsgPrint("gen", "TEST %0s", option);

/*--------------------------------------------------------------------*\
Set the type of selection required
\*--------------------------------------------------------------------*/
    type = OptionToType(option);

    status = PRO_TK_NO_ERROR;

    while( status == PRO_TK_NO_ERROR )
    {
        status = ProSelect(option, 1, NULL, 
            NULL, NULL, NULL, &p_sel_list, &p_sel_num);
        TEST_CALL_REPORT("ProSelect()", "HighlightItem()",
                              status, status == PRO_TK_COMM_ERROR);

        if( status == PRO_TK_NO_ERROR && p_sel_num > 0)
        {
            if ( type !=  PRO_TYPE_UNUSED)
            {
               status = ProSelectionModelitemGet(p_sel_list[0], &sel_obj);
               TEST_CALL_REPORT("ProSelectionModelitemGet()", "HighlightItem()",
                                 status, status != PRO_TK_NO_ERROR);

               ProTKSprintf(output, "%-15s (%-15s):   %d", option,
                       TypeToTypeStr(type), sel_obj.id );

               ProUtilMsgPrint("gen", "TEST %0s", output);
            }
            else
                ProTKSprintf(output, "%-15s (%-15s)", option, TypeToTypeStr(type));
                
               ProTKFprintf(fp, "%s\n", output);

                status = ProSelectionHighlight(p_sel_list[0], PRO_COLOR_ERROR);
                TEST_CALL_REPORT("ProSelectionHighlight()", "HighlightItem()",
                       status, status == PRO_TK_COMM_ERROR);
        }
        else
            break;
    }
    }
    
    ProTKSprintf(astr, "Filename: %s", fname);
    ProUtilMsgPrint("gen", "TEST %0s", astr);
    fclose(fp);

    return( 0 );
}



int ProTestOptionalInfo( void* p_dummy, int int_dummy )
{
    ProError		    status;
    ProSelection*	    p_sel_list;
    ProModelitem	    sel_obj, sel_obj2;
    wchar_t		    wstr1[PRO_MDLNAME_SIZE];
    char		    option[PRO_MDLNAME_SIZE], output[80];
    int			    p_sel_num;
    ProType		    type;
    ProMdl		    p_handle;
    ProAsmcomppath	    comp_path, comp_path2;
    FILE*		    fp;
    char		    fname[PRO_FILE_NAME_SIZE];
    ProSelectionEnvOption   env_options[ 3 ] = { 
				    {PRO_SELECT_DONE_REQUIRED, 0},
				    {PRO_SELECT_BY_MENU_ALLOWED, 1},
				    {PRO_SELECT_BY_BOX_ALLOWED, 1} };
    ProSelectionEnv	    sel_env;
    ProPoint3d		    point;
    double		    depth;
    ProView		    view;
    int 		    i, win_id;
    char   	        name[PRO_MDLNAME_SIZE];


    status = ProMdlCurrentGet( &p_handle );
    TEST_CALL_REPORT("ProMdlCurrentGet()", "ProTestOptionalInfo()",
                    status, status != PRO_TK_NO_ERROR);

    ProTestQcrName(&p_handle, (char*)SELECTIONS, (char*)fname);
    if( (fp = PTApplsUnicodeFopen( fname, "a" )) == NULL )
	return -1;
    ProTKFprintf(fp, "Filename: %s\n", fname);

    /* Get selection mask */
    ProUtilMsgPrint( "gen", "TEST %0s", "Enter the option mask: " );
    if( !ProUtilStringGet( wstr1, NULL, PRO_MDLNAME_SIZE ) )
        return -1;
    ProWstringToString( option, wstr1 );
    ProUtilMsgPrint( "gen", "TEST %0s", option );

    type = OptionToType(option);

    /* Allocate selection env */
    status = ProSelectionEnvAlloc( env_options, 3, &sel_env );
    TEST_CALL_REPORT("ProSelectionEnvAlloc()", "ProTestOptionalInfo()",
                    status, status != PRO_TK_NO_ERROR);
    
    status = PRO_TK_NO_ERROR;
    while( status == PRO_TK_NO_ERROR )
    {
	/* Select an item */
        status = ProSelect( option, -1, NULL,
	    NULL, sel_env, NULL, &p_sel_list, &p_sel_num );
        TEST_CALL_REPORT( "ProSelect()", "ProTestOptionalInfo()",
			    status, status == PRO_TK_COMM_ERROR );

	if( status == PRO_TK_NO_ERROR)
	for (i=0; i< p_sel_num; i++ )
        {
	    /* Get selected item id */
            ProSelectionModelitemGet( p_sel_list[i], &sel_obj );
            TEST_CALL_REPORT( "ProSelectionModelitemGet()", 
				"ProTestOptionalInfo()",
				status, status != PRO_TK_NO_ERROR );

	    /* Get component path */
	    status = ProSelectionAsmcomppathGet( p_sel_list[i], &comp_path );
	    TEST_CALL_REPORT("ProSelectionAsmcomppathGet()", 
				"ProTestOptionalInfo()",
				status, status != PRO_TK_NO_ERROR);

	    /* Print out selected item id */
            ProTKSprintf( output, "%-15s (%-15s):   %d", option,
                    TypeToTypeStr(type), sel_obj.id );
            ProTKFprintf( fp, "%s\n", output );
            ProUtilMsgPrint( "gen", "TEST %0s", output );

	    /* Get selected point */
	    status = ProSelectionPoint3dGet( p_sel_list[i], point );
            TEST_CALL_REPORT( "ProSelectionPoint3dGet()",
		"ProTestOptionalInfo()", status, status != PRO_TK_NO_ERROR );

	    /* Print out the selected point coords */
            ProTKSprintf( output, "%-15s (%-15s):   %2.4f %2.4f %2.4f", 
		"Sel coords", TypeToTypeStr(type), 
		point[0], point[1], point[2] );
            ProTKFprintf( fp, "%s\n", output );
            ProUtilMsgPrint( "gen", "TEST %0s", output );

	    /* Get the normal to a ray */
	    status = ProSelectionDepthGet( p_sel_list[i], &depth );
            TEST_CALL_REPORT( "ProSelectionDepthGet()", "ProTestOptionalInfo()",
                              status, status != PRO_TK_NO_ERROR );

	    /* Print out the normal to a ray */
            ProTKSprintf( output, "%-15s (%-15s):   %2.4f", 
		"Depth", TypeToTypeStr(type), depth );
	    ProTKFprintf( fp, "%s\n", output );
	    ProUtilMsgPrint( "gen", "TEST %0s", output );

            /* Get the view from a selection object */
	    status = ProSelectionViewGet( p_sel_list[i], &view );
            TEST_CALL_REPORT( "ProSelectionViewGet()", "ProTestOptionalInfo()",
                              status, status != PRO_TK_NO_ERROR );

#ifndef PT_PRODUCTS_BUILD
	    status = ProSelectionWindowIdGet( p_sel_list[i], &win_id);
            TEST_CALL_REPORT( "ProSelectionWindowIdGet()", 
		"ProTestOptionalInfo()", status, status != PRO_TK_NO_ERROR );

	    status = ProWindowMdlGet( win_id, &p_handle); 
            TEST_CALL_REPORT( "ProWindowMdlGet()", 
		"ProTestOptionalInfo()", status, status != PRO_TK_NO_ERROR );
	    
	    ProMdlMdlnameGet(p_handle,  wstr1);
	    ProWstringToString(name,  wstr1);

	    /* Print the model name */
	    ProTKSprintf( output, "Selected in the window of model %s", name);
	    ProTKFprintf( fp, "%s\n", output );	    
#endif

	    /* Call ProSelectionSet() only to test it  */
	    status = ProSelectionSet( p_sel_list[i], &comp_path, &sel_obj );
            TEST_CALL_REPORT( "ProSelectionSet()", "ProTestOptionalInfo()",
                              status, status != PRO_TK_NO_ERROR );

	    ProSelectionModelitemGet( p_sel_list[i], &sel_obj2 );
	    ProSelectionAsmcomppathGet( p_sel_list[i], &comp_path2 );
	    if (sel_obj.id != sel_obj2.id || sel_obj.type != sel_obj2.type ||
		comp_path.table_num != comp_path2.table_num)
	    {
		ProTKFprintf(stderr, "ProSelectionSet() error !\n");
	    }
	}
        else
           break;
    }

     /* Free allocated selection env */
    status = ProSelectionEnvFree( sel_env );
    TEST_CALL_REPORT("ProSelectionEnvFree()", "ProTestOptionalInfo()",
                    status, status != PRO_TK_NO_ERROR);

    fclose(fp);

    return 0;
}

/*=============================================================*\
Function: 	UtilSelectionFromSurfaceId
Purpose:  	Get the selection from surface id
Return :    PRO_TK_NO_ERROR if successful
Free memory outside the function
\*=============================================================*/
ProError UtilSelectionFromSurfaceId(
                                       ProMdl model,	/* In : model */
                                       int surf_id,	/* In : surface id */
                                       ProSelection *sel)	
{
    ProError status;
    ProModelitem model_item;

    status = ProModelitemInit (model, surf_id, PRO_SURFACE, &model_item);
    if (status!=PRO_TK_NO_ERROR) 
      return (status);
    status = ProSelectionAlloc(NULL, &model_item, sel);
    TEST_CALL_REPORT("ProSelectionAlloc", "UtilSelectionFromSurfaceId",  
			    status, status != PRO_TK_NO_ERROR );                      
    return status;
}

/*=============================================================*\
Function: 	UtilSelectionFromSurface
Purpose:  	Get the selection from surface 
Return :    PRO_TK_NO_ERROR if successful
Free memory outside the function
\*=============================================================*/
ProError UtilSelectionFromSurface(
                                     ProSolid solid,	/* In : model */
                                     ProSurface surf,	/* In : surface */
                                     ProSelection *sel)
{
    ProError status;
    ProModelitem modelitem;

    status = ProSurfaceToGeomitem(solid, surf, (ProGeomitem*)&modelitem);
    if(status != PRO_TK_NO_ERROR)
      return status;
    status = ProSelectionAlloc(NULL, &modelitem, sel);
    status = ProSelectionSet(*sel, NULL, &modelitem);
    TEST_CALL_REPORT("ProSelectionSet", "UtilSelectionFromSurface", 
			    status, status != PRO_TK_NO_ERROR );                        
    return status;
}

/*=============================================================*\
Function: 	UtilSelectionFromEdge
Purpose:  	Get the selection from edge 
Return :    PRO_TK_NO_ERROR if successful
Free memory outside the function
\*=============================================================*/
ProError UtilSelectionFromEdge(
                                     ProSolid solid,	/* In : model */
                                     ProEdge edge,	/* In : edge id */
                                     ProSelection *sel)
{
    ProError status;
    ProModelitem modelitem;

    status = ProEdgeToGeomitem(solid, edge, (ProGeomitem*)&modelitem);
    if(status != PRO_TK_NO_ERROR)
      return status;
    status = ProSelectionAlloc(NULL, &modelitem, sel);
    status = ProSelectionSet(*sel, NULL, &modelitem);
    TEST_CALL_REPORT("ProSelectionSet", "UtilSelectionFromSurface",  
			    status, status != PRO_TK_NO_ERROR );                      
    return status;
}

/*=============================================================*\
Function: 	UtilSelectionFromEdgeId
Purpose:  	Get the selection from edge id
Return :    PRO_TK_NO_ERROR if successful
Free memory outside the function
\*=============================================================*/
ProError UtilSelectionFromEdgeId(
                                    ProMdl model,	/* In : model */
                                    int edge_id,	/* In : edge id */
                                    ProSelection *sel)/* Out: selection*/
{
    ProError status;
    ProModelitem model_item;

    status = ProModelitemInit (model, edge_id, PRO_EDGE, &model_item);
    if (status!=PRO_TK_NO_ERROR) 
      return (status);
    status = ProSelectionAlloc(NULL, &model_item, sel);
    TEST_CALL_REPORT("ProSelectionAlloc", "UtilSelectionFromEdgeId",  
			    status, status != PRO_TK_NO_ERROR );                         
    return status;
}

/*===============================================================*\
FUNCTION : ProUtilCopySelectionSurf
PURPOSE  : Make a selection copy for a surface entity
\*===============================================================*/
ProError ProUtilCopySelectionSurf(ProMdl solid, ProSurface surf,
                           ProSelection *p_copy_sel)
{
    ProSelection p_sel = NULL;
    ProError status;
    ProModelitem item;
    status = UtilSelectionFromSurface((ProSolid)solid, surf, &p_sel);
    if(status != PRO_TK_NO_ERROR)
      return status;

    status = ProSelectionModelitemGet (p_sel, &item);
    TEST_CALL_REPORT("ProSelectionModelitemGet", "ProUtilCopySelectionSurf",  
			    status, status != PRO_TK_NO_ERROR );                            
    status = ProSelectionSet(p_sel, NULL, &item);
    TEST_CALL_REPORT("ProSelectionSet", "ProUtilCopySelectionSurf",  
			    status, status != PRO_TK_NO_ERROR );                          
    status = ProSelectionDisplay(p_sel);
    status = ProSelectionCopy( p_sel, p_copy_sel );
    TEST_CALL_REPORT("ProSelectionCopy", "ProUtilCopySelectionSurf",  
			    status, status != PRO_TK_NO_ERROR );                               
    status = ProSelectionFree(&p_sel);
    TEST_CALL_REPORT("ProSelectionFree", "ProUtilCopySelectionSurf",  
			    status, status != PRO_TK_NO_ERROR );                               
    return status;
}

