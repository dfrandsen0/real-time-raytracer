#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/mman.h>

#include "helperCode/config.h"
#include "helperCode/other.h"
#include "helperCode/movement.h"
#include "helperCode/tracerMath.h"
#include "helperCode/setup.h"
#include "helperCode/loop.h"

//globals
sbuf_t* sbuf = NULL;
sinfo_t sinfo;
currinfo_t currInfo;
starter_t templates;

int main(int argc, char **argv) {

    //prepare shared objects and threads
    sbuf_t* sbuf1 = malloc(sizeof(sbuf_t));
    sbuf_t* sbuf2 = malloc(sizeof(sbuf_t));
    init(sbuf1);
    initSyncMemory();

    sbuf = sbuf1;

    parseStarters("inputData.txt");
    fillObjConsts();

    lights_t* newLights = malloc(sizeof(lights_t));
    sbuf1->lights = newLights;

    checker_t* newFloor = malloc(sizeof(checker_t));
    sbuf1->floor = newFloor;

    copyLights(templates.startLights, newLights);
    copyFloor(templates.floor, newFloor);

    copyInitBuffer(sbuf1, sbuf2);

    fillFloorVars(sbuf1);
    fillFloorVars(sbuf2);

    initSharedMemory(sbuf1, "sharedFile1.txt");
    initSharedMemory(sbuf2, "sharedFile2.txt");

    // A pointer to the current buffer
    sbuf_t* workingBuffer;

    unsigned char pipeline[MAX_SYNC];
    bzero(pipeline, MAX_SYNC);

    struct timespec* timer = malloc(sizeof(struct timespec));
    timer->tv_sec = 0;
    timer->tv_nsec = 50000;  //0.05 ms

    //game loop
    for(;;) {
	//no threads are running here. Safe to change either shared buffer
	//(pre work)

	// Start threads
	for(int j = 0; j < NUM_THREADS; ++j) {
	    sem_post(&(sinfo.writers[j]));
	}

	// copy over commands from last frame
	memcpy(pipeline, sinfo.syncAddr + 3, MAX_COMMANDS);

	*(sinfo.syncAddr) = 1;
	msync(sinfo.syncAddr, MAX_SYNC, MS_SYNC);

	// main thread does it's work here and down there vvv
	// all must-dos before rerender starts

	if(currInfo.currentBuf == 1) {
	    workingBuffer = sbuf2;
	} else {
	    workingBuffer = sbuf1;
	}

	updateCamera(pipeline);
	updateObjects(workingBuffer);
	objectsToWorld(workingBuffer);
	worldToCamera(workingBuffer);
	fillObjVars(workingBuffer);

	// Main thread finished with work; wait for threads
	for(int j = 0; j < NUM_THREADS; ++j) {
	    for(;;) {
		//unsafe place for work
		//any work here acknowledges this might end at any time.
		//any work here is repeated up to infinite number of times, at least 4.

		if(sinfo.writersFinished[j]) {
		    sinfo.writersFinished[j] = 0;
		    break;
		} else {
		    nanosleep(timer, NULL);
		}
	    }
	}

	//no threads running, but the drawer is still technically going
	writeToSharedMemory();

	// Wait for drawer to finish. In theory, it's always finished, so it doesn't have to loop.
	for(;;) {
	    //technically, you should continue to look for key listenings here.
	    //but we won't. We're doing frame by frame listenings. (it's safer)
	    if(*(sinfo.syncAddr + 2) == 1) {
		*(sinfo.syncAddr + 2) = 0;
		//pull down changes from pipeline
		break;
	    } else {
		nanosleep(timer, NULL);
	    }
	}

	//no threads are running here. Safe to change shared buffer
	//(post work)

	// Notify drawer, switch buffers
	if(currInfo.currentBuf == 1) {
	    *(sinfo.syncAddr + 1) = 1;

	    currInfo.currentBuf = 2;
	    sbuf = sbuf2;
	} else {
	    *(sinfo.syncAddr + 1) = 2;

	    currInfo.currentBuf = 1;
	    sbuf = sbuf1;
	}
    }

    //end loop and program

    free(timer);
    cleanUpSbuf(sbuf1);
    cleanUpSbuf(sbuf2);

    exit(0);
}

