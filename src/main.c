//////////////////////////////////////////////////
// { VYSION CE } { 0.3.0 }                      //
// Author: epsilon5                             //
// License: DON'T TOUCH!!!!                     //
// Description: a cool shell for CE calculators //
//////////////////////////////////////////////////

#include "main.h"
//#include <capnhook.h>

/* Put all your code here */
void main(void) {
    //hook_start();
    vysion_InitializeSettings();
    vysion_InitializeGraphics();
    vysion_InitializeShell();
    vysion_Desktop();
    vysion_End();
}


