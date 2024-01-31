
#ifndef TESTFEAT_H
#define TESTFEAT_H

/*--------------------------------------------------------------------*\
ProToolkit includes
\*--------------------------------------------------------------------*/
#include <ProObjects.h>

/*--------------------------------------------------------------------*\
Application defines
\*--------------------------------------------------------------------*/
#define PRO_TEST_SECT_NOT_REGEN   -10

/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/
#define TEST_THICKFORM	1 << 1
#define TEST_DEPTH	1 << 2
#define TEST_ANGLE	1 << 3
#define TEST_ATTRIBUTE	1 << 4
#define TEST_SECTION	1 << 5
#define TEST_DIRECTION	1 << 6
#define TEST_MATERIAL	1 << 7 



/*--------------------------------------------------------------------*\
Application data structures
\*--------------------------------------------------------------------*/
typedef enum pro_test_option
	{
	   PRO_TEST_FEAT_UNDEF = 0,
	   PRO_TEST_FEAT_CREATE,
	   PRO_TEST_FEAT_REDEF
	} ProTestUserOpt;

typedef enum pro_test_side
	{
	   PRO_TEST_FIRST_SIDE = 0,
	   PRO_TEST_SECOND_SIDE
	} ProTestSide;

typedef struct pro_test_materside_data
	{
	   int ent_id;
	   double param;
	   ProVector dir_vector;
	} ProTestMaterSideData;
/*--------------------------------------------------------------------*\
Application functions
\*--------------------------------------------------------------------*/
int  ProTestFeatMnu();
int ProUtilAssign(void *, int);

ProError ProTestAxisPattern();
ProError ProTestCurvePattern();
ProError ProTestGeneralPattern();
ProError ProTestDirectionPattern();
ProError ProTestFillPattern();
ProError ProTestPointPattern();
ProError ProTestTablePattern();

#endif

