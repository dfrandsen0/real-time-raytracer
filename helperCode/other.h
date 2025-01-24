#ifndef OTHER_H
#define OTHER_H

#include "config.h"

// This file contains misc functions, including initializing and cleaning
//   up buffers, as well as the main thread functions. All threads are
//   detached throughout the program.

void init(sbuf_t* sbuf1);
void cleanUpSbuf(sbuf_t* sbuf1);
void cleanUpInfo(currinfo_t* ci);
void* writerThread(void* vargp);

#endif
