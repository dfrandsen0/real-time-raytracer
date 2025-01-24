#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "setup.h"
#include "tracerMath.h"

// This file handles copying objects into each other, parsing the starting data,
//   and filling each object with its appropriate information.

extern sbuf_t* sbuf;
extern sinfo_t sinfo;
extern currinfo_t currInfo;
extern starter_t templates;

void parseStarters(unsigned char* fileName) {
    int fd = fileno(fopen(fileName, "r"));

    unsigned char buf[64];
    bzero(buf, 64);

    int nread;
    int offset = 0;
    int values = 0;

    int checkerFound = 0;

    double* valuesToUpdate[64];

    cube_t* startCube = malloc(sizeof(cube_t));
    lights_t* lights = malloc(sizeof(lights_t));
    checker_t* floor = malloc(sizeof(checker_t));

    templates.startLights = lights;
    templates.startCube = startCube;
    templates.floor = floor;

    for(;;) {
        nread = read(fd, buf + offset, 1);

        if(buf[offset] == ' ' || buf[offset] == '\n' || nread == 0 || buf[offset] == '\r') {
            buf[offset] = 0;
            if(values > 0) {
                if(((buf[0] >= 48) && (buf[0] <= 57)) || (buf[0] == 46) || (buf[0] == 45)) {
                    *(valuesToUpdate[values - 1]) = strtod(buf, NULL);
                    values--;
                }
            } else if(strcmp(buf, "DirectionToLight") == 0) {
                valuesToUpdate[2] = &(lights->lightDirX);
                valuesToUpdate[1] = &(lights->lightDirY);
                valuesToUpdate[0] = &(lights->lightDirZ);
                values = 3;
            } else if(strcmp(buf, "LightColor") == 0) {
                valuesToUpdate[2] = &(lights->red);
                valuesToUpdate[1] = &(lights->green);
                valuesToUpdate[0] = &(lights->blue);
                values = 3;
            } else if(strcmp(buf, "AmbientLight") == 0) {
                valuesToUpdate[2] = &(lights->ambRed);
                valuesToUpdate[1] = &(lights->ambGreen);
                valuesToUpdate[0] = &(lights->ambBlue);
                values = 3;
            } else if(strcmp(buf, "BackgroundColor") == 0) {
                valuesToUpdate[2] = &(lights->backRed);
                valuesToUpdate[1] = &(lights->backGreen);
                valuesToUpdate[0] = &(lights->backBlue);
                values = 3;
            } else if(strcmp(buf, "Cube") == 0) {
                material_t* newMaterial = malloc(sizeof(material_t));
                startCube->mat = newMaterial;

                valuesToUpdate[33] = &(startCube->vertices[0]);
                valuesToUpdate[32] = &(startCube->vertices[1]);
                valuesToUpdate[31] = &(startCube->vertices[2]);
                valuesToUpdate[30] = &(startCube->vertices[3]);
                valuesToUpdate[29] = &(startCube->vertices[4]);
                valuesToUpdate[28] = &(startCube->vertices[5]);
                valuesToUpdate[27] = &(startCube->vertices[6]);
                valuesToUpdate[26] = &(startCube->vertices[7]);
                valuesToUpdate[25] = &(startCube->vertices[8]);
                valuesToUpdate[24] = &(startCube->vertices[9]);
                valuesToUpdate[23] = &(startCube->vertices[10]);
                valuesToUpdate[22] = &(startCube->vertices[11]);
                valuesToUpdate[21] = &(startCube->vertices[12]);
                valuesToUpdate[20] = &(startCube->vertices[13]);
                valuesToUpdate[19] = &(startCube->vertices[14]);
                valuesToUpdate[18] = &(startCube->vertices[15]);
                valuesToUpdate[17] = &(startCube->vertices[16]);
                valuesToUpdate[16] = &(startCube->vertices[17]);
                valuesToUpdate[15] = &(startCube->vertices[18]);
                valuesToUpdate[14] = &(startCube->vertices[19]);
                valuesToUpdate[13] = &(startCube->vertices[20]);
                valuesToUpdate[12] = &(startCube->vertices[21]);
                valuesToUpdate[11] = &(startCube->vertices[22]);
                valuesToUpdate[10] = &(startCube->vertices[23]);
                valuesToUpdate[9] = &(newMaterial->Kd);
                valuesToUpdate[8] = &(newMaterial->Ks);
                valuesToUpdate[7] = &(newMaterial->Ka);
                valuesToUpdate[6] = &(newMaterial->OdRed);
                valuesToUpdate[5] = &(newMaterial->OdGreen);
                valuesToUpdate[4] = &(newMaterial->OdBlue);
                valuesToUpdate[3] = &(newMaterial->OsRed);
                valuesToUpdate[2] = &(newMaterial->OsGreen);
                valuesToUpdate[1] = &(newMaterial->OsBlue);
                valuesToUpdate[0] = &(newMaterial->Kgls);

                values = 34;
            } else if((strcmp(buf, "Checker") == 0) && (!(checkerFound))) {
                material_t* newMaterial1 = malloc(sizeof(material_t));
                material_t* newMaterial2 = malloc(sizeof(material_t));
                floor->mat1 = newMaterial1;
                floor->mat2 = newMaterial2;

                checkerFound = 1;

                valuesToUpdate[24] = &(floor->nx);
                valuesToUpdate[23] = &(floor->ny);
                valuesToUpdate[22] = &(floor->nz);
                valuesToUpdate[21] = &(floor->scale);
                valuesToUpdate[20] = &(floor->modScale);

                valuesToUpdate[19] = &(newMaterial1->Kd);
                valuesToUpdate[18] = &(newMaterial1->Ks);
                valuesToUpdate[17] = &(newMaterial1->Ka);
                valuesToUpdate[16] = &(newMaterial1->OdRed);
                valuesToUpdate[15] = &(newMaterial1->OdGreen);
                valuesToUpdate[14] = &(newMaterial1->OdBlue);
                valuesToUpdate[13] = &(newMaterial1->OsRed);
                valuesToUpdate[12] = &(newMaterial1->OsGreen);
                valuesToUpdate[11] = &(newMaterial1->OsBlue);
                valuesToUpdate[10] = &(newMaterial1->Kgls);

                valuesToUpdate[9] = &(newMaterial2->Kd);
                valuesToUpdate[8] = &(newMaterial2->Ks);
                valuesToUpdate[7] = &(newMaterial2->Ka);
                valuesToUpdate[6] = &(newMaterial2->OdRed);
                valuesToUpdate[5] = &(newMaterial2->OdGreen);
                valuesToUpdate[4] = &(newMaterial2->OdBlue);
                valuesToUpdate[3] = &(newMaterial2->OsRed);
                valuesToUpdate[2] = &(newMaterial2->OsGreen);
                valuesToUpdate[1] = &(newMaterial2->OsBlue);
                valuesToUpdate[0] = &(newMaterial2->Kgls);

                values = 25;
            }

            if(nread == 0) {
                break;
            }

            bzero(buf, 64);
            offset = 0;
        } else {
            offset++;
        }
    }

    assignCubeVertices(startCube);

    close(fd);
}

cube_t* createCube() {
    cube_t* newCube = malloc(sizeof(cube_t));
    copyCube(templates.startCube, newCube);

    return newCube;
}

void colorCube(cube_t* cube) {
    cube->mat->OdRed = 0.0;
    cube->mat->OdBlue = 0.0;
    fillMaterialConsts(cube->mat, sbuf->lights);
}

void assignCubeVertices(cube_t* cube) {

    int* faceLists[6];
    int face0[] = {0, 1, 2, 9, 10, 11, 6, 7, 8, 3, 4, 5};
    int face1[] = {12, 13, 14, 21, 22, 23, 9, 10, 11, 0, 1, 2};
    int face2[] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 12, 13, 14};
    int face3[] = {3, 4, 5, 6, 7, 8, 18, 19, 20, 15, 16, 17};
    int face4[] = {9, 10, 11, 21, 22, 23, 18, 19, 20, 6, 7, 8};
    int face5[] = {12, 13, 14, 0, 1, 2, 3, 4, 5, 15, 16, 17};
    faceLists[0] = face0;
    faceLists[1] = face1;
    faceLists[2] = face2;
    faceLists[3] = face3;
    faceLists[4] = face4;
    faceLists[5] = face5;


    for(int j = 0; j < 6; ++j) {
        square_t* newSquare = malloc(sizeof(square_t));
        cube->faces[j] = newSquare;

        for(int k = 0; k < 12; ++k) {
            newSquare->startVertices[k] = cube->vertices[faceLists[j][k]];
            newSquare->vertices[k] = cube->vertices[faceLists[j][k]];
        }
    }
}

void fillObjConsts() {
    lights_t* lights = templates.startLights;
    cube_t* startCube = templates.startCube;
    checker_t* floor = templates.floor;

    normalizeVector(&(lights->lightDirX), &(lights->lightDirY), &(lights->lightDirZ));

    fillMaterialConsts(startCube->mat, lights);

    normalizeVector(&(floor->nx), &(floor->ny), &(floor->nz));

    floor->shiftX = 0;
    floor->shiftZ = 0;
    floor->rotY = 0;

    fillMaterialConsts(floor->mat1, lights);
    fillMaterialConsts(floor->mat2, lights);
}

void fillObjVars(sbuf_t* buffer) {
    cube_t** cubes = buffer->cubes;
    checker_t* floor = buffer->floor;

    for(int i = 0; i < MAX_CUBES; ++i) {
        if(cubes[i] == NULL) {
            break;
        }

        fillCubeVars(cubes[i], buffer);
    }

    fillFloorVars(buffer);
}

void fillMaterialConsts(material_t* mat, lights_t* lights) {
    mat->ambRed = mat->Ka * lights->ambRed * mat->OdRed;
    mat->ambGreen = mat->Ka * lights->ambGreen * mat->OdGreen;
    mat->ambBlue = mat->Ka * lights->ambBlue * mat->OdBlue;

    mat->difRed = mat->Kd * lights->red * mat->OdRed;
    mat->difGreen = mat->Kd * lights->green * mat->OdGreen;
    mat->difBlue = mat->Kd * lights->blue * mat->OdBlue;

    mat->specRed = mat->Ks * lights->red * mat->OsRed;
    mat->specGreen = mat->Ks * lights->green * mat->OsGreen;
    mat->specBlue = mat->Ks * lights->blue * mat->OsBlue;
}

void fillCubeVars(cube_t* cube, sbuf_t* buffer) {
    lights_t* lights = buffer->lights;

    for(int i = 0; i < 6; ++i) {
        square_t* currSquare = cube->faces[i];
        double* squareVertices = currSquare->vertices;
        cross(squareVertices[0] - squareVertices[3],
              squareVertices[1] - squareVertices[4],
              squareVertices[2] - squareVertices[5],
              squareVertices[6] - squareVertices[3],
              squareVertices[7] - squareVertices[4],
              squareVertices[8] - squareVertices[5],
              &(currSquare->nx), &(currSquare->ny), &(currSquare->nz));
        normalizeVector(&(currSquare->nx), &(currSquare->ny), &(currSquare->nz));

        double nl = dot(currSquare->nx, currSquare->ny, currSquare->nz, lights->lightDirX, lights->lightDirY, lights->lightDirZ);

        //speed up later, cube does not need a dif or a spec, just amb
        if(nl < 0) {
            currSquare->inShadow = 1;

            smallmat_t* newMat = malloc(sizeof(smallmat_t));
            currSquare->mat = newMat;

            newMat->difRed = 0;
            newMat->difGreen = 0;
            newMat->difBlue = 0;

            newMat->comboRed = 0;
            newMat->comboGreen = 0;
            newMat->comboBlue = 0;

            newMat->specRed = 0;
            newMat->specGreen = 0;
            newMat->specBlue = 0;

            currSquare->rx = 0;
            currSquare->ry = 0;
            currSquare->rz = 0;
        } else {
            currSquare->inShadow = 0;

            smallmat_t* newMat = malloc(sizeof(smallmat_t));
            currSquare->mat = newMat;

            newMat->difRed = cube->mat->difRed * nl;
            newMat->difGreen = cube->mat->difGreen * nl;
            newMat->difBlue = cube->mat->difBlue * nl;

            newMat->comboRed = cube->mat->ambRed + newMat->difRed;
            newMat->comboGreen = cube->mat->ambGreen + newMat->difGreen;
            newMat->comboBlue = cube->mat->ambBlue + newMat->difBlue;

            newMat->specRed = cube->mat->specRed;
            newMat->specGreen = cube->mat->specGreen;
            newMat->specBlue = cube->mat->specBlue;

            double subR = 2 * dot(currSquare->nx, currSquare->ny, currSquare->nz, lights->lightDirX, lights->lightDirY, lights->lightDirZ);
            currSquare->rx = (subR * currSquare->nx) - lights->lightDirX;
            currSquare->ry = (subR * currSquare->ny) - lights->lightDirY;
            currSquare->rz = (subR * currSquare->nz) - lights->lightDirZ;
        }
    }
}

void fillFloorVars(sbuf_t* buffer) {
    checker_t* floor = buffer->floor;
    lights_t* lights = buffer->lights;
    double nl = dot(floor->nx, floor->ny, floor->nz, lights->lightDirX, lights->lightDirY, lights->lightDirZ);

    material_t* mat1 = floor->mat1;
    material_t* mat2 = floor->mat2;

    if(nl < 0) {
        floor->inShadow = 1;

        mat1->difRed = 0;
        mat1->difGreen = 0;
        mat1->difBlue = 0;

        mat2->difRed = 0;
        mat2->difGreen = 0;
        mat2->difBlue = 0;

        floor->rx = 0;
        floor->ry = 0;
        floor->rz = 0;
    } else {
        floor->inShadow = 0;

        mat1->difRed = mat1->difRed * nl;
        mat1->difGreen = mat1->difGreen * nl;
        mat1->difBlue = mat1->difBlue * nl;

        mat2->difRed = mat2->difRed * nl;
        mat2->difGreen = mat2->difGreen * nl;
        mat2->difBlue = mat2->difBlue * nl;

        double subR = 2 * dot(floor->nx, floor->ny, floor->nz, lights->lightDirX, lights->lightDirY, lights->lightDirZ);
        floor->rx = (subR * floor->nx) - lights->lightDirX;
        floor->ry = (subR * floor->ny) - lights->lightDirY;
        floor->rz = (subR * floor->nz) - lights->lightDirZ;
    }
}

void copyInitBuffer(sbuf_t* sbuf1, sbuf_t* sbuf2) {
    bzero(sbuf2->pixelArray, MAX_OBJECT);

    lights_t* newLights = malloc(sizeof(lights_t));
    sbuf2->lights = newLights;

    checker_t* newFloor = malloc(sizeof(checker_t));
    sbuf2->floor = newFloor;

    copyLights(sbuf1->lights, newLights);
    copyFloor(sbuf1->floor, newFloor);
}

void copyLights(lights_t* oldLights, lights_t* newLights) {
    newLights->lightDirX = oldLights->lightDirX;
    newLights->lightDirY = oldLights->lightDirY;
    newLights->lightDirZ = oldLights->lightDirZ;

    newLights->red = oldLights->red;
    newLights->green = oldLights->green;
    newLights->blue = oldLights->blue;

    newLights->ambRed = oldLights->ambRed;
    newLights->ambGreen = oldLights->ambGreen;
    newLights->ambBlue = oldLights->ambBlue;

    newLights->backRed = oldLights->backRed;
    newLights->backGreen = oldLights->backGreen;
    newLights->backBlue = oldLights->backBlue;
}

void copySpheres(sphere_t** oldSpheres, sphere_t** newSpheres) {
    for(int i = 0; i < MAX_SPHERES; ++i) {
        if(oldSpheres[i] == NULL) {
            newSpheres[i] = NULL;
        } else {
            sphere_t* ns = malloc(sizeof(sphere_t));
            newSpheres[i] = ns;

            ns->center_x = oldSpheres[i]->center_x;
            ns->center_y = oldSpheres[i]->center_y;
            ns->center_z = oldSpheres[i]->center_z;

            ns->radius = oldSpheres[i]->radius;

            material_t* newMaterial = malloc(sizeof(material_t));
            ns->mat = newMaterial;
            copyMaterial(oldSpheres[i]->mat, newMaterial);
        }
    }
}

void copyCubes(cube_t** oldCubes, cube_t** newCubes) {
    for(int i = 0; i < MAX_CUBES; ++i) {
        if(oldCubes[i] == NULL) {
            newCubes[i] = NULL;
        } else {
            cube_t* nc = malloc(sizeof(cube_t));
            newCubes[i] = nc;

            copyCube(oldCubes[i], nc);
        }
    }
}

void copyCube(cube_t* oldCube, cube_t* newCube) {
    for(int i = 0; i < 24; ++i) {
        newCube->vertices[i] = oldCube->vertices[i];
    }

    for(int i = 0; i < 6; ++i) {
        square_t* newSquare = malloc(sizeof(square_t));
        newCube->faces[i] = newSquare;
        copySquare(oldCube->faces[i], newSquare);
    }

    material_t* newMaterial = malloc(sizeof(material_t));
    newCube->mat = newMaterial;
    copyMaterial(oldCube->mat, newMaterial);
}

void copySquare(square_t* oldSquare, square_t* newSquare) {
    for(int i = 0; i < 12; ++i) {
        newSquare->startVertices[i] = oldSquare->startVertices[i];
        newSquare->vertices[i] = oldSquare->vertices[i];
    }
}

void copyFloor(checker_t* oldFloor, checker_t* newFloor) {
    material_t* newMat1 = malloc(sizeof(material_t));
    material_t* newMat2 = malloc(sizeof(material_t));
    newFloor->mat1 = newMat1;
    newFloor->mat2 = newMat2;

    copyMaterial(oldFloor->mat1, newMat1);
    copyMaterial(oldFloor->mat2, newMat2);

    newFloor->nx = oldFloor->nx;
    newFloor->ny = oldFloor->ny;
    newFloor->nz = oldFloor->nz;

    newFloor->scale = oldFloor->scale;
    newFloor->modScale = oldFloor->modScale;

    newFloor->shiftX = oldFloor->shiftX;
    newFloor->shiftZ = oldFloor->shiftZ;
    newFloor->rotY = oldFloor->rotY;
}

void copyMaterial(material_t* oldMaterial, material_t* newMaterial) {
    newMaterial->Kd = oldMaterial->Kd;
    newMaterial->Ks = oldMaterial->Ks;
    newMaterial->Ka = oldMaterial->Ka;

    newMaterial->OdRed = oldMaterial->OdRed;
    newMaterial->OdGreen = oldMaterial->OdGreen;
    newMaterial->OdBlue = oldMaterial->OdBlue;

    newMaterial->OsRed = oldMaterial->OsRed;
    newMaterial->OsGreen = oldMaterial->OsGreen;
    newMaterial->OsBlue = oldMaterial->OsBlue;

    newMaterial->Kgls = oldMaterial->Kgls;

    newMaterial->ambRed = oldMaterial->ambRed;
    newMaterial->ambGreen = oldMaterial->ambGreen;
    newMaterial->ambBlue = oldMaterial->ambBlue;

    newMaterial->difRed = oldMaterial->difRed;
    newMaterial->difGreen = oldMaterial->difGreen;
    newMaterial->difBlue = oldMaterial->difBlue;

    newMaterial->specRed = oldMaterial->specRed;
    newMaterial->specGreen = oldMaterial->specGreen;
    newMaterial->specBlue = oldMaterial->specBlue;
}
