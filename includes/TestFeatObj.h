
#ifndef TESTFEATOBJS_H
#define TESTFEATOBJS_H

/*--------------------------------------------------------------------*\
Pro/Toolkit includes
\*--------------------------------------------------------------------*/
#include <ProSldfeatElem.h>
#include <ProSldfeatinput.h>
#include <ProSection.h>


/*--------------------------------------------------------------------*\
Object ProTestFeature: responsible for the protrusion creation 
			  and redefinition user interface
\*--------------------------------------------------------------------*/
typedef struct pro_test_feature
	{
	   ProTestUserOpt user_option;
	   int feature_type;                /* defined in ProFeatType.h */
	   ProBool first_feat;
	   ProSldfeatSideType num_sides;    /* two sided, or one sided */
	   ProSldfeatDepthOpt depth_opt[2]; /* depth option for each side */
	   ProSldfeatAngOpt   angle_opt[2]; /* angle option for each side */
	   ProSldfeatFormType form_type;
	   ProSldfeatThkForm thk_form;
	   ProTestMaterSideData *p_cut_ms_data; /* to be allocated for cut */
	   ProSelection sket_plane;
	   ProWSechandle sec_handle;
	   ProSldfeatElem attrib_elem;
	   ProSldfeatElem section_elem;
	   ProSldfeatElem depth_elem;
	   ProSldfeatElem direction_elem;
	   ProSldfeatElem angle_elem;
	   ProSldfeatElem materside_elem;
	   ProSldfeatinput feat_input;
	   ProFeature feature;
	} ProTestFeature; 

ProError ProTestFeature_Alloc(ProTestFeature **);
ProError ProTestFeature_FeatTypeSet(ProTestFeature *);
ProError ProTestFeature_OptSet(ProTestFeature *, ProTestUserOpt);
ProError ProTestFeature_FormTypeSet(ProTestFeature *);
ProError ProTestFeature_ThkFormSet(ProTestFeature *);
ProError ProTestFeature_InputAlloc(ProTestFeature *);
ProError ProTestFeature_SectionSet(ProTestFeature *);
ProError ProTestFeature_AttribSet(ProTestFeature *);
ProError ProTestFeature_DepthOptSet(ProTestFeature *, ProTestSide); /*
							   will also set the
							   depth_opt member 
							   */
ProError ProTestFeature_AngleOptSet(ProTestFeature *, ProTestSide); /*
                                                           will also set the
                                                           angle_opt member
                                                           */
ProError ProTestFeature_DepthValSet(ProTestFeature *, ProTestSide); 
ProError ProTestFeature_AngleValSet(ProTestFeature *, ProTestSide);
ProError ProTestFeature_DepthRefSet(ProTestFeature *, ProTestSide);
ProError ProTestFeature_AngleRefSet(ProTestFeature *, ProTestSide);
ProError ProTestFeature_DirectionSet(ProTestFeature *);
ProError ProTestFeature_MaterSideSet(ProTestFeature *);
/*
int ProTestFeature_Run(void *, void *, void *, void *, void *, void *);
					  1 arg : ProFeature*, casted to void*,
                                          2 arg: ProTestUserOpt, casted to int
					  3 arg: bits, casted to char
					  4 arg, 5 arg, 6 arg: dummies */
int ProTestFeature_Run(void *, int);/* 
					  1 arg: ProFeature*, casted to void*,
					  2 arg: ProTestUserOpt, casted to int 						*/
ProError ProTestFeature_Action(ProTestFeature *);  /* create or redef */
ProError ProTestFeature_Free(ProTestFeature *);

#endif
