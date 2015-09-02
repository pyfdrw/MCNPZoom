#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>

typedef struct McnpFillStruct
{
    short int* element;
    int dimxsup; int dimxinf;
    int dimysup; int dimyinf;
    int dimzsup; int dimzinf;
    float voxelsizex;
    float voxelsizey;
    float voxelsizez;
    int voxelcount;
}McnpFillStruct;
typedef struct Coordinate3D
{
    int x;
    int y;
    int z;
}Coordinate3D;
typedef struct VoxelReplaceRecord
{
    int countnum;  //count id nums
    int* record;   // id + num of times + id + num of times + .....
}VoxelReplaceRecord;

////////////////////////////////////////////////////////////////

int readMCNPFile(char*& inputpath, FILE** mcnpFile);
int storeGeo(FILE* mcnpfile, McnpFillStruct& mcnpgeo999);
void storeDensityAndMaterial(float *ORGANSDESTINY, int *ORGANMATERIAL, char* inputpath);
int convertXYZToSequence(int x, int y, int z, McnpFillStruct mcnpgeo999);
void correctPosition(McnpFillStruct& mcnpgeo999);
int outputGeo(McnpFillStruct mcnpgeo999);
int compareGeo(McnpFillStruct mcnpgeoA, McnpFillStruct mcnpgeoB);
int replacEyeLence(McnpFillStruct mcnpgeoA, McnpFillStruct mcnpgeoB, unsigned int idnum);
int replaceVoxelId(McnpFillStruct mcnpgeoA, unsigned int idnum, unsigned int idnumed);
Coordinate3D convertSequenceToXYZ(int sequence, McnpFillStruct mcnpgeo999);
void lenceCopy(unsigned int lenceleft, unsigned int lenceright, McnpFillStruct mcnpgeo999);
int lenceGen(char* inputpath, unsigned int lence, unsigned int bulb, McnpFillStruct& mcnpgeo999);
void organCopy(unsigned int organfrom, unsigned int organfor, McnpFillStruct mcnpgeo999);
Coordinate3D* calBox(McnpFillStruct mcnpgeoA, int organid);  // test
void calSurfaceWL(McnpFillStruct mcnpgeoA, float zheight);  //����߶�zheightʱ����ģ�ϲ����Ŀ�ͳ� 

void adjustAdrenal(unsigned int adrenalid, float aimvol, McnpFillStruct mcnpgeo999);
void adjustlence(unsigned int lenseid, float aimvol, McnpFillStruct mcnpgeo999);
void volAdjust(char* inputpath, McnpFillStruct mcnpgeo999);
void volAdjustAOrgan(int organid, float refvol, McnpFillStruct mcnpgeo999);;
float calOrganVol(unsigned int organid, McnpFillStruct mcnpgeo999);
void genForms(char* inputpath, McnpFillStruct mcnpgeo999);

// X��������negative(����x������һ��)��Xpositive(����x������һ��)��ע����ֵΪ��ʱ��������һ�����ز�������������ͬ��
int geoZoom(McnpFillStruct& mcnpgeo999, int Xnegative, int Xpositive, int Ynegatiove, int Ypositive, int Znegatiove, int Zpositive);
// dir��������negatiove(����dir������һ��)��positive(����dir������һ��),dir����ȡ'X','Y','Z'
int geoZoomSingledir(McnpFillStruct& mcnpgeo999, int negatiove, int positive, char dir);