// mcnp重复结构的读取
// 重复结构的缩放
#include "datastructure.h"

int main()
{
    FILE* mcnpfileA;
    char* inputpath; // = "E://MCNP//AM3mm//AM3.txt";
    readMCNPFile(inputpath, &mcnpfileA);
    //mcnpfileA = fopen(inputpath, "r");
    McnpFillStruct mcnpgeoA;
    storeGeo(mcnpfileA, mcnpgeoA);
    
    //volAdjustAOrgan(int organid, float refvol, McnpFillStruct mcnpgeo999);
    //volAdjustAOrgan(int organid, float refvol, McnpFillStruct mcnpgeo999);

    //cal skin box
    Coordinate3D* skinbox;

    skinbox = calBox(mcnpgeoA, 125);
    int Xnegatiove = mcnpgeoA.dimxinf - (*skinbox).x;
    int Xpositive  = -mcnpgeoA.dimxsup + (*(skinbox + 1)).x;
    int Ynegatiove = mcnpgeoA.dimyinf - (*skinbox).y;
    int Ypositive  = -mcnpgeoA.dimysup + (*(skinbox + 1)).y;
    int Znegatiove = mcnpgeoA.dimzinf - (*skinbox).z;
    int Zpositive  = -mcnpgeoA.dimzsup + (*(skinbox + 1)).z;
    geoZoom(mcnpgeoA, Xnegatiove, Xpositive, Ynegatiove, Ypositive, Znegatiove, Zpositive);
    geoZoom(mcnpgeoA, 3, 3, 3, 3, 3, 3);

    outputGeo(mcnpgeoA);
    fclose(mcnpfileA);
    return 0;
}

