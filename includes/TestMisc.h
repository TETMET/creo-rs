
#ifndef TEST_MISC_H
#define TEST_MISC_H

/*--------------------------------------------------------------------*\
Pro/Toolkit includes
\*--------------------------------------------------------------------*/
#include <ProToolkit.h>
#include <ProMdl.h>
#include <ProModelitem.h>
#include <ProGeomitem.h>

/*--------------------------------------------------------------------*\
Application data types
\*--------------------------------------------------------------------*/
typedef struct {
    ProMdl *p_model;
    FILE   *fp;
} MafpInfo;

int ProTestMisc(ProSolid*);

int ProTestFuncInit(MafpInfo*);

ProError ProTestVisibilityFilt(ProModelitem*, ProAppData);

ProError ProTestFeatAct ( ProFeature*, ProError, ProAppData);

ProError ProTestGeomitemVisAct ( ProGeomitem*, ProError, FILE*);

ProError ProTestQuiltVisAct ( ProGeomitem*, ProError, FILE*);

ProError ProTestNameGen(ProType, wchar_t*);

int      ProTestMiscFpClose (FILE*);

int ProTestMdlNameGet(FILE *);

int ProTestMdlDelete(FILE *);

ProError ProTestMultiCADFile();

ProError ProMultiCADFileRetrieve ();

ProError ProMultiCADFileInfo (ProMdl mdl);

#endif
