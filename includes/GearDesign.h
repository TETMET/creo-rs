

#ifndef GEARDESIGN_H
#define GEARDESIGN_H

#include <ProToolkit.h>
#include <ProPart.h>
#include <ProValue.h>
#include <ProElement.h>
#include <ProSection.h>

#include <UtilTree.h>

PRO_BEGIN_C_DECLS

#define SPUR_GEAR   0
#define RING_GEAR   1
#define HELICAL_GEAR 2

#define SECTION_MAX_PARAMS 8

#define MSGFIL L"protkgdmsg.txt"

/*--------------------------------------------------------------------*\
    Data types
\*--------------------------------------------------------------------*/
typedef enum 
{
  PTK_GD_DATUM,
  PTK_GD_DISK,
  PTK_GD_HOLE,
  PTK_GD_HUB,
  PTK_GD_BACKPLATE,
  PTK_GD_TEETH
}  ProTKGDStep;

#define NUM_GD_STEPS 6

typedef struct 
{
  ProTKGDStep step;
  ProBoolean required;
} ProTKGDStepInfo;

/*--------------------------------------------------------------------*\
    Order of creation of spur gears
\*--------------------------------------------------------------------*/
static ProTKGDStepInfo spur_gear_designinfo [] =
{
  {PTK_GD_DATUM, PRO_B_TRUE},
  {PTK_GD_DISK, PRO_B_TRUE},
  {PTK_GD_HUB, PRO_B_FALSE},
  {PTK_GD_BACKPLATE, PRO_B_FALSE},
  {PTK_GD_HOLE, PRO_B_FALSE},
  {PTK_GD_TEETH, PRO_B_FALSE}
};

static ProTKGDStepInfo helical_gear_designinfo [] =
{
  {PTK_GD_DATUM, PRO_B_TRUE},
  {PTK_GD_DISK, PRO_B_TRUE},
  {PTK_GD_HUB, PRO_B_FALSE},
  {PTK_GD_BACKPLATE, PRO_B_FALSE},
  {PTK_GD_HOLE, PRO_B_FALSE},
  {PTK_GD_TEETH, PRO_B_FALSE}
};


/*--------------------------------------------------------------------*\
    Order of creation of ring gears
\*--------------------------------------------------------------------*/
static ProTKGDStepInfo ring_gear_designinfo [] =
{
  {PTK_GD_DATUM, PRO_B_TRUE},
  {PTK_GD_DISK, PRO_B_TRUE},
  {PTK_GD_BACKPLATE, PRO_B_TRUE},
  {PTK_GD_HUB, PRO_B_FALSE},
  {PTK_GD_HOLE, PRO_B_FALSE},
  {PTK_GD_TEETH, PRO_B_FALSE}
};

/*--------------------------------------------------------------------*\
    Master gear parameter structure
\*--------------------------------------------------------------------*/
typedef struct gear_param_struct
{
    ProPart model;

/* Parameters */
    char   name[PRO_NAME_SIZE];
    int    units;
    char   material[PRO_NAME_SIZE];
    int	   gear_type;	    /* 0 - spur,1 - ring 2 - Helical*/
  ProTKGDStep current_step;

  double datum_x_offset;
  double datum_y_offset;
  double datum_z_offset;

    double gear_diam;
    double gear_depth;
    double backplate_diam;
    double backplate_depth;
    double hub_out_diam;
    double hub_depth;
    int    n_of_teeth;
    double tooth_par1;  /* normal module */
    double inner_diam;
    double keyway_in_width;
    double keyway_in_height;
    double keyway_out_width;
    double keyway_out_height;
    double backnotch_rad;
   double Pressure_angle;
   double helix_angle;
  
  int datum_x_offset_dim_id;
  int datum_y_offset_dim_id;
  int datum_z_offset_dim_id;

    int front_surface_id;
    int side_surface_id;
    int bottom_surface_id;
    int disk_front_surf_id;
    int disk_side_surf_id;
    int hole_side_surf_id;
    int hub_front_surf_id;
    int hub_side_surf_id;
    int backplate_back_surf_id;
    int backplate_side_surf_id;
    int tooth_surf_side_surf_id;

    ProBoolean datum_created, disk_created, hole_created, tooth_created, teeth_created;
    ProBoolean hub_created, backplate_created, keyway_created, backnotch_created;
    ProBoolean key_created, tooth_surf_created;  
} GearParam;

typedef union
{
    int    i;
    double d;
    void  *p;
    ProSelection r;
} Parameter;


typedef struct feature_def
{
    ElemTreeData *tree;
    int sizeof_tree;
    Parameter params[SECTION_MAX_PARAMS];
    ProError (*section_create_func)(ProSection section, Parameter *params);
    char *feat_name;
} FeatureDef;

typedef struct feat_by_name
{
    char *name;
    int   id;
} FeatByName;

/*---------------------------------------------------------------------*\
    Utility declarations (GearDesignUtils.c)
\*---------------------------------------------------------------------*/
ProError ProUtilCreateSketchedFeature(GearParam *, FeatureDef *, ProFeature *);

ProError ProUtilModelitemNameSet(ProModelitem *modelitem, char *name);
ProError ProUtilFeatsByName(ProSolid model, FeatByName *p_feat, int);
ProError ProUtilSelectionFromSurfaceId(ProMdl, int s_id, ProSelection *);
ProError ProUtilSelectionFromAxisId(ProMdl, int axis_id, ProSelection *);
ProError ProUtilAxisGeomitem(ProFeature *feature_axis, ProType,  int *);
ProError ProUtilAxisGeomitemVisit(ProGeomitem *geomitem, ProError, ProAppData);

ProError ProUtilCreateCircleSection(ProSection section, Parameter *params);
ProError ProUtilCreateToothSection(ProSection section, Parameter *params);
ProError ProUtilCreateKeySection(ProSection section, Parameter *params);
ProError ProUtilCreateNotchSection(ProSection section, Parameter *params);
ProError ProUtilFeatureSetSectionDependentValues (ProElement elemtree,
						  ElemTreeData* elemarr,
						  int n_elem);
ProError ProUtilTwoCircleIntersection(Pro2dCircledef *, Pro2dCircledef *, 
    Pro2dPnt p1, Pro2dPnt p2);
ProError ProUtilSecEntityUseCurveLoop (ProSection section, Parameter *params);

ProError ProUtilGeometryAtPointFind(ProPart part, Pro3dPnt point, ProModelitem *);
ProError ProUtilFeatFirstGeomitem(ProFeature *datum_plane, ProType,  int *);
ProError ProUtilFeatFirstDimension (ProFeature *, int*);
void ProUtilFeatErrsWrite(char *, ProError,ProElement,ProErrorlist *);

ProError ProUtilGeomItemVisitCurve (ProGeomitem *geom_item, ProError err_status, ProAppData data);

/*---------------------------------------------------------------------*\
    Function declarations (GearDesign.c)
\*---------------------------------------------------------------------*/
int ProUserNewgearUI();

int ProUserDeleteDesign(GearParam *g_ptr);
int ProUserModifyDesign(GearParam *g_ptr);
int ProUserRegenerate(GearParam *g_ptr);
int ProUserMaterial(GearParam *g_ptr);
int ProUserCreateViewAuto(GearParam *g_ptr);
int ProUserCreateViewManual(GearParam *g_ptr);

int ProUserCreateDatum(GearParam *g_ptr);
int ProUserCreateDisk(GearParam *g_ptr);
int ProUserCreateHole(GearParam *g_ptr);
int ProUserCreateTooth(GearParam *g_ptr);
int ProUserCreateTeeth(GearParam *g_ptr);
int ProUserCreateToothSurf(GearParam *g_ptr);
int ProUserCreateHub(GearParam *g_ptr);
int ProUserCreateBackPlate(GearParam *g_ptr);
int ProUserCreateKeyway(GearParam *g_ptr);
int ProUserCreateKey(GearParam *g_ptr);
int ProUserCreateBackNotch(GearParam *g_ptr);

int ProUserReadParamFile(char *, GearParam *g_ptr);
int ProUserAssignParamDef (GearParam *g_ptr);
ProError ProUserCheckFeatures(GearParam *g_ptr);

PRO_END_C_DECLS

#endif
