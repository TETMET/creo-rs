

#ifndef TEST_SELECT_H
#define TEST_SELECT_H
/*====================================================================*\
    FUNCTION :  ProTestSelection()
    PURPOSE  :  Create a menu for testing selection functions
\*====================================================================*/
int ProTestSelection( char *a, int b );

/*====================================================================*\
    FUNCTION :  HighlightItem()
    PURPOSE  :  Highlight different types of items and show info
\*====================================================================*/
int HighlightItem(char *a, int b);

/*====================================================================*\
    FUNCTION :  UnHighlightItem()
    PURPOSE  :  Unhighlight different types of items and show info
\*====================================================================*/
int UnHighlightItem(char *a, int b);

/*====================================================================*\
    FUNCTION :  DisplayItem()
    PURPOSE  :  Display different types of items and show info
\*====================================================================*/
int DisplayItem(char *a, int b);

/*====================================================================*\
    FUNCTION :  UnDisplayItem()
    PURPOSE  :  UnDisplay different types of items and show info
\*====================================================================*/
int UnDisplayItem(char *a, int b);

/*====================================================================*\
    FUNCTION :  SelParams()
    PURPOSE  :  Select a parameter and show info
\*====================================================================*/
int SelParams(char *a, int b);


/*====================================================================*\
    FUNCTION :  ProTestOptionalInfo()
    PURPOSE  :  Test some ProSelection funcs.
\*====================================================================*/
int ProTestOptionalInfo( void*, int );

/*====================================================================*\
    FUNCTION :  ProUtilCopySelectionSurf()
    PURPOSE  :  Create a copy of a surface selection.
\*====================================================================*/
ProError ProUtilCopySelectionSurf(ProMdl model, ProSurface surf, ProSelection *p_copy_sel);

/*====================================================================*\
    FUNCTION :  UtilSelectionFromEdgeId()
    PURPOSE  :  Create a selection entity from a edge id.
\*====================================================================*/
ProError UtilSelectionFromEdgeId(
                                    ProMdl model,	/* In : model */
                                    int edge_id,	/* In : edge id */
                                    ProSelection *sel);/* Out: selection*/



/*====================================================================*\
    FUNCTION :  UtilSelectionFromEdge()
    PURPOSE  :  Create a selection entity from a edge.
\*====================================================================*/
ProError UtilSelectionFromEdge(
                                     ProSolid solid,	/* In : model */
                                     ProEdge edge,	/* In : surface id */
                                     ProSelection *sel);
                                     

/*====================================================================*\
    FUNCTION :  UtilSelectionFromSurfaceId()
    PURPOSE  :  Create a selection entity from a surface id.
\*====================================================================*/
ProError UtilSelectionFromSurfaceId( ProMdl model,	/* In : model */
                                        int surf_id,	/* In : surface id */
                                        ProSelection *sel);

/*====================================================================*\
    FUNCTION :  UtilSelectionFromSurface()
    PURPOSE  :  Create a selection entity from a surface.
\*====================================================================*/
ProError UtilSelectionFromSurface(
                                     ProSolid solid,	/* In : model */
                                     ProSurface surf,	/* In : surface id */
                                     ProSelection *sel);

                                     
#endif
