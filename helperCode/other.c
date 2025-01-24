#include <stdlib.h>
#include <strings.h>
#include <semaphore.h>
#include <pthread.h>

#include "config.h"
#include "other.h"
#include "loop.h"

// This file contains misc functions, including initializing and cleaning
//   up buffers, as well as the main thread functions. All threads are
//   detached throughout the program.

extern sbuf_t* sbuf;
extern sinfo_t sinfo;
extern currinfo_t currInfo;
extern starter_t templates;

void init(sbuf_t* sbuf1) {
    bzero(sbuf1->pixelArray, MAX_OBJECT);

    lights_t* newLights = malloc(sizeof(lights_t));
    sbuf1->lights = newLights;

    for(int i = 0; i < MAX_SPHERES; ++i) {
        sbuf1->spheres[i] = NULL;
    }

    for(int i = 0; i < MAX_CUBES; ++i) {
        sbuf1->cubes[i] = NULL;
        currInfo.cubesMoves[i] = NULL;
    }

    checker_t* newFloor = malloc(sizeof(checker_t));
    sbuf1->floor = newFloor;

    currInfo.currentBuf = 1;
    currInfo.camTranX = 0;
    currInfo.camTranZ = 5;
    currInfo.camRY = 0;
    currInfo.newCubeDelay = 100;
    currInfo.nextCubeIndex = 0;
    currInfo.nextCubeMaterial = malloc(sizeof(material_t));

    writer_t* writers[NUM_THREADS];
    int offsetChange = (MAX_OBJECT) / NUM_THREADS;

    pthread_t tid;
    for(int i = 0; i < NUM_THREADS; ++i) {
        sem_init(&(sinfo.writers[i]), 0, 0);
        writer_t* newWriter = malloc(sizeof(writer_t));
        newWriter->threadNum = i;
        sinfo.writersFinished[i] = 0;
        newWriter->startOffset = offsetChange * i;
        newWriter->stopOffset = offsetChange * (i + 1);
        pthread_create(&tid, NULL, writerThread, newWriter);
    }
}

void cleanUpSbuf(sbuf_t* sbuf1) {
    free(sbuf1->lights);
    for(int i = 0; i < MAX_SPHERES; ++i) {
        if(sbuf1->spheres[i] != NULL) {
            free(sbuf1->spheres[i]->mat);
            free(sbuf1->spheres[i]);
        } else {
            break;
        }
    }

    for(int i = 0; i < MAX_CUBES; ++i) {
        if(sbuf1->cubes[i] != NULL) {
            for(int j = 0; j < 6; ++j) {
                free(sbuf1->cubes[i]->faces[j]->mat);
                free(sbuf1->cubes[i]->faces[j]);
            }
            free(sbuf1->cubes[i]);
        } else {
            break;
        }
    }

    if(sbuf1->floor != NULL) {
        free(sbuf1->floor->mat1);
        free(sbuf1->floor->mat2);
        free(sbuf1->floor);
    }

    free(sbuf1);
}

void cleanUpInfo(currinfo_t* ci) {
    for(int i = 0; i < MAX_CUBES; ++i) {
        if(ci->cubesMoves[i] != NULL) {
            free(ci->cubesMoves[i]);
        }
    }

    free(ci);
}


void* writerThread(void* vargp) {
    pthread_detach(pthread_self());

    writer_t* ws = vargp;

    for(;;) {
        sem_wait(&(sinfo.writers[ws->threadNum]));

        fillPixels(ws->startOffset, ws->stopOffset);

        sinfo.writersFinished[ws->threadNum] = 1;
    }


    return NULL;
}
