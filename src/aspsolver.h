#ifndef _ASPSOLVER_H_
#define _ASPSOLVER_H_

#include <clasp/logic_program.h>    // for defining logic programs
#include <clasp/unfounded_check.h>  // unfounded set checkers
#include <clasp/model_enumerators.h>// for enumerating answer sets
#include <clasp/solve_algorithms.h> // for enumerating answer sets
#include <clasp/minimize_constraint.h>

void solve_asp ( bool a );
void printModel ( const Clasp::SymbolTable& symTab, const Clasp::Model& model );

#endif
