#ifndef _TFTP_H
#define _TFTP_H

#include "server.h"
#include "tftp.h"

void arg_error(){
    cerr << "TTFTP_ERROR: illegal arguments" << endl;
    exit(1);
}
#endif