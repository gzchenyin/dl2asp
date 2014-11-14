
#include "aspsolver.h"
#include "dlsolver.h"

extern Formulas gformula;
extern AtomSet gatom;
extern DLsolver gdl;

void printModel ( const Clasp::SymbolTable& symTab, const Clasp::Model& model )
{
    std::cout << "Model " << model.num << ": \n";
    // Print each named atom that is true w.r.t the current model.
    for ( Clasp::SymbolTable::const_iterator it = symTab.begin(); it != symTab.end(); ++it ) {
        //if ( model.isTrue ( it->second.lit ) && !it->second.name.empty() ) {
        if ( model.isTrue ( it->second.lit ) ) {

            if (gformula.is_conclusion(it->first))
            std::cout << "(" << it->first << ") " << it->second.name.c_str() << endl;
        }
    }
    std::cout << std::endl;
}

void solve_asp ( bool a )
{
    // LogicProgram provides the interface for
    // defining logic programs.
    // It also preprocesses the program and converts it
    // to the internal solver format.
    // See logic_program.h for details.
    Clasp::Asp::LogicProgram lp;

    // Among other things, SharedContext maintains a Solver object
    // which hosts the data and functions for CDNL answer set solving.
    // SharedContext also contains the symbol table which stores the
    // mapping between atoms of the logic program and the
    // propositional literals in the solver.
    // See shared_context.h for details.
    Clasp::SharedContext ctx;

    // startProgram must be called once before we can add atoms/rules
    lp.startProgram ( ctx );

    // Populate symbol table. Each atoms must have a unique id, the name is optional.
    // The symbol table then maps the ids to the propositional
    // literals in the solver.
    for ( unsigned i = 1; i <= gformula.size(); i++ ) {
        lp.setAtomName ( i, gformula.get_string ( i ).c_str() );
    }
    // Define the rules of the program.

    for ( vector<Rule>::iterator it = gdl.rule.begin();
            it != gdl.rule.end(); it++ ) {

        lp.startRule ( Clasp::Asp::BASICRULE );
        if ( it->head.size() == 0 ) {
            lp.addHead ( 1 );
        } else {
            for ( dlint_set::iterator hit = it->head.begin();
                    hit != it->head.end(); hit++ ) {
                lp.addHead ( ( *hit ) );
            }
        }
        for ( dlint_set::iterator its = it->pos.begin();
                its != it->pos.end(); its++ ) {
            lp.addToBody ( ( *its ), true );
        }
        for ( dlint_set::iterator its = it->neg.begin();
                its != it->neg.end(); its++ ) {
            lp.addToBody ( ( *its ), false );
        }
        lp.endRule();

    }
    lp.setCompute ( 1, 0 );

    lp.endProgram();


    //lp.write ( cout );

    // Since we want to compute more than one
    // answer set, we need an enumerator.
    // See enumerator.h for details
    Clasp::ModelEnumerator enumerator;
    enumerator.init ( ctx, 0 );

    // We are done with problem setup.
    // Prepare for solving.
    ctx.endInit();
    // BasicSolve implements a basic search for a model.
    // It handles the various strategies like restarts, deletion, etc.
    Clasp::BasicSolve solve ( *ctx.master() );
    // Prepare the solver for enumeration.
    enumerator.start ( solve.solver() );
    while ( solve.solve() == Clasp::value_true ) {
        // Make the enumerator aware of the new model and
        // let it compute a new constraint and/or backtracking level.
        if ( enumerator.commitModel ( solve.solver() ) ) {
            printModel ( ctx.symbolTable(), enumerator.lastModel() );
        }
        // Integrate the model into the search and thereby prepare
        // the solver for the search for the next model.
        enumerator.update ( solve.solver() );
    }
    std::cout << "No more models!" << std::endl;
}
