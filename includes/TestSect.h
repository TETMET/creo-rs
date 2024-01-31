
#ifndef	TESTSECT_H
#define TESTSECT_H


/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProObjects.h>
#include <ProGeomitem.h>
#include <ProSection.h>
#include <ProStdSection.h>


/*--------------------------------------------------------------------*\
Application data structures
\*--------------------------------------------------------------------*/
typedef enum
{
    SEC_BOX = 0,
    SEC_CIRCLE,
    SEC_SLOT,
    SEC_SPLINE,
    SEC_WIDGET,
    SEC_ANGLE,
    SEC_DIAM,
    SEC_EXISTING,
    SEC_USE_EDGE
} Sect_types; 

typedef struct section_info
{
    ProSection3d section;  /* initialized section handle */
    ProSelection sketch_pln;
    ProUvParam sketch_uv;
    ProSecViewDirType view_dir;
    ProSelection orient_pln;
    ProSecOrientDirType orient_dir;
} ProTestSectionInfo;

typedef enum feat_geom_type
{
   GEOM_UNUSED = -5,
   GEOM_POINT         = 0,
   GEOM_EDGE          = 1,
   GEOM_SURFACE       = 2,
   GEOM_VERTEX        = 3,
   GEOM_CURVE         = 4,
   GEOM_AXIS          = 5,   
   GEOM_CSYS          = 6    
}FeatGeomType;

typedef struct section_mtrx_data
{ 
    ProVector x_axis[2];
    ProVector y_axis[2];
    ProMatrix sec_trf; 
    ProMatrix sk_mtrx; 
    ProSelection sk_plane;
    double angle;
}SectionMatrix_data;

typedef struct feat_ref
{
   FeatGeomType   type;
   int          id;  
} FeatRef;

typedef struct sec_constraint_info
{
	ProConstraintType type;
	ProConstraintStatus status;
	int num_refs;
	int* p_ent_ids;
	ProSectionPointType *p_senses;
}SectionConstraintInfo;

typedef struct ske_info 
{
   ProSolid    solid;
   ProSection  section;   
   ProSurface  prim_surface; /* seed surface */   
   FeatRef ref1; /* The first reference to place sketch */
   FeatRef ref2; /* The second reference to place sketch */
   int          orient_dir;
   int          orient_view_dir;
   double       eps_val;
   ProEdge      *edges;   
   ProSurface       orient_surface; /* surface to orient the sketch plane */
   Pro2dLinedef     *line_entity; /* Array of 2-d line(s) of sketch */
   int              *line_ids;
   Pro2dClinedef    *cline_entity; /* Array of 2-d center-line(s) of sketch */
   int              *cline_ids;
   Pro2dArcdef      *arc_entity;/* Array of arc of sketch */
   int              *arc_ids;   
   Pro2dCircledef   *circle_entity;
   int              *circle_ids;   
   Pro2dConicdef    *conic_entity;/* Array of conic of sketch */
   int              *conic_ids;   
   Pro2dPolylinedef *poly_line_ent; /* Array poly line of sketch */
   int              *poly_line_ids;   
   Pro2dSplinedef   *spline_ent;/* Array of spline of sketch */
   int              *spline_ids;   
   Pro2dEllipsedef  *ellipse_ent; /* Array of ellipse of sketch */
   int              *ellipse_ids;   
   ProFeature  sket_feature; /* The sketch feature */
   SectionConstraintInfo  *constraints_info;  
}SketchInfo;


/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/
#define FLOAT_CMP(a,b)  (fabs(a-b)<EPSM6)
#define POINT_CMP(a,b)  (FLOAT_CMP((a)[0],(b)[0]) && FLOAT_CMP((a)[1],(b)[1]))
/*--------------------------------------------------------------------*\
Function prototypes
\*--------------------------------------------------------------------*/
int ProTestSectionMenuAction();
int ProTestSectionCreate();
int ProTestSectionRetrieve();
int ProTestSectionInfoGet();
ProError ProTestSectionBuild(ProSection, ProType, ProTestSectionInfo *,
			     void *, Sect_types *);
ProError ProTestBoxSectBuild(ProSection, ProType, ProTestSectionInfo *);
ProError ProTestDiamSectBuild(ProSection, ProType, ProTestSectionInfo *);
ProError ProTestCircleSectBuild(ProSection, ProType, ProTestSectionInfo *);
ProError ProTestSlotSectBuild(ProSection, ProType, ProTestSectionInfo *);
ProError ProTestSplineSectBuild(ProSection, ProType, ProTestSectionInfo *);
ProError ProTestWidgetSectBuild(ProSection, ProType, ProTestSectionInfo *);
ProError ProTestAngleSectBuild(ProSection, ProType, ProTestSectionInfo *);
ProError ProTestSecerrAlloc(ProWSecerror *error_tbl);
ProError ProTestSecerrFree(ProWSecerror *error_tbl);
ProError ProTestSecerrCount(ProWSecerror *error_tbl, int *r_count);
ProError ProTestSecerrAdd(ProWSecerror *error_tbl, ProSecerrCode user_error,
			wchar_t *msg, ProSecerrSever severity, int ent_id,
			ProSecerrAttr attribute);
ProError ProTestSecerrMsgGet(ProWSecerror error_tbl, int ith, ProMsg PMsg);
ProError ProTestSecerrItemGet(ProWSecerror error_tbl, int ith, int *r_item);
ProError ProTestSecerrShow(ProWSecerror error_tbl);

/*--------------------------------------------------------------------*\
Section Utility functions
\*--------------------------------------------------------------------*/
ProError ProUtilSectionRetr(ProSection *section);
ProError ProUtilSectionSave(ProSection section);
ProError ProUtilSectionErase(ProSection section);
ProError ProUtilSectionDelete(ProSection section);
void ProTestSectionNameGet(Sect_types, ProName);
static int ProUtilMenuAction(char *, int);
int ProUtilLinesParallel(double line1[2][3],double line2[2][3]);
int ProUtilLinesPerpend(double line1[2][3],double line2[2][3]);
ProError ProUtilProjPntToPlane(double [3],ProSurface *,double [3]);
ProError ProUtilTransPntFromPlane(double [3],ProSurface *,double [3]);
double *ProUtilLineLineX(double l1[2][3],double l2[2][3],double int_pnt[3]);
ProError ProUtilTransEdgeToSketch(double [2][3], double [2][3], ProSelection *,
    ProVector, ProVector, double [3], double [2][3], double [2][3]);
ProError ProUtilSectCsysTransformGet(ProTestSectionInfo *,double[4][4],
				     double[2][3], double[2][3],
				     double [4][4], double *);
ProError ProUtilSectTransformGet(ProSelection *p_pln_sel,
					ProVector, ProVector, double [3],
					double [2][3], double [2][3],
					double [4][4], double *);
void ProUtil2DPointTrans(double [4][4],double[2],double[2]);
ProError ProUtilTwoOrthogEdgesOnPlaneGet(ProSection, int[2],
					 double [2][3],double[2][3]);
ProError ProUtilXyOffsetsGet(double [2]);
ProError ProUtilSectEpsModify(ProSection);
ProError ProUtilSketchCsysGet(ProTestSectionInfo *, double[4][4]);
ProError ProUtilParallelLineEntityGet(ProSection section,int ent_id,
				      int ent_ids[],int n_ids,
				      int *parallel_index);
ProError ProUtilSectionInfoGet(FILE *fp,ProSection section);
ProError ProUtilSectionRetrieve(ProSection *section);
ProError ProUtilSectionFromFeatGet(ProSection *section);
ProError ProUtilSectionUseExisting(ProSection,ProTestSectionInfo *);
ProError ProUtilSectionInfoCopy(ProSection,ProSection,ProVector);
ProError ProUtilSectionClean(ProSection section);
ProError ProUtilStdSectInfoFromElemTreeGet();

ProError ProUtilGetGeomItemToRef(ProGeomitem *geom_item, FeatRef *ref);
ProError ProUtilInitializeSketch(SketchInfo *sketch_info);
ProError ProUtilFreeSketchInfo(SketchInfo *sketch_info);
ProError ProUtilCopySectionEntities(ProSection *section, SketchInfo *sketch_info);
ProError ProUtilAddSectionEntities(ProSection section, SketchInfo *sketch_info);
ProError ProUtilCopy2dEntity(Pro2dEntdef *entity, Pro2dEntdef *copy_entity );
ProError ProUtilCopySectionConstraints(SketchInfo *sketch_info, 
                                ProFeature *sk_feat, ProSection *section);
ProError ProUtilFindSketchEntity(SketchInfo *sketch_info, ProSection *section,
                                int id, int *sec_id);

ProError ProUtilSectionEntityPropCheck(Pro2dEntdef *entity_1,
                                 Pro2dEntdef *entity_2,
                                 ProBoolean *isSame);
ProError ProUtilSketchSectionBuild (ProSection section, SketchInfo *sketch_info, 
                             ProSelection *sketch_refs);                                 
                           

#endif
