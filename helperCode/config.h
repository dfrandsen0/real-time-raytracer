#ifndef CONFIG_H
#define CONFIG_H

#include <semaphore.h>

// This file contains all of the macros and structs used throughout the program.

#define WINDOW_HEIGHT 5
#define WINDOW_WIDTH 5
#define PIXELS_X 512
#define PIXELS_Y 512
#define D 5
#define MAX_SPHERES 4
#define MAX_CUBES 2
#define MAX_OBJECT 786432
#define MAX_SYNC 16
#define NUM_THREADS 4 //NOT robust for different thread numbers. Careful not to split pixels!
#define MAX_FILE_NAME 32
#define NUM_FILES 24
#define MAX_COMMANDS 12
#define W_VALUE 0.3
#define S_VALUE 0.3
#define A_VALUE 0.3
#define D_VALUE 0.3
#define E_VALUE 0.1
#define Q_VALUE 0.1
#define NEW_CUBE_DELAY 120
#define NEW_CUBE_DISTANCE 180

typedef struct {
        double lightDirX, lightDirY, lightDirZ;
        double red, green, blue;
        double ambRed, ambGreen, ambBlue;
        double backRed, backGreen, backBlue;
} lights_t;

typedef struct {
        double Kd, Ks, Ka;
        double OdRed, OdGreen, OdBlue;
        double OsRed, OsGreen, OsBlue;
        double Kgls;
        double ambRed, ambGreen, ambBlue;
        double difRed, difGreen, difBlue;
        double specRed, specGreen, specBlue;
} material_t;

typedef struct {
        double difRed, difGreen, difBlue;
        double specRed, specGreen, specBlue;
        double comboRed, comboGreen, comboBlue;
} smallmat_t;

typedef struct {
        double center_x, center_y, center_z;
        double radius;
        material_t* mat;
} sphere_t;

typedef struct {
        double startVertices[12];
        double vertices[12];
        double nx, ny, nz;
        double rx, ry, rz;
        smallmat_t* mat;
        int inShadow;
} square_t;

typedef struct {
        double vertices[24];
        square_t* faces[6];
        material_t* mat;
} cube_t;

typedef struct {
        double tranX, tranY, tranZ;
        double rotX, rotY, rotZ;
        double vtx, vty, vtz;
        double vrx, vry, vrz;
} cubemove_t;

typedef struct {
        double nx, ny, nz;
        double scale;
        double modScale;
        double rx, ry, rz;
        material_t* mat1;
        material_t* mat2;
        double shiftX, shiftZ, rotY;
        int inShadow;
} checker_t;

typedef struct {
        int startOffset;
        int stopOffset;
        int threadNum;
} writer_t;

typedef struct {
        unsigned char pixelArray[MAX_OBJECT];
        lights_t* lights;
        sphere_t* spheres[MAX_SPHERES];
        cube_t* cubes[MAX_CUBES];
        checker_t* floor;
        char* sharedAddr;
} sbuf_t;

typedef struct {
        sem_t writers[NUM_THREADS];
        sem_t writers2[NUM_THREADS];
        volatile int writersFinished[NUM_THREADS];
        char* syncAddr;
} sinfo_t;

typedef struct {
        cubemove_t* cubesMoves[MAX_CUBES];
        double camTranX, camTranZ;
        double camRY;
        material_t* nextCubeMaterial;
        unsigned int newCubeDelay;
        unsigned char nextCubeIndex;
        char currentBuf;
} currinfo_t;

typedef struct {
        lights_t* startLights;
        cube_t* startCube;
        checker_t* floor;
} starter_t;

enum Shape {
        SPHERE,
        CUBE,
        CHECKER
};

#endif
