#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include "../include/FsmdaHeader.h"
#include "../include/parser.h"
#include "../include/valPropHeader.h"

//The following variable is used to indicate the Parser which FSMD is to be generated
//    FALSE implies FSMD M0
//    TRUE  implies FSMD M1
boolean flagVar_List;

//FSMDs are declared globally so that they can be available to the Parser
FSMD *M0, *M1;
extern void callParser(char* );

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}
void selectionSort(int arr[], int n){
    int i, j, min_idx;

    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++) {

        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;

        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}
void readHighVariables(FSMD* fsmd, char* name){
    FILE* highVariables;
    if(fsmd == NULL) return;
    if((highVariables= fopen(name, "r")) == NULL){
        printf("Can not read high variables from variables.txt\n");
        return;
    }

    int line;
    int i = 0;
    while (fscanf(highVariables, "%d", &line) > 0) {
        if(i < STACK_SIZE - 1 ) {
            fsmd->highVariables[i++] = line;
        }
    }
    fsmd->numHighVariables = i;
    selectionSort(fsmd->highVariables, fsmd->numHighVariables);
    for(i = 0 ; i < fsmd->numHighVariables; i++) {
        printf("q%d ", fsmd->highVariables[i]);
    }
    printf("\n");
    fclose( highVariables );
}
boolean isHighVariable(FSMD* fsmd, int val){
    if(fsmd == NULL) return FALSE;
    int start = 0, end = fsmd->numHighVariables-1;
    while(start <= end){
        int mid = (start+end)/2;
        int midVal = fsmd->highVariables[mid];
        if(midVal == val) return TRUE;
        else if(midVal > val) end = mid-1;
        else start = mid+1;
    }
    return FALSE;
}

void printCutPoints(FSMD* fsmd){
    if(fsmd == NULL) return;
    for(int i = 0 ; i < fsmd->numstates; i++){
        printf("%s - \t", fsmd->states[i].state_id);
        if(fsmd->states[i].node_type == 0) printf("Start State    ");
        else if(fsmd->states[i].node_type == 1) printf("Last State     ");
        else if(cutpoint(fsmd->states[i])) printf("Cut Point State");
        else printf("Normal State   ");
        printf("    No of trans : %d - ", fsmd->states[i].numtrans);
        if(fsmd->states[i].numtrans > 0) printf("%s, ", fsmd->states[fsmd->states[i].translist->outtrans].state_id);
        if(fsmd->states[i].numtrans > 1) printf("%s", fsmd->states[fsmd->states[i].translist->next->outtrans].state_id);
        printf("\n");
    }
}
void printPathCover(FSMD* fsmd) {
    if(fsmd == NULL) return;
    PATHS_LIST pathCover;
    findpaths(fsmd, &pathCover);
    Associate_R_and_r_alpha(&pathCover);                            //computes R_alpha & r_alpha
    printf("{");
    for (int i = 0; i < pathCover.numpaths; i++) {
        printf("{ ");
        PATH_NODE *iter = pathCover.paths[i].nodeslist;
        while (iter) {
            printf("%s ", fsmd->states[iter->state].state_id);
            iter = iter->next;
        }
        printf("} ");
    }
    printf("\n");
}

void printLeakProp(PATH_ST path){
    for(int i = 0 ; i < SYM_TAB_SIZE; i++){
        printf("Leak via Variable : %d", i);
        for(int j = 0 ; j < SYM_TAB_SIZE; j++){
            printf("%d ", path.propagatedLeak[i][j]);
        }
        printf("\n");
    }
}

void checkLeak( FSMD * fsmd, PATH_ST path, int leftVariable, NC *root ) {//WRITE_LISTS <- for finding fast{
    if(fsmd == NULL) return;
    char *sym_value;
    sym_value = (char * ) malloc( 1000*sizeof( char ) );

    if( root != NULL )
    {
        if(root->type == 0) root->type = 'S';
        if( root->type == 'R' || root->type == 'O' ){
            if( root->type == 'R' ) checkLeak( fsmd, path, leftVariable, root->link );
        }
        switch( root->type ){
            case 'f':
            case 'v':
            case 'y':
            case 'a':
                symbol_for_index(root->inc, sym_value);
                break;
            default:
                break;
        }; // switch( root->type )
        if(root->type == 'f' || root->type == 'v' || root->type == 'y' || root->type == 'a'){
            char* variable = sym_value;
            variable++;
            int rightVariable = atoi(variable);
            printf("q%d ", rightVariable);
            if(isHighVariable(fsmd, rightVariable)){
                path.propagatedLeak[leftVariable][rightVariable] |= TRUE;
            }
        }
        if( root->type != 'R' && root->type != 'O' ) checkLeak( fsmd, path, leftVariable, root->link );
        if( root->type == 'S' && root->list != NULL ){
            checkLeak( fsmd, path, leftVariable, root->list );
            return;
        }
        checkLeak( fsmd, path, leftVariable, root->list );
    }
}

void calculateLeakPropagation(FSMD* fsmd) {
    if(fsmd == NULL) return;
    PATHS_LIST pathCover;
    findpaths(fsmd, &pathCover);
    Associate_R_and_r_alpha(&pathCover);                            //computes R_alpha & r_alpha
    for (int i = 0; i < pathCover.numpaths; i++) {
        PATH_ST path = pathCover.paths[i];
        path.propagatedLeak = (int**)malloc(sizeof(int*) * SYM_TAB_SIZE);
        for(int j = 0 ; j < SYM_TAB_SIZE; j++) {
            path.propagatedLeak[j] = (int*) malloc(sizeof(int)*SYM_TAB_SIZE);
            memset(path.propagatedLeak[j], 0, sizeof(path.propagatedLeak));
        }
        printf("\nPath no. : %d\n", i);
        if(path.transformations == NULL) continue;
        r_alpha *trans = path.transformations;
        while (trans){
            DATA_TRANS assignment = trans->Assign;
            if (assignment.rhs == NULL) continue;
            int lhs = assignment.lhs;
            char* leftVariable = (char*) malloc(sizeof(char)*SYM_TAB_SIZE);
            symbol_for_index(lhs, leftVariable);
            printf("%s := ", leftVariable);
            checkLeak(fsmd, path, atoi(++leftVariable), assignment.rhs);
            trans = trans->next;
        }
        printLeakProp(path);
    }
    printf("\n");
}

int main(int argc, char* argv[]){
    if(argc != 5){
        printf("\nInadequate number of parameters provided\nExiting System\n");
        return 0;
    }
    flagVar_List = FALSE;
    outputVar0.no_of_elements = 0;
    callParser(argv[1]);
    readHighVariables(M0, argv[3]);

#ifdef DETAILS
    print_fsmd( M0 );
#endif

    flagVar_List = TRUE;
    outputVar1.no_of_elements = 0;
    callParser(argv[2]);
    readHighVariables(M1, argv[4]);

#ifdef DETAILS
    print_fsmd( M1 );
#endif

#ifdef DEbug
    printf("\n FSMDs read successfully \n");
#endif


#ifdef DETAILS
    print_fsmd( M1 );
#endif

#ifdef DEbug
    printf("\n FSMDs read successfully \n");
#endif

    NO_OF_VARIABLES = stab.numsymbols;

    printf("\nM0 Cycles : \n");
    findAllCutpoints(M0);
    printf("\nM0 Cutpoints : \n");
    printCutPoints(M0);
    printf("\nM0 PathCover : \n");
    printPathCover(M0);
    calculateLeakPropagation(M0);
//    calculateLeakPropagation(M0);

//    PATHS_LIST p0;
//    findpaths(M0,&p0);
//    Associate_R_and_r_alpha(&p0); // computes R_alpha & r_alpha
//    displayallpaths(&p0);
//    getchar();

    printf("\n\nM1 Cycles : \n");
    findAllCutpoints(M1);
    printf("\nM1 Cutpoints : \n");
    printCutPoints(M1);
    printf("\nM1 PathCover : \n");
    printPathCover(M1);

//    print_fsmd(M1);


//    securityChecker(M0, M1);

    return 0;
}