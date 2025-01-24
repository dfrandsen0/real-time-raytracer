#ifndef LOOP_H
#define LOOP_H

// The "loop" files are for functions mainly used by the main game loop, and the math
//   behind each pixel.

void assign(int offset, unsigned char red, unsigned char green, unsigned char blue);
void writeToSharedMemory();
void initSharedMemory(sbuf_t* sbuf1, unsigned char* fileName);
void initSyncMemory();
void fillPixels(int startOffset, int stopOffset);
void computePixel(double rayDirX, double rayDirY, double rayDirZ, unsigned char *red, unsigned char *green, unsigned char *blue);
void calcColor(double px, double py, double pz, double vx, double vy, double vz, double t, int index, int collideFace, enum Shape currShape, unsigned char *red, unsigned char *green, unsigned char *blue);
char inShadow(double ox, double oy, double oz, enum Shape type, int index);

#endif
