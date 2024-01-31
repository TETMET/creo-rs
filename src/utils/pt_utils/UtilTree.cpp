

#ifndef lint
static char UtilTree_c [] = "@(#)UtilTree.c     1.2 11/26/96";
#endif

/*--------------------------------------------------------------------*\
Pro/Toolkit includes
\*--------------------------------------------------------------------*/
#include "ProToolkit.h"
#include "ProMdl.h"
#include "ProDisplist.h"
#include "ProFeatType.h"
#include "ProElement.h"
#include "ProElemId.h"
#include "ProElempath.h"
#include "ProDtmPln.h"
#include "ProUtil.h"

#include <string.h>
#include <stdio.h>
#include <ProTKRunTime.h>
#include <PTApplsUnicodeUtils.h>

/*--------------------------------------------------------------------*\
Pro/Develop includes
\*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*\
Application macros
\*--------------------------------------------------------------------*/
#include "UtilTree.h"
#include "TestError.h"
#include "UtilString.h"
#include "UtilCollect.h"
/*--------------------------------------------------------------------*\
Application data types
\*--------------------------------------------------------------------*/
extern ProError ProUtilIdToStr();

/*--------------------------------------------------------------------*\
Application global/external data
\*--------------------------------------------------------------------*/
typedef enum
{
   PRO_ELEMTREE_PRINT_ALL = 1,
   PRO_ELEMTREE_PRINT_BRANCHES = 2
} ProUtilElemtreePrintOpts;


typedef struct
{
   ProUtilElemtreePrintOpts *options;
   FILE                     *f;
   int                       branches[2*PRO_ELEMTREE_MAX_LEVEL];
   UtilTreeprintWindow window;
} UtilElemtreePrintdata;

static UtilGrWindowData win_data;

static ProError ProUtilElemValuePrint ( ProElement, ProBoolean, ProBoolean, UtilElemtreePrintdata, char*);
static ProError ProUtilValuePrint ( ProElement, UtilElemtreePrintdata, char*);
static ProError ProUtilValuePrintDeprecated ( ProValue, UtilElemtreePrintdata, char*);
static ProError ProUtilElemtreePPrint
    (ProElement, ProElempath,ProUtilElemtreePrintOpts*,UtilTreeprintWindow*);
static ProError ProUtilElemtreeElemPrint ( ProElement, ProElement, ProElempath, UtilElemtreePrintdata*);
static ProError ProUtilElemPrint ( ProElement, ProBoolean, ProBoolean, ProBoolean, ProBoolean, UtilElemtreePrintdata, char*);
static ProError ProUtilElemIdPrint ( ProElement, ProBoolean, ProBoolean, UtilElemtreePrintdata, char*);
static ProError ProUtilDisp(char*, ProBool);
static ProError ProUtilElemtreeInGrWindow( ProElement, ProElempath, ProUtilElemtreePrintOpts*s, int*);

/*====================================================================*\
FUNCTION : ProUtilElementtreePrint
PURPOSE  : Print an entire element tree
\*====================================================================*/
ProError ProUtilElementtreePrint (
   ProElement elem_tree,
   UtilTreeprintWindow window,
   int *grwin_id )
{
   ProError    status = PRO_TK_NO_ERROR;
   ProElempath path = (ProElempath)NULL;
   char *p_env_win;

    /* get empty path */
    status = ProElempathAlloc ( &path );
    TEST_CALL_REPORT("ProElempathAlloc()", "ProUtilElementtreePrint()",
                                status, (status != PRO_TK_NO_ERROR));

    if ( status == PRO_TK_NO_ERROR )
    {
        switch (window)
        {
            case PRO_TEST_INFO_WINDOW:
                status = ProUtilElemtreePPrint (elem_tree,path,NULL,&window);
                break;

            case PRO_TEST_GRAPHICS_WINDOW:
                p_env_win = getenv("GRAPH_WINDOW");

                if ((p_env_win == NULL) || (ProUtilStrcmp(p_env_win, 
                					(char *)"yes")))
                  {
                    *grwin_id = -1;
                    break;
                  }
                status = ProUtilElemtreeInGrWindow( elem_tree, path, NULL,
                                                                grwin_id);
                break;
        }
    }

    if ( path != (ProElempath)NULL )
      {
        status = ProElempathFree ( &path );
        TEST_CALL_REPORT("ProElempathFree()", "ProUtilElementtreePrint",
                                      status, status != PRO_TK_NO_ERROR);
      }

    return ( status );
}

/*====================================================================*\
FUNCTION : ProUtilElemtreePPrint
PURPOSE  : Print an element tree
\*====================================================================*/
static ProError ProUtilElemtreePPrint (
   ProElement                  elem_tree,
   ProElempath                 elem_path,
   ProUtilElemtreePrintOpts   *options,
   UtilTreeprintWindow *p_window)
{
    UtilElemtreePrintdata  data;
    ProError               status = PRO_TK_NO_ERROR;
    char                   fname[PRO_NAME_SIZE];
    wchar_t               wfname[PRO_NAME_SIZE];

    data.options = options;
    ProTKSprintf (fname, "%s", "elemtree.inf");
    data.f = PTApplsUnicodeFopen (fname, "w");
    if ( data.f == (FILE *)NULL )
    {
        status = PRO_TK_GENERAL_ERROR;
    }
    else
    {
        ProTKFprintf(data.f, "Element tree\n============\n\n");
        if (*p_window == PRO_TEST_GRAPHICS_WINDOW)
        {
            ProUtilDisp((char *)"Element tree", PRO_B_FALSE);
            win_data.point[1] = win_data.point[1] - win_data.decrease;
            ProUtilDisp((char *)"============", PRO_B_FALSE);
            win_data.point[1] = win_data.point[1] - 2 * win_data.decrease;
        }
    }

    memset(data.branches, 0, sizeof(data.branches));

    data.window = *p_window;

    if ( status == PRO_TK_NO_ERROR )
    {
        status = ProElemtreeElementVisit ( elem_tree, elem_path,
                (ProElemtreeVisitFilter)NULL,
                (ProElemtreeVisitAction)ProUtilElemtreeElemPrint,
                (ProAppData)&data );
        TEST_CALL_REPORT("ProElemtreeElementVisit()","ProUtilElemtreePPrint()",
                                        status, (status != PRO_TK_NO_ERROR));
    }

    if ( (data.f != (FILE *)NULL)&&(*p_window == PRO_TEST_INFO_WINDOW))
    {
        fclose ( data.f );
        ProInfoWindowDisplay(ProStringToWstring(wfname, fname), NULL, NULL);
    }

    return ( status );
}

/*====================================================================*\
FUNCTION : ProUtilElemtreeElemPrint
PURPOSE  : Print the element tree
\*====================================================================*/
static ProError ProUtilElemtreeElemPrint (
   ProElement     elem_tree,
   ProElement     elem,
   ProElempath    elem_path,
   UtilElemtreePrintdata   *p_data)
{
    int          i, level = -1, n_elems = -1;
    ProElemId    id;
    ProError     status = PRO_TK_NO_ERROR;
    char         indent[2*PRO_ELEMTREE_MAX_LEVEL];
    ProBoolean   is_compound = PRO_B_FALSE, is_array = PRO_B_FALSE,
                 is_multi_val = PRO_B_FALSE, are_more_items = PRO_B_FALSE;
    ProElement  *children;

    status = ProElementIdGet (elem, &id );
    TEST_CALL_REPORT("ProElementIdGet()","ProUtilElemtreeElemPrint()",
                                        status, (status != PRO_TK_NO_ERROR));
    status = ProElementIsCompound (elem_tree, elem_path, &is_compound);
    TEST_CALL_REPORT("ProElementIsCompound()","ProUtilElemtreeElemPrint()",
                                        status, (status != PRO_TK_NO_ERROR));

    if (status == PRO_TK_NO_ERROR)
    {
        status = ProElementIsArray (elem_tree, elem_path, &is_array);
        TEST_CALL_REPORT("ProElementIsArray()",
                         "ProUtilElemtreeElemPrint()",
                            status, (status != PRO_TK_NO_ERROR));
    }

    if (status == PRO_TK_NO_ERROR)
    {
        status = ProElempathSizeGet ( elem_path, &level );
        TEST_CALL_REPORT("ProElempathSizeGet()",
                         "ProUtilElemtreeElemPrint()",
                            status, (status != PRO_TK_NO_ERROR));
    }

/*--------------------------------------------------------------------*\
   Check to see if element is multivalued
\*--------------------------------------------------------------------*/
    if (status == PRO_TK_NO_ERROR)
    {
        status = ProElementIsMultival(elem_tree,elem_path, &is_multi_val);
        TEST_CALL_REPORT("ProElementIsMultival()","ProUtilElemtreeElemPrint()",
                                        status, (status != PRO_TK_NO_ERROR));
    }

/*--------------------------------------------------------------------*\
    Set the indents properly
\*--------------------------------------------------------------------*/
    if (status != PRO_TK_NO_ERROR)
        return PRO_TK_BAD_INPUTS;

    if (is_compound || is_array)
    {
        status = ProArrayAlloc(0,sizeof(ProElement),1,(ProArray*) &children);
        TEST_CALL_REPORT("ProArrayAlloc()","ProUtilElemtreeElemPrint()",
                                        status, (status != PRO_TK_NO_ERROR));

        if (status == PRO_TK_NO_ERROR)
        {
            if (is_compound)
            {
                status = ProElementChildrenGet(elem_tree, elem_path, &children);
                TEST_CALL_REPORT("ProElementChildrenGet()",
                             "ProUtilElemtreeElemPrint()",
                              status, (status != PRO_TK_NO_ERROR));
                if (status == PRO_TK_NO_ERROR)
                {
                    status = ProArraySizeGet((ProArray) children, &n_elems);
                    TEST_CALL_REPORT("ProArraySizeGet()",
                                 "ProUtilElemtreeElemPrint()",
                                  status, (status != PRO_TK_NO_ERROR));
                }
            }
            else
            {
                status = ProElementArrayGet(elem_tree, elem_path, &children);
                TEST_CALL_REPORT("ProElementArrayGet()",
                             "ProUtilElemtreeElemPrint()",
                              status, (status != PRO_TK_NO_ERROR));
		status = ProElementArrayCount(elem_tree, elem_path, &n_elems);
		TEST_CALL_REPORT("ProElementArrayCount()",
                                 "ProUtilElemtreeElemPrint()",
                                  status, (status != PRO_TK_NO_ERROR));

            }

            if (level > 0)
                (p_data->branches[level-1])--;

            if (n_elems > 0)
            {
                p_data->branches[level] = n_elems;
            }
        }

        status = ProArrayFree((ProArray*) &children);
        TEST_CALL_REPORT("ProArrayFree()","ProUtilElemtreeElemPrint()",
                                    status, (status != PRO_TK_NO_ERROR));
    }
    else
    {
        if (level == -1 )
            return PRO_TK_BAD_INPUTS;

        if (level > 0)
            (p_data->branches[level-1])--;
    }

    indent[0] = '\0';

    if ( level > 0 )
    {
        for ( i = 0; i < (level-1); i++ )
        {
            if ( p_data->branches[i] > 0 )
            {
                ProUtilstrcat (indent, " |   " );
            }
            else
                ProUtilstrcat (indent, "     " );
        }
            ProUtilstrcat(indent," |---");
    }

    for (i = 0; i < 20; i++)
    {
        if ( p_data->branches[i] > 0 )
        {
            are_more_items = PRO_B_TRUE;
            break;
        }
    }

    if ( status == PRO_TK_NO_ERROR )
    {
        status = ProUtilElemPrint ( elem, is_compound, is_array, is_multi_val,
                        are_more_items, *p_data, indent );
        if (status != PRO_TK_NO_ERROR)
        {
            ProTKFprintf(p_data->f, "\n");
            ProTKFprintf(stderr, "Element id %d is bad valued \n", id);
        }

    }

   return ( PRO_TK_NO_ERROR );
}

/*====================================================================*\
FUNCTION : ProUtilElemPrint
PURPOSE  : Print a single element
\*====================================================================*/
static ProError ProUtilElemPrint (
   ProElement                elem,
   ProBoolean                is_compound,
   ProBoolean                is_array,
   ProBoolean                is_multi_val,
   ProBoolean                are_more_items,
   UtilElemtreePrintdata     data,
   char                     *indent)
{
    ProError     status = PRO_TK_NO_ERROR;

    status = ProUtilElemIdPrint ( elem,is_compound,is_array,data, indent);

    if ( status == PRO_TK_NO_ERROR && !is_compound && !is_array)
    {
        status = ProUtilElemValuePrint ( elem, is_multi_val,
                                are_more_items, data, indent);
    }
    else
    {
        ProTKFprintf ( data.f, "\n");
        if (data.window == PRO_TEST_GRAPHICS_WINDOW)
        {
            win_data.point[1] = win_data.point[1] - win_data.decrease;
            win_data.point[0] = 0;
        }
    }

    return ( status );
}

/*====================================================================*\
FUNCTION : ProUtilElemIdPrint
PURPOSE  : Print the element name ( or id if it is not in the str table)
\*====================================================================*/
static ProError ProUtilElemIdPrint (
    ProElement                elem,
    ProBoolean                is_compound,
    ProBoolean                is_array,
    UtilElemtreePrintdata     data,
    char                     *indent)
{
    ProElemId    elem_id;
    ProError     status = PRO_TK_NO_ERROR;
    char elem_name[40], s[100];

   status = ProElementIdGet ( elem, &elem_id );
   TEST_CALL_REPORT("ProElementIdGet()","ProUtilElemtreePPrint()",
                                        status, (status != PRO_TK_NO_ERROR));

    if (status == PRO_TK_NO_ERROR)
    {
        status = ProUtilIdToStr(elem_id, elem_name);
    }

    if (status == PRO_TK_NO_ERROR)
    {
        ProTKSprintf ( s, "%s%s  ", indent, elem_name);
        ProTKFprintf ( data.f, s);
        if (data.window == PRO_TEST_GRAPHICS_WINDOW)
        {
            ProUtilDisp(s, PRO_B_TRUE);
        }
    }
    else if ( status == PRO_TK_E_NOT_FOUND )
    {
        ProTKSprintf(s, "%sElem Id: %3d  ", indent, elem_id );
        ProTKFprintf ( data.f, s);
        if (data.window == PRO_TEST_GRAPHICS_WINDOW)
        {
            ProUtilDisp(s, PRO_B_TRUE);
        }

        status = PRO_TK_NO_ERROR;
    }

    if ( status == PRO_TK_NO_ERROR && is_compound )
    {
        ProTKSprintf ( s, " (COMPOUND) ");
        ProTKFprintf ( data.f, s);
        if (data.window == PRO_TEST_GRAPHICS_WINDOW)
        {
            ProUtilDisp(s, PRO_B_TRUE);
        }
    }

    if ( status == PRO_TK_NO_ERROR && is_array )
    {
        ProTKSprintf ( s, " (ARRAY) ");
        ProTKFprintf ( data.f, s);
        if (data.window == PRO_TEST_GRAPHICS_WINDOW)
        {
            ProUtilDisp(s, PRO_B_TRUE);
        }
    }

    return ( status );
}

/*====================================================================*\
FUNCTION : ProUtilElemValuePrint
PURPOSE  : Print the value of an element
\*====================================================================*/
static ProError ProUtilElemValuePrint (
    ProElement                elem,
    ProBoolean                is_multi_val,
    ProBoolean                are_more_items,
    UtilElemtreePrintdata     data,
    char                     *indent)
{
    ProValue     value = (ProValue)NULL;
    ProError     status = PRO_TK_NO_ERROR;
    char s[100], *chp = indent;

    if ( is_multi_val == PRO_B_FALSE)
    {

        status = ProUtilValuePrint ( elem, data, NULL);

    }
    else /* the case of multipule values is deprecated; check the documentation of specific feature trees */ 
    {
        ProValue    *p_value = (ProValue*)NULL;
        int          i, n_vals, ind_len;

        status = ProArrayAlloc(0, sizeof(ProValue), 1, (ProArray*)&p_value);
        TEST_CALL_REPORT("ProArrayAlloc()","ProUtilElemValuePrint()",
                                status, (status != PRO_TK_NO_ERROR));

        if (status == PRO_TK_NO_ERROR)
        {   
            status = ProElementValuesGet(elem, &p_value);
            TEST_CALL_REPORT("ProElementValuesGet()","ProUtilElemValuePrint()",
                                        status, (status != PRO_TK_NO_ERROR));
        }

        if (status == PRO_TK_NO_ERROR)
        {
            status = ProArraySizeGet((ProArray)p_value, &n_vals);
            TEST_CALL_REPORT("ProArraySizeGet()", "ProUtilElemValuePrint()",
                                    status, (status != PRO_TK_NO_ERROR));


            ind_len = strlen(indent);

            if (are_more_items == PRO_B_TRUE)
            {
                *(chp+=ind_len) = '\0';
                for (i = 0; i < 3; i++)
                    *(--chp) = ' ';
            }
            else
            {
                for (i = 0; i < ind_len; i++)
                    *(chp++) = ' ';
                *(chp) = '\0';
            }

            for (i = 0 ; i < n_vals; i++)
            {
                status = ProUtilValuePrintDeprecated (p_value[i], data,
                                    i == 0 ? (char*)NULL : indent );
            }

	    if( n_vals == 0 )
		ProTKFprintf ( data.f, "\n" );
        }

        if (p_value != NULL)
        {
            status = ProArrayFree((ProArray*)&p_value);
            TEST_CALL_REPORT("ProArrayFree()","ProUtilElemValuePrint()",
                                    status, (status != PRO_TK_NO_ERROR));
        }
    }

    return ( status );
}

/*====================================================================*\
FUNCTION : ProUtilValuePrintDeprecated
PURPOSE  : Print out an elements value
\*====================================================================*/
static ProError ProUtilValuePrintDeprecated (
    ProValue              value,
    UtilElemtreePrintdata data,
    char                 *indent)
{
    ProValueData value_data;
    ProError     status = PRO_TK_NO_ERROR;
    char temp[100], temp2[100], li[2*PRO_ELEMTREE_MAX_LEVEL];
    ProModelitem mdl_item;
    ProAsmcomppath path;
    double *d;

    int k;

    li[0] = '\0';

    if (indent != (char*) NULL)
    {
        ProUtilstrcat(li, (const char *)indent);
        ProUtilstrcat(li,"                 ");
    }

    status = ProValueDataGet (value, &value_data);
    TEST_CALL_REPORT("ProValueDataGet()","ProUtilValuePrintDeprecated()",
                                        status, (status != PRO_TK_NO_ERROR));

    if ( status == PRO_TK_NO_ERROR )
    {
        switch ( value_data.type )
        {
            case PRO_VALUE_TYPE_TRANSFORM:
                if ( value_data.v.t == NULL )
                {
                   ProTKSprintf (temp, "%sVALUE (TRANSFORM)", li );
                   break;
                }
                ProTKSprintf (temp, "%sVALUE (TRANSFORM)", li );
                d = (double*)value_data.v.t;
                for (k=0; k<4; k++)
                    ProTKPrintf("%8.5f %8.5f %8.5f %8.5f\n", d[k*4],
                          d[k*4+1], d[k*4+2], d[k*4+3]);
                break;

            case PRO_VALUE_TYPE_INT:
                ProTKSprintf (temp, "%sVALUE (INT)       = %d",li,value_data.v.i );
                break;

            case PRO_VALUE_TYPE_DOUBLE:
                ProTKSprintf (temp, "%sVALUE (DOUBLE)    = %f",li,value_data.v.d );
                break;

            case PRO_VALUE_TYPE_STRING:
                if ( value_data.v.s == NULL )
                {
                   ProTKSprintf (temp, "%sVALUE (STRING)    = ",li);
                   break;
                }
                ProTKSprintf (temp, "%sVALUE (STRING)    = %s",li,value_data.v.s );
                break;

            case PRO_VALUE_TYPE_WSTRING:
                if ( value_data.v.w == NULL )
                {
                   ProTKSprintf (temp, "%sVALUE (WSTRING)   = ",li);
                   break;
                }
                ProTKSprintf (temp, "%sVALUE (WSTRING)   = %s",li,
                                ProWstringToString(temp2, value_data.v.w) );
                break;

            case PRO_VALUE_TYPE_SELECTION:
                if ( value_data.v.r == NULL )
                {
                   ProTKSprintf (temp, "%sVALUE (SELECTION) = ", li);
                   break;
                }
                status = ProSelectionModelitemGet(value_data.v.r, &mdl_item);
                TEST_CALL_REPORT("ProSelectionModelitemGet()",
                    "ProUtilValuePrintDeprecated()", status, status != PRO_TK_NO_ERROR);
                status = ProSelectionAsmcomppathGet(value_data.v.r, &path);
                TEST_CALL_REPORT("ProSelectionAsmcomppathGet()",
                    "ProUtilValuePrintDeprecated()", status, status != PRO_TK_NO_ERROR);
                ProTKSprintf (temp, "%sVALUE (SELECTION) Id = %d Type = %d Owner = %x",
                    li, mdl_item.id, mdl_item.type, mdl_item.owner);

                if (path.table_num>0)
                {
                    ProUtilstrcat(temp, " Path = ");
                    for(k=0; k<path.table_num; k++)
                        ProTKSprintf(temp+strlen(temp), " %d", path.comp_id_table[k]);
                    ProTKSprintf(temp+strlen(temp), " Owner %x", path.owner);

                }
                ProTKPrintf("%s\n", temp);
                ProTKSprintf (temp, "%sVALUE (SELECTION)   id = %d", li,
                    mdl_item.id);

                break;

            case PRO_VALUE_TYPE_POINTER:
                /* For QCR files */
                if (value_data.v.p != NULL)
                    ProTKSprintf(temp,  "%sPOINTER           = **", li);
                else
                    ProTKSprintf (temp, "%sPOINTER           = %x",li,
                             value_data.v.p );
                ProTKPrintf("%x\n",value_data.v.p);

                break;
        }

        ProTKFprintf (data.f, temp );
        ProTKFprintf (data.f, "\n");
        if(data.window == PRO_TEST_GRAPHICS_WINDOW)
        {
            ProUtilDisp(temp, PRO_B_FALSE);
            win_data.point[1] = win_data.point[1] - win_data.decrease;
            win_data.point[0] = 0;
        }
    }

    return ( status );
}

/*====================================================================*\
FUNCTION : ProUtilValuePrint
PURPOSE  : Print out an elements value
\*====================================================================*/
static ProError ProUtilValuePrint (
    ProElement            element,
    UtilElemtreePrintdata data,
    char                 *indent)
{
    ProValueDataType value_type;
    ProError     status = PRO_TK_NO_ERROR;
    char temp[100], temp2[100], li[2*PRO_ELEMTREE_MAX_LEVEL];
    ProModelitem mdl_item;
    ProAsmcomppath path;
    double *d;

    int k;

    li[0] = '\0';

    if (indent != (char*) NULL)
    {
        ProUtilstrcat(li, (const char *)indent);
        ProUtilstrcat(li,"                 ");
    }

		status = ProElementValuetypeGet (element, &value_type);
		TEST_CALL_REPORT("ProElementValuetypeGet()","ProUtilValuePrint()",
                                        status, (status != PRO_TK_NO_ERROR && status != PRO_TK_EMPTY));

    if ( status == PRO_TK_NO_ERROR )
    {
        switch ( value_type )
        {
            case PRO_VALUE_TYPE_TRANSFORM:
	    {
                ProMatrix value;
                status = ProElementTransformGet(element, NULL, value);
                if ( status != PRO_TK_NO_ERROR )
                {
                   ProTKSprintf (temp, "%sVALUE (TRANSFORM)", li );
                   break;
                }
                ProTKSprintf (temp, "%sVALUE (TRANSFORM)", li );
		d = (double*)value;		
		for (k=0; k<4; k++)
		    ProTKPrintf("%8.5f %8.5f %8.5f %8.5f\n", d[k*4],
			  d[k*4+1], d[k*4+2], d[k*4+3]);
                break;
	    }
            case PRO_VALUE_TYPE_INT:
	    {
                int value;
                status = ProElementIntegerGet(element, NULL, &value);
                ProTKSprintf (temp, "%sVALUE (INT)       = %d",li,value );
                break;
	    }
            case PRO_VALUE_TYPE_DOUBLE:
	    {
                double value;
                status = ProElementDoubleGet(element, NULL, &value);
                ProTKSprintf (temp, "%sVALUE (DOUBLE)    = %f",li,value );
                break;
	    }
            case PRO_VALUE_TYPE_STRING:
	    {
                char *value;
                status = ProElementStringGet(element, NULL, &value);
                if ( value == NULL )
                {
                   ProTKSprintf (temp, "%sVALUE (STRING)    = ",li);
                   break;
                }
                ProTKSprintf (temp, "%sVALUE (STRING)    = %s",li,value );
                status = ProStringFree( value );
                break;
	    }
            case PRO_VALUE_TYPE_WSTRING:
	    {
                wchar_t *value;
                status = ProElementWstringGet(element, NULL, &value);
                if ( value == NULL )
                {
                   ProTKSprintf (temp, "%sVALUE (WSTRING)   = ",li);
                   break;
                }
                ProTKSprintf (temp, "%sVALUE (WSTRING)   = %s",li,
                                ProWstringToString(temp2, value) );
                status = ProWstringFree( value );
                break;
	    }
            case PRO_VALUE_TYPE_SELECTION:
	    {
                ProReference value;
		ProSelection sel;
                status = ProElementReferenceGet(element, NULL, &value);
                if ( value == NULL )
                {
                   ProTKSprintf (temp, "%sVALUE (SELECTION) = ", li);
                   break;
                }
		status = ProReferenceToSelection(value, &sel);
                status = ProSelectionModelitemGet(sel, &mdl_item);
    		TEST_CALL_REPORT("ProSelectionModelitemGet()",
		    "ProUtilValuePrint()", status, status != PRO_TK_NO_ERROR);
		status = ProSelectionAsmcomppathGet(sel, &path);
    		TEST_CALL_REPORT("ProSelectionAsmcomppathGet()",
		    "ProUtilValuePrint()", status, status != PRO_TK_NO_ERROR);
                ProTKSprintf (temp, "%sVALUE (SELECTION) Id = %d Type = %d Owner = %x",
		    li, mdl_item.id, mdl_item.type, mdl_item.owner);

		if (path.table_num>0)
		{
		    ProUtilstrcat(temp, " Path = ");		    
		    for(k=0; k<path.table_num; k++)
			ProTKSprintf(temp+strlen(temp), " %d", path.comp_id_table[k]);
		    ProTKSprintf(temp+strlen(temp), " Owner %x", path.owner);
		    
		}
		ProTKPrintf("%s\n", temp);		
                ProTKSprintf (temp, "%sVALUE (SELECTION)   id = %d", li, 
		    mdl_item.id);
		status = ProReferenceFree( value );
                break;
	    }
            case PRO_VALUE_TYPE_POINTER:
	    {
                ProAppData value;
                status = ProElementSpecialvalueGet(element, NULL, &value);
		/* For QCR files */
		if (value != NULL)
		    ProTKSprintf(temp,  "%sPOINTER           = **", li);
		else    
		    ProTKSprintf (temp, "%sPOINTER           = %x",li, 
			     value );
		ProTKPrintf("%x\n",value);
		
                break;
	    }
        }

        ProTKFprintf (data.f, temp );
        ProTKFprintf (data.f, "\n");
        if(data.window == PRO_TEST_GRAPHICS_WINDOW)
        {
            ProUtilDisp(temp, PRO_B_FALSE);
            win_data.point[1] = win_data.point[1] - win_data.decrease;
            win_data.point[0] = 0;
        }
    }

    return ( status );
}


/*====================================================================*\
FUNCTION : ProUtilDisp
PURPOSE  : Display a feature tree in an information window
\*====================================================================*/
static ProError ProUtilDisp(char *text, ProBool move_carret)
{
   ProError err;
   wchar_t w_str[150];
   ProTextAttribute attribute, new_attribute;
   double char_length;

   err = ProTextAttributesCurrentGet(&attribute);
   TEST_CALL_REPORT("ProTextAttributesCurrentGet","ProUtilDisp",
                                        err, err!=PRO_TK_NO_ERROR);

   new_attribute = attribute;
   new_attribute.height = 15;
   new_attribute.width_factor = 0.75;

#if 0

   err = ProTextHeightCurrentSet(new_attribute.height);
          TEST_CALL_REPORT("ProTextHeightCurrentSet","ProUtilDisp",
                                        err, err!=PRO_TK_NO_ERROR);
   err =  ProTextWidthFactorCurrentSet(new_attribute.width_factor);
          TEST_CALL_REPORT("ProTextWidthFactorCurrentSet","ProUtilDisp",
                                        err, err!=PRO_TK_NO_ERROR);
#endif
   ProStringToWstring(w_str, text);
   err = ProGraphicsTextDisplay(win_data.point, w_str);
   TEST_CALL_REPORT("ProGraphicsTextDisplay","ProUtilDisp",
                                       err, err!=PRO_TK_NO_ERROR);
#if 0
   err = ProTextHeightCurrentSet(attribute.height);
          TEST_CALL_REPORT("ProTextHeightCurrentSet","ProUtilDisp",
                                        err, err!=PRO_TK_NO_ERROR);
   err = ProTextWidthFactorCurrentSet(attribute.width_factor);
          TEST_CALL_REPORT("ProTextWidthFactorCurrentSet","ProUtilDisp",
                                        err, err!=PRO_TK_NO_ERROR);
#endif

   if (move_carret == PRO_B_TRUE)
     {
       char_length = new_attribute.height * new_attribute.width_factor;
       win_data.point[0] += 2 * char_length * (double)strlen(text);
     }

   return err;
}


/*====================================================================*\
FUNCTION : ProUtilElemtreeInGrWindow
PURPOSE  :
\*====================================================================*/
static ProError ProUtilElemtreeInGrWindow(
   ProElement  elem_tree,
   ProElempath elem_path,
   ProUtilElemtreePrintOpts *options,
   int *p_win_id)
{
   ProError err = PRO_TK_NO_ERROR;
#ifndef PT_PRODUCTS_BUILD
   ProMdlName obj_name;
   ProType obj_type = PRO_PART;
   ProSolid tree_obj;
   int cur_id, newwin_id;
   ProMatrix transform;
   UtilTreeprintWindow window = PRO_TEST_GRAPHICS_WINDOW;

   ProStringToWstring(obj_name, (char *)"feature_tree");

   err = ProSolidMdlnameCreate(obj_name, (ProMdlfileType)obj_type, &tree_obj);
   TEST_CALL_REPORT("ProSolidMdlnameCreate()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);

   if (err != PRO_TK_NO_ERROR)
     {
       err = ProMdlnameInit(obj_name, (ProMdlfileType)obj_type, (ProMdl *)&tree_obj);
       TEST_CALL_REPORT("ProMdlnameInit()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);
     }

    if (err != PRO_TK_NO_ERROR)
     {
       err = ProMdlnameRetrieve(obj_name, (ProMdlfileType)obj_type, (ProMdl *)&tree_obj);
       TEST_CALL_REPORT("ProMdlnameRetrieve()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);
     }

   err = ProWindowCurrentGet(&cur_id);
   TEST_CALL_REPORT("ProWindowCurrentGet()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);

   err = ProObjectwindowMdlnameCreate(obj_name, obj_type, &newwin_id);
   TEST_CALL_REPORT("ProObjectwindowMdlnameCreate()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);

   err = ProWindowCurrentSet(newwin_id);
   TEST_CALL_REPORT("ProWindowCurrentSet()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);

   err = ProSolidDisplay(tree_obj);
   TEST_CALL_REPORT("ProSolidDisplay()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);


   err = ProViewMatrixGet((ProMdl) tree_obj, NULL, transform);
   TEST_CALL_REPORT("ProViewMatrixGet", "ProUtilElemtreeInGrWindow", err,
                                                err != PRO_TK_NO_ERROR);

   err = ProDisplist2dDelete(5);
   TEST_CALL_REPORT("ProDisplist2dDelete()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);

   win_data.point[0] = 0.0;
   win_data.point[1] = 800;
   win_data.decrease = 35;

   err = ProDisplist2dCreate(5, transform, 
        (ProDisplistCallback)ProUtilElemtreePPrint,
        (void *)elem_tree, (void *)elem_path, (void *)options, (void *)&window);
   TEST_CALL_REPORT("ProDisplist3dCreate", "ProUtilElemtreeInGrWindow", err,
                                                err != PRO_TK_NO_ERROR);

   err = ProWindowRefresh( newwin_id );
   TEST_CALL_REPORT("ProWindowRefresh()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);


   err = ProDisplist2dDisplay(5, transform);
   TEST_CALL_REPORT("ProDisplist2dDisplay()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);

   err = ProWindowCurrentSet(cur_id);
   TEST_CALL_REPORT("ProWindowCurrentSet()", "ProUtilElemtreeInGrWindow",
                                             err, err != PRO_TK_NO_ERROR);

   if(p_win_id != NULL)
     *p_win_id = newwin_id;
#endif
   return err;
}

/*====================================================================*\
    FUNCTION :	ProUtilShowTreeInInfo()
    PURPOSE  :	show tree
\*====================================================================*/
ProError ProUtilShowTreeInInfo(
    ProElement elem_tree )
{
    return (ProUtilElementtreePrint(elem_tree,PRO_TEST_INFO_WINDOW,NULL));
}


/*====================================================================*\
    FUNCTION :	ProUtilElemWithSelectionFilter()
    PURPOSE  :	Finf Elements with selection data
\*====================================================================*/
ProError ProUtilElemWithSelectionFilter(
    ProElement elem_tree,
    ProElement elem,
    ProElempath elempath)
{
    ProError err;
    ProBoolean is_comp, is_multi, has_sel = PRO_B_FALSE;
    ProValue value, *p_values;
    ProValueData val_data;
    int i, n_val = -1;
	ProReference *p_references, p_reference;

    err = ProElementIsArray(elem, NULL, &is_comp);
    TEST_CALL_REPORT("ProElementIsArray()", 
	    "ProUtilElemWithSelectionFilter()", err, err != PRO_TK_NO_ERROR);

    
    if (is_comp == PRO_B_FALSE)
    {
	err = ProElementIsCompound(elem, NULL, &is_comp);
	TEST_CALL_REPORT("ProElementIsCompound()",
	    "ProUtilElemWithSelectionFilter()", err, err != PRO_TK_NO_ERROR);
    }

    if (is_comp == PRO_B_TRUE)
	return (PRO_TK_CONTINUE);

    err = ProElementIsMultival(elem, NULL, &is_multi);
    TEST_CALL_REPORT("ProElementIsMultival()",
	    "ProUtilElemWithSelectionFilter()", err, err != PRO_TK_NO_ERROR);

    if (is_multi)
    {
	  err = ProElementReferencesGet(elem, NULL, &p_references);
	  TEST_CALL_REPORT("ProElementReferencesGet()",
	    "ProUtilElemWithSelectionFilter()", err, err != PRO_TK_NO_ERROR);

	  if(err == PRO_TK_NO_ERROR)
	  {
		err = ProArraySizeGet((ProArray)p_references, &n_val);
		
		TEST_CALL_REPORT("ProArraySizeGet()",
		     "ProUtilElemWithSelectionFilter()", err, err != PRO_TK_NO_ERROR);
	
		if(n_val > 0)
		{
			has_sel = PRO_B_TRUE;
		
			ProReferencearrayFree((ProArray*)&p_references);
			    TEST_CALL_REPORT("ProReferencearrayFree()",
				"ProUtilElemWithSelectionFilter()", 
				err, err != PRO_TK_NO_ERROR);			
		}
	  }
	}

    else
    {


		
		err = ProElementReferenceGet(elem, NULL, &p_reference);
		TEST_CALL_REPORT("ProElementReferenceGet()",
			"ProUtilElemWithSelectionFilter()", err, err != PRO_TK_NO_ERROR);

		if ( err == PRO_TK_NO_ERROR )
			has_sel = PRO_B_TRUE;
    }

    return (has_sel == PRO_B_TRUE ? PRO_TK_NO_ERROR : PRO_TK_CONTINUE);
}

/*====================================================================*\
FUNCTION : ProUtilSelectionChangeAsmcomppath
PURPOSE  : Move selection from part and subassembly refs to the root 
	   assembly level 
\*====================================================================*/
ProError ProUtilSelectionChangeAsmcomppath(
    ProSelection old_sel,
    ProSelection *p_new_sel,
    ProAppData p_asmcomppath)
{
    ProAsmcomppath *p_fpath = ((ProAsmcomppath *)p_asmcomppath);
    ProAsmcomppath  ref_path, path;
    ProModelitem    modelitem;
    ProMdl	    mdl;
    ProError	    err;
    int i;

    /* Model and feature must be on the one branch from the root assy
       Model path must be less that feature path */	
    err = ProSelectionAsmcomppathGet(old_sel, &ref_path);
    TEST_CALL_REPORT("ProSelectionAsmcomppathGet()",
	    "ProUtilSelectionChangeAsmcomppath()", err, err != PRO_TK_NO_ERROR);
    if (ref_path.table_num >= p_fpath->table_num)
	return (PRO_TK_E_NOT_FOUND);

    err = ProSelectionModelitemGet(old_sel, &modelitem);
    TEST_CALL_REPORT("ProSelectionModelitemGet()",
	    "ProUtilSelectionChangeAsmcomppath()", err, err != PRO_TK_NO_ERROR);

    if (ref_path.table_num == 0)
	ref_path.owner = (ProSolid)modelitem.owner;

    for (i=0; i< p_fpath->table_num - ref_path.table_num; i++)   
    {
	err = ProAsmcomppathInit(p_fpath->owner, p_fpath->comp_id_table, i+1,
	    &path);
	TEST_CALL_REPORT("ProAsmcomppathInit()",
	    "ProUtilSelectionChangeAsmcomppath()", err, err != PRO_TK_NO_ERROR);

	err = ProAsmcomppathMdlGet(&path, &mdl);
	TEST_CALL_REPORT("ProAsmcomppathMdlGet()",
	    "ProUtilSelectionChangeAsmcomppath()", err, err != PRO_TK_NO_ERROR);

	/* Owner found on same branch as feature */
	if (mdl == (ProMdl)ref_path.owner)
	    break;
    }
    if (i >= p_fpath->table_num - ref_path.table_num)
	return (PRO_TK_E_NOT_FOUND);

    /* build new ref path */
    memcpy(path.comp_id_table + i + 1, ref_path.comp_id_table, 
	(ref_path.table_num+1) * sizeof(path.comp_id_table[0]));
    path.table_num = i + 1 + ref_path.table_num;

    err = ProSelectionAlloc(&path, &modelitem, p_new_sel);
    TEST_CALL_REPORT("ProSelectionAlloc()",
	    "ProUtilSelectionChangeAsmcomppath()", err, err != PRO_TK_NO_ERROR);

    return (PRO_TK_NO_ERROR);
}

/*====================================================================*\
FUNCTION : ProUtilOriginalGeomSelection
PURPOSE  : Changes selection to 'internal' surfaces to original geometry
\*====================================================================*/
ProError ProUtilOriginalGeomSelection(
    ProSelection old_sel,
    ProSelection *p_new_sel,
    ProAppData dummy)
{
    ProError err, er1;
    ProElempath path;
    ProModelitem modelitem;
    ProFeature feature;
    ProFeattype feat_type;
    ProBoolean replace = PRO_B_FALSE, is_vis;
    int i;
    ProElempathItem constr_type[3]; 
    ProElempathItem constr_ref[3]; 
    ProElement elem_tree, elem;
    ProReference ref;
    ProSelection sel;
    int type;

    constr_type[0].type = PRO_ELEM_PATH_ITEM_TYPE_ID;
    constr_type[0].path_item.elem_id = PRO_E_DTMPLN_CONSTRAINTS;
    constr_type[1].type = PRO_ELEM_PATH_ITEM_TYPE_INDEX;
    constr_type[1].path_item.elem_id = 0;
    constr_type[2].type = PRO_ELEM_PATH_ITEM_TYPE_ID;
    constr_type[2].path_item.elem_id = PRO_E_DTMPLN_CONSTR_TYPE;

    constr_ref[0].type = PRO_ELEM_PATH_ITEM_TYPE_ID;
    constr_ref[0].path_item.elem_id = PRO_E_DTMPLN_CONSTRAINTS;
    constr_ref[1].type = PRO_ELEM_PATH_ITEM_TYPE_INDEX;
    constr_ref[1].path_item.elem_id = 0;
    constr_ref[2].type = PRO_ELEM_PATH_ITEM_TYPE_ID;
    constr_ref[2].path_item.elem_id = PRO_E_DTMPLN_CONSTR_REF;

    if (old_sel != NULL)
    {
    err = ProSelectionModelitemGet(old_sel, &modelitem);
    TEST_CALL_REPORT("ProSelectionModelitemGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

    /* Do any wotk only if selection is surface on internal datum plane */
    if (modelitem.type != PRO_SURFACE)
	return (PRO_TK_E_NOT_FOUND);

    err = ProGeomitemFeatureGet((ProGeomitem*)&modelitem, &feature);
    TEST_CALL_REPORT("ProGeomitemFeatureGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

    err = ProFeatureVisibilityGet(&feature, &is_vis);
    TEST_CALL_REPORT("ProFeatureVisibilityGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);
    if (err != PRO_TK_NO_ERROR || is_vis != PRO_B_FALSE)
	return (PRO_TK_E_NOT_FOUND);

    err = ProFeatureTypeGet(&feature, &feat_type);
    TEST_CALL_REPORT("ProFeatureTypeGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

    if (err != PRO_TK_NO_ERROR || feat_type != PRO_FEAT_DATUM)
	return (PRO_TK_E_NOT_FOUND);

    /* internal datum plane found - create elemtree and check constraints*/
    for (i=0; i<2; i++)
    {
	err = ProElempathAlloc(&path);
	TEST_CALL_REPORT("ProElempathAlloc()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	constr_type[1].path_item.elem_index = i;
	err = ProElempathDataSet(path, constr_type, 3);
	TEST_CALL_REPORT("ProElempathDataSet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	err = ProFeatureElemtreeExtract (&feature, NULL,
                             PRO_FEAT_EXTRACT_NO_OPTS, &elem_tree );
	if (err != PRO_TK_NO_ERROR)
	    continue;
        err = ProElemtreeElementGet(elem_tree, path, &elem);
	if (err != PRO_TK_NO_ERROR)
	    continue;
        err = ProElementIntegerGet(elem, NULL, &type);
	if (err != PRO_TK_NO_ERROR)
	    continue;

	/* this printout is left for compatibility with existing tests */
	TEST_CALL_REPORT("ProFeatureElemValueGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	er1 = ProElempathFree(&path);
	TEST_CALL_REPORT("ProElempathFree()",
		"ProUtilOriginalGeomSelection()", er1, er1 != PRO_TK_NO_ERROR);

	if (err != PRO_TK_NO_ERROR)
	    continue;

	/* this printout is left for compatibility with existing tests */
	TEST_CALL_REPORT("ProValueDataGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);
	if (type != PRO_DTMPLN_THRU)
	    continue;

	err = ProElempathAlloc(&path);
	TEST_CALL_REPORT("ProElempathAlloc()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	constr_ref[1].path_item.elem_index = i;
	err = ProElempathDataSet(path, constr_ref, 3);
	TEST_CALL_REPORT("ProElempathDataSet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	err = ProFeatureElemtreeExtract (&feature, NULL,
                             PRO_FEAT_EXTRACT_NO_OPTS, &elem_tree );
	if (err != PRO_TK_NO_ERROR)
	    continue;
        err = ProElemtreeElementGet(elem_tree, path, &elem);
	if (err != PRO_TK_NO_ERROR)
	    continue;
        err = ProElementReferenceGet(elem, NULL, &ref);
	if (err != PRO_TK_NO_ERROR)
	    continue;
    err = ProReferenceToSelection(ref, &sel);
	if (err != PRO_TK_NO_ERROR)
	    continue;

	/* this printout is left for compatibility with existing tests */
	TEST_CALL_REPORT("ProFeatureElemValueGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	er1 = ProElempathFree(&path);
	TEST_CALL_REPORT("ProElempathFree()",
		"ProUtilOriginalGeomSelection()", er1, er1 != PRO_TK_NO_ERROR);

	if (err != PRO_TK_NO_ERROR)
	    continue;

	/* this printout is left for compatibility with existing tests */
	TEST_CALL_REPORT("ProValueDataGet()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	/* Original Selection found. Replace old one */
	err = ProSelectionCopy(sel, p_new_sel);
	TEST_CALL_REPORT("ProSelectionCopy()",
		"ProUtilOriginalGeomSelection()", err, err != PRO_TK_NO_ERROR);

	replace = PRO_B_TRUE;
	break;
      }
    }

    return (replace == PRO_B_TRUE ? PRO_TK_NO_ERROR : PRO_TK_E_NOT_FOUND);
}


/*====================================================================*\
FUNCTION : ProUtilValueReplaceSelection
PURPOSE  : Replace selection in value
\*====================================================================*/
/*
ProError ProUtilValueReplaceSelection(
    ProValue *p_value,
    ProUtilChahgeSelection user_func,
    ProAppData	appdata)
{
    ProSelection new_sel;
    ProValueData val_data,new_data;
    ProError err;

    err = ProValueDataGet(*p_value, &val_data);
    TEST_CALL_REPORT("ProValueDataGet()",
		"ProUtilValueReplaceSelection()", err, err != PRO_TK_NO_ERROR);
    
    if (err != PRO_TK_NO_ERROR || val_data.type != PRO_VALUE_TYPE_SELECTION)
	return (PRO_TK_E_NOT_FOUND);

        
    err = user_func(val_data.v.r, &new_sel, appdata);
    if (err != PRO_TK_NO_ERROR)
	return err;

    // Original Selection found. Replace old one 
    err = ProValueFree(*p_value);
    TEST_CALL_REPORT("ProValueFree()", 
	      "ProUtilValueReplaceSelection()", err, 0);
    err = ProValueAlloc(p_value);
    TEST_CALL_REPORT("ProValueAlloc()",
	      "ProUtilValueReplaceSelection()", err, err != PRO_TK_NO_ERROR);
    new_data.type = PRO_VALUE_TYPE_SELECTION;

    new_data.type = PRO_VALUE_TYPE_SELECTION;
    new_data.v.r = new_sel;

    err = ProValueDataSet(*p_value, &new_data);
    TEST_CALL_REPORT("ProValueDataSet()",
	    "ProUtilValueReplaceSelection()", err, err != PRO_TK_NO_ERROR);

    return (err);
}
*/

/*====================================================================*\
FUNCTION : ProUtilElemtreeReplaceSelection
PURPOSE  : Replace selection for elemtree
\*====================================================================*/
ProError ProUtilElemtreeReplaceSelection(
    ProElement elem_tree,
    ProUtilChahgeSelection user_func,
    ProAppData	appdata)
{
    ProError err;
    ElemtreeElement *p_elems;
    int i, j, n_elems, n_val;
    ProValue value, *p_values;
    ProBoolean  replace, is_multi;

    err = ProUtilCollectElemtreeElements(elem_tree, NULL, &p_elems);
    if (err != PRO_TK_NO_ERROR)
	return (err);
    
    err = ProArraySizeGet((ProArray)p_elems, &n_elems);
    TEST_CALL_REPORT("ProArraySizeGet()",
	    "ProUtilElemtreeReplaceSelection()", err, err != PRO_TK_NO_ERROR);
    for(i=0; i<n_elems; i++)
    {
	if (ProUtilElemWithSelectionFilter(elem_tree, p_elems[i].p_element,
	    p_elems[i].p_elempath) != PRO_TK_NO_ERROR)
	    continue;
	
	err = ProElementIsMultival(p_elems[i].p_element, NULL, &is_multi);
	TEST_CALL_REPORT("ProElementIsMultival()",
		"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);

	replace =PRO_B_FALSE;

	if (is_multi)
	{
		ProReference* p_references;

		ProElementReferencesGet(p_elems[i].p_element, NULL, &p_references);
		TEST_CALL_REPORT("ProElementReferencesGet()",
		"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);

		err = ProArraySizeGet((ProArray)p_references, &n_val);
	    TEST_CALL_REPORT("ProArraySizeGet()",
		"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);
						
	    for (j=0; j<n_val; j++)
	    {
			ProSelection old_sel, new_sel;
			
			err = ProReferenceToSelection(p_references[j], &old_sel);
			TEST_CALL_REPORT("ProReferenceToSelection()",
					"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);

			err = user_func(old_sel, &new_sel, appdata);

			if (err == PRO_TK_NO_ERROR)
			{
				replace = PRO_B_TRUE;
				err = ProSelectionToReference(new_sel, &p_references[j]);
				 TEST_CALL_REPORT("ProSelectionToReference()",
					"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);
						
			}
	    }
	    
	    if (replace == PRO_B_TRUE)
	    {
			ProElementReferencesSet(p_elems[i].p_element, p_references);
		TEST_CALL_REPORT("ProElementReferencesSet()",
		    "ProUtilElemtreeReplaceSelection()", 
	    		    			err, err != PRO_TK_NO_ERROR);
	    }
		ProReferencearrayFree((ProArray*)&p_references);
	    TEST_CALL_REPORT("ProReferencearrayFree()",
		"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);
	}
	else
	{
		ProReference p_reference;
		ProSelection old_sel2, new_sel2;

		ProElementReferenceGet(p_elems[i].p_element, NULL, &p_reference);
		TEST_CALL_REPORT("ProElementReferenceGet()",
		"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);

		err = ProReferenceToSelection(p_reference, &old_sel2);
			TEST_CALL_REPORT("ProReferenceToSelection()",
					"ProUtilElemtreeReplaceSelection()", 
						err, err != PRO_TK_NO_ERROR);

		err = user_func(old_sel2, &new_sel2, appdata);

		if (err == PRO_TK_NO_ERROR)
		{
			replace = PRO_B_TRUE;
			err = ProSelectionToReference(new_sel2, &p_reference);
				TEST_CALL_REPORT("ProSelectionToReference()",
				"ProUtilElemtreeReplaceSelection()", 
					err, err != PRO_TK_NO_ERROR);

			ProElementReferenceSet(p_elems[i].p_element, p_reference);
			TEST_CALL_REPORT("ProElementReferenceSet()",
						"ProUtilElemtreeReplaceSelection()", 
	    		    			err, err != PRO_TK_NO_ERROR);

		}
	}
    }	
    err = ProUtilElemtreeElementArrayFree(&p_elems);
    return (err);
}

/*=============================================================*\
  Function: 	ProUtilElemtreeCreate
  Purpose:	Create element tree by the ElemTreeData structure
  Return :      PRO_TK_NO_ERROR if successfull,
\*=============================================================*/
ProError ProUtilElemtreeCreate(
    ElemTreeData *elemarr,	    /* In : The array of ElemTreeData */
    int n_elem,			    /* In : number of ElemTreeData in elemarr */
    ProElement base_tree,	    /* In : the base tree */
    ProElement *elem_tree)	    /* Out : element tree */
{
    ProElement elem_level[10], elem;
    ProValue value;
    int i, type;
    ProError err;
    ProValueDataType data_type;
    ProSelection sel_1;
    ProReference ref_1;

    if (base_tree!=NULL)
	elem_level[0] = base_tree;

    for (i=0; i<n_elem; i++)
    {
	err = ProElementAlloc((ProElemId)elemarr[i].elem_id, &elem);
	TEST_CALL_REPORT("ProElementAlloc()", 
		 "ProUtilElemtreeCreate()", err, err != PRO_TK_NO_ERROR);
	if (err != PRO_TK_NO_ERROR)
	    return (err);
	if (elemarr[i].data.type != -1)
	{
/*----------------------------------------------------------*\
    Add Element value
\*----------------------------------------------------------*/

	    type = elemarr[i].data.type;
	    
	   switch(type)
	    {
	    	case PRO_VALUE_TYPE_INT :
		{
			ProElementIntegerSet(elem, elemarr[i].data.v.i);
			break;
		}
		case PRO_VALUE_TYPE_DOUBLE :
		{
			ProElementDoubleSet(elem, elemarr[i].data.v.d);
			break;
		}
		case PRO_VALUE_TYPE_POINTER :
		{
			ProElementSpecialvalueSet(elem, (ProAppData*)elemarr[i].data.v.p);
			break;
		}
		case PRO_VALUE_TYPE_STRING :
		{
			ProElementStringSet(elem, (char*)elemarr[i].data.v.s);
			break;
		}
		case PRO_VALUE_TYPE_WSTRING :
		{
			ProElementWstringSet(elem, (wchar_t*)elemarr[i].data.v.w);
			break;
		}
		case PRO_VALUE_TYPE_SELECTION :
		{
		        ProSelectionToReference(elemarr[i].data.v.r, &ref_1);
			ProElementReferenceSet(elem, ref_1);
			break;
		}
		case PRO_VALUE_TYPE_TRANSFORM :
		{
			ProElementTransformSet(elem, (double (*)[4])elemarr[i].data.v.t);
			break;
		}
		case PRO_VALUE_TYPE_BOOLEAN :
		{
			ProElementBooleanSet(elem, elemarr[i].data.v.b);
			break;
		}
	    }
	}
/*----------------------------------------------------------*\
    Add Element to the tree
\*----------------------------------------------------------*/
	if (elemarr[i].level!=0)
	{
	    err = ProElemtreeElementAdd(elem_level[elemarr[i].level-1], NULL, 
		elem);
	    TEST_CALL_REPORT("ProElemtreeElementAdd()", 
		 "ProUtilElemtreeCreate()", err, err != PRO_TK_NO_ERROR);
	}
	elem_level[elemarr[i].level] = elem;
    }
    elem_tree[0] = elem_level[0];
    return (PRO_TK_NO_ERROR);
}

/*=============================================================*\
  Function: 	ProUtilElemtreeElementGet
  Purpose:	Find Element by array of elempathitems
\*=============================================================*/
ProError ProUtilElemtreeElementGet(
    ProElement elem_tree,
    ProElempathItem *elempath_data,
    int	path_size,
    ProElement *p_element)
{
    ProElempath path;
    ProError err, status;

    err = ProElempathAlloc(&path);
    TEST_CALL_REPORT("ProElempathAlloc()",
		"ProUtilElemtreeElementGet()", err, err != PRO_TK_NO_ERROR);

    err = ProElempathDataSet(path, elempath_data, path_size);
    TEST_CALL_REPORT("ProElempathDataSet()", 
		"ProUtilElemtreeElementGet()", err, err != PRO_TK_NO_ERROR);

    err = ProElemtreeElementGet(elem_tree, path, p_element);
    TEST_CALL_REPORT("ProElemtreeElementGet()", 
		"ProUtilElemtreeElementGet()", err, err != PRO_TK_NO_ERROR &&
		err != PRO_TK_E_NOT_FOUND);
   
    status = ProElempathFree(&path);
    TEST_CALL_REPORT("ProElempathFree()", 
	    "ProUtilElemtreeElementGet()", status, status != PRO_TK_NO_ERROR);

    return(err);
}


/*====================================================================*\
FUNCTION : ProUtilFeatErrsPrint
PURPOSE  : Prints feature errors
\*====================================================================*/
ProError ProUtilFeatErrsPrint(ProErrorlist *errs)
{
    int i;

    for (i=0; i<errs->error_number; i++)
    {
        ProTKPrintf("Error %d: Error ID %d, Type %d, Error %d\n",
                i+1,errs->error_list[i].err_item_id,
                errs->error_list[i].err_item_type,
                errs->error_list[i].error);
    }

    return(PRO_TK_NO_ERROR);
}

/*====================================================================*\
FUNCTION : ProUtilElementValueSet()
PURPOSE  : Replace an element value by new one
\*====================================================================*/
ProError ProUtilElementValueSet(
    ProElement element,
    ProValueData  *value_data)
{
	ProError err;
    ProValue value;
    
    ProError status;
	ProValueDataType value_type;
	ProReference ref;

    switch(value_data->type)
    {
        case  PRO_VALUE_TYPE_INT :
            status = ProElementIntegerSet( element, value_data->v.i);
            TEST_CALL_REPORT("ProElementIntegerSet", "ProUtilElementValueSet",  
			    status, status != PRO_TK_NO_ERROR );                       
            break;
		case  PRO_VALUE_TYPE_DOUBLE:
            status = ProElementDoubleSet( element, value_data->v.d);
            TEST_CALL_REPORT("ProElementDoubleSet", "ProUtilElementValueSet",  
			    status, status != PRO_TK_NO_ERROR );                        
            break;
		case  PRO_VALUE_TYPE_POINTER:
            status = ProElementSpecialvalueSet( element, value_data->v.p);        
            TEST_CALL_REPORT("ProElementSpecialvalueSet", "ProUtilElementValueSet",  
			    status, status != PRO_TK_NO_ERROR );                      
            break;
		case  PRO_VALUE_TYPE_STRING:
            status = ProElementStringSet( element, value_data->v.s);   
            TEST_CALL_REPORT("ProElementStringSet", "ProUtilElementValueSet", 
			    status, status != PRO_TK_NO_ERROR );                                         
            break;
		case  PRO_VALUE_TYPE_WSTRING:
            status = ProElementWstringSet( element, value_data->v.w);   
            TEST_CALL_REPORT("ProElementWstringSet", "ProUtilElementValueSet",  
			    status, status != PRO_TK_NO_ERROR );                                        
            break;
		case  PRO_VALUE_TYPE_SELECTION: 
		ProSelectionToReference(value_data->v.r, &ref);
            status = ProElementReferenceSet( element, ref);
            TEST_CALL_REPORT("ProElementReferenceSet", "ProUtilElementValueSet",  
			    status, status != PRO_TK_NO_ERROR );                                        
            break;
		case  PRO_VALUE_TYPE_TRANSFORM:
            status = ProElementTransformSet( element, (double (*)[4])(value_data->v.t));   
            TEST_CALL_REPORT("ProElementTransformSet", "ProUtilElementValueSet", 
			    status, status != PRO_TK_NO_ERROR );                       
            break;
		case  PRO_VALUE_TYPE_BOOLEAN:
            status = ProElementBooleanSet( element, value_data->v.b);   
            TEST_CALL_REPORT("ProElementBooleanSet", "ProUtilElementValueSet", 
			    status, status != PRO_TK_NO_ERROR );                       
            break;
	}
    return(PRO_TK_NO_ERROR);
}


ProError ProUtilElementFeatTypeGet(ProFeature *feat, ProElemId *name_id)
{
   ProError status;
   UtilElemData   elem_data;
   ProElempathItem feat_elem_path [] =
       {
           { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_FEATURE_TYPE}
       };
   status = ProUtilElementFeatDataGet(*feat, feat_elem_path, 1, &elem_data);   
   if(PRO_TK_NO_ERROR == status)
       *name_id = elem_data.val.i;
   return status;
}

/*===============================================================*\
FUNCTION : ProUtilElementFeatDataGet
PURPOSE  : Fill the element data from the element path of a feature
\*===============================================================*/
ProError ProUtilElementFeatDataGet(ProFeature feature, 
   ProElempathItem ref_path_items[], int num_path_ele,
   UtilElemData *elem_data)
{
    ProError status,err;
    ProElement created_elemtree;
    ProAsmcomppath          *p_comp_path = NULL;

    status = ProFeatureElemtreeExtract (&feature, p_comp_path, 
        PRO_FEAT_EXTRACT_NO_OPTS, &created_elemtree);
    if(status != PRO_TK_NO_ERROR)
     return status;
    status = ProUtilElementTreeDataGet(created_elemtree, ref_path_items, num_path_ele,
                elem_data);
    ProElementFree(&created_elemtree);
    return status;
}

/*===============================================================*\
FUNCTION : ProUtilElementTreeDataGet
PURPOSE  : Fill the element data from the element tree
\*===============================================================*/
ProError ProUtilElementTreeDataGet(ProElement created_elemtree, 
   ProElempathItem ref_path_items[], int num_path_ele,
   UtilElemData *elem_data)
{
    ProError status;
    ProElement element = NULL;
    ProElempath path;
    ProValue p_value;
    ProValueDataType value_type = PRO_VALUE_TYPE_INT; 
    ProElemId  elem_id; 

    status = ProElempathAlloc(&path);
    status = ProElempathDataSet(path, ref_path_items, num_path_ele);
    status = ProElemtreeElementGet (created_elemtree, path, &element);
    status = ProElementIdGet( element, &elem_id);
    status = ProElementValuetypeGet ( element, &value_type ); 
    if(status != PRO_TK_NO_ERROR)   
     return PRO_TK_NO_ERROR;
    status = ProUtilElementValueGet(element, elem_data);
    status = ProElempathFree (&path);
    return status;
}


/*===============================================================*\
FUNCTION : ProUtilElementValueGet 
PURPOSE  : Get the element value from the element
\*===============================================================*/
ProError ProUtilElementValueGet(ProElement element, UtilElemData *elem_data)
{
    ProError status;
    ProValueDataType value_type = PRO_VALUE_TYPE_INT; 
    ProElemId  elem_id; 
    ProReference ref;

    status = ProElementIdGet( element, &elem_id);
    if ( elem_id == PRO_E_FEATURE_TREE || elem_id < 1) 
        return PRO_TK_NO_ERROR;

    status = ProElementValuetypeGet ( element, &value_type ); 
    if(status != PRO_TK_NO_ERROR)   
        return PRO_TK_NO_ERROR;
     
    if ( ( elem_id == PRO_E_STD_CURVE_COLLECTION_APPL ) || 
       ( elem_id == PRO_E_STD_SURF_COLLECTION_APPL ) )
    {
        elem_data->type = ELEM_VALUE_TYPE_COLLECTION;    
    }
    else
      elem_data->type = (UtilElemType)value_type;  

    switch(elem_data->type)
    {
        case  ELEM_VALUE_TYPE_INT :
            status = ProElementIntegerGet( element,
                ( ProElementIntegerOptions) NULL, &(elem_data->val.i));
            TEST_CALL_REPORT("ProElementIntegerGet", "ProUtilElementValueGet",  
			    status, status != PRO_TK_NO_ERROR );                       
            break;        
        case  ELEM_VALUE_TYPE_DOUBLE:
            status = ProElementDoubleGet( element,
                ( ProElementDoubleOptions) NULL, &(elem_data->val.d));
            TEST_CALL_REPORT("ProElementDoubleGet", "ProUtilElementValueGet",  
			    status, status != PRO_TK_NO_ERROR );                        
            break;        
        case  ELEM_VALUE_TYPE_POINTER:
            status = ProElementSpecialvalueGet( element,
                ( ProElementSpecialOptions) NULL, &(elem_data->val.p));        
            TEST_CALL_REPORT("ProElementDoubleGet", "ProUtilElementValueGet",  
			    status, status != PRO_TK_NO_ERROR );                      
            break;        
        case  ELEM_VALUE_TYPE_STRING:
            status = ProElementStringGet( element,
                    ( ProElementStringOptions) NULL, &(elem_data->val.s));   
            TEST_CALL_REPORT("ProElementDoubleGet", "ProUtilElementValueGet", 
			    status, status != PRO_TK_NO_ERROR );                                         
            break;        
        case  ELEM_VALUE_TYPE_WSTRING:
            status = ProElementWstringGet( element,
               ( ProElementWstringOptions) NULL, &(elem_data->val.w));   
            TEST_CALL_REPORT("ProElementWstringGet", "ProUtilElementValueGet",  
			    status, status != PRO_TK_NO_ERROR );                                        
            break;        
        case  ELEM_VALUE_TYPE_SELECTION: 
            status = ProElementReferenceGet( element,
                (ProElementReferenceOptions) NULL, &ref);
            TEST_CALL_REPORT("ProElementReferenceGet", "ProUtilElementValueGet",  
			    status, status != PRO_TK_NO_ERROR );                      
            status = ProReferenceToSelection(ref, &(elem_data->val.r));                    
            break;    
        case  ELEM_VALUE_TYPE_TRANSFORM:
            status = ProElementTransformGet( element,
                (ProElementTransformOptions) NULL, (double (*)[4])(elem_data->val.t));   
            TEST_CALL_REPORT("ProElementTransformGet", "ProUtilElementValueGet", 
			    status, status != PRO_TK_NO_ERROR );                       
            break;        
        case  ELEM_VALUE_TYPE_BOOLEAN:
            status = ProElementBooleanGet( element,
                (ProElementBooleanOptions) NULL, &(elem_data->val.b));   
            TEST_CALL_REPORT("ProElementBooleanGet", "ProUtilElementValueGet", 
			    status, status != PRO_TK_NO_ERROR );                       
            break;        
        case  ELEM_VALUE_TYPE_COLLECTION:         
             status = ProElementCollectionGet( element,
                (ProCollectionGetOptions) NULL, &(elem_data->val.collection));  
            TEST_CALL_REPORT("ProElementCollectionGet", "ProUtilElementValueGet",  
			    status, status != PRO_TK_NO_ERROR );                      
            break;        
        default:
             printf("NOT SUPPORTED FOR OTHER TYPE ..");
    }
    return status;
}

/*===============================================================*\
FUNCTION : ProUtilElemtreeElementAdd 
PURPOSE  : Add the element in the element tree
           Incase of integer, double and boolean pass the value,
           In other cases, pass the pointer.
\*===============================================================*/
ProError ProUtilElemtreeElementAdd( ProElement parent_element, ProElemId child_id_name,
                                 UtilElemType elem_type, ProAppData app_data)
{
    ProError status = PRO_TK_NO_ERROR;
    ProReference ref;
    double *dbl_val;
    int    *int_val;   /* integer */
    ProBoolean     *bool_val;	/* boolean */

    ProElement child_element;    
    status = ProElementAlloc ( child_id_name, &child_element);
    switch(elem_type)
    {
        case  ELEM_VALUE_TYPE_INT :
            int_val = (int *)app_data;
            status = ProElementIntegerSet( child_element, *int_val);
            TEST_CALL_REPORT("ProElementIntegerSet", "ProUtilElemtreeElementAdd",  
			    status, status != PRO_TK_NO_ERROR );                                 
            break;        
        case  ELEM_VALUE_TYPE_DOUBLE:
            dbl_val = (double *)(app_data);
            status = ProElementDoubleSet( child_element, *dbl_val);
            TEST_CALL_REPORT("ProElementDoubleSet", "ProUtilElemtreeElementAdd",  
			    status, status != PRO_TK_NO_ERROR );                                  
            break;        
        case  ELEM_VALUE_TYPE_POINTER:
            status = ProElementSpecialvalueSet( child_element, (void *)(app_data));        
            TEST_CALL_REPORT("ProElementSpecialvalueSet", "ProUtilElemtreeElementAdd", 
			    status, status != PRO_TK_NO_ERROR );                                            
            break;        
        case  ELEM_VALUE_TYPE_STRING:
            status = ProElementStringSet( child_element, (char *)(app_data));   
            TEST_CALL_REPORT("ProElementStringSet", "ProUtilElemtreeElementAdd", 
			    status, status != PRO_TK_NO_ERROR );                                             
            break;        
        case  ELEM_VALUE_TYPE_WSTRING:
            status = ProElementWstringSet( child_element, (wchar_t *)(app_data));   
            TEST_CALL_REPORT("ProElementWstringSet", "ProUtilElemtreeElementAdd", 
			    status, status != PRO_TK_NO_ERROR );                                            
            break;        
        case  ELEM_VALUE_TYPE_SELECTION: 
            status = ProSelectionToReference((ProSelection)(app_data), &ref);                
            status = ProElementReferenceSet( child_element, ref);
            TEST_CALL_REPORT("ProSelectionToReference", "ProUtilElemtreeElementAdd", 
			    status, status != PRO_TK_NO_ERROR );                                                 
            break;    
        case  ELEM_VALUE_TYPE_TRANSFORM:
            status = ProElementTransformSet( child_element, (double (*)[4])((app_data)));   
            TEST_CALL_REPORT("ProElementTransformSet", "ProUtilElemtreeElementAdd",  
			    status, status != PRO_TK_NO_ERROR );                                             
            break;        
        case  ELEM_VALUE_TYPE_BOOLEAN:
            bool_val = (ProBoolean *)(app_data); 
            status = ProElementBooleanSet( child_element, *bool_val);   
            TEST_CALL_REPORT("ProElementBooleanSet", "ProUtilElemtreeElementAdd",  
			    status, status != PRO_TK_NO_ERROR );                                             
            break;        
        case  ELEM_VALUE_TYPE_COLLECTION:         
            status = ProElementCollectionSet( child_element, (ProCollection)(app_data));  
            TEST_CALL_REPORT("ProElementCollectionSet", "ProUtilElemtreeElementAdd",  
			    status, status != PRO_TK_NO_ERROR );                      
            break;        
        default:
            status = PRO_TK_GENERAL_ERROR;
            printf("NOT SUPPORTED FOR OTHER TYPE ..");
            break;
    }
    if(PRO_TK_NO_ERROR != status)
        return status;     
    status = ProElemtreeElementAdd ( parent_element, NULL, child_element );
    return status;
}

ProError ProUtilSketchFeatureReset(ProFeature* feat, ProSection *section)
{
  
  ProError status;
  ProElement elem_tree, elem;
  ProElempath elem_path;
  ProFeatureCreateOptions *opts = 0;
  ProErrorlist err_list;

  
  ProElempathItem sketch_path_item[] = {
     {PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_STD_SECTION},
     {PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_SKETCHER}
  };
  
  status = ProFeatureElemtreeExtract (feat, NULL,
               PRO_FEAT_EXTRACT_NO_OPTS, &elem_tree );
  
  status = ProElempathAlloc(&elem_path);
  
  status = ProElempathDataSet(elem_path, sketch_path_item, 2);
  
  status = ProElemtreeElementGet(elem_tree, elem_path, &elem);
  
  status = ProElementSpecialvalueSet(elem, (ProAppData)*section);
  
  status = ProElemtreeElementAdd(elem_tree, elem_path, elem);
  status = ProArrayAlloc(1,sizeof(ProFeatureCreateOptions),
    1, (ProArray*)&opts);

  opts[0]= PRO_FEAT_CR_NO_OPTS;

  status = ProFeatureWithoptionsRedefine(NULL, feat, elem_tree,
    opts, PRO_REGEN_NO_FLAGS, &err_list);  
  TEST_CALL_REPORT("ProFeatureWithoptionsRedefine", "ProUtilSketchFeatureReset", 
			    status, status != PRO_TK_NO_ERROR );                       

  status = ProArrayFree((ProArray*)&opts);
  return status;

}

/*===============================================================*\
FUNCTION : ProUtilElementValueDataFree 
PURPOSE  : Get the element value from the element
\*===============================================================*/
ProError ProUtilElementValueDataFree(UtilElemData *elem_data)
  {
     ProError status = PRO_TK_NO_ERROR;
     switch(elem_data->type)
     {
         case  ELEM_VALUE_TYPE_STRING:
            status = ProStringFree( elem_data->val.s);   
            TEST_CALL_REPORT("ProStringFree", "ProUtilElementValueDataFree",  
			    status, status != PRO_TK_NO_ERROR );                      
            break;
         case  ELEM_VALUE_TYPE_WSTRING:
             ProWstringFree (elem_data->val.w);   
             TEST_CALL_REPORT("ProWstringFree", "ProUtilElementValueDataFree",  
			    status, status != PRO_TK_NO_ERROR );                      
            break;
         case  ELEM_VALUE_TYPE_SELECTION:
             status = ProSelectionFree (&(elem_data->val.r));   
             TEST_CALL_REPORT("ProSelectionFree", "ProUtilElementValueDataFree", 
			    status, status != PRO_TK_NO_ERROR );                                   
            break;
         case  ELEM_VALUE_TYPE_COLLECTION:         
             status = ProCollectionFree ( &(elem_data->val.collection));   
             TEST_CALL_REPORT("ProCollectionFree", "ProUtilElementValueDataFree",  
			    status, status != PRO_TK_NO_ERROR );                                  
             break;
         default:
             ProTKPrintf("Not Applicable");
             /* Not applicable for  other types */
     }
     return status;
  }
