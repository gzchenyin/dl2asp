
#include "aspsolver.h"
#include "dlsolver.h"

extern Formulas gformula;
extern AtomSet gatom;
extern DLsolver gdl;

class ModelHandler : public Clasp::EventHandler
{
public:
    ModelHandler() {}
    bool onModel ( const Clasp::Solver& s, const Clasp::Model& m ) {
        printModel ( s.symbolTable(), m );
        // exclude this model
        Clasp::LitVec clause;
        for ( uint32 i = 1; i <= s.decisionLevel(); ++i ) {
            clause.push_back ( ~s.decision ( i ) );
        }
        return m.ctx->commitClause ( clause );
    }
};

void printModel ( const Clasp::SymbolTable& symTab, const Clasp::Model& model )
{
    if ( ( gdl.mt == MT_NONE ) || ( model.opt == 1 ) ) {
        cout << "Model " << model.num;
        if ( gdl.mt != MT_NONE ) {
            /*cout << "[";
            for ( long i = 0; i < model.costs->size(); i++ ) {
                if ( i > 0 )
                    cout << ",";
                cout << ( * ( model.costs ) ) [i];
            }
            cout << "]";*/
            cout << "[costs:";
            for ( list<OptRule>::iterator lt = gdl.opt_rule.begin();
                    lt != gdl.opt_rule.end(); lt++ ) {
                long costs = 0;
                for ( vector<pair<dlint, dlint> >::iterator vt = lt->watom.begin();
                        vt != lt->watom.end(); vt++ ) {
                    if ( ( model.isTrue ( symTab[vt->first].lit ) ) && ( gdl.mt == MT_MIN ) ) {
                        costs = costs + vt->second;
                    }
                    if ( ( !model.isTrue ( symTab[vt->first].lit ) ) && ( gdl.mt == MT_MAX ) ) {
                        costs = costs + vt->second;
                    }

                }
                cout << " " << costs;
            }
            cout << "]";
        }
        cout << ": " << endl;
        for ( Clasp::SymbolTable::const_iterator it = symTab.begin(); it != symTab.end(); ++it ) {

            if ( model.isTrue ( it->second.lit ) ) {
                if ( gdl.con.find ( it->first ) != gdl.con.end() ) {
                    //if ( gdl.mt == MT_NONE ) {
                    std::cout << "(" << it->first << ") " << it->second.name.c_str() << endl;
                    //} else {
                    //    gdl.last_model.push_back ( it->first );
                    //}
                }
            }
        }
        //cout << "costs: " << model.opt << endl;
        cout << endl;
    }
}

void solve_asp ( bool a )
{
    // See example2()
    Clasp::ClaspConfig config;
    config.solve.numModels = gdl.mn;
    config.solve.optMode = Clasp::MinimizeMode_t::enumOpt;

    // The "interface" to the clasp library.
    Clasp::ClaspFacade libclasp;
    Clasp::Asp::LogicProgram& lp = libclasp.startAsp ( config );

    // define the atoms of the program
    for ( unsigned i= 1; i <= gformula.size(); i++ ) {
        lp.setAtomName ( i, gformula.get_formula ( i ).st.c_str() );
    }
    for ( set<dlint>::iterator it = gdl.opt_atoms.begin();
            it != gdl.opt_atoms.end(); it++ ) {
        string s = gformula.get_formula ( *it - gformula.size() ).st;
        s = "_MAX_" + s;
        lp.setAtomName ( *it, s.c_str() );
    }

    // define the rules of the program.
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

    //add max/min optimize if any
    if ( ( gdl.mt == MT_MIN ) || ( gdl.mt == MT_MAX ) ) {
        for ( list<OptRule>::iterator it = gdl.opt_rule.begin();
                it != gdl.opt_rule.end(); it++ ) {
            if ( !it->watom.empty() ) {
                lp.startRule ( Clasp::Asp::OPTIMIZERULE );
                for ( vector<pair<dlint, dlint> >::iterator itp = it->watom.begin();
                        itp != it->watom.end(); itp++ ) {
                    lp.addToBody ( itp->first, true, itp->second );
                }
                lp.endRule();
            }
        }
    }
    lp.setCompute ( 1, 0 );

    libclasp.prepare();

    ModelHandler handler;
    libclasp.solve ( &handler );
    std::cout << "No more models!" << std::endl;
}
/*
void printModel ( const Clasp::SymbolTable& symTab, const Clasp::Model& model )
{
    if ( gdl.mt == MT_NONE ) {
        std::cout << "Model " << model.num << ": \n";
    } else {
        gdl.last_model.clear();
    }

    for ( Clasp::SymbolTable::const_iterator it = symTab.begin(); it != symTab.end(); ++it ) {

        if ( model.isTrue ( it->second.lit ) ) {
            if ( gdl.con.find ( it->first ) != gdl.con.end() ) {
                //if ( gdl.mt == MT_NONE ) {
                    std::cout << "(" << it->first << ") " << it->second.name.c_str() << endl;
                //} else {
                    gdl.last_model.push_back ( it->first );
                //}
            }
        }
    }
    std::cout << std::endl;
}

void solve_asp ( bool a )
{
    Clasp::Asp::LogicProgram lp;
    Clasp::SharedContext ctx;

    lp.startProgram ( ctx );

    // define the atoms of the program
    for ( unsigned i= 1; i <= gformula.size(); i++ ) {
        lp.setAtomName ( i, gformula.get_formula ( i ).st.c_str() );
    }
    for ( set<dlint>::iterator it = gdl.opt_atoms.begin();
            it != gdl.opt_atoms.end(); it++ ) {
        string s = gformula.get_formula ( *it - gformula.size() ).st;
        s = "_MAX_" + s;
        lp.setAtomName ( *it, s.c_str() );
    }

    // define the rules of the program.
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

    //add max/min optimize if any
    if ( ( gdl.mt == MT_MIN ) || ( gdl.mt == MT_MAX ) ) {
        for ( list<OptRule>::iterator it = gdl.opt_rule.begin();
                it != gdl.opt_rule.end(); it++ ) {
            if ( !it->watom.empty() ) {
                lp.startRule ( Clasp::Asp::OPTIMIZERULE );
                for ( vector<pair<dlint, dlint> >::iterator itp = it->watom.begin();
                        itp != it->watom.end(); itp++ ) {
                    lp.addToBody ( itp->first, true, itp->second );
                }
                lp.endRule();
            }
        }
    }

    lp.setCompute ( 1, 0 );
    lp.endProgram();
    //lp.write ( cout );

    Clasp::ModelEnumerator enumerator;
    if ( gdl.mt == MT_NONE ) {
        enumerator.init ( ctx, 0 );
    } else {
        enumerator.init ( ctx, lp.getMinimizeConstraint()->share(), 0);
    }

    int m = 0;
    if ( ctx.endInit() ) {
        Clasp::BasicSolve solve ( *ctx.master() );
        enumerator.start ( solve.solver() );
        while ( ( ( m == 0 ) || ( gdl.mn != MN_SINGLE )|| ( gdl.mt != MT_NONE ) )
          && ( solve.solve() == Clasp::value_true ) ) {
            if ( enumerator.commitModel ( solve.solver() ) ) {
                printModel ( ctx.symbolTable(), enumerator.lastModel() );
                m++;
            }
            enumerator.update ( solve.solver() );
        }
    }

    if ( m == 0 ) {
        std::cout << "No model!" << std::endl;
    } else if ( gdl.mt != MT_NONE ) {
        std::cout << "Model: \n";
        for ( vector<dlint>::iterator it = gdl.last_model.begin();
                it != gdl.last_model.end(); it++ ) {
            std::cout << "(" << *it << ") " << gformula.get_formula ( *it ).st << endl;
        }
    }
}
*/
