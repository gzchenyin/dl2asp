#ifndef _DLSOLVER_H_
#define _DLSOLVER_H_

#include <vector>
#include <set>
#include <map>

#include "tool.h"

///////////////////////////////////////////////////////////
//the class for default logic solver
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//the class for atoms in the default theory
///////////////////////////////////////////////////////////
class AtomSet
{
private:
    dlint num;
    vector<string> stvec;
    map<string, dlint> stmap;

public:
    AtomSet();
    dlint size();
    string get_string ( dlint i );
    dlint get_index ( string s );
    dlint add_atom ( const char * s );
    dlint get_new_atom();

    void dump();
};

///////////////////////////////////////////////////////////
//the class for formulas
///////////////////////////////////////////////////////////
class Formulas
{
private:
    dlint num;
    map<string, dlint> mf;
    vector<string> stvec;
    vector<formula_cnf> cnf;
    vector<formula_cnf> neg_cnf;
  vector<bool> con;
public:
    Formulas();

    dlint size();
    string get_string ( dlint i );
    formula_cnf & get_cnf ( dlint i, bool neg );
    dlint get_index ( string s );

    void set_conclusion(dlint i);
    bool is_conclusion(dlint i);
    
    void dump();
};

///////////////////////////////////////////////////////////
//the class for asp rule
///////////////////////////////////////////////////////////

class Rule
{
public:
    dlint_set head;
    dlint_set pos;
    dlint_set neg;
};

///////////////////////////////////////////////////////////
//the class for default rule
///////////////////////////////////////////////////////////

class DF
{
public:
    string sdl; //the string of the default rule

    //the prerequisite of the default rule
    //be aware that it always refers to a formula which is its negation
    vector<dlint> prerequisite;

    //the justification of the default rule
    vector<dlint> justifications;

    //the conclusion  of the default rule
    vector<dlint>  conclusions;
};

class DLsolver
{
public:
    DLsolver();

    //the set of background theory
    set<dlint> bktheory;
    //the vector of default rules
    vector<DF> defaults;

    //the set of all formulas in prerequistie and the negation of justification
    set<dlint> pre_just;
    //the set of all formulas in conclusions
    set<dlint> con;

    vector<Rule> rule;
    
    void dump();
};

#endif
