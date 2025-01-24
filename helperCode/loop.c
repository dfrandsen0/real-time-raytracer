#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "config.h"
#include "loop.h"
#include "tracerMath.h"

// The "loop" files are for functions mainly used by the main game loop, and the math
//   behind each pixel.

extern sbuf_t* sbuf;
extern sinfo_t sinfo;
extern currinfo_t currInfo;
extern starter_t templates;

void assign(int offset, unsigned char red, unsigned char green, unsigned char blue) {
    *(sbuf->pixelArray + offset + 2) = red;
    *(sbuf->pixelArray + offset + 1) = green;
    *(sbuf->pixelArray + offset) = blue;
}


void writeToSharedMemory() {
    memcpy(sbuf->sharedAddr, sbuf->pixelArray, MAX_OBJECT);
}

void initSyncMemory() {
    int sfd = fileno(fopen("syncFile.txt", "r+"));

    char* addr;
    addr = mmap(NULL, MAX_SYNC, PROT_READ | PROT_WRITE, MAP_SHARED, sfd, 0);
    close(sfd);

    if(addr == MAP_FAILED) {
        printf("Mapping failed\n");
        exit(1);
    }

    sinfo.syncAddr = addr;

    *(addr + 1) = 2;
}


void initSharedMemory(sbuf_t* sbuf1, unsigned char* fileName) {
    int sfd = fileno(fopen(fileName, "r+"));

    char* addr;
    addr = mmap(NULL, MAX_OBJECT, PROT_READ | PROT_WRITE, MAP_SHARED, sfd, 0);
    close(sfd);

    if(addr == MAP_FAILED) {
        printf("Mapping failed\n");
        exit(1);
    }

    sbuf1->sharedAddr = addr;
}

void fillPixels(int startOffset, int stopOffset) {
    lights_t* lights = sbuf->lights;
    sphere_t** spheres = sbuf->spheres;
    cube_t** cubes = sbuf->cubes;
    checker_t* floor = sbuf->floor;

    double windowHeight = (double)WINDOW_HEIGHT;
    double windowWidth = (double)WINDOW_WIDTH;
    double pixelsX = (double)PIXELS_X;
    double pixelsY = (double)PIXELS_Y;

    double pixelWidth = windowWidth / pixelsX;
    double pixelHeight = windowHeight / pixelsY;

    //PC: Pixel Center, starting at the top left pixel
    double startPCX = -(windowWidth / 2) + (0.5 * pixelWidth);
    double startPCY = (windowHeight / 2 ) - (0.5 * pixelHeight);

    //thread specific
    int offset = startOffset;
    int pixelsCovered = startOffset / 3;
    int rowNum = pixelsCovered / PIXELS_Y;
    int colNum = pixelsCovered % PIXELS_X;

    double currentPCX = startPCX + (colNum * pixelWidth);
    double currentPCY = startPCY - (rowNum * pixelHeight);

    unsigned char color[3];

    double pcz = -(double)D;

    int x = colNum;
    for(int y = rowNum; (y < PIXELS_Y) && (offset < stopOffset); ++y) {
        for(; (x < PIXELS_X) && (offset < stopOffset); ++x) {
            computePixel(currentPCX, currentPCY, pcz, color, color + 1, color + 2);
            assign(offset, color[0], color[1], color[2]);
            offset += 3;
            currentPCX += pixelWidth;
        }

        currentPCX = startPCX;
        currentPCY -= pixelHeight;
        x = 0;
    }
}

void computePixel(double rayDirX, double rayDirY, double rayDirZ, unsigned char *red, unsigned char *green, unsigned char *blue) {
    lights_t* lights = sbuf->lights;
    sphere_t** spheres = sbuf->spheres;
    cube_t** cubes = sbuf->cubes;
    checker_t* floor = sbuf->floor;

    normalizeVector(&rayDirX, &rayDirY, &rayDirZ);

    int closestIndex = -1;
    enum Shape currShape = SPHERE;
    double smallestT = -1;
    double px, py, pz;
    double b, c, disc, t;
    double oz = (double)D;

    for(int i = 0; i < MAX_SPHERES; ++i) {
        if(spheres[i] != NULL) {
            b = 2 * (0 - (rayDirX * spheres[i]->center_x) - (rayDirY * spheres[i]->center_y) + (rayDirZ * oz) - (rayDirZ * spheres[i]->center_z));
            c = pow(spheres[i]->center_x, 2) +
                pow(spheres[i]->center_y, 2) +
                pow(oz, 2) - (2 * oz * spheres[i]->center_z) +
                pow(spheres[i]->center_z, 2) -
                pow(spheres[i]->radius, 2);

            disc = pow(b, 2) - (4 * c);
            t = -1;
            if(disc == 0) {
                t = -b/2;
            } else if(disc > 0) {
                t = (-b - sqrt(pow(b, 2) - (4*c))) / 2;
                if(t < 0) {
                    t = (-b + sqrt(pow(b, 2) - (4*c))) / 2;
                }
            }

            if(t > 0) {
                if((smallestT < 0) || (t < smallestT)) {
                    smallestT = t;
                    currShape = SPHERE;
                    closestIndex = i;
                }
            }

        } else {
            break;
        }
    }

    double eD, nr;
    int collideFace;

    for(int i = 0; i < MAX_CUBES; ++i) {
        if(cubes[i] != NULL) {
            for(int j = 0; j < 6; j++) {
                square_t* currSqr = cubes[i]->faces[j];

                nr = dot(currSqr->nx, currSqr->ny, currSqr->nz, rayDirX, rayDirY, rayDirZ);

                //back culled
                if(nr >= 0) {
                    continue;
                }

                eD = -1 * ((currSqr->nx * currSqr->vertices[0]) + (currSqr->ny * currSqr->vertices[1]) + (currSqr->nz * currSqr->vertices[2]));

                t = (-1 * ((currSqr->nz * oz) + eD)) / nr;

                if(t > 0) {
                    if((smallestT < 0) || (t < smallestT)) {
                        double tpx = rayDirX*t;
                        double tpy = rayDirY*t;
                        double tpz = oz + (rayDirZ*t);

                        if(pointInSquare(tpx, tpy, tpz, currSqr)) {
                            smallestT = t;
                            closestIndex = i;
                            collideFace = j;
                            currShape = CUBE;
                            px = tpx;
                            py = tpy;
                            pz = tpz;
                            break;
                        }
                    }
                }
            }
        } else {
            break;
        }
    }

    //just making sure it exists...
    if(floor->mat1 != NULL) {
        nr = dot(floor->nx, floor->ny, floor->nz, rayDirX, rayDirY, rayDirZ);

        //not back culled
        if(nr < 0) {
            t = (-1 * ((floor->nz * oz) + floor->scale)) / nr;

            if(t > 0) {
                if((smallestT < 0) || (t < smallestT)) {
                    px = rayDirX*t;
                    py = rayDirY*t;
                    pz = oz + (rayDirZ*t);

                    smallestT = t;
                    closestIndex = -1;
                    currShape = CHECKER;
                }
            }
        }
    }


    if(smallestT < 0) {
        *red = convertToPixel(lights->backRed);
        *green = convertToPixel(lights->backGreen);
        *blue = convertToPixel(lights->backBlue);
    } else {
        if(currShape == SPHERE) {
            px = rayDirX*smallestT;
            py = rayDirY*smallestT;
            pz = oz + rayDirZ*smallestT;
        } else if(currShape == CUBE) {
            cube_t* currCube = cubes[closestIndex];

            if(currCube->faces[collideFace]->inShadow) {
                *red = convertToPixel(currCube->mat->ambRed);
                *green = convertToPixel(currCube->mat->ambGreen);
                *blue = convertToPixel(currCube->mat->ambBlue);
                return;
            }
        } else {
            if(floor->inShadow) {
                int which = whichFloorMat(px, pz, floor->modScale, floor->shiftX, floor->shiftZ, floor->rotY);

                material_t* mat;
                if(which) {
                    mat = floor->mat1;
                } else {
                    mat = floor->mat2;
                }

                *red = convertToPixel(mat->ambRed);
                *green = convertToPixel(mat->ambGreen);
                *blue = convertToPixel(mat->ambBlue);
                return;
            }
        }

        if(inShadow(px, py, pz, currShape, closestIndex)) {
            if(currShape == SPHERE) {
                *red = convertToPixel(spheres[closestIndex]->mat->ambRed);
                *green = convertToPixel(spheres[closestIndex]->mat->ambGreen);
                *blue = convertToPixel(spheres[closestIndex]->mat->ambBlue);
            } else if(currShape == CUBE) {
                *red = convertToPixel(cubes[closestIndex]->mat->ambRed);
                *green = convertToPixel(cubes[closestIndex]->mat->ambGreen);
                *blue = convertToPixel(cubes[closestIndex]->mat->ambBlue);
            } else {
                int which = whichFloorMat(px, pz, floor->modScale, floor->shiftX, floor->shiftZ, floor->rotY);

                material_t* mat;
                if(which) {
                    mat = floor->mat1;
                } else {
                    mat = floor->mat2;
                }

                *red = convertToPixel(mat->ambRed);
                *green = convertToPixel(mat->ambGreen);
                *blue = convertToPixel(mat->ambBlue);
            }
        } else {
            calcColor(px, py, pz, rayDirX, rayDirY, rayDirZ, smallestT, closestIndex, collideFace, currShape, red, green, blue);
        }
    }
}

char inShadow(double ox, double oy, double oz, enum Shape type, int index) {
    lights_t* lights = sbuf->lights;
    sphere_t** spheres = sbuf->spheres;
    cube_t** cubes = sbuf->cubes;
    checker_t* floor = sbuf->floor;

    double rayDirX = lights->lightDirX;
    double rayDirY = lights->lightDirY;
    double rayDirZ = lights->lightDirZ;

    double b, c, disc, t;

    for(int i = 0; i < MAX_SPHERES; ++i) {
        if(spheres[i] != NULL) {
            if((type == SPHERE) && (index == i)) {
                continue;
            }

            b = 2 * ((rayDirX * ox) - (rayDirX * spheres[i]->center_x) + (rayDirY * oy) - (rayDirY * spheres[i]->center_y) + (rayDirZ * oz) - (rayDirZ * spheres[i]->center_z));
            c = pow(ox, 2) - (2 * ox * spheres[i]->center_x) + pow(spheres[i]->center_x, 2) +
                        pow(oy, 2) - (2 * oy * spheres[i]->center_y) + pow(spheres[i]->center_y, 2) +
                        pow(oz, 2) - (2 * oz * spheres[i]->center_z) + pow(spheres[i]->center_z, 2) -
                        pow(spheres[i]->radius, 2);


            disc = pow(b, 2) - (4 * c);
            t = -1;
            if(disc == 0) {
                t = -b/2;
            } else if(disc > 0) {
                t = (-b + sqrt(pow(b, 2) - (4*c))) / 2;
                if(t > 0) {
                    return 1;
                }
            }

            if(t > 0) {
                return 1;
            }

        } else {
            break;
        }
    }

    double eD, nr;

    for(int i = 0; i < MAX_CUBES; ++i) {
        if(cubes[i] != NULL) {
            if((type == CUBE) && (index == i)) {
                continue;
            }

            for(int j = 0; j < 6; ++j) {
                square_t* currSqr = cubes[i]->faces[j];

                nr = dot(currSqr->nx, currSqr->ny, currSqr->nz, rayDirX, rayDirY, rayDirZ);
                if(nr >= 0) {
                    continue;
                }

                eD = -1 * ((currSqr->nx * currSqr->vertices[0]) + (currSqr->ny * currSqr->vertices[1]) + (currSqr->nz * currSqr->vertices[2]));
                t = (-1 * ((currSqr->nx * ox) + (currSqr->ny * oy) + (currSqr->nz * oz) + eD)) / nr;

                if(t > 0) {
                    double tpx = ox + rayDirX*t;
                    double tpy = oy + rayDirY*t;
                    double tpz = oz + (rayDirZ*t);

                    if(pointInSquare(tpx, tpy, tpz, currSqr)) {
                        return 1;
                    }
                }
            }
        } else {
            break;
        }
    }

    if((floor->mat1 != NULL) && (type != CHECKER)) {
        nr = dot(floor->nx, floor->ny, floor->nz, rayDirX, rayDirY, rayDirZ);

        //back culled, if your floor is back culled... you got bigger problems.
        if(nr <= 0) {
            return 0;
        }

        t = (-1 * ((floor->nx * ox) + (floor->ny * oy) + (floor->nz * oz) + floor->scale)) / nr;

        if(t > 0) {
            return 1;
        }
    }

    return 0;
}

void calcColor(double px, double py, double pz, double vx, double vy, double vz, double t, int index, int collideFace, enum Shape currShape, unsigned char *red, unsigned char *green, unsigned char *blue) {
    lights_t* lights = sbuf->lights;
    sphere_t** spheres = sbuf->spheres;
    cube_t** cubes = sbuf->cubes;
    checker_t* floor = sbuf->floor;


    double newRed;
    double newGreen;
    double newBlue;

    if(currShape == CHECKER) {
        double vr = dot(vx, vy, vz, floor->rx, floor->ry, floor->rz);
        material_t* mat;

        int which = whichFloorMat(px, pz, floor->modScale, floor->shiftX, floor->shiftZ, floor->rotY);
        if(which) {
            mat = floor->mat1;
        } else {
            mat = floor->mat2;
        }

        double vrGls = pow(vr, mat->Kgls);

        newRed = mat->ambRed + mat->difRed +
                 (mat->specRed * vrGls);
        newGreen = mat->ambGreen + mat->difGreen +
                 (mat->specGreen * vrGls);
        newBlue = mat->ambBlue + mat->difBlue +
                 (mat->specBlue * vrGls);

    } else if(currShape == CUBE) {
        square_t* currSqr = cubes[index]->faces[collideFace];
        double vr = dot(vx, vy, vz, currSqr->rx, currSqr->ry, currSqr->rz);
        smallmat_t* mat = currSqr->mat;

        double vrGls = pow(vr, cubes[index]->mat->Kgls);

        newRed = mat->comboRed +
                 (mat->specRed * vrGls);
        newGreen = mat->comboGreen +
                 (mat->specGreen * vrGls);
        newBlue = mat->comboBlue +
                 (mat->specBlue * vrGls);
    } else {
        material_t* mat = spheres[index]->mat;

        //n = (p - sc) / r
        double nx = (px - spheres[index]->center_x) / spheres[index]->radius;
        double ny = (py - spheres[index]->center_y) / spheres[index]->radius;
        double nz = (pz - spheres[index]->center_z) / spheres[index]->radius;

        double nl = dot(nx, ny, nz, lights->lightDirX, lights->lightDirY, lights->lightDirZ);

        if(nl <= 0) {
            newRed = mat->ambRed;
            newGreen = mat->ambGreen;
            newBlue = mat->ambBlue;
        } else {
            double rx = (2 * nl * nx) - lights->lightDirX;
            double ry = (2 * nl * ny) - lights->lightDirY;
            double rz = (2 * nl * nz) - lights->lightDirZ;
            double vr = dot(vx, vy, vz, rx, ry, rz);
            double subspec = pow(vr, mat->Kgls);

            newRed = mat->ambRed +
                 (mat->difRed * nl) +
                 (mat->specRed * subspec);

            newGreen = mat->ambGreen +
                   (mat->difGreen * nl) +
                   (mat->specGreen * subspec);

            newBlue = mat->ambBlue +
                   (mat->difBlue * nl) +
                   (mat->specBlue * subspec);
        }
    }

    *red = convertToPixel(newRed);
    *green = convertToPixel(newGreen);
    *blue = convertToPixel(newBlue);
}
