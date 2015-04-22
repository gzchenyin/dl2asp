#include <stdio.h>
#include <string>
#include "tool.h"
#include "dlparser.h"
#include "dlsolver.h"

extern FILE *yyin;
extern int yyparse();
extern AtomSet gatom;
extern Formulas gformula;
extern DLsolver gdl;

ParseNode * gtree;

ParseNode::ParseNode ( ParseType t )
{
    type = t;
    ival = 0;
    iweight = 0;
    ilevel = 0;
    sval = "";
    next = NULL;
    for ( int i = 0; i < MAX_PARSE_CHILD; i++ )
    {
        child[i] = NULL;
    }
};
/*
ParseNode::ParseNode ( ParseType t, int i )
{
    type = t;
    ival = i;
    ival1 = 0;
    sval = "";
    next = NULL;
    for ( int i = 0; i < MAX_PARSE_CHILD; i++ ) {
        child[i] = NULL;
    }
};
*/
ParseNode::~ParseNode()
{
    if ( next )
    {
        delete next;
        next = NULL;
    }

    for ( int i = 0; i < MAX_PARSE_CHILD; i++ )
    {
        if ( child[i] )
        {
            delete child[i];
            child[i] = NULL;
        }
    }
}

void ParseNode::add_bktheory()
{
    dlint i;
    clause_cnf c;
    i = gformula.get_index ( this->sval );
    formula_cnf &cnf = gformula.get_formula ( i ).cnf;
    if ( cnf.empty() )
    {
        formula_to_cnf ( this, cnf, false, c );
    }

    gdl.bktheory.insert ( i );
}

void ParseNode::add_default()
{
    if ( type != PT_DEFAULT )
    {
        return;
    }

    DF d;
    d.sdl = sval;
    ParseNode *p;
    dlint i;
    clause_cnf c;

    p = child[0];
    while ( p )
    {
        c.clear();
        i = gformula.get_index ( p->child[0]->sval );
        formula_cnf &cnf = gformula.get_formula ( i ).neg_cnf;
        if ( cnf.empty() )
        {
            formula_to_cnf ( p->child[0], cnf, true, c );
        }
        d.prerequisite.push_back ( i );
        gdl.pre_just.insert ( i );
        p = p->next;
    }
    p = child[1];
    while ( p )
    {
        c.clear();
        string s = p->child[0]->sval;
        if ( s.at ( 0 ) == '~' )
        {
            s = s.substr ( 1, s.size() );
        }
        else
        {
            s = "~" + s;
        }
        i = gformula.get_index ( s );
        formula_cnf &cnf = gformula.get_formula ( i ).neg_cnf;
        if ( cnf.empty() )
        {
            formula_to_cnf ( p->child[0], cnf, false, c );
        }
        d.justifications.push_back ( i );
        gdl.pre_just.insert ( i );
        p = p->next;
    }
    p = child[2];
    while ( p )
    {
        c.clear();
        i = gformula.get_index ( p->child[0]->sval );
        formula_cnf &cnf = gformula.get_formula ( i ).cnf;
        if ( cnf.empty() )
        {
            formula_to_cnf ( p->child[0], cnf, false, c );
        }
        formula_cnf &neg_cnf = gformula.get_formula ( i ).neg_cnf;
        if ( neg_cnf.empty() )
        {
            formula_to_cnf ( p->child[0], neg_cnf, true, c );
        }
        gformula.get_formula ( i ).weight = p->child[0]->iweight;
        gformula.get_formula ( i ).level = p->child[0]->ilevel;
        d.conclusions.push_back ( i );
        gdl.con.insert ( i );
        p = p->next;
    }

    if ( child[0] || child[1] || child[2] )
    {
        gdl.defaults.push_back ( d );
    }
}

void ParseNode::formula_to_cnf ( ParseNode * pn, formula_cnf &f, bool neg, clause_cnf c )
{
    if ( !pn )
    {
        cout << "!!error!!, never get here." << endl;
        return;
    }

    if ( pn->type !=PT_FORMULA )
    {
        cout << "!!error!!, never get here." << endl;
        return;
    }

    //if it is an id
    if ( ( pn->type == PT_FORMULA ) && ( pn->ival >= 0 ) )
    {
        //a leaf
        if ( neg )
        {
            c.push_back ( pn->ival* ( -1 ) );    //negative atom
        }
        else
        {
            c.push_back ( pn->ival );    //positive atom
        }
        f.push_back ( c );
        return;
    }

    //so it is a formula

    //if it is a negation operator
    if ( pn->ival == FOR_NEGATION )
    {
        // a negation
        formula_to_cnf ( pn->child[0], f, !neg, c );
        //      cout << "FOR_NEGATION" << endl;
        return;
    }

    //if it is a conjunction
    if ( ( !neg && ( pn->ival == FOR_CONJUNCTION ) )
            || ( neg && ( pn->ival == FOR_DISJUNCTION ) ) )
    {
        formula_to_cnf ( pn->child[0], f, neg, c );
        formula_to_cnf ( pn->child[1], f, neg, c );
        //      cout << "FOR_CONJUNCTION" << endl;
        return;
    }

    //so it is a disjunction
    //cout << "disjunction" << endl;
    //add left child
    int s1 = f.size();
    formula_to_cnf ( pn->child[0], f, neg, c );
    int s2 = f.size(); // the number of the clauses add from left child

    if ( s2 - s1 == 1 )   //could not be zero
    {
        c.clear();
        c.insert ( c.end(), f[s2-1].begin(), f[s2-1].end() );
        f.pop_back();
    }

    int s3 = f.size();
    formula_to_cnf ( pn->child[1], f, neg, c );
    int s4 = f.size();

    clause_cnf cl;
    if ( s4 - s3 == 1 )   //could not be zero
    {
        if ( s2 - s1 == 1 )
        {
            //already finished
        }
        else
        {
            for ( int i = s1; i < s2; i++ )
            {
                f[i].insert ( f[i].end(), f[s4-1].begin() +c.size(), f[s4-1].end() );
            }
            f.pop_back();
        }
    }
    else
    {
        if ( s2 - s1 == 1 )
        {
            //already finished
        }
        else
        {
            int na = gatom.get_new_atom();
            for ( int i = s1; i < s2; i++ )
            {
                f[i].push_back ( na );
            }
            for ( int i = s3; i < s4; i++ )
            {
                f[i].push_back ( -na );
            }
        }
    }
}

void ParseNode::dl_gk()
{
    if ( type ==PT_FORMULA )
    {
        cout << "know(" << formula_to_gk() << ")." << endl;
    }
    else if ( type == PT_DEFAULT )
    {
        string s1, s2, s3;
        bool first;
        ParseNode *p;

        p = child[0];
        first = true;
        s1 = "";
        while ( p )
        {
            if ( first )
            {
                s1 = s1 + p->child[0]->formula_to_gk();
                first = false;
            }
            else
            {
                s1 = "and(" + s1;
                s1 = s1 + ", ";
                s1 = s1 + p->child[0]->formula_to_gk();
                s1 = s1 + ")";
            }
            p = p->next;
        }
        if ( first )
            s1 = "true";
        s1 = "know(" + s1;
	s1 = s1 + ")";

        p = child[1];
        first = true;
        s2 = "";
        while ( p )
        {
            if ( first )
            {
                s2 = "neg(assume(neg(";
                s2 = s2 + p->child[0]->formula_to_gk();
                s2 = s2+")))";
                first = false;
            }
            else
            {
                s2 = "and(" + s2;
                s2 = s2 + ", ";
                s2 = s2 + "neg(assume(neg(";
                s2 = s2 + p->child[0]->formula_to_gk();
                s2 = s2+"))))";
            }
            p = p->next;
        }
        if ( first )
            s2 = "true";

        p = child[2];
        s3 = "know(" + p->child[0]->formula_to_gk();
        s3 = s3 + ")";

	//cout << "s1: " << s1 << endl;
	//cout << "s2: " << s2 << endl;
	//cout << "s3: " << s3 << endl;
        cout << "imply(and("
             << s1 << "," << s2 << ")," << s3 
             << ")."
             << endl;
    }
    else
    {
        cout << "error, never get here!" << endl;
    }
}

string ParseNode::formula_to_gk()
{
    string s = "";
    if ( type !=PT_FORMULA )
    {
        cout << "!!error!!, never get here." << endl;
    }
    else if ( ( type == PT_FORMULA ) && ( ival >= 0 ) )
    {
        s = sval;
    }
    else if ( ival == FOR_NEGATION )
    {
        s = "neg(";
        s = s + child[0]->formula_to_gk();
        s = s + ")";
    }
    else if ( ival == FOR_CONJUNCTION )
    {
        s = "and(";
        s = s + child[0]->formula_to_gk();
        s = s + ", ";
        s = s + child[1]->formula_to_gk();
        s = s + ")";
    }
    else if ( ival == FOR_DISJUNCTION )
    {
        s = "or(";
        s = s + child[0]->formula_to_gk();
        s = s + ", ";
        s = s + child[1]->formula_to_gk();
        s = s + ")";
    }
    return s;
}
