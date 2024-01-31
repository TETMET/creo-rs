
  
#ifndef TESTFEATTYPEH
#define TESTFEATTYPEH

/*--------------------------------------------------------------------*\
Pro/Toolkit includes -- include this first
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <TestError.h>
   
/*--------------------------------------------------------------------*\
Pro/Develop includes
\*--------------------------------------------------------------------*/
#include <ProElement.h>
#include <ProElempath.h>
#include <ProFeature.h>
#include <ProStdSection.h>
#include <ProModFeat.h>
#include <ProFeatForm.h>
#include <ProRevolve.h>
#include <ProDtmCrv.h>

    
/*--------------------------------------------------------------------*\
Application includes
\*--------------------------------------------------------------------*/
#include <TestTreeObj.h>
     
#define MAX_ELEM_PATH_DEPTH 10

typedef struct feat_def
{
    int cr_opts_size;
    ProFeatureCreateOptions create_opts[4];
    ProSelection model;
    ProBool first_feat;
    int feat_type;
    ProFeatFormType feat_form;
} ProFeatDef;

typedef struct tree_element_data
{
    ProElement tree;
    ProElement parent_element;
    ProElempath elem_path;
    ProElemId elem_id;
    ProValueData value_data;
    ProElempathItem elem_path_data[MAX_ELEM_PATH_DEPTH];
    int path_depth;
} ProTreeElemdata;

ProError ProTestFeatByTypeCreate(ProTestFeat *feat_def);
int ProTestFeatureExtrudeCreate(ProFeatDef *feat_def);
ProError ProTestFeatureTypeElemAdd(ProTreeElemdata *,ProBool,int);
ProError ProTestFeatureFormElemAdd(ProTreeElemdata *,int);
ProError ProTestFeatElemAdd(ProTreeElemdata *);
ProError ProTestFeatureSidesElemAdd(ProTreeElemdata *,ProSides);
ProError ProTestFeatureDirectionElemAdd(ProTreeElemdata *,
					ProStdDir);
ProError ProTestFeatureDepthElemsAdd(ProTreeElemdata *);
ProError ProTestProtr2DSectionElemAdd(ProTreeElemdata *);
ProError ProUtilFeatErrsPrint(ProErrorlist *errs);
ProError ProUtilMdlitemSelect(char *,ProSelection *);
ProError ProTestFeatSectionFromFeatElemAdd(ProFeature *,
				      ProTreeElemdata *elem,
				      ProSection *);
ProError ProTestFeature3DSecRefElemsAdd(ProTreeElemdata *elem,
				      ProTestSectionInfo *);
ProError ProUtilElementDataGet(ProElement ,ProElempathItem *,
                               int, void *);
ProError ProUtilSectInfoFromElemTreeGet(ProElement,
					ProTestSectionInfo *);
ProError ProUtilSectionFromFeatureGet(ProFeature *feature,
                                      ProSection *);
int ProTestFeatureRevolveCreate(ProFeatDef *feat_def);
ProError ProTestFeatureRevolveAngleElemsAdd(ProTreeElemdata *elem);
ProError ProTestFeatureDtmPln (ProSelection model_sel);
int ProTestSketchedDtmCurveCreate(ProFeatDef *feat_def);
ProError ProTestDtmCurveTypeElemAdd(ProTreeElemdata *elem,
				   ProCurveType type); 
int ProTestFlatDtmSurfaceCreate(ProFeatDef *feat_def);
ProError ProUtilStdSectInfoFromElemTreeGet(ProElement,
				       ProTestSectionInfo *);
ProError ProStdSectVisitAction(ProElement, ProElement,
                               ProElempath,ProAppData);
ProError ProTestFeatureMtrlSideElemAdd(ProTreeElemdata *elem,
				       ProStdMatSide side);
ProError ProTestFeatureStdSectionElemAdd(ProTreeElemdata *elem);
ProError ProTestFeatureAttributesElemAdd(ProTreeElemdata *elem);
ProError ProTestFeatureEndcapsElemAdd(ProTreeElemdata *elem,
				      ProSrfEndCond);
#endif
