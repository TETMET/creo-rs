
#ifndef TREETYPESTBL_H
#define TREETYPESTBL_H
 
/*--------------------------------------------------------------------*\
Pro/Toolkit includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProFeatType.h>
#include <ProFeatForm.h>

struct feat_type
{
  char str[50];
  int  type;
} feattypes_tbl[] = 
   {
     {"FIRST_FEAT", PRO_FEAT_FIRST_FEAT},
     {"HOLE", PRO_FEAT_HOLE},
     {"SHAFT", PRO_FEAT_SHAFT},
     {"ROUND", PRO_FEAT_ROUND},
     {"CHAMFER", PRO_FEAT_CHAMFER},
     {"SLOT", PRO_FEAT_SLOT},
     {"CUT", PRO_FEAT_CUT},
     {"PROTRUSION", PRO_FEAT_PROTRUSION},
     {"DATUM", PRO_FEAT_DATUM},
     {"UDF", PRO_FEAT_UDF},
     {"DATUM_AXIS", PRO_FEAT_DATUM_AXIS},
     {"DATUM_POINT", PRO_FEAT_DATUM_POINT},
     {"MOLD", PRO_FEAT_MOLD},
     {"SAW", PRO_FEAT_SAW},
     {"TURN", PRO_FEAT_TURN},
     {"MILL", PRO_FEAT_MILL},
     {"DRILL", PRO_FEAT_DRILL},
     {"DATUM_SURF", PRO_FEAT_DATUM_SURF},
     {"DATUM_QUILT", PRO_FEAT_DATUM_QUILT},
     {"ASSEM_CUT", PRO_FEAT_ASSEM_CUT},
     {"CURVE", PRO_FEAT_CURVE },
     {"SRF_MDL", PRO_FEAT_SRF_MDL},
     {"MFG_GATHER", PRO_FEAT_MFG_GATHER},
     {"MFG_TRIM", PRO_FEAT_MFG_TRIM},
     {"CSYS", PRO_FEAT_CSYS},
     {"XSEC", PRO_FEAT_XSEC},
     {"COMPONENT", PRO_FEAT_COMPONENT},
     {"FIXSETUP", PRO_FEAT_FIXSETUP},
     {"OPERATION", PRO_FEAT_OPERATION},
     {"WORKCELL", PRO_FEAT_WORKCELL},
     {"MAT_REMOVAL", PRO_FEAT_MAT_REMOVAL},
     {"ASMCUT_COPY", PRO_FEAT_ASMCUT_COPY},
     {"THREAD", PRO_FEAT_THREAD},
   };

struct feat_form
{
   char str[50];
   ProFeatFormType form;
} featforms_tbl[] =
   {
     {"NO_FORM", PRO_NOTYPE},
     {"EXTRUDE", PRO_EXTRUDE},
     {"REVOLVE", PRO_REVOLVE},
     {"FLAT", PRO_FLAT}
   };




#endif

