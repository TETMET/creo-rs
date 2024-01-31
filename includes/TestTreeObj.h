
#ifndef TESTTREEOBJ_H
#define TESTTREEOBJ_H

/*--------------------------------------------------------------------*\
Pro/Toolkit includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProFeature.h>
#include <ProElemId.h>
#include <ProElement.h>
#include <ProElempath.h>

#include <TestFeattree.h>


/*--------------------------------------------------------------------*\
OBJECT ProTestValue
ACTION responsible for the UI and creation of the ProValue
\*--------------------------------------------------------------------*/
typedef struct pro_test_value
{
 /* Members */	
	ProTestValueOpt value_option;
	ProValueData value_data;
	ProValue current_value;
	int num_values;					/* if value_opt = MANY_VALUED_ELEM */
	ProValue *value_array;

 /* Member functions */
	ProError (*set_value_opt)(struct pro_test_value *, ProTestValueOpt);
	ProError (*set_value_type)(struct pro_test_value *, ProValueDataType);
	ProError (*ui_value_opt_menu)(struct pro_test_value *);
	ProError (*ui_value_type_menu)(struct pro_test_value *);
	ProError (*ui_value_data_set)(struct pro_test_value *);
	ProError (*pointer_data_set)(struct pro_test_value *, void *);
	ProError (*create_current_value)(struct pro_test_value *);
	ProError (*set_num_values)(struct pro_test_value *, int);
	ProError (*ui_num_values_menu)(struct pro_test_value *);
	ProError (*add_to_value_array)(struct pro_test_value *);
	ProError (*value_free)(struct pro_test_value *);
} ProTestValue;

ProError ProTestValue_Alloc( ProTestValue **);
ProError ProTestValOptSet( ProTestValue *, ProTestValueOpt);
ProError ProTestValTypeSet( ProTestValue *, ProValueDataType);
ProError ProTestOptMenu( ProTestValue * );
ProError ProTestTypeMenu( ProTestValue * );
ProError ProTestValueDataSet( ProTestValue * );
ProError ProTestValuePointerSet( ProTestValue *, void *);
ProError ProTestCreateCurrentValue( ProTestValue * );
ProError ProTestValueFree( ProTestValue * );
ProError ProTestAddToValueArray( ProTestValue * );


/*--------------------------------------------------------------------*\
OBJECT ProTestPath
ACTION responsible for the UI and path construction
\*--------------------------------------------------------------------*/
typedef struct pro_test_path
{
 /* Members */
   ProTestElemAction option;   /* user action option */
   ProElement tree;                     /* the existing tree */
   ProElempathItem *path_array;/* x_array of elements in the path */
   ProElement  last_elem;

 /* Member functions */
   ProError (*update_path_size)(struct pro_test_path *);
   ProError (*add_to_path_array)(struct pro_test_path *, ProElempathItem);
   ProError (*ui_navigate)(struct pro_test_path *);
   ProElement (*return_last_elem)(struct pro_test_path *);
}ProTestElempath;

ProError ProTestPath_Alloc( ProTestElempath **, ProElement, ProTestElemAction);
ProError ProTestUpdateSize( ProTestElempath * );
ProError ProTestAddToPathArray( ProTestElempath *, ProElempathItem );
ProError ProTestNavigate( ProTestElempath * );
ProElement ProTestGetLastElem( ProTestElempath * );
ProError ProTestElempathFree( ProTestElempath * );


/*--------------------------------------------------------------------*\
OBJECT ProTestElem
ACTION responsible for the UI and creation of the element
\*--------------------------------------------------------------------*/
typedef struct pro_test_elem
{
 /* Members */
   ProTestUIOption option;	/* user interface option */
   ProElemId name_id;           /* element id to add from ProElemId.h */
   ProTestValue *value;		/* value(s) of this element */	
   ProTestElempath *modify_path;/* used when modifying a sketcher element */ 
   ProElement element;		/* the created element */

 /* Member functions */
   ProError (*set_name_id)(struct pro_test_elem *,ProElemId);
   ProError (*set_value)(struct pro_test_elem *);
   ProError (*set_sketch_value)(struct pro_test_elem *);
   ProError (*ui_name_id_menu)(struct pro_test_elem *);
   ProBool (*is_value_needed)(struct pro_test_elem *);
   ProTestValueOpt (*how_many_values)(struct pro_test_elem *);
   ProError (*create_element)(struct pro_test_elem *);
   ProElement (*return_element)(struct pro_test_elem *);
}ProTestElem;

ProError ProTestElem_Alloc( ProTestElem **, ProTestUIOption, ProTestElempath * );
ProError ProTestNameidSet( ProTestElem *, ProElemId );
ProError ProTestValueSet( ProTestElem * );
ProError ProTestSketchValSet( ProTestElem * );
ProError ProTestNameidFromMenu( ProTestElem * );
ProBool ProTestIsValueDataNeeded( ProTestElem * );
ProTestValueOpt ProTestHowManyValues( ProTestElem * );
ProError ProTestElementCreate( ProTestElem * );
ProElement ProTestElementReturn( ProTestElem * );
ProError ProTestElemFree( ProTestElem * );


/*--------------------------------------------------------------------*\
OBJECT ProTestTree
ACTION adds an element to a feature tree
\*--------------------------------------------------------------------*/
typedef struct pro_test_feature_tree
{
 /* Members */
   ProTestUIOption option;			/* user interface option */
   ProTestElem  *element;				/* element to add to a tree */
   ProTestElempath *elem_path;			/* the path of elem in a tree */
   ProElement  elemtree;			/* the element tree top level */
   ProTestElemAction elem_act;


 /* Member functions */
   ProError (*add_ui_opt)(struct pro_test_feature_tree *);
   ProError (*element_set)(struct pro_test_feature_tree *); 
   ProError (*path_set)(struct pro_test_feature_tree *);
   ProError (*elem_action_set)(struct pro_test_feature_tree *);
   ProError (*add_to_tree)(struct pro_test_feature_tree *);
   ProError (*remove_from_tree)(struct pro_test_feature_tree *);
   ProError (*modify_in_tree)(struct pro_test_feature_tree *);
   ProElement (*return_tree)(struct pro_test_feature_tree *);

}ProTestTree;

ProError ProTestTree_Alloc( ProTestTree ** , ProElement );
ProError ProTestUIOptSet(ProTestTree *);
ProError ProTestElemActionSet(ProTestTree *);
ProError ProTestUIOptSet(ProTestTree *);
ProError ProTestTreeElemAdd(ProTestTree *);
ProError ProTestTreeElemRemove(ProTestTree *);
ProError ProTestTreeElemModify(ProTestTree *);
ProError ProTestElementSet(ProTestTree *);
ProError ProTestElempathSet(ProTestTree *);
ProElement ProTestTreeReturn(ProTestTree *);

/*--------------------------------------------------------------------*\
OBJECT ProTestFeat
ACTION actions for creating/redefining a feature
\*--------------------------------------------------------------------*/
typedef struct pro_test_feat
{
 /* Members */
   ProTestFeatOpt feat_opt;
   int cr_opts_size;
   ProFeatureCreateOptions create_opts[4];
   ProTestTreeDefOpt tree_def_opt;
   ProElement elemtree;
   ProSelection model;
   ProBool first_feat;

 /* Member functions */
   ProError (*get_options)(struct pro_test_feat *);
   ProError (*create_tree)(struct pro_test_feat *);
   ProError (*get_model)(struct pro_test_feat *);
   ProError (*create_feature)(struct pro_test_feat *);
   ProError (*redefine_feature)(struct pro_test_feat *);
   ProError (*get_feature)(struct pro_test_feat *);
   ProError (*is_first_feat)(struct pro_test_feat *);
   ProError (*create_tree_interactively)(struct pro_test_feat *);
   ProError (*create_tree_by_type)(struct pro_test_feat *);

} ProTestFeat;

ProError ProTestFeat_Alloc(ProTestFeat **,ProTestFeatOpt);
ProError ProTestFeatcrOptsGet(ProTestFeat *);
int ProUtilCrOptFlagSet(ProAppData, int);
int ProUtilCrOptMenuExit(ProAppData, int);
ProError ProUtilFeatModelGet(ProTestFeat *);
ProError ProUtilModelGet(ProSelection *);
ProBool ProUtilIsFirstFeat(ProSelection );
ProError ProUtilFeatModelIsFirstFeat(ProTestFeat *);
ProError ProUtilFeatureGet(ProTestFeat *);
ProError ProTestTreeCreate(ProTestFeat *);
ProError ProTestTreeInteractiveCreate(ProTestFeat *);
ProError ProTestFeatureCreate(ProTestFeat *);
ProError ProTestFeatureRedefine(ProTestFeat *);

#endif
