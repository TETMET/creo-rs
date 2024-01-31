
#ifndef TESTGEOM_H
#define TESTGEOM_H

#include <ProObjects.h>

typedef struct testmeshdata
{
    double resolution;
    int nlines[2];
} ProTestMeshData;

typedef struct testtessdata
{ 
    int n_tess;
} ProTestTessData;

typedef union testdata
{
    ProTestMeshData meshdata;
    ProTestTessData tessdata;
} ProTestData;

typedef struct testgeomdata
{
    unsigned int action; /* TRAVERSE, ... */

    ProMdl	*model;
    ProSurface	*surface;
    ProContour	*contour;
    ProEdge	*edge;
    FILE	*fp;
    ProTestData	data;
} ProTestGeomData;

#endif
