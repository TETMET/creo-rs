
#ifndef TREEOBJECTS_H
#define TREEOBJECTS_H
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
 Const to set pattern/feature creation
\*--------------------------------------------------------------------*/
#define USER_CREATE_FEATS 0
#define USER_CREATE_PAT 1

/*--------------------------------------------------------------------*\
 Const to select
\*--------------------------------------------------------------------*/
#define USER_SELECT_FEATS 0
#define USER_SELECT_COMP  1


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
        void *the_elem;
	ProValue *value_array;

 /* Member functions */
	ProError (*set_value_opt)(struct pro_test_value *, ProTestValueOpt);
	ProError (*set_value_type)(struct pro_test_value *, ProValueDataType);
        ProError (*set_value_data)(struct pro_test_value *, char *);
	ProError (*create_current_value)(struct pro_test_value *);
	ProError (*add_to_value_array)(struct pro_test_value *);
	ProError (*value_free)(struct pro_test_value *);
        ProError (*value_data_free)(struct pro_test_value *);
        ProError (*value_array_free)(struct pro_test_value *);
} ProTestValue;

ProError ProTestValue_Alloc( ProTestValue **, void *);
ProError ProTestValOptSet( ProTestValue *, ProTestValueOpt);
ProError ProTestValTypeSet( ProTestValue *, ProValueDataType);
ProError ProTestCreateCurrentValue( ProTestValue * );
ProError ProTestValueFree( ProTestValue * );
ProError ProTestAddToValueArray( ProTestValue * );
ProError ProTestSetValueData( ProTestValue *, char *);
ProError ProTestValDataFree( ProTestValue * );
ProError ProTestValArrayFree( ProTestValue * );




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
   ProElement element;		/* the created element */

 /* Member functions */
   ProError (*set_name_id)(struct pro_test_elem *,ProElemId);
   ProError (*set_sketch_value)(struct pro_test_elem *, void *);
   ProError (*ui_name_id_menu)(struct pro_test_elem *);
   ProBool (*is_value_needed)(struct pro_test_elem *);
   ProTestValueOpt (*how_many_values)(struct pro_test_elem *);
   ProError (*create_element)(struct pro_test_elem *);
   ProElement (*return_element)(struct pro_test_elem *);
   ProError (*free_elem)(struct pro_test_elem *);
}ProTestElem;

ProError ProTestElem_Alloc( ProTestElem **);
ProError ProTestNameidSet( ProTestElem *, ProElemId );
ProError ProTestSketchValSet( ProTestElem *, void *);

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
   ProTestElem  *element;			/* element to add to a tree */
   ProElement  elemtree;			/* the element tree top level */
   ProTestElemAction elem_act;
   ProElement action_elem;


 /* Member functions */
   ProError (*add_ui_opt)(struct pro_test_feature_tree *);
   ProError (*elem_action_set)(struct pro_test_feature_tree *);
   ProError (*add_to_tree)(struct pro_test_feature_tree *);
   ProError (*remove_from_tree)(struct pro_test_feature_tree *);
   ProError (*modify_in_tree)(struct pro_test_feature_tree *);
   ProElement (*return_tree)(struct pro_test_feature_tree *);
   int (*dialog_tree_editor)(struct pro_test_feature_tree *);

}ProTestTree;

ProError ProTestTree_Alloc( ProTestTree ** , ProElement );
ProError ProTestUIOptSet(ProTestTree *);
ProError ProTestElemActionSet(ProTestTree *);
ProError ProTestUIOptSet(ProTestTree *);
ProError ProTestTreeElemAdd(ProTestTree *);
ProError ProTestTreeElemRemove(ProTestTree *);
ProError ProTestTreeElemModify(ProTestTree *);
ProElement ProTestTreeReturn(ProTestTree *);
int DialogTreeEditor(ProTestTree *);

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
   ProError (*create_tree_by_type)(struct pro_test_feat *);
   ProError (*print_errors)(ProErrorlist);

} ProTestFeat;

ProError ProTestFeat_Alloc(ProTestFeat **,ProTestFeatOpt);
ProError ProTestFeat_Free(ProTestFeat **);
ProError ProTestFeatcrOptsGet(ProTestFeat *);
int ProUtilCrOptFlagSet(ProAppData, int);
int ProUtilCrOptMenuExit(ProAppData, int);
ProError ProUtilFeatModelGet(ProTestFeat *);
ProError ProUtilModelGet(ProSelection *);
ProBool ProUtilIsFirstFeat(ProSelection );
ProError ProUtilFeatModelIsFirstFeat(ProTestFeat *);
ProError ProUtilFeatureGet(ProTestFeat *);
ProError ProTestTreeCreate(ProTestFeat *);
ProError ProTestFeatureCreate(ProTestFeat *);
ProError ProTestFeatureRedefine(ProTestFeat *);
ProError ProTestPrintErrs(ProErrorlist);



/*--- Some function prototypes ----*/

typedef struct user_supplied_data
{
   ProElement elem;
   ProElempath elem_path;
} UserData;

ProError ProUtilElempathGet(ProElement, ProElement, ProElempath);
ProError GetpathAction(ProElement, ProElement, ProElempath, UserData *);


int ProTestFeatureCreateAndRedefine(ProTestFeat *,ProTestFeatOpt);
int ProTestFeatureTreeInfoGet();
ProError ProTestSketchPntPostFilter(ProSelection, ProAppData);
void ProTestOptionMaskGet (char *);

/*--------------------------------------------------------------------*\
 Variable to set patter/feature creation
\*--------------------------------------------------------------------*/
extern int user_create_feats;  
extern int user_select_feats;

#endif
