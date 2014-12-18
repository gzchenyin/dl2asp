#ifndef _DLPARSER_H_
#define _DLPARSER_H_

#include "tool.h"

//logic symbol
#define FOR_CONJUNCTION -2
#define FOR_DISJUNCTION -3
#define FOR_NEGATION -4

#define MAX_PARSE_CHILD 3
#define MAX_TOKEN_LENGTH 256
enum ParseType {
    PT_DEFAULT,
    PT_FORMULA,
    PT_FORMULAS,
};

class ParseNode
{
public:
    ParseNode() {};
    ParseNode ( ParseType t );
    ParseNode ( ParseType t, int i );
    ~ParseNode();

    ParseType type;
    ParseNode * child[MAX_PARSE_CHILD];
    ParseNode * next;
    int ival;
    int iweight;
    int ilevel;
    string sval;

    void add_bktheory();
    void add_default();

    void formula_to_cnf ( ParseNode * pn, formula_cnf &f, bool neg, clause_cnf c );
};


#endif
