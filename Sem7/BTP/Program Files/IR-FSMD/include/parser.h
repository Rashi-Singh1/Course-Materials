#ifndef PARSER_HEADER
#define PARSER_HEADER

#define DIM 100

typedef struct Assign_list{
    DATA_TRANS action[DIM];
    int numOperations;
}DT_LIST;
typedef struct Substatement{
    NC *condition;
    DT_LIST *assignments;
    char stateName[DIM];
}SUBSTMT;
typedef struct Substatement_list{
    SUBSTMT *substmt[DIM];
    int numSubstmts;
}SUBSTMT_LIST;
typedef struct Statement{
    char stateName[10];
    char numTransitions[5]; //should be int - some problem with parser
    SUBSTMT_LIST *substmts;
}STMT;
typedef struct Statement_list{
    STMT *statements[1000];
    int numStatements;
}STMT_LIST;

//Functions for normalization

//Functions related normalized arithmetic expressions
//Creation of normalized "primary" expressions
NC* createVariable( char* );
NC* createConstant( char* );
NC* createArray( char*, NC* );
NC* createExpression_Mod_Div( NC*, NC*, char );
NC* createFunction( NC*, NC* );

NC* addArrayDimension( NC*, NC* );
NC* addArgument( NC*, NC* );

//Creation of data transformations
DATA_TRANS* createDataTransVar( char*, NC* );
DATA_TRANS* createDataTransArray( NC*, NC* );
DATA_TRANS* createDataTransVarRead( char*, char* );
DATA_TRANS* createDataTransVarOut( char*, NC* );
void markOutputVariables( int );

void generateAssignments( DT_LIST*, TRANSITION_ST* );

//Functions related to normalized conditional expressions
//Creation of normalized conditional expressions
int getRelOperatorIndex( char* );
void createFSMD( char*, STMT_LIST* );

//Functions for performing Boolean operations - NOT, AND, OR
NC* negateExpression( NC* );
NC* computeNegation( NC* );
NC* negateConditionalExpression( NC* );
NC* createConditionalExpression( NC*, char*, NC* );
NC* createAndExpression( NC*, NC* );
NC* createOrExpression( NC*, NC* );
NC* compareRelations( NC*, NC* );


#endif
