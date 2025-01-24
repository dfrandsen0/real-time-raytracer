#ifndef SETUP_H
#define SETUP_H

// This file handles copying objects into each other, parsing the starting data,
//   and filling each object with its appropriate information.

void parseStarters(unsigned char* fileName);
cube_t* createCube();
void colorCube(cube_t* cube);
void assignCubeVertices(cube_t* cube);
void fillObjConsts();
void fillObjVars(sbuf_t* buffer);
void fillMaterialConsts(material_t* mat, lights_t* lights);
void fillCubeVars(cube_t* cube, sbuf_t* buffer);
void fillFloorVars(sbuf_t* buffer);
void copyInitBuffer(sbuf_t* sbuf1, sbuf_t* sbuf2);
void copyLights(lights_t* oldLights, lights_t* newLights);
void copySpheres(sphere_t** oldSpheres, sphere_t** newSpheres);
void copyCubes(cube_t** oldCubes, cube_t** newCubes);
void copyCube(cube_t* oldCube, cube_t* newCube);
void copySquare(square_t* oldSquare, square_t* newSquare);
void copyFloor(checker_t* oldFloor, checker_t* newFloor);
void copyMaterial(material_t* oldMaterial, material_t* newMaterial);

#endif
