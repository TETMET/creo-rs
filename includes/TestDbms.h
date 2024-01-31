

#include "ProObjects.h"
#include "ProSurfacedata.h"
#include "ProQuiltdata.h"
#include "ProEdgedata.h"
#include "ProDatumdata.h"

/*====================================================================*\
  Function : ProTestDbms
  Purpose  : Create a menu for testing dbms functions
\*====================================================================*/
/*int ProTestDbms(char *a, int b);*/
  int ProTestDbms();

/*====================================================================*\
     FUNCTION :  ProUtilSurfacedataCopy()
     PURPOSE  :  copy surfacedata using different functions
\*====================================================================*/
 ProError ProUtilSurfacedataCopy(
     ProSurfacedata **p_s_data);   /* In, Out : surface data */


/*====================================================================*\
     FUNCTION :  ProUtilQuiltdataCopy()
     PURPOSE  :  Copy quilt data using different commands
\*====================================================================*/
 ProError ProUtilQuiltdataCopy(
     ProQuiltdata **p_q_data);    /* In, out - quilt data */

/*====================================================================*\
     FUNCTION :  ProUtilEdgedataCopy()
     PURPOSE  :  copy edgedata using different functions
\*====================================================================*/
 ProError ProUtilEdgedataCopy(
     ProEdgedata **pp_edge_data); /* In, out : edge data */


/*====================================================================*\
     FUNCTION :  ProUtilDatumdataCopy()
     PURPOSE  :  Copy Datum data using different commands
\*====================================================================*/
 ProError ProUtilDatumdataCopy(
     ProDatumdata **p_d_data);    /* In, out - Datum data */

int ProTestImpfeatAttrMenuAction( ProAppData data, int action );

