#ifndef TEST_ANIMATION
#define TEST_ANIMATION



/*--------------------------------------------------------------------*\
    Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProAnimate.h>


/*--------------------------------------------------------------------*\
    Application data types
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
    Application global/external data
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
    Top level function
\*--------------------------------------------------------------------*/
int	    ProTestAnimation( void* p_dummy, int int_dummy );
int	    ProTestAniObjectCreate( void*, int );
int	    ProTestSelAniObjectDelete( void*, int );
int	    ProTestAnimframeCreate( void*, int );
int	    ProTestAnimframeDelete( void* , int );
int	    ProTestAniObjectAdd( void* , int );
int	    ProTestSingleAnimation( void* , int );
int	    ProTestBatchAnimation( void* , int );
int	    ProTestAniObjectDelete( int i_obj );
ProError    ProTestSingleAnimAct( ProAnimObj, ProAppData );


#endif	/* TEST_ANIMATION */

