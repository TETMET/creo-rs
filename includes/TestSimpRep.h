#ifndef TEST_SREP
#define TEST_SREP

#include <ProSolid.h>
#include <ProSimprep.h>
#include <ProSimprepdata.h>

/*--------------------------------------------------------------------*\
Application data types
\*--------------------------------------------------------------------*/

typedef enum simp_rep_actions {
    SET_CURR_REP = 1,
    REP_INFO,
    COPY_REP,
    REDEFINE_REP,
    DELETE_REP,
    CREATE_REP_EX,
    CREATE_REP_IN,
    CREATE_REP_GRAPH,
    CREATE_REP_GEOM,
    SUBSTITUTE,
    REVERSE,
    RENAME,
    MAKE_PERM,
    DEF_RULE,
    ADD_ITEM,
    DEL_ITEM,
    DEFAULT
} SimpRepActions;

typedef struct simp_rep_name {
    wchar_t         name[PRO_NAME_SIZE];
    ProSolid       *owner;
    ProSimprep      simp_rep;
    ProSimprepdata *data;
    int             item_count;
} SimpRepData;

typedef struct simp_rep_item_data {
    ProSimprep *owner;
    ProColortype color;	
} ItemColorData;

typedef struct owner_fp_data {
    ProSolid *solid;
    FILE     *fp;
} SolidFpData;

/*--------------------------------------------------------------------*\
Application global/external data
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Top level function
\*--------------------------------------------------------------------*/
int ProTestSimpRep(ProSolid*);

/*--------------------------------------------------------------------*\
Menu functions
\*--------------------------------------------------------------------*/
int ProTestSRepCreate(SimpRepData*, SimpRepActions);
int ProTestSRepEdit(SimpRepData*, SimpRepActions);
int ProTestSRepList(ProSolid*, SimpRepActions);
int ProTestSRepDefRule(ProSolid*);

/*--------------------------------------------------------------------*\
Info functions
\*--------------------------------------------------------------------*/
int ProTestSRepInfo(ProSolid*);
ProError ProTestSRepVisitInfo(ProSimprep*, ProError, ProAppData);
ProError ProTestSRepItemsVisit ( ProSimprepitem* ,ProError,SolidFpData*); 
ProError ProTestSRepVisitFilt ( ProSimprep*, FILE*);
ProError ProTestSRepItemInfo( ProSimprep*, FILE*);

/*--------------------------------------------------------------------*\
Action functions
\*--------------------------------------------------------------------*/
int ProTestSRepItem( SimpRepData*, SimpRepActions);
int ProTestSRepRepaint(ProSolid*);
int ProTestSRepRetrieve();
ProError ProTestSRepCopy( ProSimprep* simp_rep); 
ProError ProTestSRepCreateNew( SimpRepData *sr_data, SimpRepActions action);
int ProTestSRepAction( SimpRepData *sr_data, SimpRepActions action);
int ProTestSRepRedefine( SimpRepData *sr_data, SimpRepActions action);
int ProTestSRepRedefineMenu( SimpRepData *sr_data, SimpRepActions action);
 
/*--------------------------------------------------------------------*\
Helper functions
\*--------------------------------------------------------------------*/
ProError ProTestSRepNewNameGet( wchar_t*);
void ProTestSRepVisActionSet (ProSimprepActionType);
ProError ProTestSRepItemsColor (ProSimprepitem*, ProError, ItemColorData*);
ProError ProTestSRep2ColorDisplay(SimpRepData *sr_data);
ProError ProTestSubstitutionInit( ProSolid* p_solid, ProSubstitution* p_subst );
int ProTestSubstTypeSet( ProSubstType* p_subst_type, ProSubstType subst_type );
int ProTestRulesToggle( void* p_dummy, int int_dummy );

#endif
