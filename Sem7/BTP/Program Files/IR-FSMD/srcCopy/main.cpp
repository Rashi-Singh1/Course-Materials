#include <iostream>
#include <bits/stdc++.h>
#include <sys/resource.h>
#include "../include/FsmdaHeader.h"
#include "../include/parser.h"
#include "../include/valPropHeader.h"

using namespace std;

extern void callParser( char* );


//The following variable is used to indicate the Parser which FSMD is to be generated
//    FALSE implies FSMD M0
//    TRUE  implies FSMD M1
boolean flagVar_List;

//FSMDs are declared globally so that they can be available to the Parser
FSMD *M0, *M1;

//Main function
int main(int argc, char** argv){

    if(argc != 3) {
        cout << "Inadequate number of parameters" << endl << "Ending System" <<endl;
        return 0;
    }

    flagVar_List = FALSE;
    outputVar0.no_of_elements = 0;
    callParser(argv[1]);

    #ifdef DETAILS
        print_fsmd( M0 );
    #endif

    flagVar_List = TRUE;
    outputVar1.no_of_elements = 0;
    callParser(argv[2]);

    #ifdef DETAILS
        print_fsmd( M1 );
    #endif

    #ifdef DEbug
        printf("\n FSMDs read successfully \n");
    #endif

    NO_OF_VARIABLES = stab.numsymbols;
    containmentChecker(M0, M1); //V0 and V1 are globally defined

    return 0;
}