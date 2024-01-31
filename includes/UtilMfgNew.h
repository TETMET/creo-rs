

#ifndef UTIL_MFG_NEW_H
#define UTIL_MFG_NEW_H

/*--------------------------------------------------------------------*\
Pro/Toolkit includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <MfgMenu.h>

ProError ProUtilElementBuild (ProValueDataType, void*, ProElement*);
ProError ProUtilNcseqElemSurfacesAdd ( ProElement *surfaces );
ProError ProUtilNcseqElemHolesAdd ( ProElement *holes );
ProError ProTestNcseqToolPathDisplay();
typedef ProError (*MfgActionFunc)(
     ProAppData type, /* Wcell, ncseq, oper, fixt or tool handle */ 
     ProAppData app_data, /* Application data */
     int        num );    

ProError ProUtilMfgobjAction (
    MfgMenuChoice *obj_action,
    int            obj_type,
    int            max_sels,
    MfgActionFunc  func,
    ProAppData     app_data,
    int            int_data ) ;
			


#endif
