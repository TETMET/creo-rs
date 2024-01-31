#ifndef TEST_SETUP
#define TEST_SETUP


/*--------------------------------------------------------------------*\
Menu functions
\*--------------------------------------------------------------------*/
int ProTestSetupMnu(ProMdl *model);
int ProTestNoteMnu(ProMdl model);
int ProTestSetupNameMnu(ProMdl model);
int ProTestNameMnu(ProMdl model, int item_type);


/*--------------------------------------------------------------------*\
Test functions
\*--------------------------------------------------------------------*/
int ProTestNote(ProMdl model, int action);
int ProTestSetupName(ProMdl model, int item_type, int action);
int ProTestListAllNames(ProMdl model);
int ProTestListNames(ProMdl model, FILE *fp);

/*--------------------------------------------------------------------*\
Action functions
\*--------------------------------------------------------------------*/
ProError ProTestNotecollAction(ProModelitem *note, ProError filt_status,
                               ProAppData app_data);
ProError ProTestListNameAction(ProFeature *p_feat, ProError stat,
				ProAppData app_data);
ProError ProTestListGeomAction(ProModelitem *p_item, ProError stat,
				ProAppData app_data);

/*--------------------------------------------------------------------*\
Helper functions
\*--------------------------------------------------------------------*/
ProError ProTestNoteCollect(ProMdl model, ProModelitem **notes);

#endif
