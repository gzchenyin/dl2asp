#ifndef _DLRULE_H_
#define _DLRULE_H_


extern "C" {
#include "picosat.h"
}

void dl_asp();

void encode_formula ( PicoSAT *ps, dlint f, bool neg, dlint g );
void find_implication_rule();
void mcs_to_mus();

void add_constraint ( dlint_set p );
void add_rule ( dlint_set h, dlint_set p, dlint_set n );

void find_mus ( dlint f );

#endif
