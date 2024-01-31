#define PRO_USE_VAR_ARGS true

#include <iostream>
#include <math.h>
#include <ProCore.h>
#include <ProToolkit.h>
#include <ProMessage.h>
#include <ProMenu.h>
#include <ProUtil.h>
#include <ProUICmd.h>
#include <ProMenuBar.h>
#include <ProSelection.h>
#include <ProModelitem.h>
#include <ProCurve.h>
#include <ProDtmCrv.h>
#include <ProFeatType.h>
#include <ProStdSection.h>
#include <ProSolid.h>
#include <ProSecdim.h>

#include "TestSect.h"
#include "TestError.h"
#include "UtilTree.h"
#include "UtilMessage.h"
#include "TestSelect.h"

using namespace std;

static ProFileName msg_file;

/*===========================================================================*\
  Function : main
  Purpose  : Test the ProToolkitMain() function. main is optional function.
\*===========================================================================*/
int main(int argc, char **argv)
{
    ProToolkitMain(argc, argv);
    return (0);
}

uiCmdAccessState IsSketchAvailable(uiCmdAccessMode access_mode)
{
    return ACCESS_AVAILABLE;
}

/*===============================================================*\
FUNCTION: PromptFeatureWithSection
PURPOSE:  Prompt the user to select a sketch feature
\*===============================================================*/
ProError PromptFeatureWithSection(ProFeature *feat, ProSection *section)
{
    ProError status;
    ProSelection *sel = NULL;
    int n_sel, n, n_sections;
    int range[2];
    
    /* select the 3D-section */
    ProMessageDisplay(msg_file, "SelectSurfaceLabel");     
    n_sel = 0; 
    status = ProSelect((char *)"feature", 1, NULL, NULL, NULL, NULL, &sel, &n_sel);    
    TEST_CALL_REPORT("ProSelect", "PromptFeatureWithSection", 
			status, status != PRO_TK_NO_ERROR );
    if (status != PRO_TK_NO_ERROR || n_sel != 1)
	     return status;
	     
    status = ProSelectionModelitemGet(sel[0], feat);
    TEST_CALL_REPORT("ProSelectionModelitemGet", "PromptFeatureWithSection", 
			status, status != PRO_TK_NO_ERROR );
    status = ProFeatureNumSectionsGet(feat, &n_sections);
    TEST_CALL_REPORT("ProFeatureNumSectionsGet", "PromptFeatureWithSection", 
			status, status != PRO_TK_NO_ERROR );     
        
    if (n_sections < 1)
    {
	    return(PRO_TK_GENERAL_ERROR);
    }

    range[0] = 0; range[1] = n_sections-1;
    n = 0;
    if ( range[0] != range[1] )
    {
	ProUtilMsgPrint((char *)"gen",(char *)"TEST %0s", (char *)"Specify section number to retrieve.");
    ProUtilIntGet(range,&range[0],&n);
    }
    status = ProFeatureSectionCopy(feat, n, section); 
    TEST_CALL_REPORT("ProFeatureSectionCopy", "PromptFeatureWithSection", 
			status, status != PRO_TK_NO_ERROR );    
    return status;
}

/*===============================================================*\
FUNCTION: GetSectionProjectionReference
PURPOSE:  Fills the projection of sketch information
          from the section information
\*===============================================================*/
ProError GetSectionProjectionReference(ProSection *section, SketchInfo *sketch_info)
{
    ProError status;
    int i = 0, n_ids = 0;
    ProIntlist      p_id_list;
    Pro2dEntdef *p_ent;
    ProBoolean is_proj;
    ProGeomitem geom_item;
    ProSelection sel;
 
    status = ProSectionEntityIdsGet(*section, &p_id_list, &n_ids);
    TEST_CALL_REPORT("ProSectionEntityIdsGet", "GetSectionProjectionReference", 
			status, status != PRO_TK_NO_ERROR );    
    if(status != PRO_TK_NO_ERROR)
        return status;
    
    for(i=0; i<2; i++) 
    {
        status = ProSectionEntityGet(*section, p_id_list[i], (Pro2dEntdef**)&p_ent);    
        TEST_CALL_REPORT("ProSectionEntityIdsGet", "GetSectionProjectionReference", 
			status, status != PRO_TK_NO_ERROR );
        status = ProSectionEntityIsProjection(*section, p_id_list[i], &is_proj);
        if (PRO_TK_NO_ERROR != status || is_proj != PRO_B_TRUE)
            return PRO_TK_GENERAL_ERROR;
        status = ProSectionEntityReferenceGet ( *section, p_id_list[i], &sel);
        if (status == PRO_TK_NO_ERROR)
        {
            status = ProSelectionModelitemGet (sel, (ProModelitem *)&geom_item);
            TEST_CALL_REPORT("ProSelectionModelitemGet", "GetSectionProjectionReference",
			        status, status != PRO_TK_NO_ERROR ); 
            if(i == 0)
                status = ProUtilGetGeomItemToRef(&geom_item, &sketch_info->ref1);
            if(i == 1)
                status = ProUtilGetGeomItemToRef(&geom_item, &sketch_info->ref2);
            if(PRO_TK_NO_ERROR != status)
                return PRO_TK_GENERAL_ERROR;
        }
    }
    return status;
}

/*===============================================================*\
FUNCTION: SectionInfoCollectAutoMode
PURPOSE:  Fills the sketch information from the section's information 
\*===============================================================*/
ProError SectionInfoCollectAutoMode(SketchInfo *sketch_info, ProSection *section)
{
    ProError status;
    ProFeature feat;
    ProGeomitem geom_item;
    ProSurface place_surface, orient_surface;
    UtilElemData   elem_data;
    ProElemId name_id;

    status = PromptFeatureWithSection(&feat, section); 
    if(status != PRO_TK_NO_ERROR)
        return status;
        
    status = ProUtilElementFeatTypeGet(&feat, &name_id);
    if(name_id != PRO_FEAT_CURVE)
        return PRO_TK_BAD_CONTEXT;
        
    sketch_info->solid = (ProSolid)feat.owner;	    

    /* The surface on which the sketch is to be placed */
    if (status == PRO_TK_NO_ERROR)
	{
        ProElempathItem path_items_surface [] =
                   {{ PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SECTION},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_SETUP_PLANE},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_PLANE}};
        status = ProUtilElementFeatDataGet(feat, path_items_surface, 3, &elem_data);
        if(PRO_TK_NO_ERROR == status)
        {
            status = ProSelectionModelitemGet((ProSelection)elem_data.val.p, 
                                        (ProModelitem *)&geom_item);
            TEST_CALL_REPORT("ProSelectionModelitemGet", "SectionInfoCollectAutoMode", 
			        status, status != PRO_TK_NO_ERROR );
            status = ProGeomitemToSurface (&geom_item, &place_surface);		
            TEST_CALL_REPORT("ProGeomitemToSurface", "SectionInfoCollectAutoMode", 
			        status, status != PRO_TK_NO_ERROR );
            sketch_info->prim_surface = place_surface;	               
        }
    }
	
    if (status == PRO_TK_NO_ERROR)
	{
        ProElempathItem path_items_orient_ref [] =
                   {{ PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SECTION},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_SETUP_PLANE},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_PLANE_ORIENT_REF}};
        status = ProUtilElementFeatDataGet(feat, path_items_orient_ref, 3, &elem_data);                    
        if(PRO_TK_NO_ERROR == status)
        {
            status = ProSelectionModelitemGet((ProSelection)elem_data.val.p,
                                          (ProModelitem *)&geom_item);
            TEST_CALL_REPORT("ProSelectionModelitemGet", "SectionInfoCollectAutoMode", 
		    	status, status != PRO_TK_NO_ERROR );                                         
            status = ProGeomitemToSurface (&geom_item, &orient_surface);		
            TEST_CALL_REPORT("ProGeomitemToSurface", "SectionInfoCollectAutoMode", 
			    status, status != PRO_TK_NO_ERROR );            
            sketch_info->orient_surface = orient_surface;	       
        }
	}
	
    if (status == PRO_TK_NO_ERROR )
	{
        ProElempathItem path_items_orient_dir [] =
                   {{ PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SECTION},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_SETUP_PLANE},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_PLANE_ORIENT_DIR}};
                    
        ProElempathItem path_items_orient_view_dir [] =
                   {{ PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SECTION},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_SETUP_PLANE},
                    { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SEC_PLANE_VIEW_DIR}};                                        

        status = ProUtilElementFeatDataGet(feat, path_items_orient_dir, 3, &elem_data);                     
        if(PRO_TK_NO_ERROR == status)
        {
            sketch_info->orient_dir = elem_data.val.i;
        }
        
        status = ProUtilElementFeatDataGet(feat, path_items_orient_view_dir, 3, &elem_data);             
        if(PRO_TK_NO_ERROR == status)
        {
            sketch_info->orient_view_dir = elem_data.val.i;
	    }
    }
	/* Get the section from the feature */
	/*if(PRO_TK_NO_ERROR == status)
	    status = ProUtilFeatureSectionGet(feat, &section);*/
    if(PRO_TK_NO_ERROR == status)	    
	    status = ProSectionEpsilonGet(*section, &sketch_info->eps_val);
    if(PRO_TK_NO_ERROR == status)	  	    
	    status = ProUtilCopySectionEntities(section, sketch_info);
    if(PRO_TK_NO_ERROR == status)	  	    	    
        status = GetSectionProjectionReference(section, sketch_info);
	return status;
}

/*====================================================================*\
FUNCTION : CreateSketchFeatures()
PURPOSE  : Creates sketch feature from sketch information 
\*====================================================================*/
ProError CreateSketchFeatures(SketchInfo *sketch_info, ProFeature *feature)
{
  ProError 	status;
  ProErrorlist            errors;
  ProModelitem            model_item;
  ProSelection            model_sel;
  ProFeatureCreateOptions *opts = 0;
  ProSection              section;
  ProAsmcomppath          comp_path;
  ProAsmcomppath          *p_comp_path = NULL;

  ProElement created_elemtree; 
  ProElement pro_e_feature_tree;
  ProElement pro_e_std_section;
  ProElement pro_e_std_sec_setup_plane;

  ProSelection *sketch_refs;
  ProValueData 	value_data;
  UtilElemData elem_data;
  ProElempathItem path_items [] = {{ PRO_ELEM_PATH_ITEM_TYPE_ID, 
                                       PRO_E_STD_SECTION},
                                     { PRO_ELEM_PATH_ITEM_TYPE_ID, 
                                       PRO_E_SKETCHER}}; 
  /*---------------------------------------------------------------*\
    Populating root element PRO_E_FEATURE_TREE 
  \*---------------------------------------------------------------*/
  status = ProElementAlloc ( PRO_E_FEATURE_TREE, &pro_e_feature_tree ); 
  
  /*---------------------------------------------------------------*\
    Populating element PRO_E_FEATURE_TYPE 
  \*---------------------------------------------------------------*/
  value_data.v.i = PRO_FEAT_CURVE; /* 949 */ 
  status = ProUtilElemtreeElementAdd( pro_e_feature_tree, PRO_E_FEATURE_TYPE,
                                 ELEM_VALUE_TYPE_INT, &value_data.v.i);
                                   
  /*---------------------------------------------------------------*\
    Populating element PRO_E_CURVE_TYPE
  \*---------------------------------------------------------------*/
  value_data.v.i = PRO_CURVE_TYPE_SKETCHED; /* 0 */ 
  status = ProUtilElemtreeElementAdd( pro_e_feature_tree, PRO_E_CURVE_TYPE,
                                 ELEM_VALUE_TYPE_INT, &value_data.v.i);
  
  /*---------------------------------------------------------------*\
    Populating  element PRO_E_STD_SECTION
  \*---------------------------------------------------------------*/
  status = ProElementAlloc ( PRO_E_STD_SECTION, &pro_e_std_section );
  status = ProElemtreeElementAdd ( pro_e_feature_tree, NULL, 
		pro_e_std_section  );

  /*---------------------------------------------------------------*\
    Populating element PRO_E_STD_SECTION 
			-> PRO_E_STD_SEC_SETUP_PLANE
  \*---------------------------------------------------------------*/
  status = ProElementAlloc ( PRO_E_STD_SEC_SETUP_PLANE, 
		&pro_e_std_sec_setup_plane );
  status = ProElemtreeElementAdd ( pro_e_std_section, NULL, 
		pro_e_std_sec_setup_plane  );

  sketch_refs = ( ProSelection *) calloc ( 2, sizeof ( ProSelection ));

  /*---------------------------------------------------------------*\
    Populating element PRO_E_STD_SECTION 
			-> PRO_E_STD_SEC_SETUP_PLANE
			   -> PRO_E_STD_SEC_PLANE
  \*---------------------------------------------------------------*/
  status = ProUtilCopySelectionSurf((ProSolid)sketch_info->solid,
            sketch_info->prim_surface, &(value_data.v.r));
  status = ProSelectionCopy( value_data.v.r, &sketch_refs[0]);
  TEST_CALL_REPORT("ProSelectionCopy", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );   
  if(PRO_TK_NO_ERROR != status)
      return status;
  status = ProUtilElemtreeElementAdd( pro_e_std_sec_setup_plane, PRO_E_STD_SEC_PLANE,
                                 ELEM_VALUE_TYPE_SELECTION, value_data.v.r);  

  /*---------------------------------------------------------------*\
    Populating element PRO_E_STD_SECTION 
			-> PRO_E_STD_SEC_SETUP_PLANE
			   -> PRO_E_STD_SEC_PLANE_VIEW_DIR
  \*---------------------------------------------------------------*/
  value_data.v.i = sketch_info->orient_view_dir; /*PRO_SEC_VIEW_DIR_SIDE_ONE  1 */ 
  status = ProUtilElemtreeElementAdd( pro_e_std_sec_setup_plane, PRO_E_STD_SEC_PLANE_VIEW_DIR,
                                 ELEM_VALUE_TYPE_INT, &value_data.v.i);  

  /*---------------------------------------------------------------*\
    Populating element PRO_E_STD_SECTION 
			-> PRO_E_STD_SEC_SETUP_PLANE
			   -> PRO_E_STD_SEC_PLANE_ORIENT_DIR
  \*---------------------------------------------------------------*/
  value_data.v.i = sketch_info->orient_dir; /* PRO_SEC_ORIENT_DIR_UP 1*/
  status = ProUtilElemtreeElementAdd( pro_e_std_sec_setup_plane, PRO_E_STD_SEC_PLANE_ORIENT_DIR,
                                 ELEM_VALUE_TYPE_INT, &value_data.v.i);  


  /*---------------------------------------------------------------*\
    Populating element PRO_E_STD_SECTION 
			-> PRO_E_STD_SEC_SETUP_PLANE
			   -> PRO_E_STD_SEC_PLANE_ORIENT_REF
  \*---------------------------------------------------------------*/
  status = ProUtilCopySelectionSurf((ProSolid)sketch_info->solid,
         sketch_info->orient_surface, &(value_data.v.r));
  status = ProSelectionCopy( value_data.v.r, &sketch_refs[1]);
  TEST_CALL_REPORT("ProSelectionCopy", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );
  if(PRO_TK_NO_ERROR != status)
      return status;
  status = ProUtilElemtreeElementAdd( pro_e_std_sec_setup_plane, PRO_E_STD_SEC_PLANE_ORIENT_REF,
                                 ELEM_VALUE_TYPE_SELECTION, value_data.v.r);  
  
  /*---------------------------------------------------------------*\
    Creating incomplete feature in the current model.
  \*---------------------------------------------------------------*/
  status = ProMdlToModelitem( sketch_info->solid, &model_item ); 
  TEST_CALL_REPORT("ProMdlToModelitem", "CreateSketchFeatures",
			status, status != PRO_TK_NO_ERROR ); 
  status = ProSelectionAlloc (p_comp_path, &model_item,
			   &model_sel);
  TEST_CALL_REPORT("ProSelectionAlloc", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );
  

  status = ProArrayAlloc(1,sizeof(ProFeatureCreateOptions),
    1, (ProArray*)&opts);

  opts[0]= PRO_FEAT_CR_INCOMPLETE_FEAT;

  status = ProFeatureWithoptionsCreate (model_sel, pro_e_feature_tree,
    opts, PRO_REGEN_NO_FLAGS, feature, &errors);
  TEST_CALL_REPORT("ProFeatureWithoptionsCreate", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );

  status = ProArrayFree((ProArray*)&opts);
    			  
  /* Using the element tree from created feature */ 
  status = ProFeatureElemtreeExtract ( feature, p_comp_path, 
    PRO_FEAT_EXTRACT_NO_OPTS, &created_elemtree ); 
  TEST_CALL_REPORT("ProFeatureElemtreeExtract", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );
  
  status = ProUtilElementTreeDataGet(created_elemtree, path_items, 2, &elem_data);
  section = (ProSection)elem_data.val.p;  
  sketch_info->section = section;
/*---------------------------------------------------------------*\
   Creating a 3-D section 
\*---------------------------------------------------------------*/
  status = ProUtilSketchSectionBuild (section, sketch_info, sketch_refs );   

/*---------------------------------------------------------------*\
   Redefining the feature to make it complete.
\*---------------------------------------------------------------*/

    if(PRO_TK_NO_ERROR == status)
    {
        status = ProSelectionAsmcomppathGet (model_sel, &comp_path);  
        TEST_CALL_REPORT("ProSelectionAsmcomppathGet", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );  
        status = ProArrayAlloc(1,sizeof(ProFeatureCreateOptions),
                1, (ProArray*)&opts);

        opts[0]= PRO_FEAT_CR_DEFINE_MISS_ELEMS;

        status = ProFeatureWithoptionsRedefine (&comp_path, feature, created_elemtree,
                opts, PRO_REGEN_NO_FLAGS, &errors);
        TEST_CALL_REPORT("ProFeatureWithoptionsRedefine", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );

        status = ProArrayFree((ProArray*)&opts);
	}		            
/*---------------------------------------------------------------*\
    Free up the allocated memory.
\*---------------------------------------------------------------*/
  status = ProFeatureElemtreeFree (feature, created_elemtree ); 
  TEST_CALL_REPORT("ProFeatureElemtreeFree", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR );
  
  status = ProElementFree (&pro_e_feature_tree ); 
  TEST_CALL_REPORT("ProElementFree", "CreateSketchFeatures", 
			status, status != PRO_TK_NO_ERROR ); 
  return (status);
}

ProError CreateSketch()
{
    ProError status;
    SketchInfo sketch_info;
    ProFeature copy_feat;
    ProSection section;
    status = ProUtilInitializeSketch(&sketch_info);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = SectionInfoCollectAutoMode(&sketch_info, &section);
    ERROR_CHECK("SectionInfoCollectAutoMode", "CreateSketch", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    status = CreateSketchFeatures(&sketch_info, &copy_feat);
    ERROR_CHECK("CreateSketchFeatures", "CreateSketch", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    status = ProFeatureSectionCopy(&copy_feat, 0, &sketch_info.section);
    ERROR_CHECK("ProFeatureSectionCopy", "CreateSketch", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    status = ProUtilCopySectionConstraints(&sketch_info, &copy_feat, &section);
    ERROR_CHECK("ProUtilCopySectionConstraints", "CreateSketch", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    status = ProUtilInitializeSketch(&sketch_info);
    ERROR_CHECK("ProUtilInitializeSketch", "CreateSketch", status);
    return status;
}

extern "C" int user_initialize(
    int argc,
    char *argv[],
    char *version,
    char *build,
    wchar_t errbuf[80])
{
    ProError status;
    uiCmdCmdId cmd_id;

    ProTestErrlogOpen("aslm", version, build);
    ProStringToWstring(msg_file, "aslm_msg.txt");
    status = ProMenubarMenuAdd("ASLM", "AslmLabel", NULL, PRO_B_TRUE, msg_file);
    status = ProCmdActionAdd("SketchAction", (uiCmdCmdActFn)CreateSketch,
                             uiProe2ndImmediate, IsSketchAvailable,
                             PRO_B_TRUE, PRO_B_TRUE, &cmd_id);
    status = ProMenubarmenuPushbuttonAdd(
        "ASLM", "SketchButton",
        "SketchLabel",
        "SketchHelpLabel",
        NULL, PRO_B_TRUE, cmd_id, msg_file);

    return status;
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{
    ProTestErrlogClose();
}
