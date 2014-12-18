#ifndef _DLSOLVER_H_
#define _DLSOLVER_H_

#include <vector>
#include <set>
#include <map>
#include <list>
#include <utility>

#include "tool.h"

///////////////////////////////////////////////////////////
//the class for default logic solver
///////////////////////////////////////////////////////////

enum ModelType {
    MT_SINGLE = 0,
    MT_ALL = 1,
    MT_MIN = 2,
    MT_MAX = 3
};

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
class Formula
{
public:
  Formula();
  string st;
  formula_cnf cnf;
  formula_cnf neg_cnf;
  dlint weight;
  dlint level;
};

class Formulas
{
private:
    dlint num;
    map<string, dlint> mf;
    vector<Formula> vf;
public:
    Formulas();

    dlint size();
    Formula & get_formula(dlint i);
    dlint get_index(string s);
    
    //string get_string ( dlint i );
    //formula_cnf & get_cnf ( dlint i, bool neg );
    //dlint get_index ( string s );
    //void set_weight ( dlint i, dlint w );
    //dlint get_weight ( dlint i );
    //void set_level ( dlint i, dlint w );
    //dlint get_level ( dlint i );

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

class OptRule
{
public:
  dlint level;
  vector<pair<dlint, dlint> > watom;
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

    string infile;
    ModelType mt;
    vector<dlint> last_model;
    
    set<dlint> opt_atoms;
    list<OptRule> opt_rule;

    void dump();
};

#endif
