#include <math.h>
#include <semaphore.h>

#include "config.h"
#include "tracerMath.h"

// This file handles copying objects into each other, parsing the starting data,
//   and filling each object with its appropriate information.


int whichFloorMat(double x, double z, double modScale, double shiftX, double shiftZ, double rotY) {
    double cosr = cos(-rotY);
    double sinr = sin(-rotY);

    //technical bug, multiply by modscale
    double newX = (x * cosr) + (z * sinr);
    double newZ = (x * (-1 * sinr)) + (z * cosr);

    double resx = newX - shiftX;
    double resz = newZ - shiftZ;

    int BLUE = 0;
    int WHITE = 1;

    resx = fmod(resx, 2 * modScale);
    resz = fmod(resz, 2 * modScale);

    //multiply by modscale
    unsigned char xwhite = ((resx < -1) || ((resx < 1) && (resx >= 0)));
    unsigned char zwhite = ((resz < -1) || ((resz < 1) && (resz >= 0)));

    if(xwhite ^ zwhite) {
        return WHITE;
    } else {
        return BLUE;
    }
}

void normalizeVector(double *x, double *y, double *z) {
    double mag = sqrt((pow(*x, 2.0) + pow(*y, 2.0) + pow(*z, 2.0)));
    *x = *x / mag;
    *y = *y / mag;
    *z = *z / mag;
}

double dot(double x1, double y1, double z1, double x2, double y2, double z2) {
    return (x1 * x2) + (y1 * y2) + (z1 * z2);
}

void cross(double x1, double y1, double z1, double x2, double y2, double z2, double* xf, double* yf, double* zf) {
    *xf = (y1 * z2) - (z1 * y2);
    *yf = (z1 * x2) - (x1 * z2);
    *zf = (x1 * y2) - (y1 * x2);
}

char pointInSquare(double px, double py, double pz, square_t* square) {
    double* vertices = square->vertices;

//    0 < am . ab < ab . ab
// &&  0 < am . ac < ac . ac

    double amx = px - vertices[0];
    double amy = py - vertices[1];
    double amz = pz - vertices[2];

    double abx = vertices[3] - vertices[0];
    double aby = vertices[4] - vertices[1];
    double abz = vertices[5] - vertices[2];

    double amab = dot(amx, amy, amz, abx, aby, abz);
    if(amab < 0) {
        return 0;
    }

    double abab = dot(abx, aby, abz, abx, aby, abz);
    if(abab < amab) {
        return 0;
    }

    double acx = vertices[9] - vertices[0];
    double acy = vertices[10] - vertices[1];
    double acz = vertices[11] - vertices[2];

    double amac = dot(amx, amy, amz, acx, acy, acz);
    if(amac < 0) {
        return 0;
    }

    if(abab < amac) {
        return 0;
    }

    return 1;
}


unsigned char convertToPixel(double value) {
    if(value > 1) {
        value = 1;
    } else if(value < 0) {
        value = 0;
    }

    return (unsigned char)round(255*value);
}
