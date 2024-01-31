#ifndef TEST_RULE
#define TEST_RULE




/*--------------------------------------------------------------------*\
    Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProAsmcomppath.h>
#include <ProSelection.h>
#include <ProRule.h>


/*--------------------------------------------------------------------*\
    Application data types
\*--------------------------------------------------------------------*/


/* An array of the ProAsmcomppath objects with the specified size */
typedef struct tag_AsmcomppathList
	{
	    ProAsmcomppath*	p_comp_path_list;
	    int			size;
	} AsmcomppathList;


/*--------------------------------------------------------------------*\
    Application global/external data
\*--------------------------------------------------------------------*/


/*---------------------------------------------------------------------*\
    Functions declaration
\*---------------------------------------------------------------------*/
ProError    ProSelectWithRules( char option[], int, ProSelection*, 
				ProSelFunctions*, ProSelectionEnv, 
				ProSelAppAction, ProSelection**, int* );
int	    ProTestRules( AsmcomppathList* );
int	    ProTestRuleInitName( ProRule*, int );
int	    ProTestRuleInitExpr( ProRule*, int );
int	    ProTestRuleInitZone( ProRule*, int );
int	    ProTestRuleInitDist( ProRule*, int );
int	    ProTestRuleInitSize( ProRule*, int );
int	    ProTestRuleInitRep( ProRule*, int );


#endif	/* TEST_RULE */
