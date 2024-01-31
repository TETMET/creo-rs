

#ifndef MFG_MENUS_H
#define MFG_MENUS_H

#include <ProTool.h>
#include <ProToolElem.h>
#include <ProToolinput.h>
#include <ProElemId.h>
#include <ProMfgoper.h>
#include <ProNcseq.h>
#include <ProNcseqElem.h>
#include <ProFeatType.h>

#define MFGOBJ_WKCELL     1 << 0
#define MFGOBJ_FIXTURE    1 << 1
#define MFGOBJ_OPER       1 << 2
#define MFGOBJ_NC_SEQ     1 << 3
#define MFGOBJ_TOOL       1 << 4

#define MFGACT_CREATE     1 << 8
#define MFGACT_REDEFINE   1 << 9
#define MFGACT_DISPLAY    1 << 10
#define MFGACT_ROUTE      1 << 11

#define WKCL_AXIS_2     -2
#define WKCL_AXIS_3     -3
#define WKCL_AXIS_4     -4
#define WKCL_AXIS_5     -5


#define MFGTK_DONE       -100
#define MFGTK_QUIT       -101

typedef struct mfg_obj_list {
        ProWcell    *wcell;
        int         n_wcell;
        ProFixture  *fixture;
        int         n_fixture;
        ProMfgoper  *oper;
        int         n_oper;
        ProNcseq    *nc_seq;
        int         n_nc_seq;
        ProTool     *tool;
        int         n_tool;
} MfgObjList;

typedef struct mfg_menu_choice {
        int         mfgobj_type;
        int         mfgobj_action;
        MfgObjList  *mfgobj_list;
} MfgMenuChoice;


#endif /* MFG_MENUS_H */

