
#include <sstream>
#include "tool.h"
#include "dlsolver.h"


AtomSet gatom;
Formulas gformula;
DLsolver gdl;

///////////////////////////////////////////////////////////
//the implementation for atoms in the default theory
///////////////////////////////////////////////////////////
AtomSet::AtomSet()
{
    stvec.push_back ( "_empty_atom_" );
    stmap.insert ( pair<string, dlint> ( "_empty_atom_", 0 ) );
    num = 0;
}

dlint AtomSet::size()
{
    return num;
}

string AtomSet::get_string ( dlint i )
{
    return stvec[i];
}

dlint AtomSet::get_index ( string s )
{
    return stmap[s];
}

dlint AtomSet::add_atom ( const char * s )
{
    map<string, dlint>::iterator it;
    it = stmap.find ( s );
    if ( it == stmap.end() ) {
        stmap.insert ( pair<string, dlint> ( s, ++num ) );
        stvec.push_back ( s );
        return num;
    } else {
        return it->second;
    }
}

dlint AtomSet::get_new_atom()
{
    ostringstream os;
    os << ++num;
    string s = "_n_" + os.str();
    stmap.insert ( pair<string, dlint> ( s, num ) );
    stvec.push_back ( s );
    return num;
}

void AtomSet::dump()
{
    cout << "Atoms (" << num << ")" << endl;

    for ( dlint i = 1; i <= num; i++ ) {
        cout << i<< "* " << get_index ( stvec[i] ) << " : " << get_string ( i ) << endl;
    }
}

///////////////////////////////////////////////////////////
//the implementation for formulas
///////////////////////////////////////////////////////////

Formulas::Formulas()
{
    formula_cnf f;
    mf.insert ( pair<string, dlint> ( "_empty_formula_", 0 ) );
    stvec.push_back ( "_empty_formula_" );
    cnf.push_back ( f );
    neg_cnf.push_back ( f );
    con.push_back(false);
    num = 0;

    mf.insert ( pair<string, dlint> ( "_false", 1 ) );
    stvec.push_back ( "_false" );
    cnf.push_back ( f );
    neg_cnf.push_back ( f );
    con.push_back(false);
    num = 1;
}

dlint Formulas::size()
{
    return num;
}

string Formulas::get_string ( dlint i )
{
    return stvec[i];
}

formula_cnf & Formulas::get_cnf ( dlint i, bool neg )
{
    if ( !neg ) {
        return ( cnf[i] );
    } else {
        return ( neg_cnf[i] );
    }
}

bool Formulas::is_conclusion ( dlint i )
{
    return con[i];
}

void Formulas::set_conclusion ( dlint i )
{
    con[i] = true;
}

dlint Formulas::get_index ( string s )
{
    dlint i;
    map<string, dlint>::iterator it;
    it = mf.find ( s );

    if ( it != mf.end() ) {
        i = it->second;
    } else {
        formula_cnf f;
        mf.insert ( pair<string, dlint> ( s, ++num ) );
        stvec.push_back ( s );
        cnf.push_back ( f );
        neg_cnf.push_back ( f );
        i = num;
    }

    return i;
}

void Formulas::dump()
{
    cout << "Formulas (" << num << ")" << endl;
    for ( dlint i = 1; i <= num; i++ ) {
        cout << i<< "* " << get_index ( stvec[i] ) << " " << stvec[i] << endl;
        cout << "  cnf: ";
        for ( vector<clause_cnf>::iterator it = cnf[i].begin(); it < cnf[i].end(); it ++ ) {
            for ( clause_cnf::iterator itc = it->begin(); itc < it->end(); itc++ ) {
                cout << ( *itc ) << " ";
            }
            cout << ", ";
        }
        cout << endl;
        cout << "  neg_cnf: ";
        for ( vector<clause_cnf>::iterator it = neg_cnf[i].begin(); it < neg_cnf[i].end(); it ++ ) {
            for ( clause_cnf::iterator itc = it->begin(); itc < it->end(); itc++ ) {
                cout << ( *itc ) << " ";
            }
            cout << ", ";
        }
        cout << endl;
    }

}
///////////////////////////////////////////////////////////
//the implementation for solver
///////////////////////////////////////////////////////////
DLsolver::DLsolver()
{
}

void DLsolver::dump()
{
    cout << "Background (" << bktheory.size() << ")" << endl;
    for ( set<dlint>::iterator it = bktheory.begin();
            it != bktheory.end(); it++ ) {
        cout << *it << " ";
    }
    cout << endl;

    cout << "Defaults (" << defaults.size() << ")" << endl;
    for ( dlint i = 0; i < defaults.size(); i++ ) {
        DF d = defaults[i];
        cout << i << " " << d.sdl << endl;
        cout << "  (";
        for ( vector<dlint>::iterator it = d.prerequisite.begin();
                it != d.prerequisite.end(); it ++ ) {
            cout << *it << " ";
        }
        cout << ") : ";
        cout << "(";

        for ( vector<dlint>::iterator it = d.justifications.begin();
                it != d.justifications.end(); it ++ ) {
            cout << *it << " ";
        }
        cout << ") / ";
        cout << "(";

        for ( vector<dlint>::iterator it = d.conclusions.begin();
                it != d.conclusions.end(); it ++ ) {
            cout << *it << " ";
        }
        cout << ")" << endl;
    }

    cout << "pre and neg of just: ";
    for ( set<dlint>::iterator it = pre_just.begin();
            it != pre_just.end(); it++ ) {
        cout << *it << ", ";
    }
    cout << endl;
    cout << "con: ";
    for ( set<dlint>::iterator it = con.begin();
            it != con.end(); it++ ) {
        cout << *it << ", ";
    }
    cout << endl;

    cout << "asp rules: (" << rule.size() << ")" << endl;
    for ( vector<Rule>::iterator it = rule.begin();
            it != rule.end(); it++ ) {
        cout << "(";
        for ( dlint_set::iterator its = it->head.begin();
                its != it->head.end(); its++ ) {
            cout << *its << " ";
        }
        cout << ") :- ";

        cout << "(";
        for ( dlint_set::iterator its = it->pos.begin();
                its != it->pos.end(); its++ ) {
            cout << *its << " ";
        }
        cout << ") ";
        cout << "not(";
        for ( dlint_set::iterator its = it->neg.begin();
                its != it->neg.end(); its++ ) {
            cout << *its << " ";
        }
        cout << ")" << endl;
    }
}
