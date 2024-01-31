

#ifndef TESTCABLE_H
#define TESTCABLE_H

#include <UtilCable.h>

#define TEST_CABLE	    0
#define TEST_CONNECTOR	    1

#define TEST_SPOOL_READ	    0
#define TEST_SPOOL_WRITE    1
#define TEST_SPOOL_CREATE   2
#define TEST_SPOOL_EDIT	    3
#define TEST_SPOOL_LIST	    4

#define TEST_CABLES_ASSIGN  0
#define TEST_CABLES_SHOW    1 

#define TEST_SPOOL_LIST_FILE "spools.lst"
#define TEST_MAX_TOKENS	    10

#define TEST_CONN_MODIFY	0
#define TEST_CONN_DESIGNATE	1
#define TEST_CONN_UNDESIGNATE	2


#define TEST_PARAM_MOD		0
#define TEST_PARAM_SHOW		1
#define TEST_PARAM_SAVE_DEFAULT 2
#define TEST_PARAM_RETR		3
#define TEST_PARAM_SAVE		4

#define TEST_PORT_MODIFY	0
#define TEST_PORT_DELETE	1	

#define TEST_HARNESS_ITEMS	0
#define TEST_HARNESS_WIRELIST	1
#define TEST_HARNESS_ROUTING	2


#define MSGFIL _msgfil
#define SET_CURRENT_HARNESS(p_harness) {_harness = *p_harness; \
					_is_harness = 1;}
#define IS_CURRENT_HARNESS (_is_harness)

#define TEMP_FILE "temp.cbl"

#define TEST_MAX_TOKEN	20

#define TEST_CABLE_CREATE 0

typedef  struct test_param_owner
{
    void *param_owner;
    int  owner_type; /* 0 - ProCblCable, 1 - ProCblConnector */
} TestParamOwner;

/*--------------------------------------------------------------------*\
Application global/external data
\*--------------------------------------------------------------------*/
extern ProName _msgfil;
extern int _is_harness;
extern ProCblHarness _harness;


/*--------------------------------------------------------------------*\
Function prototypes
\*--------------------------------------------------------------------*/
void ProUtilSetCablingMenu();

/* Harness related functions */
int ProTestHarness(ProAssembly*);
int ProTestHarnessAction(ProAssembly*, int);
int ProTestHarnessCablesAction(ProAssembly*, int);

/* Cables related functions */
int ProTestCable(ProAssembly *);
int ProTestCableCreate(ProAssembly *, int);
int ProTestCableAction(ProAssembly *, int);
int ProTestCablingInfoAction(ProAssembly *, int);
ProError ProTestCablingInfoCableWrite(FILE *, ProAssembly, ProCblCable *, int);
ProError ProTestCableFromLogical(ProAssembly *);
int ProTestCableModifyParameters(ProAssembly *p_assy);
int ProTestCableModify(ProAssembly *);
int ProTestCurrentHarnessPreFilter(Select3d *sel);

/* Spools related functions */
int ProTestSpool(ProAssembly*);
int ProTestSpoolAction(ProAssembly*, int);
ProError ProTestSpoolParametersEdit(ProCblSpool*);
ProError ProUtilSpoolParametersToSpoolType(ProCblParameterInfo **, int *,
    ProName, Procbl_cable_type *, Procbl_sheath_type *);
void ProUtilSetSpoolMenu(ProAssembly *);
ProError ProTestSpoolSelect(ProAssembly *, int, Procbl_cable_type,
    ProCblSpool *);

/* Connector related functions */
int ProTestConnector(ProAssembly *);
int ProTestConnectorAction(ProAssembly *, int);
int ProTestConnectorParameters(ProCblConnector *);
int ProTestConnectorPorts(ProCblConnector *);
ProError ProTestConnParametersRead(ProCblConnector *, char*);
int ProTestConnPortsAction(ProCblConnector *, int);

/*  Logical ref & Info related functions */
int ProTestLogicalRefImport(ProAssembly *);
int ProTestLogicalRef(ProAssembly *);
int ProTestCablingInfo(ProAssembly *);

/* Rounting related functions */
int ProTestCableRoute (ProAssembly *);
int ProTestSelLoc(void **, int);

/* Different utility functions */
ProError ProUtilFileOpen(char *,char *);
ProError ProUtilParametersFileWrite(FILE *, ProCblParameterInfo *, int);
ProError ProUtilParametersFileRead(char *, ProCblParameterInfo **,int *);
ProError ProUtilParametersFree(ProCblParameterInfo **);
int ProUtilParametersMenu(TestParamOwner *);
int ProUtilParametersAction(TestParamOwner *, int);
ProError ProUtilParametersRead(TestParamOwner *, char *);


#endif /* TESTCABLE_H  */
