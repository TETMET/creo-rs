
#ifndef _TEST_ANALYSIS_H_
#define _TEST_ANALYSIS_H_

#include <ProObjects.h>
#include <ProExtdata.h>

#ifdef __cplusplus
extern "C" {
#endif



/*--------------------------------------------------------------------*\
    Data structure to describe the information needed by an analysis
    feature that creates a surface sys. The csys is centered at a
    datum point on a surface, with its X and Y axes aligned with the
    U and V mesh lines. The Z axis can be inwards or outwards, by
    user choice, and the origin can be offset from the datum point
    by an offset value that is stored as a feature dimension.
\*--------------------------------------------------------------------*/
typedef struct surf_csys_data
{
    /* References */
    ProSelection point;   /* The datum point referenced by the csys */

    /* Application-specific data */
    ProBoolean outwards;
    double offset;

    /* Results storage */
    ProCsysdata csysdata; /* The geometry of the csys */
} Surfcsysdata_t;

typedef struct curve_wrap_data
{
    /* References */
    ProFeature curve; /* Sketched datum curve containing a 2d csys */
    ProFeature point; /* Datum point on surface into which to wrap */

    /* Application-specific data */
    double angle;
    double offset;

    /* Results storage */
    ProSurfcurvedata *curves;
} Curvewrapdata_t;

typedef struct curve_data
{
    ProFeature curve;
    ProCurvedata *p_curve_data;
    double   offset;
} Curvedata_t;

typedef struct surface_data
{
    ProGeomitem surface;
    ProSurfacedata surf_data;
    double   offset;
    double   numpts;
} Surfacedata_t;

typedef struct comp_curve_data
{
    ProModelitem *curves;
    ProComponentCurvedata *components;
} CompCurvedata_t;

/*--------------------------------------------------------------------*\
    Common functions prototypes
\*--------------------------------------------------------------------*/
int ProTestAnalysisRegister ();

int ProTestClassRegister(
    ProMdl model,
    char *class_name,
    ProExtdataClass *extdata_class);

int ProTestIntegerSave(
    ProExtdataClass *extdata_class,
    char *slotname,
    int i);

int ProTestIntegerRetrieve(
    ProExtdataClass *extdata_class,
    char *slotname,
    int *i);

int ProTestOutwardsSave(
    ProFeature *feature,
    ProBoolean outwards);

int ProTestOutwardsRetrieve(
    ProFeature *feature,
    ProBoolean *outwards);

int ProTestClassExists(
    ProMdl model,
    char *name,
    ProExtdataClass *extdata_class);

int ProTestAtts ();

/*--------------------------------------------------------------------*\
    Analysis Csys callbacks prototypes
\*--------------------------------------------------------------------*/
ProError ProTestSurfcsysUiAction (
    ProAnalysis analysis);

ProError ProTestSurfcsysDimsAction(
    ProAnalysis analysis,
    double **dims);

ProError ProTestSurfcsysInfoallocAction(
    ProAnalysis analysis);

ProError ProTestSurfcsysInfofreeAction(
    ProAnalysis analysis);

ProError ProTestSurfcsysCompcheckAction(
    ProAnalysis analysis);

ProError ProTestSurfcsysComputeAction(
    ProAnalysis analysis);

ProError ProTestSurfcsysDisplayAction(
    ProAnalysis analysis);

ProError ProTestSurfcsysOutputAction(
    ProAnalysis analysis,
    ProLine **lines);

ProError ProTestSurfcsysSavecheckAction(
    ProAnalysis analysis);

ProError ProTestSurfcsysInfosaveAction(
    ProAnalysis analysis,
    ProFeature  *feature,
    ProSelection **references);

ProError ProTestSurfcsysInforetrieveAction(
    ProAnalysis analysis,
    ProFeature *feature,
    ProSelection *references);

ProError ProTestSurfcsysInfocopyAction(
    ProAnalysis from,
    ProAnalysis to);

ProError ProTestSurfcsysResultAction(
    ProAnalysis analysis,
    ProBoolean names_only,
    ProAnalysisParameter **parameters,
    ProAnalysisGeomitem  **geometry);

/*--------------------------------------------------------------------*\
    Analysis point callbacks prototypes
\*--------------------------------------------------------------------*/
ProError ProTestSurfpointResultAction(
    ProAnalysis analysis,
    ProBoolean names_only,
    ProAnalysisParameter **parameters,
    ProAnalysisGeomitem  **geometry);

/*--------------------------------------------------------------------*\
    Analysis surface curve callbacks prototypes
\*--------------------------------------------------------------------*/
ProError ProTestCurvewrapUiAction(
    ProAnalysis analysis);

ProError ProTestCurvewrapDimsAction(
    ProAnalysis analysis,
    double **dims);

ProError ProTestCurvewrapInfoallocAction(
    ProAnalysis analysis);

ProError ProTestCurvewrapInfofreeAction(
    ProAnalysis analysis);

ProError ProTestCurvewrapCompcheckAction(
    ProAnalysis analysis);

ProError ProTestCurvewrapComputeAction(
    ProAnalysis analysis);

ProError ProTestCurvewrapDisplayAction(
    ProAnalysis analysis);

ProError ProTestCurvewrapOutputAction(
    ProAnalysis analysis,
    ProLine **lines);

ProError ProTestCurvewrapSavecheckAction(
    ProAnalysis analysis);

ProError ProTestCurvewrapInfosaveAction(
    ProAnalysis analysis,
    ProFeature *feature,
    ProSelection **references);

ProError ProTestCurvewrapInforetrieveAction(
    ProAnalysis analysis,
    ProFeature *feature,
    ProSelection *references);

ProError ProTestCurvewrapInfocopyAction(
    ProAnalysis from,
    ProAnalysis to);

ProError ProTestCurvewrapResultAction(
    ProAnalysis analysis,
    ProBoolean names_only,
    ProAnalysisParameter **parameters,
    ProAnalysisGeomitem  **geometry);

/*--------------------------------------------------------------------*\
    Analysis Composite curve callbacks prototypes
\*--------------------------------------------------------------------*/
ProError ProTestCompCurveUiAction(
    ProAnalysis analysis);

ProError ProTestCompCurveDimsAction(
    ProAnalysis analysis,
    double **dims);

ProError ProTestCompCurveInfoallocAction(
    ProAnalysis analysis);

ProError ProTestCompCurveInfofreeAction(
    ProAnalysis analysis);

ProError ProTestCompCurveCompcheckAction(
    ProAnalysis analysis);

ProError ProTestCompCurveComputeAction(
    ProAnalysis analysis);

ProError ProTestCompCurveDisplayAction(
    ProAnalysis analysis);

ProError ProTestCompCurveOutputAction(
    ProAnalysis analysis,
    ProLine **lines);

ProError ProTestCompCurveSavecheckAction(
    ProAnalysis analysis);

ProError ProTestCompCurveInfosaveAction(
    ProAnalysis analysis,
    ProFeature *feature,
    ProSelection **references);

ProError ProTestCompCurveInforetrieveAction(
    ProAnalysis analysis,
    ProFeature *feature,
    ProSelection *references);

ProError ProTestCompCurveInfocopyAction(
    ProAnalysis from,
    ProAnalysis to);

ProError ProTestCompCurveResultAction(
    ProAnalysis analysis,
    ProBoolean names_only,
    ProAnalysisParameter **parameters,
    ProAnalysisGeomitem  **geometry);

/*--------------------------------------------------------------------*\
    Analysis datum curve callbacks prototypes
\*--------------------------------------------------------------------*/
ProError ProTestCurveUiAction (
    ProAnalysis analysis);

ProError ProTestCurveDimsAction(
    ProAnalysis analysis,
    double **dims);

ProError ProTestCurveInfoallocAction(
    ProAnalysis analysis);


ProError ProTestCurveInfofreeAction(
    ProAnalysis analysis);

ProError ProTestCurveCompcheckAction(
    ProAnalysis analysis);

ProError ProTestCurveComputeAction(
    ProAnalysis analysis);

ProError ProTestCurveDisplayAction(
    ProAnalysis analysis);

ProError ProTestCurveOutputAction(
    ProAnalysis analysis,
    ProLine **lines);

ProError ProTestCurveSavecheckAction(
    ProAnalysis analysis);

ProError ProTestCurveInfosaveAction(
    ProAnalysis analysis,
    ProFeature  *feature,
    ProSelection **references);

ProError ProTestCurveInforetrieveAction(
    ProAnalysis analysis,
    ProFeature *feature,
    ProSelection *references);

ProError ProTestCurveInfocopyAction(
    ProAnalysis from,
    ProAnalysis to);

ProError ProTestCurveResultAction(
    ProAnalysis analysis,
    ProBoolean names_only,
    ProAnalysisParameter **parameters,
    ProAnalysisGeomitem  **geometry);

/*--------------------------------------------------------------------*\
    Analysis surface callbacks prototypes
\*--------------------------------------------------------------------*/
ProError ProTestSurfaceUiAction (
    ProAnalysis analysis);

ProError ProTestSurfaceDimsAction(
    ProAnalysis analysis,
    double **dims);

ProError ProTestSurfaceInfoallocAction(
    ProAnalysis analysis);


ProError ProTestSurfaceInfofreeAction(
    ProAnalysis analysis);

ProError ProTestSurfaceCompcheckAction(
    ProAnalysis analysis);

ProError ProTestSurfaceComputeAction(
    ProAnalysis analysis);

ProError ProTestSurfaceDisplayAction(
    ProAnalysis analysis);

ProError ProTestSurfaceOutputAction(
    ProAnalysis analysis,
    ProLine **lines);

ProError ProTestSurfaceSavecheckAction(
    ProAnalysis analysis);

ProError ProTestSurfaceInfosaveAction(
    ProAnalysis analysis,
    ProFeature  *feature,
    ProSelection **references);

ProError ProTestSurfaceInforetrieveAction(
    ProAnalysis analysis,
    ProFeature *feature,
    ProSelection *references);

ProError ProTestSurfaceInfocopyAction(
    ProAnalysis from,
    ProAnalysis to);

/*--------------------------------------------------------------------*\
    Results callback for non-NURBS surface
\*--------------------------------------------------------------------*/
ProError ProTestSurfaceResultAction(
    ProAnalysis analysis,
    ProBoolean names_only,
    ProAnalysisParameter **parameters,
    ProAnalysisGeomitem  **geometry);

/*--------------------------------------------------------------------*\
    Results callback for NURBS surface
\*--------------------------------------------------------------------*/
ProError ProTestNURBSSurfaceResultAction(
    ProAnalysis analysis,
    ProBoolean names_only,
    ProAnalysisParameter **parameters,
    ProAnalysisGeomitem  **geometry);

/*====================================================================*\
FUNCTION : ProTestCurvePlaneGet()
PURPOSE  : 
\*====================================================================*/
int ProTestCurvePlaneGet(
    ProFeature *feature,
    ProVector origin,
    ProVector x,
    ProVector y);
    
 #ifdef __cplusplus
 }
 #endif

#endif /* _TEST_ANALYSIS_H_ */
