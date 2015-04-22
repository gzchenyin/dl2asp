#ifndef _TOOL_H_
#define _TOOL_H_

#include <iostream>
#include <vector>
#include <set>
#include <sys/time.h>
#include <sys/resource.h>


using namespace std;

#define CPUTIME (getrusage(RUSAGE_SELF,&ruse),\
  ruse.ru_utime.tv_sec + ruse.ru_stime.tv_sec + \
  1e-6 * (ruse.ru_utime.tv_usec + ruse.ru_stime.tv_usec))

typedef long dlint;
typedef set<dlint> dlint_set;

//a clause is a vector of atoms or negated atom
typedef vector<dlint> clause_cnf;

//a formula is a stored as a vector of clause
typedef vector<clause_cnf> formula_cnf;

void handle_param(int argc, char** argv);

#endif
