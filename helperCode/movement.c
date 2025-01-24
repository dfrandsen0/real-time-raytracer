#include <stdlib.h>
#include <math.h>

#include "config.h"
#include "movement.h"
#include "setup.h"
#include "tracerMath.h"

// The "loop" files are for functions mainly used by the main game loop, and the math
//   behind each pixel.

extern sbuf_t* sbuf;
extern sinfo_t sinfo;
extern currinfo_t currInfo;
extern starter_t templates;

void updateCamera(unsigned char* pipeline) {
    int ws = 0;
    int ss = 0;
    int ds = 0;
    int as = 0;
    int es = 0;
    int qs = 0;

    for(int i = 0; i < MAX_COMMANDS; ++i) {
        if(pipeline[i] == 65) {
            as++;
        } else if(pipeline[i] == 68) {
            ds++;
        } else if(pipeline[i] == 83) {
            ss++;
        } else if(pipeline[i] == 87) {
            ws++;
        } else if(pipeline[i] == 69) {
            es++;
        } else if(pipeline[i] == 81) {
            qs++;
        }
    }

    currInfo.camRY += es * E_VALUE;
    currInfo.camRY -= qs * Q_VALUE;

    double cosr = cos(currInfo.camRY);
    double sinr = sin(currInfo.camRY);

    currInfo.camTranZ += ws * W_VALUE * cosr;
    currInfo.camTranX -= ws * W_VALUE * sinr;

    currInfo.camTranZ += as * A_VALUE * sinr;
    currInfo.camTranX += as * A_VALUE * cosr;

    currInfo.camTranZ -= ds * D_VALUE * sinr;
    currInfo.camTranX -= ds * D_VALUE * cosr;

    currInfo.camTranZ -= ss * S_VALUE * cosr;
    currInfo.camTranX += ss * S_VALUE * sinr;

}

void updateObjects(sbuf_t* workingBuffer) {
    for(int i = 0; i < MAX_CUBES; ++i) {
        if(workingBuffer->cubes[i] == NULL) {
            break;
        }

        cubemove_t* currCubeInfo = currInfo.cubesMoves[i];
        currCubeInfo->rotX += currCubeInfo->vrx;
        currCubeInfo->rotY += currCubeInfo->vry;
        currCubeInfo->rotZ += currCubeInfo->vrz;
        currCubeInfo->tranX += currCubeInfo->vtx;
        currCubeInfo->tranY += currCubeInfo->vty;
        currCubeInfo->tranZ += currCubeInfo->vtz;
    }

    //duplicate code...
    if(currInfo.newCubeDelay == 1) {
        cube_t* newCube = createCube();
        colorCube(newCube);
        copyMaterial(newCube->mat, currInfo.nextCubeMaterial);

        cubemove_t* newMoveInfo = malloc(sizeof(cubemove_t));
        int nextIndex = currInfo.nextCubeIndex;

        if(workingBuffer->cubes[nextIndex] != NULL) {
            for(int i = 0; i < 6; ++i) {
                free(workingBuffer->cubes[nextIndex]->faces[i]->mat);
                free(workingBuffer->cubes[nextIndex]->faces[i]);
            }

            free(workingBuffer->cubes[nextIndex]->mat);
            free(workingBuffer->cubes[nextIndex]);
            free(currInfo.cubesMoves[nextIndex]);
        }

        workingBuffer->cubes[nextIndex] = newCube;
        currInfo.cubesMoves[nextIndex] = newMoveInfo;

        newMoveInfo->tranX = (-1 * currInfo.camTranX);
        newMoveInfo->tranY = -2;
        newMoveInfo->tranZ = (-1 * currInfo.camTranZ) - NEW_CUBE_DISTANCE;

        double randRad = drand48() * 6.283185307;
        double cosr = cos(randRad);
        double sinr = sin(randRad);

        double newX = (newMoveInfo->tranX * cosr) + (newMoveInfo->tranZ * sinr);
        double newZ = (-1 * newMoveInfo->tranX * sinr) + (newMoveInfo->tranZ * cosr);
        newMoveInfo->tranX = newX;
        newMoveInfo->tranZ = newZ;

        newMoveInfo->rotX = 0;
        newMoveInfo->rotY = 0;
        newMoveInfo->rotZ = 0;

        double aimx = -1 * (newX + currInfo.camTranX);
        double aimy = 0;
        double aimz = -1 * (currInfo.camTranZ + newZ);

        normalizeVector(&aimx, &aimy, &aimz);

        newMoveInfo->vtx = aimx;
        newMoveInfo->vty = 0;
        newMoveInfo->vtz = aimz;

        newMoveInfo->vrx = 0.3 * aimx;
        newMoveInfo->vry = 0;
        newMoveInfo->vrz = 0.3 * aimz;

        --(currInfo.newCubeDelay);
    } else if(currInfo.newCubeDelay == 0) {
        cube_t* newCube = createCube();
        int nextIndex = currInfo.nextCubeIndex;
        copyMaterial(currInfo.nextCubeMaterial, newCube->mat);

        if(workingBuffer->cubes[nextIndex] != NULL) {
            free(workingBuffer->cubes[nextIndex]);
        }

        workingBuffer->cubes[nextIndex] = newCube;
        currInfo.newCubeDelay = NEW_CUBE_DELAY;

        if(currInfo.nextCubeIndex < (MAX_CUBES - 1)) {
            (currInfo.nextCubeIndex)++;
        } else {
            currInfo.nextCubeIndex = 0;
        }

    } else {
        --(currInfo.newCubeDelay);
    }

    workingBuffer->floor->shiftX = currInfo.camTranX;
    workingBuffer->floor->shiftZ = currInfo.camTranZ;
    workingBuffer->floor->rotY = currInfo.camRY;
}

void objectsToWorld(sbuf_t* workingBuffer) {
    cube_t* currCube;
    cubemove_t* currCubeInfo;
    for(int i = 0; i < MAX_CUBES; ++i) {
        if(workingBuffer->cubes[i] == NULL) {
            break;
        }

        currCube = workingBuffer->cubes[i];
        currCubeInfo = currInfo.cubesMoves[i];
        resetCube(currCube);
        rotateCube(currCube, currCubeInfo->rotX, currCubeInfo->rotY, currCubeInfo->rotZ);
        translateCube(currCube, currCubeInfo->tranX, currCubeInfo->tranY, currCubeInfo->tranZ);
    }
}

void worldToCamera(sbuf_t* workingBuffer) {
    cube_t* currCube;
    cubemove_t* currCubeInfo;
    for(int i = 0; i < MAX_CUBES; ++i) {
        if(workingBuffer->cubes[i] == NULL) {
            break;
        }

        currCube = workingBuffer->cubes[i];
        translateCube(currCube, currInfo.camTranX, 0, currInfo.camTranZ);
        rotateCubeCam(currCube);
    }
}


void translateCube(cube_t* cube, double tx, double ty, double tz) {
    square_t* currSqr;

    for(int i = 0; i < 6; ++i) {
        currSqr = cube->faces[i];
        for(int j = 0; j < 12; j += 3) {
            currSqr->vertices[j] = currSqr->vertices[j] + tx;
            currSqr->vertices[j + 1] = currSqr->vertices[j + 1] + ty;
            currSqr->vertices[j + 2] = currSqr->vertices[j + 2] + tz;
        }
    }
}

void resetCube(cube_t* cube) {
    square_t* currSqr;
    for(int i = 0; i < 6; ++i) {
        currSqr = cube->faces[i];
        for(int j = 0; j < 12; ++j) {
            currSqr->vertices[j] = currSqr->startVertices[j];
        }
    }
}

void rotateCube(cube_t* cube, double xrad, double yrad, double zrad) {
    square_t* currSqr;
    double x, y, z;
    double newX, newY, newZ;
    double sx = sin(xrad);
    double cx = cos(xrad);
    double sy = sin(yrad);
    double cy = cos(yrad);
    double sz = sin(zrad);
    double cz = cos(zrad);

    double mv[9];
    mv[0] = cz * cy;
    mv[1] = ((-1 * sz) * cx) + (cz * sy * sx);
    mv[2] = (sz * sx) + (cz * sy * cx);

    mv[3] = sz * cy;
    mv[4] = (cz * cx) + (sz * sy * sx);
    mv[5] = ((-1 * sx) * cz) + (sz * sy * cx);

    mv[6] = -1 * sy;
    mv[7] = cy * sx;
    mv[8] = cy * cx;


    for(int i = 0; i < 6; ++i) {
        currSqr = cube->faces[i];
        for(int j = 0; j < 12; j += 3) {
            x = currSqr->vertices[j];
            y = currSqr->vertices[j + 1];
            z = currSqr->vertices[j + 2];

            newX = (x * mv[0]) +
                   (y * mv[1]) +
                   (z * mv[2]);
            newY = (x * mv[3]) +
                   (y * mv[4]) +
                   (z * mv[5]);
            newZ = (x * mv[6]) +
                   (y * mv[7]) +
                   (z * mv[8]);

            currSqr->vertices[j] = newX;
            currSqr->vertices[j + 1] = newY;
            currSqr->vertices[j + 2] = newZ;
        }
    }
}

void rotateCubeCam(cube_t* cube) {
    square_t* currSqr;
    double newX, newZ;
    double cosr = cos(currInfo.camRY);
    double sinr = sin(currInfo.camRY);

    for(int i = 0; i < 6; ++i) {
        currSqr = cube->faces[i];
        for(int j = 0; j < 12; j += 3) {
            newX = (currSqr->vertices[j] * cosr) + (currSqr->vertices[j + 2] * sinr);
            newZ = (currSqr->vertices[j] * (-1 * sinr)) + (currSqr->vertices[j + 2] * cosr);
            currSqr->vertices[j] = newX;
            currSqr->vertices[j + 2] = newZ;
        }
    }
}

