#define PRO_USE_VAR_ARGS true

#include <cstdio>
#include <iostream>
#include <math.h>
#include <ProCore.h>
#include <ProToolkit.h>
#include <ProDimension.h>
#include <ProMessage.h>
#include <ProMenu.h>
#include <ProUtil.h>
#include <ProUICmd.h>
#include <ProMenuBar.h>
#include <ProSelection.h>
#include <ProModelitem.h>
#include <ProCurve.h>
#include <ProDtmCrv.h>
#include <ProFeatType.h>
#include <ProStdSection.h>
#include <ProSolid.h>
#include <ProSecdim.h>
#include <ProDtmPnt.h>
#include <ProAxis.h>

#include "TestError.h"

using namespace std;

static ProFileName msg_file;

// TODO:
// - Create smart pointers with custom de-allocation mechanisms for the Creo stuff
int main(int argc, char **argv)
{
    ProToolkitMain(argc, argv);
    return (0);
}

uiCmdAccessState IsSketchAvailable(uiCmdAccessMode access_mode)
{
    return ACCESS_AVAILABLE;
}

ProError VisitFeatureDimensions(ProDimension *dimension,
                                ProError filter_status, /* ignored */
                                ProAppData data)
{
    ProDimension **p_dims = (ProDimension **)data;
    ProDimension cur_dim;
    ProError status;

    cur_dim.owner = dimension->owner;
    cur_dim.id = dimension->id;
    cur_dim.type = dimension->type;
    status = ProArrayObjectAdd((ProArray *)p_dims, PRO_VALUE_UNUSED, 1, &cur_dim);
    ERROR_CHECK("ProArrayObjectAdd", "VisitFeatureDimensions", status);

    return status;
}

/**
 * Visit dimensions and add them to the array
 */
ProError UtilCollectDimension(
    ProFeature *feature,   /* In:  The handle to the feature */
    ProDimension **pp_dims /* Out: ProArray with collected dimensions.
                  The function allocates memory
                  for this argument, but you must
                  free it. To free the memory,
                  call the function ProArrayFree()*/
)
{
    ProError status;
    status = ProArrayAlloc(0, sizeof(ProDimension), 1, (ProArray *)pp_dims);
    ERROR_CHECK("ProArrayAlloc", "UtilCollectDimension", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProFeatureDimensionVisit(feature,
                                      (ProDimensionVisitAction)VisitFeatureDimensions,
                                      (ProDimensionFilterAction)NULL,
                                      (ProAppData)pp_dims);

    return status;
}

/**
 * Visit axis and add it to the array
 */
ProError VisitAxis(
    ProAxis axis,
    ProError filter_status, /* ignored */
    ProAppData data)
{
    ProError status;
    ProDimension **p_axis = (ProDimension **)data;
    status = ProArrayObjectAdd((ProArray *)p_axis, PRO_VALUE_UNUSED, 1, &axis);
    return status;
}

/**
 * Collect all axis from a solid
 */
ProError UtilCollectAxis(
    ProSolid solid,
    ProAxis **pp_axis /* Out: ProArray with collected axis.
                  The function allocates memory
                  for this argument, but you must
                  free it. To free the memory,
                  call the function ProArrayFree()*/
)
{
    ProError status;
    status = ProArrayAlloc(0, sizeof(ProAxis), 1, (ProArray *)pp_axis);
    ERROR_CHECK("ProArrayAlloc", "UtilCollectDimension", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProSolidAxisVisit(solid, (ProAxisVisitAction)VisitAxis, (ProAxisFilterAction)NULL, (ProAppData)pp_axis);
    return status;
}

/**
 * Visit surface and add it to the array
 */
ProError VisitSurface(
    ProSurface surface,
    ProError filter_status, /* ignored */
    ProAppData data)
{
    ProError status;
    ProSurface **p_surfaces = (ProSurface **)data;
    ProGeomitemdata *geomitemdata;
    int surface_id;
    ProSurfaceIdGet(surface, &surface_id);
    cout << "Surface ID " << surface_id << endl;
    status = ProSurfaceDataGet(surface, &geomitemdata);
    ERROR_CHECK("ProSurfaceDataGet", "VisitSurface", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    if (geomitemdata == NULL)
    {
        return PRO_TK_NO_ERROR;
    }
    double u_min = geomitemdata->data.p_surface_data->uv_min[0];
    double v_min = geomitemdata->data.p_surface_data->uv_min[1];
    double u_max = geomitemdata->data.p_surface_data->uv_max[0];
    double v_max = geomitemdata->data.p_surface_data->uv_max[1];
    cout << "Surface uv_min: " << u_min << ", " << v_min << endl;
    cout << "Surface uv_max: " << u_max << ", " << v_max << endl;
    status = ProArrayObjectAdd((ProArray *)p_surfaces, PRO_VALUE_UNUSED, 1, &surface);

    // WARNING: ALSO A MEMORY LEAK
    ProGeomitemdataFree(&geomitemdata);
    return status;
}

/**
 * Collect all surfaces from a solid
 */
ProError UtilCollectSurfaces(
    ProSolid solid,
    ProSurface **pp_surfaces /* Out: ProArray with collected surfaces.
                  The function allocates memory
                  for this argument, but you must
                  free it. To free the memory,
                  call the function ProArrayFree()*/
)
{
    ProError status;
    status = ProArrayAlloc(0, sizeof(ProSurface), 1, (ProArray *)pp_surfaces);
    ERROR_CHECK("ProArrayAlloc", "UtilCollectSurfaces", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProSolidSurfaceVisit(solid, (ProSurfaceVisitAction)VisitSurface, (ProSurfaceFilterAction)NULL, (ProAppData)pp_surfaces);
    return status;
}

ProError VisitFeature(
    ProFeature feature,
    ProError filter_status, /* ignored */
    ProAppData data)
{
    ProError status;
    ProFeattype ftype;
    ProFeature **p_features = (ProFeature **)data;
    status = ProFeatureTypeGet(&feature, &ftype);
    ERROR_CHECK("ProFeatureTypeGet", "VisitFeature", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    cout << "Feature type " << ftype << endl;
    cout << "Feature ID " << feature.id << endl;
    status = ProArrayObjectAdd((ProArray *)p_features, PRO_VALUE_UNUSED, 1, &feature);
    return status;
}

ProError UtilCollectFeatures(
    ProSolid solid,
    ProFeature **pp_features /* Out: ProArray with collected features.
                  The function allocates memory
                  for this argument, but you must
                  free it. To free the memory,
                  call the function ProArrayFree()*/
)
{
    ProError status;
    status = ProArrayAlloc(0, sizeof(ProFeature), 1, (ProArray *)pp_features);
    ERROR_CHECK("ProArrayAlloc", "UtilCollectFeatures", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProSolidFeatVisit(solid, (ProFeatureVisitAction)VisitFeature, (ProFeatureFilterAction)NULL, (ProAppData)pp_features);
    return status;
}

/**
 * Calculate the length of the axis.
 * NOTE: This is dumb and inaccurate, we should fetch this from Creo somewhere
 */
ProError CalculateAxisLength(ProAxis axis, double &length)
{
    ProError status;
    ProGeomitemdata *geomitemdata;
    Pro3dPnt pt1;
    Pro3dPnt pt2;
    ProAxisDataGet(axis, &geomitemdata);
    status = ProLinedataGet(geomitemdata->data.p_curve_data,
                            pt1, pt2);
    length = (int)(sqrt(pow(pt1[0] - pt2[0], 2) + pow(pt1[1] - pt2[1], 2) + pow(pt1[2] - pt2[2], 2)));
    ProGeomitemdataFree(&geomitemdata);
    return status;
}

/**
 * Create a datum point on the axis at the offset
 */
ProError CreateDatumPoint(ProSolid solid, ProSurface surface, ProAxis axis, double offset, ProElement &point)
{
    ProError status;
    ProName wide_string;

    ProElement pro_e_dpoint_point_name;
    ProElement pro_e_dpoint_pla_constraints;
    ProElement pro_e_dpoint_pla_constraint;
    ProElement pro_e_dpoint_pla_constr_ref;
    ProElement pro_e_dpoint_pla_constr_type;
    ProElement pro_e_dpoint_pla_constr_val;

    ProElement pro_e_dpoint_dim_constraints;
    ProElement pro_e_dpoint_dim_constraint;
    ProElement pro_e_dpoint_dim_constr_ref;
    ProElement pro_e_dpoint_dim_constr_type;
    ProElement pro_e_dpoint_dim_constr_val;

    status = ProElementAlloc(PRO_E_DPOINT_POINT, &point);
    status = ProElementAlloc(PRO_E_DPOINT_POINT_NAME,
                             &pro_e_dpoint_point_name);
    ProStringToWstring(wide_string, "POINT_1");
    status = ProElementWstringSet(pro_e_dpoint_point_name, wide_string);
    status = ProElemtreeElementAdd(point, NULL,
                                   pro_e_dpoint_point_name);
    status = ProElementAlloc(PRO_E_DPOINT_PLA_CONSTRAINTS,
                             &pro_e_dpoint_pla_constraints);
    status = ProElemtreeElementAdd(point, NULL,
                                   pro_e_dpoint_pla_constraints);
    status = ProElementAlloc(PRO_E_DPOINT_PLA_CONSTRAINT,
                             &pro_e_dpoint_pla_constraint);
    status = ProElemtreeElementAdd(pro_e_dpoint_pla_constraints, NULL,
                                   pro_e_dpoint_pla_constraint);

    // Add the point constraints
    // More info at protoolkit/protkdoc/manual0/Datum_Feature_Create.html#1000006

    // Constraint 1 (the axis)
    ProGeomitem axis_geom_item;
    status = ProAxisToGeomitem(solid, axis, &axis_geom_item);
    ERROR_CHECK("ProAxisToGeomitem", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    ProReference axis_reference;
    status = ProReferenceAlloc(&axis_reference);
    ERROR_CHECK("ProReferenceAlloc", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProReferenceSet(axis_reference, NULL, &axis_geom_item);
    ERROR_CHECK("ProReferenceSet", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    // Constraint 1 PRO_E_DPOINT_PLA_CONSTR_REF
    status = ProElementAlloc(PRO_E_DPOINT_PLA_CONSTR_REF,
                             &pro_e_dpoint_pla_constr_ref);
    status = ProElementReferenceSet(pro_e_dpoint_pla_constr_ref, axis_reference);
    status = ProElemtreeElementAdd(pro_e_dpoint_pla_constraint, NULL,
                                   pro_e_dpoint_pla_constr_ref);

    // Constraint 1 PRO_E_DPOINT_PLA_CONSTR_TYPE
    status = ProElementAlloc(PRO_E_DPOINT_PLA_CONSTR_TYPE,
                             &pro_e_dpoint_pla_constr_type);
    status = ProElementIntegerSet(pro_e_dpoint_pla_constr_type, PRO_DTMPNT_CONSTR_TYPE_ON);
    status = ProElemtreeElementAdd(pro_e_dpoint_pla_constraint, NULL,
                                   pro_e_dpoint_pla_constr_type);

    // Constraint 2
    status = ProElementAlloc(PRO_E_DPOINT_DIM_CONSTRAINTS,
                             &pro_e_dpoint_dim_constraints);
    status = ProElemtreeElementAdd(point, NULL,
                                   pro_e_dpoint_dim_constraints);

    // Constraint 2 PRO_E_DPOINT_DIM_CONSTRAINT
    status = ProElementAlloc(PRO_E_DPOINT_DIM_CONSTRAINT,
                             &pro_e_dpoint_dim_constraint);
    status = ProElemtreeElementAdd(pro_e_dpoint_dim_constraints, NULL,
                                   pro_e_dpoint_dim_constraint);

    // Constraint 2 PRO_E_DPOINT_DIM_CONSTR_TYPE
    status = ProElementAlloc(PRO_E_DPOINT_DIM_CONSTR_TYPE,
                             &pro_e_dpoint_dim_constr_type);
    ;
    status = ProElementIntegerSet(pro_e_dpoint_dim_constr_type, PRO_DTMPNT_CONSTR_TYPE_OFFSET);
    status = ProElemtreeElementAdd(pro_e_dpoint_dim_constraint, NULL,
                                   pro_e_dpoint_dim_constr_type);

    // Constraint 2 (the surface)
    int surface_id;
    status = ProSurfaceIdGet(surface, &surface_id);
    ERROR_CHECK("ProSurfaceIdGet", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    ProModelitem surface_item;
    status = ProModelitemInit(solid, surface_id, PRO_SURFACE, &surface_item);
    ERROR_CHECK("ProModelitemInit", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    // Constraint 2 PRO_E_DPOINT_DIM_CONSTR_REF
    ProReference surface_reference;
    status = ProReferenceAlloc(&surface_reference);
    status = ProReferenceSet(surface_reference, NULL, &surface_item);
    status = ProElementAlloc(PRO_E_DPOINT_DIM_CONSTR_REF,
                             &pro_e_dpoint_dim_constr_ref);
    status = ProElementReferenceSet(pro_e_dpoint_dim_constr_ref, surface_reference);
    status = ProElemtreeElementAdd(pro_e_dpoint_dim_constraint, NULL,
                                   pro_e_dpoint_dim_constr_ref);

    // Constraint 2 PRO_E_DPOINT_DIM_CONSTR_VAL
    status = ProElementAlloc(PRO_E_DPOINT_DIM_CONSTR_VAL,
                             &pro_e_dpoint_dim_constr_val);
    status = ProElementDecimalsSet(pro_e_dpoint_dim_constr_val, 4);
    status = ProElementDoubleSet(pro_e_dpoint_dim_constr_val, -offset);
    status = ProElemtreeElementAdd(pro_e_dpoint_dim_constraint, NULL,
                                   pro_e_dpoint_dim_constr_val);
    return status;
}

/**
 * Fill a BCC lattice structure along the axis
 * @param extrusion_solid The solid to fill
 * @param extrusion_surface The surface to fill
 * @param extrusion_axis The axis to fill, must be 5 axis
 */
ProError FillBCC(ProSolid extrusion_solid, ProSurface extrusion_surface, ProAxis extrusion_axis[5])
{
    ProError status;
    ProName wide_string;
    ProErrorlist errors;
    ProFeature feature;

    ProElement pro_e_feature_tree;
    ProElement pro_e_feature_type;
    ProElement pro_e_dpoint;
    ProElement pro_e_dpoint_type;
    ProElement pro_e_std_feature_name;
    ProElement pro_e_dpoint_points_array;

    status = ProElementAlloc(PRO_E_FEATURE_TREE, &pro_e_feature_tree);
    status = ProElementAlloc(PRO_E_FEATURE_TYPE, &pro_e_feature_type);
    status = ProElementIntegerSet(pro_e_feature_type, PRO_FEAT_DATUM_POINT);
    status = ProElemtreeElementAdd(pro_e_feature_tree, NULL, pro_e_feature_type);
    status = ProElementAlloc(PRO_E_DPOINT_TYPE, &pro_e_dpoint_type);
    status = ProElementIntegerSet(pro_e_dpoint_type, PRO_DPOINT_TYPE_GENERAL);
    status = ProElemtreeElementAdd(pro_e_feature_tree, NULL, pro_e_dpoint_type);

    // Create the point with a POINTS_ARRAY
    status = ProElementAlloc(PRO_E_STD_FEATURE_NAME,
                             &pro_e_std_feature_name);
    ProStringToWstring(wide_string, "BCC_LATTICE");
    status = ProElementWstringSet(pro_e_std_feature_name, wide_string);
    status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
                                   pro_e_std_feature_name);
    status = ProElementAlloc(PRO_E_DPOINT_POINTS_ARRAY,
                             &pro_e_dpoint_points_array);
    status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
                                   pro_e_dpoint_points_array);

    // We fill up the structure by moving along the the axis and creating a BCC
    double offsets[5] = {0, 0, 0, 0, 25}; // The middle axis is offsetted
    double axis_lengths[5];
    for (int i = 0; i < 5; i++)
    {
        CalculateAxisLength(extrusion_axis[i], axis_lengths[i]);
    }
    bool filled = false;
    while (!filled)
    {
        for (int i = 0; i < 5; i++)
        {
            status = CreateDatumPoint(extrusion_solid, extrusion_surface, extrusion_axis[i], offsets[i], pro_e_dpoint);
            status = ProElemtreeElementAdd(pro_e_dpoint_points_array, NULL,
                                           pro_e_dpoint);
            // TODO: Add a cell to a vector with a reference to the points
            // Add 50 to the offset or the maximum if the maximum is reached
            offsets[i] = max(offsets[i] + 50, axis_lengths[i]);
        }
        // If all offsets are at the max, abort
        if (offsets[0] >= axis_lengths[0] && 
            offsets[1] >= axis_lengths[1] && 
            offsets[2] >= axis_lengths[2] && 
            offsets[3] >= axis_lengths[3] && 
            offsets[4] >= axis_lengths[4])
        {
            filled = true;
        }
    }

    // Now create the feature
    ProMdl model = ProSolidToMdl(extrusion_solid);
    ProSelection model_sel;
    ProModelitem model_item;
    status = ProMdlCurrentGet(&model);
    ERROR_CHECK("ProMdlCurrentGet", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProMdlToModelitem(model, &model_item);
    ERROR_CHECK("ProMdlToModelitem", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProSelectionAlloc(NULL, &model_item,
                               &model_sel);
    ProFeatureCreateOptions *opts = 0;
    status = ProArrayAlloc(1, sizeof(ProFeatureCreateOptions),
                           1, (ProArray *)&opts);
    opts[0] = PRO_FEAT_CR_INCOMPLETE_FEAT;
    status = ProFeatureWithoptionsCreate(model_sel, pro_e_feature_tree,
                                         opts, PRO_REGEN_NO_FLAGS, &feature, &errors);
    ERROR_CHECK("ProFeatureWithoptionsCreate", "CreateGeneralDatumPoint", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    status = ProArrayFree((ProArray *)&opts);
    status = ProElementFree(&pro_e_feature_tree);
    return status;
}

/**
 * Create a new sweep as a BCC lattice structure along the axis
 */
ProError SweepLatticeBCC(ProSolid solid, ProAxis axis[5])
{
    return PRO_TK_NO_ERROR;
}

/**
 * Select an extrusion and extract all Datum points. The user must be sure to create a sketch with Datum points.
 */
ProError FetchDatumPointsFromSolid()
{
    ProError status;
    ProName name;
    ProSelection *sel = NULL;
    ProFeattype ftype;
    int id, n_sel;
    int *parent_ids = NULL;

    cout << "Selecting object " << endl;

    // Select a solid
    status = ProSelect((char *)"feature", 1, NULL, NULL, NULL, NULL, &sel, &n_sel);
    ERROR_CHECK("ProSelect", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    // Get model
    ProFeature feature;
    status = ProSelectionModelitemGet(sel[0], (ProFeature *)&feature);
    ERROR_CHECK("ProSelectinModelitemGet", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    cout << "Feature ID: " << feature.id << " Feature Type: " << feature.type << endl;

    status = ProFeatureTypeGet(&feature, &ftype);
    ERROR_CHECK("ProFeatureTypeGet", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    cout << "Feature type from ProFeatureTypeGet: " << ftype << endl;
    // Feature type is: PRO_FEAT_PROTRUSION for extrusion

    // Get parent information
    ProSolid solid;
    ProMdl model;
    status = ProFeatureSolidGet(&feature, &solid);
    ERROR_CHECK("ProFeatureSolidGet", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;

    // Try to get the parent from the model
    status = ProFeatureParentsGet(&feature, &parent_ids, &n_sel);
    ERROR_CHECK("ProFeatureParentsGet", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    cout << "Selected feature has " << n_sel << " parents" << endl;

    // Now select the parent and get the parent type, should be a sketch
    ProFeature parent_feature;
    status = ProFeatureInit(solid, parent_ids[0], &parent_feature);
    ERROR_CHECK("ProFeatureInit", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProFeatureTypeGet(&parent_feature, &ftype);
    ERROR_CHECK("ProFeatureTypeGet", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    cout << "Parent feature type from ProFeatureTypeGet: " << ftype << endl;
    // Feature type is: PRO_FEAT_CURVE for sketch

    // Now get all dimensions for this curve
    ProDimension *p_dims = NULL;
    int n_dims;
    status = UtilCollectDimension(&parent_feature, &p_dims);
    ERROR_CHECK("UtilCollectDimension", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProArraySizeGet((ProArray)p_dims, &n_dims);
    ERROR_CHECK("ProArraySizeGet", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    cout << "Parent feature has " << n_dims << " dimensions" << endl;

    // Find the axis created by the extrusion solid
    ProAxis *p_axis = NULL;
    int n_axis;
    status = UtilCollectAxis(solid, &p_axis);
    ERROR_CHECK("UtilCollectAxis", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProArraySizeGet((ProArray)p_axis, &n_axis);
    cout << "Solid has " << n_axis << " axis" << endl;
    cout << "Get name of the axis" << endl;
    ProAxisIdGet(p_axis[1], &id);
    cout << "ID is " << id << endl;

    // Find the surfaces for the extrusion solid
    ProSurface *p_surfaces = NULL;
    int n_surfaces;
    status = UtilCollectSurfaces(solid, &p_surfaces);
    ERROR_CHECK("ProSolidSurfaceVisit", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProArraySizeGet((ProArray)p_surfaces, &n_surfaces);
    cout << "Solid has " << n_surfaces << " surfaces" << endl;

    // Find all features in the extrusion solid
    ProFeature *p_features = NULL;
    int n_features;
    status = UtilCollectFeatures(solid, &p_features);
    ERROR_CHECK("UtilCollectFeatures", "FetchDatumPoints", status);
    if (PRO_TK_NO_ERROR != status)
        return status;
    status = ProArraySizeGet((ProArray)p_features, &n_features);
    cout << "Solid has " << n_features << " features" << endl;

    // For now we assume the most basic and simple thing
    // 1. Cell is a BCC
    // 2. Cell has 5 axis and ordered by left top corner, right top corner, right bottom corner, left bottom corner, middle
    // 3. The middle axis is slightly offsetted to create the BCC
    FillBCC(solid, p_surfaces[0], p_axis);

    // DANGER: This is a memory leak if the function exits early, we need to make it global or use C++ smart pointers
    ProArrayFree((ProArray *)&p_surfaces);
    ProArrayFree((ProArray *)&p_axis);
    ProArrayFree((ProArray *)&p_dims);
    ProArrayFree((ProArray *)&parent_ids);
    return status;
}

extern "C" int user_initialize(
    int argc,
    char *argv[],
    char *version,
    char *build,
    wchar_t errbuf[80])
{
    ProError status;
    uiCmdCmdId cmd_id;

    ProTestErrlogOpen("aslm", version, build);
    freopen("aslm_cout.log", "w", stdout);

    ProStringToWstring(msg_file, "aslm_msg.txt");
    status = ProMenubarMenuAdd("ASLM", "AslmLabel", NULL, PRO_B_TRUE, msg_file);
    status = ProCmdActionAdd("SketchAction", (uiCmdCmdActFn)FetchDatumPointsFromSolid,
                             uiProe2ndImmediate, IsSketchAvailable,
                             PRO_B_TRUE, PRO_B_TRUE, &cmd_id);
    status = ProMenubarmenuPushbuttonAdd(
        "ASLM", "SketchButton",
        "SketchLabel",
        "SketchHelpLabel",
        NULL, PRO_B_TRUE, cmd_id, msg_file);

    return status;
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{
    ProTestErrlogClose();
}
