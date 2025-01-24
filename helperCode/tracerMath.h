#ifndef TRACER_MATH_H
#define TRACER_MATH_H

// This file handles basic math, dot products, cross products, converting color values
//   into usable pixel values, etc. Also determines texture mapping for the infinite plane.

int whichFloorMat(double x, double z, double scale, double shiftX, double shiftZ, double rotY);
void normalizeVector(double *x, double *y, double *z);
double dot(double x1, double y1, double z1, double x2, double y2, double z2);
void cross(double x1, double y1, double z1, double x2, double y2, double z2, double* xf, double* yf, double* zf);
char pointInSquare(double px, double py, double pz, square_t* square);
unsigned char convertToPixel(double value);


#endif
