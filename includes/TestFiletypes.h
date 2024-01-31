
#ifndef TESTFILETYPES_H
#define TESTFILETYPES_H

/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/
#define BEND_TABLE	".sbt"		/* Surface bend table */
#define BOM		".bil"		/* Bill of Materials */
#define CABLE           ".cbl"          /* Cable information */
#define COLOR_PROPS     ".clr"          /* Color file obj models */
#define CONNECTOR       ".cnt"          /* Connector parameters */
#define CONSTRAINTS     ".cst"          /* Assembly constraints */
#define DCM		".dcm"		/* DCM */
#define DETAIL		".udt"		/* Detail items */
#define DIAGRAM		".dia"		/* Diagram contents */
#define DIMENSIONS	".dim"		/* List of dimensions */
#define DRAWING_MODELS	".dmd"		/* List of models in a drawing */
#define DRAWING_VIEWS	".dvw"		/* List of views in a drawing */
#define EDGES		".edg"		/* Edge information */
#define	EDGE_POINTS	".pts"		/* Assembly edge points */
#define EVALUATION	".evl"		/* Geometry evaluation */
#define ENV		".env"		/* Environment */
#define EXTERNAL_OBJ    ".ext"		/* External objects */
#define FEATURES	".fin"		/* Feature information */
#define FEMINFO		".fmi"		/* Fem Constraint information */
#define GENERIC		".gen"		/* Generic Data dump file */
#define GTOL		".gtl"		/* Geometric Tolerances */
#define MASSPROP	".mpr"		/* Mass Properties */
#define MATRIX		".mtx"		/* Assembly matrices */
#define	MESH		".msh"		/* Mesh points */
#define	MODEL_UNITS	".mun"		/* Model Units */
#define	NAMES		".nam"		/* Model item names */
#define NOTIFY_DBMS	".dbs"		/* Notify dbms */
#define NOTIFY_GFX	".gfx"		/* Notify graphics */
#define	PARAMETERS	".gpm"		/* General parameters */
#define POSTSCRIPT	".pst"		/* PostScript */
#define RELATIONS	".rel"		/* Output of relations */
#define SELECTIONS	".sel"		/* Output of selections */
#define SURFACES	".srf"		/* Surface data */
#define TABLE		".tbl"		/* Table contents */
#define TOLERANCES	".tol"		/* Dimension tolerance settins */
#define TOOLIDS         ".tid"          /* Tools parameters information */
#define TRAVERSAL	".gtr"		/* Geometry traversal */
#define	UDF		".udf"		/* User Defined Fearures */
#define VIEW_MATRIX	".vwm"		/* View Matrix */
#define	XSEC		".xsc"		/* X-Sections */
#define XYZ		".xyz"		/* XYZ coordinates */
#define STATUS          ".stt"          /* status in a layer funct */
#define NEWGTOL         ".ngt"          /* output new gtol funct */

extern char *user_gen_filename();

#endif /* ifdef TESTFILETYPES_H */
