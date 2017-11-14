#ifndef SDP_COMPILE_H
#define SDP_COMPILE_H

#include "sdp.h"

/**
 * compile whole program or macros
 *
 * compilation consumes FilterDescription
 * returns filter tree
 */
Filter *Compile(FilterDescription &src,
                OutletMap &outlet,  // predefined outlets for input of macro,
                // used to add other labeled outlets during compilation
                bool scriptVerbose
               );

void compileMacro(const std::string &filename, bool scriptVerbose);

#endif
