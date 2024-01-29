#include <iostream>
#include <ProToolkit.h>
#include <ProCore.h>
#include <ProTKRunTime.h>

using namespace std;

/*===========================================================================*\
  Function : main
  Purpose  : Test the ProToolkitMain() function. main is optional function.
\*===========================================================================*/
int main(int argc, char **argv)
{
    cerr << endl << "\tWelcome to Pro/TOOLKIT  -  The \"pt_install_test\" program" << endl;

    ProToolkitMain(argc, argv);

    return(0);
}

extern "C" int user_initialize(
    int argc,			
    char *argv[],
    char *version, 
    char *build,
    wchar_t errbuf[80])
{
    return 0;   
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{
/*---------------------------------------------------------------------*\
   Loging file close. 
\*---------------------------------------------------------------------*/
    cout << "user_terminate" << endl;
}
