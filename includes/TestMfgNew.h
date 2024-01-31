

#ifndef TESTMFGNEW_H
#define TESTMFGNEW_H

/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProMdl.h>
#include <ProMfg.h>
#include <ProTool.h>
#include <ProToolElem.h>
#include <ProToolinput.h>
#include <ProFeature.h>
#include <ProObjects.h>
#include <ProSolid.h>
#include <ProElemId.h>
#include <ProMfgoper.h>
#include <ProNcseq.h>
#include <ProNcseqElem.h>
#include <ProGeomitem.h>
#include <ProParamval.h>
#include <ProItemerr.h>
#include <MfgMenu.h>
#include <UtilMfgNew.h>

typedef enum {ERROR = -1, EMPTY, STRING, INT, DOUBLE} ValueType ;

#define ARRAY            101
#define COMPOUND         102
#define COMPLEX          103

#define PARAM_FIELD_WIDTH 21

typedef struct {
    char     param_name[PRO_NAME_SIZE];
    double   paramval;
} ParamTable;

typedef struct elem_table{
    int elem_type;
    int val_type;
} ElemTable;

typedef struct workcell {
        ProWcellType   type;
        int            n_axis;
        wchar_t        *wname;
} Workcell;

typedef struct retract_info
{
     wchar_t name[PRO_NAME_SIZE];
     int retract_id;
} RetractInfo;

typedef struct pro_test_ncseq
{
    int num_ncseq;
    wchar_t **p_ncseq_names;
    ProNcseq* *p_ncseqptr_array;
} Pro_Test_Ncseq;


typedef struct operation {
        int      wcell_id;
        int      csys_id;
        wchar_t  *wname;
} Operation ;

typedef enum tool_creation_type
{
     FROM_SOLID,
     FROM_PARAMFILE,
     FROM_SOLID_BY_COPY,
     FROM_PARAMFILE_BY_COPY,
     FROM_SOLID_BY_REF,
     FROM_PARAMFILE_BY_REF,
     BY_COPY,
     BY_REF
} ToolCreationType;

typedef struct pro_test_tool
{
    wchar_t **tool_ids;
    int num_tools;
} Pro_Test_Tool;


typedef enum param_file_type
{
     TOOL,
     MACHINE
} ParamFileType;

ProError ProTempCreateWkcell(Workcell, ProFeature *);
ProError ProTestMfgObjectsCollect(ProMdl, MfgObjList *);
ProError ProTempOperationCreate(Operation, ProFeature*);
ProError ProTestOperationCreate(MfgMenuChoice*);
ProError ProTestNCSequenceCreate(MfgMenuChoice*);
ProError ProTestWkcellRedefine(MfgMenuChoice *obj_action);
ProError ProTestWkcellCreate(MfgMenuChoice *obj_action);
ProError ProTestCreateTool(MfgMenuChoice *obj_action);
ProError ProTestToolRedefine(MfgMenuChoice *obj_action);
ProError ProTestRouteNCseq();
ProError ProTestRouteTool();
ProError ProTestGetNcseqInfo(ProNcseq*);
ProError ProTestGetToolInfo( ProTool *);

ProError ProTestFixtureCreate(MfgMenuChoice *);
int ProTestFixtCreate(ProAppData, int);
int ProTestFixtAction(ProAppData, int);
int ProTestFixtList(MfgMenuChoice *);
ProError ProTestFixtureDisp(MfgMenuChoice *);
ProError ProTestFixtDeactivate();
ProError ProTestFixtureActivate (MfgMenuChoice *);
ProError ProTestFixtActivate(ProAppData, ProAppData, int);
ProError ProTestFixtureDelete(MfgMenuChoice *);
ProError ProTestFixtDelete(ProAppData, ProAppData, int);

ProError ProTestOperationDisplay(MfgMenuChoice *);
ProError ProTestOperToolPathDisp(ProAppData, ProAppData, int);


/*--------------------------------------------------------------------*\
Utility functions
\*--------------------------------------------------------------------*/
ProError ProTestCreateSelection( int, ProType, ProSelection*);
ProError ProTestGetModels( ProMfg*, ProSolid*, ProAsmcomppath*);
ProError ProUtilElementBuild (ProValueDataType, void*, ProElement*);
ToolCreationType ProUtilSelectCreationType();
ProError ProUtilToolFromSolidAdd(ProToolElem);
ProError ProTestToolCount( PRO_CONST_ARG ProTool *, void *);
ProError ProTestToolsGet( PRO_CONST_ARG ProTool *, void *);
ProNcseq* ProUtilGetNcptr(wchar_t *, Pro_Test_Ncseq *);
ProError ProTestFeatFilterNcseq(ProModelitem *, ProAppData);
ProError ProTestNCseqVisit(ProFeature *, ProError, ProAppData);
ProError ProUtilParamFileRead(char*, ParamFileType, void*);
ProNcseq* ProUtilNcseqChoose(ProSolid *);

#endif
