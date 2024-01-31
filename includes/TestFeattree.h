
#ifndef TESTFEATTREE_H
#define TESTFEATTREE_H

/*--------------------------------------------------------------------*\
ProToolkit includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProSelection.h>
#include <ProElemId.h>
#include <TestSect.h>

/*--------------------------------------------------------------------*\
Application defines
\*--------------------------------------------------------------------*/

#define    MASK_SURFACE		1
#define    MASK_EDGE		(1 << 1)
#define    MASK_DATUM		(1 << 2)
#define    MASK_CSYS		(1 << 3)
#define    MASK_AXIS		(1 << 4)
#define    MASK_POINT		(1 << 5)
#define    MASK_CURVE		(1 << 6)
#define    MASK_EDGE_END	(1 << 7)
#define    MASK_CURVE_END	(1 << 8)
#define    MASK_FEATURE		(1 << 9)
#define    MASK_DIMENSION	(1 << 10)
#define    MASK_PART		(1 << 11)
#define    MASK_QUILT		(1 << 12)

/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application data structures
\*--------------------------------------------------------------------*/
typedef enum tree_add_option
{
   MENU_DRIVEN,
   FROM_FILE,
   DIALOG_BASED 
}ProTestUIOption;

typedef enum tree_def_option
{
   TREE_INTERACTIVE,
   TREE_BY_TYPE,
   TREE_EXISTING
}ProTestTreeDefOpt;

typedef enum feat_opt
{
   CREATE,
   REDEFINE,
   REPLACE,
   LOAD
} ProTestFeatOpt;

typedef enum elem_action
{
   ADD,
   REMOVE,
   MODIFY,
   DONE
} ProTestElemAction;

typedef enum value_opt
{
    ONE_VALUED_ELEM,
    MANY_VALUED_ELEM    
} ProTestValueOpt;

typedef enum pro_test_sketch_type
{
    SKET_2D,
    SKET_3D
} ProTestSketType;

typedef struct pro_sel_option_mask
{
     char	*name;
     int	id;
} ProSelOptionMask;

/*--------------------------------------------------------------------*\
Prototypes                 
\*--------------------------------------------------------------------*/
#endif
