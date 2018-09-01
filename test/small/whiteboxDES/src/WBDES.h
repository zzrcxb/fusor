#ifndef WBDES_H
#define WBDES_H

#include <stdint.h>

#include "DES.h"

void before_rounds(unsigned int *in, unsigned int *out);

void rounds(unsigned int *in, unsigned int *out, int round);

void end_rounds(unsigned int *in, unsigned int *out);

#endif
