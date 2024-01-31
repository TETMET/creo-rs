#ifndef TEST_GRAPHICS
#define TEST_GRAPHICS



/*--------------------------------------------------------------------*\
    Pro/TOOLKIT includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProGraphic.h>
#include <ProColor.h>


/*--------------------------------------------------------------------*\
    Application data types
\*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*\
    Application global/external data
\*--------------------------------------------------------------------*/


/*---------------------------------------------------------------------*\
    Functions declaration
\*---------------------------------------------------------------------*/
/* Top level function */
int	    ProTestGraphics( void*, int );

int	    ProTestEntityDrawMenu( void*, int );
int	    ProTestColorSet( void*, int );
int	    ProTestAssignColor( ProColor*, ProColortype );

int	    ProTestLinestyleGet( int );
int	    ProTestLinestyleDataPrint( ProLinestyle, ProLinestyleData* );
int	    ProTestLineSetMenu( void*, int );
int	    ProTestLinestyleSet( ProLinestyle*, ProLinestyle );

int	    ProTestGraphicsArcDraw( void*, int );
int	    ProTestGraphicsCircleDraw( void*, int );
int	    ProTestGraphicsTextDisplay( void*, int );
int	    ProTestGraphicsLineDraw( void*, int );
int	    ProTestGraphicsPolylineDraw( void*, int );
int	    ProTestGraphicsPolygonDraw( void*, int );

int	    ProTest2dDisplistMenu( void*, int );
int	    ProTest2dDisplist( void*, int );
int	    ProTest3dDisplistMenu( void*, int );
int	    ProTest3dDisplist( void*, int );
int	    ProTestDisplistInvalidate( ProMdl*, int );

int	    ProTestText( void*, int );
int	    ProTestTextFontIdCurrentSet( void*, int );
int	    ProTestTextHeightCurrentSet( void*, int );
int	    ProTestTextWidthFactorCurrentSet( void*, int );
int	    ProTestTextRotationAngleCurrentSet( void*, int );
int	    ProTestTextSlantAngleCurrentSet( void*, int );
int	    ProTestTextAttributesCurrentGet( void*, int );
int	    ProTestTextAttributesCurrentUnset( void*, int );
int	    ProTestTextFontIdGet( void*, int );


#endif	/* TEST_GRAPHICS */
