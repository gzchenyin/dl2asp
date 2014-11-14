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
    find_implication_rule();
}

void find_implication_rule()
{
    find_mus ( 0 ); //find constraint
    vector<dlint_set> cons;
    for ( vector<dlint_set>::iterator its = fmus.begin();
            its != fmus.end(); its++ ) {
        cons.push_back ( *its );
        add_constraint ( *its );
    }

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

    int res = picosat_sat ( ps, -1 );
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
    formula_cnf c = gformula.get_cnf ( f, neg );

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

