#ifndef TEST_CONFIG
#define TEST_CONFIG



/*--------------------------------------------------------------------*\
    Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
    Application data types
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
    Application global/external data
\*--------------------------------------------------------------------*/


/*---------------------------------------------------------------------*\
    Functions declaration
\*---------------------------------------------------------------------*/
int	ProTestConfig( void*, int );
int	ProTestGetOption( void*, int );
int	ProTestGetOptionByName( void*, int );
int	ProTestConfigOptionSetMenu( void*, int );
int	ProTestSetOptionByName( void*, int );
int	ProTestFileEdit( void*, int );
int	ProTestOptionOrderedVerify( void*, int );
int	ProTestConfigOptionSet( char*, int );
int	ProTestMode( void*, int );


#endif	/* TEST_CONFIG */
