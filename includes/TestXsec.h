
#ifndef	XSEC_H
#define XSEC_H


/*--------------------------------------------------------------------*\
Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProXsec.h>

/*--------------------------------------------------------------------*\
Application data structures
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Function prototypes
\*--------------------------------------------------------------------*/
int         ProTestNotSupported         ();
int         ProTestXsec                 (ProSolid*);
int         ProTestXsecCreate           (ProSolid*);
int         ProTestXsecTypeMenu         ();
int         ProTestXsecDelete           (ProSolid*);
int         ProTestXsecModify           (ProSolid*);
int         ProTestXsecShow             (ProSolid*);
int         ProTestXsecListAllParams    (ProSolid*);
ProError    ProTestXsecSelect           (ProSolid, ProXsec *xsec_ptr);
ProError    ProTestXsecCsysSelect  (ProSolid solid, ProName  csys_name);

#endif
