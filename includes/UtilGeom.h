
#ifndef UTILGEOM_H
#define UTILGEOM_H

/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application data types
\*--------------------------------------------------------------------*/

typedef int (*ProUtilMeshAct)( ProSurface *surface,
			       double uv[2],
			       int start,
			       ProAppData app_data);
/*
    Purpose:	
	Type for the mesh point action function passed to ProUtilSurfaceMesh()

    Input Arguments:
	surface		- The surface being meshed
	uv		- The U and V values at the mesh point
	start		- 1 - this the start of a new mesh line,
			  0 - this point continues and existing mesh line
	app_data	- General data
    Return Value:
	PRO_TK_NO_ERROR	- Continue the meshing
	other		- Quit meshing this surface, and return this value
				from ProUtilSurfaceMesh().
*/

typedef int (*ProUtilCurveAct)(	ProFeature *feature,
				ProCurve *curve,
				ProAppData app_data);
/*
    Purpose:
	Type for the curve visit action function passed to ProUtilCurveVisit()

    Input Arguments:
	feature		- The feature
	curve		- The curve
	app_data	- General data

    Return Value:
	PRO_TK_NO_ERROR	- Continue visiting curves
	other		- Quit visiting
*/
    typedef struct edge_info
    {
     double uvparam;
     int edgeId;
    } edgeInfo;

/* Purpose : Bind the Id of the edge and its uv param value */
            
  


/*--------------------------------------------------------------------*\
Application external data and functions
\*--------------------------------------------------------------------*/

extern int ProUtilSurfaceMesh( ProSurface *surface,
			       double resolution,
			       int nlines[2],
			       ProUtilMeshAct action,
			       ProAppData app_data);
/*
    Purpose:	
	Calculate UV values for a mesh of points over the given surface, and
	call a given action function at each point.

    Input Arguments:
	surface		- The surface
	resolution	- The distance in model units between successive evaulated
			    points on a mesh line.
	nlines		- The number of U and V lines
	action		- The function to be called at each UV mesh point
	app_data	- General data

    Return Value:
	PRO_TK_NO_ERROR	- mesh completed successfully
	other		- the non zero value returned from action function
				which caused a quit from the mesh
*/

extern int ProUtilPointMindist(ProVector point,
			       ProSelection *item,
			       double *distance,
			       double *closest);
/*
    Purpose:
	Get the minimum distance between a point and and
	edge, curve or surface.

    Input Arguments:
	point		- The point in root coordinates
	item		- The Edge, Curve or Surface

    Output Arguments:
	distance	- The minimum distance
	closest		- The closest point in root coords

    Return Value:
	0		- No error
	1		- Input error

*/

extern int ProUtilGeomitemDump(
    FILE *fp,
    ProSelection *item);

/*
    Purpose:	Dump the geometry of a selected geometry item ..
		EDGE, AXIS, CSYS, CURVE, POINT, QUILT, SURFACE

    Input Arguments:
	fp		- Pointer to file to write to.
	item		- Selected geometry item (including assembly
					comp path)

    Return Value:
	0			- Success
	other			- Invalid geom type
*/

#endif /* ifdef UTILGEOM_H */
