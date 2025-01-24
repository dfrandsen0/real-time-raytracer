#ifndef MOVEMENT_H
#define MOVEMENT_H

// This file handles updating objects and the camera according to user input, as well
//   as natural movements of the world.

void updateCamera(unsigned char* pipeline);
void updateObjects(sbuf_t* workingBuffer);
void objectsToWorld(sbuf_t* workingBuffer);
void worldToCamera(sbuf_t* workingBuffer);
void translateCube(cube_t* cube, double tx, double ty, double tz);
void resetCube(cube_t* cube);
void rotateCube(cube_t* cube, double xrad, double yrad, double zrad);
void rotateCubeCam(cube_t* cube);

#endif
