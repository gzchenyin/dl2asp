/*
 * tool.cpp
 *
 *  Created on: Nov 19, 2014
 *      Author: ychen
 */

#include "tool.h"
#include "dlsolver.h"
#include <cstring>

extern DLsolver gdl;

void handle_param ( int argc, char** argv )
{
    gdl.gk = false;
    gdl.mt = MT_NONE;
    gdl.mn = 1;
    gdl.infile = "";

    for ( int i = 1; i < argc; i++ ) {
        if ( argv[i][0] != '-' ) {
            if ( gdl.infile.empty() ) {
                gdl.infile = argv[i];
            }
        } else {
            if ( strcmp ( argv[i], "-gk" ) == 0 ) {
                gdl.gk = true;
	    } else if ( strcmp ( argv[i], "-max" ) == 0 ) {
                gdl.mt = MT_MAX;
            } else if ( strcmp ( argv[i], "-min" ) == 0 ) {
                gdl.mt = MT_MIN;
            } else if ( strcmp ( argv[i], "-all" ) == 0 ) {
                gdl.mn = 0;
            }
            else {
              cout << "error parameter" << endl;
            }
        }
    }
}

