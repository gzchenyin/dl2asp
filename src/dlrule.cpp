#include "dlsolver.h"
#include "dlrule.h"
#include "MUSbuilder.h"

extern AtomSet gatom;
extern DLsolver gdl;
extern Formulas gformula;

vector<dlint_set> fmcs;
vector<dlint_set> fmus;

void dl_asp()
{
    for ( vector<DF>::iterator it = gdl.defaults.begin();
            it != gdl.defaults.end(); it++ ) {
        dlint_set h, p, n;
        for ( vector<dlint>::iterator itd = it->prerequisite.begin();
                itd != it->prerequisite.end(); itd++ ) {
            p.insert ( *itd );
        }
        for ( vector<dlint>::iterator itd = it->justifications.begin();
                itd != it->justifications.end(); itd++ ) {
            n.insert ( *itd );
        }
        for ( vector<dlint>::iterator itd = it->conclusions.begin();
                itd != it->conclusions.end(); itd++ ) {
            h.insert ( *itd );
        }

        add_rule ( h, p, n );
    }

    gdl.drule_num = gdl.rule.size();
    
    find_implication_rule();

    gdl.imp_rule_num = gdl.rule.size() - gdl.drule_num;
    
    if ( ( gdl.mt == MT_MAX ) || ( gdl.mt == MT_MIN ) ) {
        find_opt_rule();
    }
}

void find_implication_rule()
{
    //find constraint
    find_mus ( 0 );
    vector<dlint_set> cons;
    for ( vector<dlint_set>::iterator its = fmus.begin();
            its != fmus.end(); its++ ) {
        cons.push_back ( *its );
        add_constraint ( *its );
    }

    //find implication rules for prerequisite and justifications
    for ( dlint_set::iterator it = gdl.pre_just.begin();
            it != gdl.pre_just.end(); it++ ) {
        find_mus ( *it );

        dlint_set h;
        dlint_set n;
        h.insert ( *it );

        for ( vector<dlint_set>::iterator its = fmus.begin();
                its != fmus.end(); its++ ) {

            bool b = true;

            if ( its->find ( *it ) != its->end() ) {
                b = false;
            }

            for ( vector<dlint_set>::iterator itc = cons.begin();
                    b && ( itc != cons.end() ); itc++ )
                if ( *itc == *its ) {
                    b = false;
                }

            if ( b ) {
                add_rule ( h, *its, n );
            }
        }
    }

    //if MAX/MIN, find implication rules for conclusions
    if ( ( gdl.mt == MT_MIN ) || ( gdl.mt == MT_MAX ) ) {
        for ( dlint_set::iterator it = gdl.con.begin();
                it != gdl.con.end(); it++ ) {
            find_mus ( *it );

            dlint_set h;
            dlint_set n;
            h.insert ( *it );

            for ( vector<dlint_set>::iterator its = fmus.begin();
                    its != fmus.end(); its++ ) {

                bool b = true;

                if ( its->find ( *it ) != its->end() ) {
                    b = false;
                }

                for ( vector<dlint_set>::iterator itc = cons.begin();
                        b && ( itc != cons.end() ); itc++ )
                    if ( *itc == *its ) {
                        b = false;
                    }

                if ( b ) {
                    add_rule ( h, *its, n );
                }
            }
        }
    }


}

void add_constraint ( dlint_set p )
{
    Rule r;

    r.pos.insert ( p.begin(), p.end() );

    gdl.rule.push_back ( r );
}

void add_rule ( dlint_set h, dlint_set p, dlint_set n )
{
    Rule r;

    r.head.insert ( h.begin(), h.end() );
    r.pos.insert ( p.begin(), p.end() );
    r.neg.insert ( n.begin(), n.end() );

    gdl.rule.push_back ( r );
}

void find_mus ( dlint f )
{
    //cout << "find mus for " << f << endl;

    PicoSAT *ps = picosat_init();

    //add bktheory
    for ( set<dlint>::iterator it = gdl.bktheory.begin();
            it != gdl.bktheory.end(); it++ ) {
        encode_formula ( ps, *it, false, -1 );
    }

    //add the conclusion of the defaults
    for ( vector<DF>::iterator it = gdl.defaults.begin();
            it != gdl.defaults.end(); it++ ) {
        for ( vector<dlint>::iterator it1 = it->conclusions.begin();
                it1 != it->conclusions.end(); it1++ ) {
            encode_formula ( ps, *it1, false, *it1 + gatom.size() );
        }

    }

    if ( f > 0 ) {
        encode_formula ( ps, f, true, -1 );
    }

    for ( set<dlint>::iterator it = gdl.con.begin();
            it != gdl.con.end(); it++ ) {
        picosat_set_default_phase_lit ( ps, *it  + gatom.size(), 1 );
    }

    for ( set<dlint>::iterator it = gdl.con.begin();
            it != gdl.con.end(); it++ ) {
        picosat_assume ( ps, *it + + gatom.size() );
    }

    fmcs.clear();
    fmus.clear();

    //int res = picosat_sat ( ps, -1 );
    picosat_sat ( ps, -1 );
    const int *m, *p;
    int cid;
    while ( ( m = picosat_next_minimal_correcting_subset_of_assumptions ( ps ) ) ) {
        dlint_set mcs;
        for ( p = m; ( cid = *p ); p++ ) {
            mcs.insert ( *p -  gatom.size() );
        }
        fmcs.push_back ( mcs );
    }

    mcs_to_mus();

    picosat_reset ( ps );
}

void encode_formula ( PicoSAT *ps, dlint f, bool neg, dlint g )
{
    formula_cnf c;
    if ( neg ) {
        c = gformula.get_formula ( f ).neg_cnf;
    } else {
        c = gformula.get_formula ( f ).cnf;
    }

    if ( c.empty() ) {
        cout << "empty cnf" << endl;
    }

    for ( vector<clause_cnf>::iterator itc = c.begin();
            itc != c.end(); itc++ ) {
        if ( g > 0 ) {
            picosat_add ( ps, -g );
        }
        for ( vector<dlint>::iterator itd = itc->begin();
                itd != itc->end(); itd ++ ) {
            picosat_add ( ps, *itd );

        }
        picosat_add ( ps, 0 );
    }
}

void mcs_to_mus()
{
    list<Cover> MUSCovers;
    vector<Num> singletons;
    for ( vector<dlint_set>::iterator itc = fmcs.begin();
            itc != fmcs.end() ; itc++ ) {
        Cover newCover;
        Num clause;
        int iflag = 0;
        for ( dlint_set::iterator itv = itc->begin();
                itv!= itc->end(); itv++ ) {
            clause = *itv;
            newCover.insert ( clause );
            iflag++;
        }
        if ( iflag == 1 ) {
            singletons.push_back ( clause );
        } else {
            MUSCovers.push_back ( newCover );
        }
    }

    MUSbuilder builder;
    builder.addSingletons ( singletons );

    ClauseMap clauseMapping;
    ClauseMap clauseMappingRev;

    MUSbuilder::generateMappingSorted ( MUSCovers, clauseMapping, clauseMappingRev );
    //MUSbuilder::generateMappingStraight(MUSCovers, clauseMapping, clauseMappingRev);

    builder.addClauseMapping ( clauseMappingRev );

    MUSbuilder::translateClauses ( MUSCovers, clauseMapping );

    // setup the initial assignment
    ClauseAssign clauseAssignment;
    clauseAssignment.resize ( clauseMapping.size(), 0 );
    // Main function
    builder.constructMUS ( MUSCovers, clauseAssignment );

}

void find_opt_rule()
{
    OptRule opt_r;

    opt_r.level = 0;
    list<OptRule>::iterator it = gdl.opt_rule.begin();
    gdl.opt_rule.insert ( it, opt_r );

    dlint sum_max = 1; //the sum of the weight for MT_MAX
    if ( gdl.mt == MT_MAX ) {
        for ( set<dlint>::iterator cit = gdl.con.begin();
                cit != gdl.con.end(); cit++ ) {
            dlint w = gformula.get_formula ( *cit ).weight;
            if ( w >= 0 ) {
                gdl.opt_atoms.insert ( *cit + gformula.size() ); // new atom for sum
                sum_max += w;
                Rule r;
                r.head.insert ( *cit + gformula.size() );
                r.neg.insert ( *cit );
                gdl.rule.push_back ( r );
            }
        }
    }

    for ( set<dlint>::iterator cit = gdl.con.begin();
            cit != gdl.con.end(); cit++ ) {
        dlint l = gformula.get_formula ( *cit ).level;
        dlint w = gformula.get_formula ( *cit ).weight;

        if ( w >= 0 ) {
            bool found = false;
            bool next = true;
            it = gdl.opt_rule.begin();

            while ( next && ( !found ) && ( it != gdl.opt_rule.end() ) ) {
                if ( it->level == l ) {
                    found = true;
                    next = false;
                } else {
                    if ( it->level > l ) {
                        next = false;
                    } else {
                        it++;
                    }
                }
            }

            pair<dlint, dlint> p;
            if ( gdl.mt == MT_MAX ) {
                p.first = *cit + gformula.size();
                //p.second = sum_max - w;
                p.second = w;
            } else {
                p.first = *cit;
                p.second = w;
            }


            if ( found ) {
                it->watom.push_back ( p );
            } else {
                OptRule pr;
                pr.level = l;
                pr.watom.push_back ( p );
                gdl.opt_rule.insert ( it, pr );
            }
        }
    }
}
