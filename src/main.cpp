#include "tool.h"
#include "dlsolver.h"
#include "dlrule.h"
#include "aspsolver.h"

extern int yyparse();
extern FILE *yyin;
extern Formulas gformula;
extern AtomSet gatom;
extern DLsolver gdl;

int main ( int argc, char** argv )
{
    double cpu_time = CPUTIME;

    bool r = true;
    yyin = fopen ( argv[1], "r" );
    if ( !yyin ) {
        r = false;
        cout << "cannot open input file : " << argv[1] << endl;
    }
    if ( ! ( r && ( yyparse() == 0 ) ) ) {
        r = false;
        cout << "parse file error : " << argv[1] << endl;
    }

    //gatom.dump();
    //gformula.dump();

    dl_asp();
    solve_asp ( true );

    //gdl.dump();

    cout << "total: " << CPUTIME - cpu_time << "s" << endl;
    return r;
}

