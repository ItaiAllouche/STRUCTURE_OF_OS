#ifndef _TTFTPS_H
#define _TTFTPS_H

#include "server.h"
#include "ttftps.h"

void arg_error(){
    cerr << "TTFTP_ERROR: illegal arguments" << endl;
    exit(1);
}
#endif