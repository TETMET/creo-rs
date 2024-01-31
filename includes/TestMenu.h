


/*----------------------------------------------------------------------------*\
Application macros
\*----------------------------------------------------------------------------*/

#define TRUE 1
#define FALSE 0

#define PRO_TEST_MENU_DISPLAY		101
#define PRO_TEST_MENU_HIDE		102
#define PRO_TEST_MENU_STRINGS		103
#define PRO_TEST_MENU_ACTIVATE		104
#define PRO_TEST_MENU_DEACTIVATE	105
#define PRO_TEST_MENU_DONE		106

#define PRO_TEST_NUM_STRINGS		5

/*----------------------------------------------------------------------------*\
Application global/external data
\*----------------------------------------------------------------------------*/

/*============================================================================*\
  Function : ProTestMenus
  Purpose  : Create a menu to test Menu functions
\*============================================================================*/
int ProTestMenus(void *a, int b);

/*============================================================================*\
  Function : ProTestMenuAction
  Purpose  : Available actions for menu picks
\*============================================================================*/
int ProTestMenuAction(FILE *fp, int choice);

/*============================================================================*\
  Function : ProTestMenuPostMode
  Purpose  : Function used for testing ProMenubuttonPostactionSet()
\*============================================================================*/
int ProTestMenuPostmode(FILE *fp, int b);

/*============================================================================*\
  Function : ProTestMenuPreMode
  Purpose  : Function used for testing ProMenubuttonPreactionSet()
\*============================================================================*/
int ProTestMenuPremode(FILE *fp, int b);

/*=============================================================*\
 Function:	ProTestMessage
 Purpose:	Function used for testing ProMessage... functions
\*=============================================================*/
int ProTestMessage(void *a, int b);
