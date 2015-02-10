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
    
    handle_param(argc, argv);
    
    yyin = fopen ( gdl.infile.c_str(), "r" );
    if ( !yyin ) {
        r = false;
        cout << "cannot open input file : " << gdl.infile << endl;
    }
    if ( ! ( r && ( yyparse() == 0 ) ) ) {
        r = false;
        cout << "parse file error : " << gdl.infile << endl;
    }

    //gatom.dump();
    //gformula.dump();

    dl_asp();
    
    double trans_time = CPUTIME - cpu_time;
    //gdl.dump();
    
    solve_asp ( true );

    //gdl.dump();

    cout << endl;
    cout << "time: " << CPUTIME - cpu_time << "s(" << trans_time << "s)" << endl;
    cout << "defaults: " << gdl.defaults.size() << endl;
    cout << "asp rules(implication rules): " << gdl.rule.size() 
      << "(" << gdl.imp_rule_num << ")" << endl; 
    return r;
}

