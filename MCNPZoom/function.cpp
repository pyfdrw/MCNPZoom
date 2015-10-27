#include "datastructure.h"
#include "objdata.h"

//name of the biggest lattice structure, you can edit it.  
static const char* REPETSTRNUM = "666";

//destiny for every organ, There are 141 organs, 0th organ with 0 destiny
//If a organ with number 15, ORGANSDESTINY[15] is the destiny value of this organ.
const int ORGANNUM = 152;
float ORGANSDESTINY[ORGANNUM] = { 0 }; // store Density data
int ORGANMATERIAL[152] = { 0 };        // store Material data

int readMCNPFile(char*& inputpath, FILE** mcnpFile)
{
    inputpath = new char[500];
    std::cout << "Please input a VOXEL MCNP Phantom filename,\nVOXEL MCNP Phantom filename should be the same file with the exe \nand end you input with enter key!" << std::endl;
    int i = 0; char intmp = '*';
    while ('\n' != intmp)
    {
        intmp = getchar();
        *(inputpath + i) = intmp;
        i++;
    }
    *(inputpath + i - 1) = '\0';
    puts(inputpath);

    *(mcnpFile) = fopen(inputpath, "r");
    if (0 == *mcnpFile)
    {
        std::cout << "Wrong path!" << std::endl;
        exit(0);
    }
    else
        std::cout << "OK!" << std::endl;

    //delete[]path;
    return 0;
}

void storeDensityAndMaterial(float *ORGANSDESTINY, int *ORGANMATERIAL, char* inputpath)
{
    // density.txt located in the same dir with inputpath
    // gei the dir path
    char* dirpath = new char[500];
    strcpy(dirpath, inputpath);

    for (int i = 0; i < strlen(dirpath); i++)
    {
        if (*(dirpath + strlen(dirpath) - i) == '\\' || *(dirpath + strlen(dirpath) - i) == '/')
        {
            *(dirpath + strlen(dirpath) - i + 1) = '\0';
            break;
        }
    }
    strcat(dirpath, "density.txt");
    FILE* densityfile = fopen(dirpath, "r");
    if (0 == densityfile)
    {
        printf("File for density information not exist!\n");
        exit(0);
    }

    char* linetmp = new char[100];
    while (0 != fgets(linetmp, 100, densityfile))  // search cell REPETSTRNUM, '\n' will be insert at the end of linetmp before '\0' by using fgets.
    {
        int organid, matid;
        float densityvalue;
        sscanf(linetmp, "%d%f%d", &organid, &densityvalue, &matid);
        ORGANSDESTINY[organid] = densityvalue;
        ORGANMATERIAL[organid] = matid;
    }
}

int storeGeo(FILE* mcnpfile, McnpFillStruct& mcnpgeo999)
{
    char* linetmp = new char[200]; // max num of char in a line is 99
    char* dimtmp = new char[80];   // for storing dims of cell REPETSTRNUM
    int voxelfillid = -1;
    char* voxelfillidtmp = new char[10];
    while (0 != fgets(linetmp, 190, mcnpfile))  // search cell REPETSTRNUM, '\n' will be insert at the end of linetmp before '\0' by using fgets.
    {
        if (0 == strstr(linetmp, REPETSTRNUM))
            continue;
        else if (strstr(linetmp, REPETSTRNUM) - linetmp <= 2 && 'c' != *linetmp && 'C' != *linetmp)
        {
            // change every capital letters in linetmp to  small letters
            for (int i = 0; i < strlen(linetmp); i++)
            {
                if (*(linetmp + i) >= 'A'&&*(linetmp + i) <= 'Z')
                {
                    *(linetmp + i) = *(linetmp + i) + 32;
                }
            }
            sscanf(linetmp, "%*d%*d%d", &voxelfillid);
            voxelfillid = abs(voxelfillid);
            sprintf(voxelfillidtmp, "%d", voxelfillid);
            if (0 != strstr(linetmp, "fill"))          // obtain dims of mcnpgeo999
            {
                strcpy(dimtmp, 4 + strstr(linetmp, "fill"));
                strcpy(dimtmp, 1 + strchr(dimtmp, '='));
                sscanf(dimtmp, "%d:%d%d:%d%d:%d", &(mcnpgeo999.dimxinf), &(mcnpgeo999.dimxsup), &(mcnpgeo999.dimyinf), &(mcnpgeo999.dimysup), \
                    &(mcnpgeo999.dimzinf), &(mcnpgeo999.dimzsup));
                mcnpgeo999.element = new short int[(mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * \
                    (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1)];
                break;
            }
            else
            {
                while (0 != fgets(linetmp, 190, mcnpfile))
                {
                    // change every capital letters in linetmp to  small letters
                    for (int i = 0; i < strlen(linetmp); i++)
                    {
                        if (*(linetmp + i) >= 'A'&&*(linetmp + i) <= 'Z')
                        {
                            *(linetmp + i) = *(linetmp + i) + 32;
                        }
                    }
                    if (0 != strstr(linetmp, "fill"))
                    {
                        strcpy(dimtmp, 4 + strstr(linetmp, "fill"));
                        strcpy(dimtmp, 1 + strchr(dimtmp, '='));
                        sscanf(dimtmp, "%d:%d%d:%d%d:%d", &(mcnpgeo999.dimxinf), &(mcnpgeo999.dimxsup), &(mcnpgeo999.dimyinf), &(mcnpgeo999.dimysup), \
                            &(mcnpgeo999.dimzinf), &(mcnpgeo999.dimzsup));
                        mcnpgeo999.element = new short int[(mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * \
                            (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1)];
                        break;
                    }
                }
                break;
            }
        }
    }

    //Finish repetitive structure info get

    //read every voxel and store them to mcnpgeo999
    int voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
    mcnpgeo999.voxelcount = voxelcount;
    int voxelindex = 0; //Record last voxel ID
    int recordvoxel = 0;  //Record all inserted voxel num
    char* voxelid = new char[20];
    while (1)
    {
        fscanf(mcnpfile, "%s", voxelid);
        if ('r' == *(voxelid + strlen(voxelid) - 1) || 'R' == *(voxelid + strlen(voxelid) - 1))
        {
            *(voxelid + strlen(voxelid) - 1) = '\0';
            int nummax;
            sscanf(voxelid, "%d", &nummax);
            for (int i = 0; i < nummax; i++)
            {
                *(mcnpgeo999.element + recordvoxel + i) = voxelindex;
            }
            recordvoxel += nummax;
            /*if (recordvoxel >= 24188400)
            {
            std::cout << recordvoxel << " ";
            }*/
        }
        else
        {
            sscanf(voxelid, "%d", &voxelindex);
            *(mcnpgeo999.element + recordvoxel) = voxelindex;
            recordvoxel++;
            /*if (recordvoxel >= 24188400)
            {
            std::cout << recordvoxel << " ";
            }*/
        }
        if (recordvoxel >= voxelcount)
            break;
    }

    //voxelfillidtmp
    fseek(mcnpfile, 0, SEEK_SET);
    while (0 != fgets(linetmp, 190, mcnpfile))
    {
        if (0 == strlen(linetmp) || '\n' == *(linetmp))
        {
            while (0 != fgets(linetmp, 190, mcnpfile))
            {
                int idtmp = 0;
                if ('c' != *linetmp && 'C' != *linetmp)
                {
                    sscanf(linetmp, "%d", &idtmp);
                    if (voxelfillid == idtmp)
                    {
                        sscanf(linetmp, "%*s %*s %*s %f %*s %f %*s %f", &(mcnpgeo999.voxelsizex), &(mcnpgeo999.voxelsizey), &(mcnpgeo999.voxelsizez));
                        break;
                    }
                }
                else
                {
                    continue;
                }
            }
            break;
        }
        else
        {
            continue;
        }
    }

    delete[]linetmp;
    delete[]dimtmp;
    fclose(mcnpfile);
    return 0;
}

void correctPosition(McnpFillStruct& mcnpgeoA)
{
    int Xmax97 = -1000, Xmin97 = 10000;
    int Xmax99 = -1000, Xmin99 = 10000;

    Coordinate3D coordtmp;
    for (int i = 0; i < mcnpgeoA.voxelcount; i++)
    {
        if (97 == *(mcnpgeoA.element + i))
        {
            coordtmp = convertSequenceToXYZ(i, mcnpgeoA);
            Xmax97 = Xmax97 > coordtmp.x ? Xmax97 : coordtmp.x;
            Xmin97 = Xmin97 < coordtmp.x ? Xmin97 : coordtmp.x;
        }
        if (99 == *(mcnpgeoA.element + i))
        {
            coordtmp = convertSequenceToXYZ(i, mcnpgeoA);
            Xmax99 = Xmax99 > coordtmp.x ? Xmax99 : coordtmp.x;
            Xmin99 = Xmin99 < coordtmp.x ? Xmin99 : coordtmp.x;
        }
    }

    if ((Xmin99 + Xmax99) < (Xmin97 + Xmax97)) //x and y wrong
    {
        short int* elementtmp = new short int[mcnpgeoA.voxelcount];
        for (int i = 0; i < mcnpgeoA.voxelcount; i++)
        {
            Coordinate3D xxyyzz = convertSequenceToXYZ(i, mcnpgeoA);
            xxyyzz.x = (mcnpgeoA.dimxinf + mcnpgeoA.dimxsup) - xxyyzz.x;
            *(elementtmp + convertXYZToSequence(xxyyzz.x, xxyyzz.y, xxyyzz.z, mcnpgeoA)) = *(mcnpgeoA.element + i);
        }

        delete[]mcnpgeoA.element;
        mcnpgeoA.element = elementtmp;
    }
}

int outputGeo(McnpFillStruct mcnpgeo999)
{
    std::cout << "Output mcnpfile,Input name. name.txt should put into the same dir with you output file" << std::endl;
    char *path = new char[500];
    char *organlistpath = new char[500];
    int i = 0; char intmp = '*';
    while ('\n' != intmp)
    {
        intmp = getchar();
        *(path + i) = intmp;
        i++;
    }
    *(path + i - 1) = '\0';
    std::cout << "Output file located in: ";
    puts(path);
    FILE* outputfile;
    if (outputfile = fopen(path, "w+"))
        std::cout << "OK" << std::endl;
    else
    {
        std::cout << "Wrong input!" << std::endl;
        exit(0);
    }
    strcpy(organlistpath, path);
    for (int i = 0; i < strlen(organlistpath); i++)
    {
        if (*(organlistpath + strlen(organlistpath) - i) == '\\' || *(organlistpath + strlen(organlistpath) - i) == '/')
        {
            *(organlistpath + strlen(organlistpath) - i + 1) = '\0';
            break;
        }
    }
    strcat(organlistpath, "name.txt");
    char* namelist = new char[200 * 80]; for (int i = 0; i < 200; i++) *(namelist + i * 80) = '\0';
    FILE* organlistfile = fopen(organlistpath, "r");
    char* linetmp = new char[200];   //Storing a line temporarily

    if (0 == organlistfile)
        std::cout << "name.txt not exist!", exit(0);
    else
    {
        int organname = 0;
        while (0 != fgets(linetmp, 190, organlistfile))
        {
            sscanf(linetmp, "%d", &organname);
            sscanf(linetmp, "%*s%[^\n]", namelist + 80 * organname);
        }
    }


    strcpy(linetmp, "      "); *(linetmp + 6) = '\0';
    int repetcount = 0;
    int lastvoxelid = -1;
    char* voxelidtmp = new char[30];
    *voxelidtmp = '\0';
    fprintf(outputfile, "%s %s\n", path, "input file");
    fprintf(outputfile, "%s", "C ******************************************************************************\n");
    fprintf(outputfile, "%s", "C                               Cells cards\n");
    fprintf(outputfile, "%s", "C ******************************************************************************\n");
    fprintf(outputfile, "%s", "888   0  2                          $ exterior - zero importance\n");
    //fprintf(outputfile, "%s", "777   21   -0.001293   2   -4       $ bigger box   \n");
    fprintf(outputfile, "%s", "555   0  -2     fill=666            $ Others phantom box \n");

    fprintf(outputfile, "%s", "C ****************  Image data start from here *******************************\n");
    fprintf(outputfile, "%s%.3f X %.3f X %.3f %c", "C Voxel size : ", mcnpgeo999.voxelsizex, mcnpgeo999.voxelsizey, mcnpgeo999.voxelsizez, '\n');
    fprintf(outputfile, "C Voxel size : %5d %5d %5d\n", (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1), (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1), (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1));
    fprintf(outputfile, "%s %s%s%s%d:%d %d:%d %d:%d", REPETSTRNUM, " 0  -1  lat=1 u=", REPETSTRNUM, " fill=", mcnpgeo999.dimxinf, mcnpgeo999.dimxsup, mcnpgeo999.dimyinf, mcnpgeo999.dimysup, mcnpgeo999.dimzinf, mcnpgeo999.dimzsup);
    fputc('\n', outputfile);

    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (75 <= strlen(linetmp))
        {
            fputs(linetmp, outputfile);
            fputc('\n', outputfile);
            *linetmp = '\0';
            strcpy(linetmp, "      "); *(linetmp + 6) = '\0';
        }
        if (0 == repetcount)
        {
            if (lastvoxelid == *(mcnpgeo999.element + i))
            {
                repetcount++;
                continue;
            }
            else
            {
                lastvoxelid = *(mcnpgeo999.element + i);
                sprintf(voxelidtmp, "%d", lastvoxelid);
                if (79 > strlen(voxelidtmp) + strlen(linetmp))
                {
                    strcat(linetmp, voxelidtmp);
                    strcat(linetmp, " ");
                }
                else
                {
                    fputs(linetmp, outputfile);
                    ///////
                    fputc('\n', outputfile);
                    strcpy(linetmp, "      "); *(linetmp + 6) = '\0';
                    strcat(linetmp, voxelidtmp);
                    strcat(linetmp, " ");
                }
            }
        }
        else
        {
            if (lastvoxelid == *(mcnpgeo999.element + i))
            {
                repetcount++;
                continue;
            }
            else
            {
                sprintf(voxelidtmp, "%d", repetcount);
                if (78 - 5 > strlen(voxelidtmp) + strlen(linetmp))
                {
                    strcat(linetmp, voxelidtmp);
                    strcat(linetmp, "r ");
                    lastvoxelid = *(mcnpgeo999.element + i);
                    sprintf(voxelidtmp, "%d", lastvoxelid);
                    strcat(linetmp, voxelidtmp);
                    strcat(linetmp, " ");
                    repetcount = 0;
                }
                else
                {
                    fputs(linetmp, outputfile);
                    fputc('\n', outputfile);
                    strcpy(linetmp, "      "); *(linetmp + 6) = '\0';
                    strcat(linetmp, voxelidtmp);
                    strcat(linetmp, "r ");
                    lastvoxelid = *(mcnpgeo999.element + i);
                    sprintf(voxelidtmp, "%d", lastvoxelid);
                    strcat(linetmp, voxelidtmp);
                    strcat(linetmp, " ");
                    repetcount = 0;
                }
            }
        }
    }

    sprintf(voxelidtmp, "%d", repetcount);
    if (77 > strlen(voxelidtmp) + strlen(linetmp))
    {
        strcat(linetmp, voxelidtmp);
        strcat(linetmp, "r ");
        fputs(linetmp, outputfile);
        fputc('\n', outputfile);
    }
    else
    {
        fputs(linetmp, outputfile);
        fputc('\n', outputfile);
        strcat(voxelidtmp, "r ");
        fputs("     ", outputfile);
        fputs(voxelidtmp, outputfile);
        fputc('\n', outputfile);
    }
    //no more than 200 organs
    int* organscount = new int[200]; //count organ num
    for (int i = 0; i < 200; i++)  *(organscount + i) = 0;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
        *(organscount + *(mcnpgeo999.element + i)) = *(organscount + *(mcnpgeo999.element + i)) + 1;
    for (int i = 0; i < 200; i++)
    {
        if ('\0' != *(namelist + i * 80))
        {
            fprintf(outputfile, "%-5d %-5d %-5d %s %-4d %s %-12.4f %s %-10d", \
                i, i, 3, " u=", i, " vol=", *(organscount + i)*mcnpgeo999.voxelsizex*mcnpgeo999.voxelsizey*mcnpgeo999.voxelsizez, "   $n=", *(organscount + i));

            fprintf(outputfile, "  %s", (namelist + i * 80));
            fputc('\n', outputfile);
        }
    }

    //surface cards
    fputc('\n', outputfile);
    fprintf(outputfile, "%s", "C ******************************************************************************\n");
    fprintf(outputfile, "%s", "C                               Surface cards\n");
    fprintf(outputfile, "%s", "C ******************************************************************************\n");
    fprintf(outputfile, "1     %s %-4.2f %-4.2f %-4.2f %-4.2f %-4.2f %-4.2f $Voxel size\n", "rpp", 0.0, mcnpgeo999.voxelsizex, 0.0, mcnpgeo999.voxelsizey, 0.0, mcnpgeo999.voxelsizez);
    fprintf(outputfile, "2     rpp 0.0 %.4f 0.0 %.4f 0.0 %.4f $ Box\n", mcnpgeo999.voxelsizex*(mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1), mcnpgeo999.voxelsizey*(mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1), mcnpgeo999.voxelsizez*(mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1));
    fprintf(outputfile, "3     pz  -1e2  $ XY plane used in universe definition\n");
    //fprintf(outputfile, "4     rpp -20 80 -20 60 -20 200 $ Box");
    delete[]linetmp;
    delete[]organlistpath;

    fclose(outputfile);
    return 0;
}

// return -1 represents differents dims between mcnpgeoA and mcnpgeoB
// return the number of different elements between mcnpgeoA and mcnpgeoB
int compareGeo(McnpFillStruct mcnpgeoA, McnpFillStruct mcnpgeoB)
{
    if (mcnpgeoA.dimxinf != mcnpgeoB.dimxinf || mcnpgeoA.dimxsup != mcnpgeoB.dimxsup || mcnpgeoA.dimyinf != mcnpgeoB.dimyinf || mcnpgeoA.dimysup != mcnpgeoB.dimysup || mcnpgeoA.dimzsup != mcnpgeoB.dimzsup || mcnpgeoA.dimzinf != mcnpgeoB.dimzinf)
    {
        return -1;
    }
    else
    {
        int count = 0;
        for (int i = 0; i < mcnpgeoA.voxelcount; i++)
        {
            if (*(mcnpgeoA.element + i) != *(mcnpgeoB.element + i))
            {
                count++;
            }
        }
        return count;
    }
}

int replacEyeLence(McnpFillStruct mcnpgeoA, McnpFillStruct mcnpgeoB, unsigned int idnum)
{
    VoxelReplaceRecord replacerecord;
    replacerecord.countnum = 0;
    replacerecord.record = new int[50];

    for (int i = 0; i < mcnpgeoB.voxelcount; i++)
    {
        if (*(mcnpgeoB.element + i) == idnum)
        {
            int j = 0;
            for (; j < 2 * replacerecord.countnum; j += 2)
            {
                if (*(mcnpgeoA.element + i) == *(replacerecord.record + j))
                {
                    *(replacerecord.record + j + 1) = *(replacerecord.record + j + 1) + 1;
                }
            }
            if (j = 2 * replacerecord.countnum)
            {
                *(replacerecord.record + j) = *(mcnpgeoA.element + i);
                *(replacerecord.record + j + 1) = 1;
            }
            *(mcnpgeoA.element + i) = idnum;
        }
    }

    for (int i = 0; i < 2 * replacerecord.countnum; i += 2)
    {
        std::cout << " " << *(replacerecord.record + i) << " " << *(replacerecord.record + i + 1);
    }
    std::cout << std::endl;

    return 0;
}

//idnum is used for replace idnumed
int replaceVoxelId(McnpFillStruct mcnpgeoA, unsigned int idnum, unsigned int idnumed)
{
    int count = 0;
    for (int i = 0; i < mcnpgeoA.voxelcount; i++)
    {
        if (*(mcnpgeoA.element + i) == idnumed)
        {
            *(mcnpgeoA.element + i) = idnum;
            count++;
        }
    }
    return count;
}

void lenceCopy(unsigned int lenceleft, unsigned int lenceright, McnpFillStruct mcnpgeo999)
{
    Coordinate3D XYZtmp;
    //int symmetricalline;
    int symmetricalsequence = 0;
    int count = 0;
    //symmetricalline = (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1 + 1)/2;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (lenceleft == *(mcnpgeo999.element + i))
        {
            XYZtmp = convertSequenceToXYZ(i, mcnpgeo999);
            //Calculate the symmetrical point
            XYZtmp.x = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1 + 1) - XYZtmp.x;
            symmetricalsequence = convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999);
            if ((lenceright + 1) == *(mcnpgeo999.element + symmetricalsequence))
            {
                *(mcnpgeo999.element + symmetricalsequence) = lenceright;
                count++;
            }
        }
    }
    std::cout << "Replace num = " << count << std::endl;
}

void organCopy(unsigned int organfrom, unsigned int organfor, McnpFillStruct mcnpgeo999)
{
    Coordinate3D XYZtmp;
    //int symmetricalline;
    int symmetricalsequence = 0;
    int count = 0;
    //symmetricalline = (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1 + 1)/2;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (organfrom == *(mcnpgeo999.element + i))
        {
            XYZtmp = convertSequenceToXYZ(i, mcnpgeo999);
            //Calculate the symmetrical point
            XYZtmp.x = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1 + 1) - XYZtmp.x;
            symmetricalsequence = convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999);
            if ((106 <= *(mcnpgeo999.element + symmetricalsequence) && 109 >= *(mcnpgeo999.element + symmetricalsequence)) ||
                119 == *(mcnpgeo999.element + symmetricalsequence))
            {
                *(mcnpgeo999.element + symmetricalsequence) = organfor;
                count++;
            }
        }
    }
    std::cout << "Replace num = " << count << std::endl;
}

int lenceGen(char* inputpath, unsigned int lence, unsigned int bulb, McnpFillStruct& mcnpgeo999)
{
    int judgetmp = 0; // 判断bulb是不是不存在，防止多做操作
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (lence == *(mcnpgeo999.element + i))
        {
            judgetmp++;
        }
    }

    if (judgetmp >0)
    {
        std::cout << "\n" << lence << " lence already exist!" << std::endl;
        return 1;
    }
    //Generate dirpath
    char* dirpath = new char[500];
    char* objfilepath = new char[500]; // store lence.obj path
    *objfilepath = '\0';
    for (int i = 0; i < strlen(inputpath); i++)
    {
        if (*(inputpath + strlen(inputpath) - i) == '\\' || *(inputpath + strlen(inputpath) - i) == '/')
        {
            for (int j = 0; j <= strlen(inputpath) - i; j++)
            {
                *(dirpath + j) = *(inputpath + j);
            }
            *(dirpath + strlen(inputpath) - i + 1) = '\0';
            break;
        }
    }
    strcat(objfilepath, dirpath);
    sprintf(objfilepath + strlen(objfilepath), "%d%s", lence, ".obj");
    FILE* objfile;
    objfile = fopen(objfilepath, "r");
    if (0 == objfile)
    {
        puts(objfilepath);
        puts("not exist!");

        delete[200]dirpath;

        return 2;
    }
    else
    {
        fclose(objfile);
        ObjGeo objgeo;
        storeGeo(objfilepath, objgeo);
        float lenceradius;
        float lencerefvolume = fabs(calGeoVol(calGeoCenter(objgeo), objgeo));
        lenceradius = pow(lencerefvolume / 4.0 * 3.0 / 3.14159, 1.0 / 3.0);
        int maxreplacenum = lencerefvolume / (mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey * mcnpgeo999.voxelsizez);

        Point lencecenter;

        float Xminbulb = 10000, Xmaxbulb = -10000, Yminbulb = 10000, Ymaxbulb = -10000, Zminbulb = 10000, Zmaxbulb = -10000;
        for (int i = 0; i < mcnpgeo999.voxelcount; i++)
        {
            if (bulb == *(mcnpgeo999.element + i))
            {
                Coordinate3D XYZ = convertSequenceToXYZ(i, mcnpgeo999);
                Xminbulb = Xminbulb >  XYZ.x ? XYZ.x : Xminbulb;
                Xmaxbulb = Xmaxbulb <  XYZ.x ? XYZ.x : Xmaxbulb;
                Yminbulb = Yminbulb >  XYZ.y ? XYZ.y : Yminbulb;
                Ymaxbulb = Ymaxbulb <  XYZ.y ? XYZ.y : Ymaxbulb;
                Zminbulb = Zminbulb >  XYZ.z ? XYZ.z : Zminbulb;
                Zmaxbulb = Zmaxbulb <  XYZ.z ? XYZ.z : Zmaxbulb;
            }
        }
        Xminbulb = Xminbulb * mcnpgeo999.voxelsizex;
        Xmaxbulb = (Xmaxbulb + 1) * mcnpgeo999.voxelsizex;
        Yminbulb = Yminbulb * mcnpgeo999.voxelsizey;
        Ymaxbulb = (Ymaxbulb + 1) * mcnpgeo999.voxelsizey;
        Zminbulb = Zminbulb * mcnpgeo999.voxelsizez;
        Zmaxbulb = (Zmaxbulb + 1) * mcnpgeo999.voxelsizez;
        //包围盒 xminbulb xmaxbulb yminbulb ymaxbulb zminbulb zmaxbulb for bulb

        lencecenter.setcoorX((Xmaxbulb + Xminbulb) / 2.0);
        lencecenter.setcoorY((Ymaxbulb + Yminbulb) / 2.0 > (Ymaxbulb - 1.5 * lenceradius) ? (Ymaxbulb + Yminbulb) / 2.0 : (Ymaxbulb - 1.5 * lenceradius));
        lencecenter.setcoorZ((Zmaxbulb + Zminbulb) / 2.0);

        int replacecount = 0;
        for (int i = 0; i < mcnpgeo999.voxelcount; i++)
        {
            if (bulb == *(mcnpgeo999.element + i))
            {
                Coordinate3D XYZ = convertSequenceToXYZ(i, mcnpgeo999);
                float coordX = XYZ.x * mcnpgeo999.voxelsizex;
                float coordY = XYZ.y * mcnpgeo999.voxelsizey;
                float coordZ = XYZ.z * mcnpgeo999.voxelsizez;
                if ((coordX - lencecenter.getcoorX()) * (coordX - lencecenter.getcoorX()) +
                    (coordY - lencecenter.getcoorY()) * (coordY - lencecenter.getcoorY()) +
                    (coordZ - lencecenter.getcoorZ()) * (coordZ - lencecenter.getcoorZ()) <= lenceradius * lenceradius * 1.2) //distance
                {
                    *(mcnpgeo999.element + i) = lence;
                    replacecount++;
                    if (replacecount >= maxreplacenum)
                    {
                        break;
                    }
                }
            }
        }

        std::cout << replacecount << " bulbs were replaced" << std::endl;
        delete[]objfilepath;
        delete[]dirpath;

        return 0;
    }
}

Coordinate3D* calBox(McnpFillStruct mcnpgeoA, int organid)
{
    int Xmax = -1000, Xmin = 10000;
    int Ymax = -1000, Ymin = 10000;
    int Zmax = -1000, Zmin = 10000;

    Coordinate3D coordtmp;
    for (int i = 0; i < mcnpgeoA.voxelcount; i++)
    {
        if (organid == *(mcnpgeoA.element + i))
        {
            coordtmp = convertSequenceToXYZ(i, mcnpgeoA);
            Xmax = Xmax > coordtmp.x ? Xmax : coordtmp.x;
            Xmin = Xmin < coordtmp.x ? Xmin : coordtmp.x;
            Ymax = Ymax > coordtmp.y ? Ymax : coordtmp.y;
            Ymin = Ymin < coordtmp.y ? Ymin : coordtmp.y;
            Zmax = Zmax > coordtmp.z ? Zmax : coordtmp.z;
            Zmin = Zmin < coordtmp.z ? Zmin : coordtmp.z;
        }
    }

    std::cout << "=========BOX============" << std::endl;
    std::cout << "Xmin = " << Xmin << "  Xmax" << Xmax << std::endl;
    std::cout << "Ymin = " << Ymin << "  Ymax" << Ymax << std::endl;
    std::cout << "Zmin = " << Zmin << "  Zmax" << Zmax << std::endl;

    Coordinate3D* skinbox = new Coordinate3D[2];
    (*skinbox).x = Xmin; (*(skinbox + 1)).x = Xmax;
    (*skinbox).y = Ymin; (*(skinbox + 1)).y = Ymax;
    (*skinbox).z = Zmin; (*(skinbox + 1)).z = Zmax;

    return skinbox;
}

void calSurfaceWL(McnpFillStruct mcnpgeoA, float zheight)  //计算高度zheight时的体模断层截面的宽和长 
{
    int slicenumtmp = 0;
    slicenumtmp = zheight / mcnpgeoA.voxelsizez;
    slicenumtmp = mcnpgeoA.dimzinf + slicenumtmp;
    int Xmax = -1000, Xmin = 10000;
    int Ymax = -1000, Ymin = 10000;

    Coordinate3D coordtmp;
    for (int i = 0; i < mcnpgeoA.voxelcount; i++)
    {
        coordtmp = convertSequenceToXYZ(i, mcnpgeoA);
        if (coordtmp.z == slicenumtmp && *(mcnpgeoA.element + i) >= 1 && *(mcnpgeoA.element + i) < 150)
        {
            Xmax = Xmax > coordtmp.x ? Xmax : coordtmp.x;
            Xmin = Xmin < coordtmp.x ? Xmin : coordtmp.x;
            Ymax = Ymax > coordtmp.y ? Ymax : coordtmp.y;
            Ymin = Ymin < coordtmp.y ? Ymin : coordtmp.y;
        }
    }

    std::cout << "=====================" << std::endl;
    //std::cout << "Xmin = " << Xmin << "  Xmax = " << Xmax << std::endl;

    //std::cout << "Ymin = " << Ymin << "  Ymax = " << Ymax << std::endl;
    std::cout << "YLength = " << (Ymax - Ymin) * mcnpgeoA.voxelsizey << "   ";
    std::cout << "XLength = " << (Xmax - Xmin) * mcnpgeoA.voxelsizex << std::endl;
}

bool isInside(Point pointnow, ObjGeo objgeo)
{
    int counttmp = 0;
    float Xmin = 10000, Xmax = -10000, Ymin = 10000, Ymax = -10000, Zmin = 10000, Zmax = -10000;
    for (int i = 0; i < objgeo.pointcount; i++)
    {
        Xmin = Xmin >(objgeo.point + i)->getcoorX() ? (objgeo.point + i)->getcoorX() : Xmin;
        Xmax = Xmax < (objgeo.point + i)->getcoorX() ? (objgeo.point + i)->getcoorX() : Xmax;
        Ymin = Ymin >(objgeo.point + i)->getcoorY() ? (objgeo.point + i)->getcoorY() : Ymin;
        Ymax = Ymax < (objgeo.point + i)->getcoorY() ? (objgeo.point + i)->getcoorY() : Ymax;
        Zmin = Zmin >(objgeo.point + i)->getcoorZ() ? (objgeo.point + i)->getcoorZ() : Zmin;
        Zmax = Zmax < (objgeo.point + i)->getcoorZ() ? (objgeo.point + i)->getcoorZ() : Zmax;
    }

    if (pointnow.getcoorX() > Xmin && pointnow.getcoorX() < Xmax &&
        pointnow.getcoorY() > Zmin && pointnow.getcoorY() < Zmax &&
        pointnow.getcoorZ() > Ymin && pointnow.getcoorZ() < Ymax)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int convertXYZToSequence(int x, int y, int z, McnpFillStruct mcnpgeo999)
{
    int sequence = 0;
    sequence = z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
        y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + x;

    return sequence;
}

Coordinate3D convertSequenceToXYZ(int sequence, McnpFillStruct mcnpgeo999)
{
    Coordinate3D XYZ;
    XYZ.z = sequence / ((mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1));
    XYZ.y = (sequence - XYZ.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1)) / ((mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1));
    XYZ.x = sequence - XYZ.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) - XYZ.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);

    return XYZ;
}

void adjustAdrenal(unsigned int adrenalid, float aimvol, McnpFillStruct mcnpgeo999)
{
    int numcount = 0;
    int numcounttmp = 0;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (adrenalid == *(mcnpgeo999.element + i))
        {
            numcount++;
        }
    }
    float adrenalvol = numcount * mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey *mcnpgeo999.voxelsizez;
    int missnum = (aimvol - adrenalvol) / (mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey *mcnpgeo999.voxelsizez);
    if (missnum > 0)
    {
        for (int i = 0; i < mcnpgeo999.voxelcount; i++)
        {
            if (adrenalid == *(mcnpgeo999.element + i))
            {
                //std::cout << *(mcnpgeo999.element + i - 1) << std::endl;
                if ((*(mcnpgeo999.element + i - 1) >= 106 && *(mcnpgeo999.element + i - 1) <= 109) || 119 == (*(mcnpgeo999.element + i - 1)) ||
                    150 == (*(mcnpgeo999.element + i - 1)))
                {
                    *(mcnpgeo999.element + i - 1) = adrenalid;
                    numcounttmp++;
                }
            }
            if (numcounttmp >= missnum)
            {
                //std::cout << "adrenal numcounttmp =" << numcounttmp << std::endl;
                break;
            }
        }
    }
    std::cout << "adrenal numcounttmp =" << numcounttmp << std::endl;
}

void adjustlence(unsigned int lenseid, float aimvol, McnpFillStruct mcnpgeo999)
{
    int numcount = 0;
    int numcounttmp = 0;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (lenseid == *(mcnpgeo999.element + i))
        {
            numcount++;
        }
    }
    float adrenalvol = numcount * mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey *mcnpgeo999.voxelsizez;
    int missnum = (aimvol - adrenalvol) / (mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey *mcnpgeo999.voxelsizez);
    std::cout << "lense miss num = " << missnum << std::endl;
    if (missnum > 0)
    {
        for (int i = 0; i < mcnpgeo999.voxelcount; i++)
        {
            if (lenseid == *(mcnpgeo999.element + i))
            {
                //std::cout << *(mcnpgeo999.element + i - 1) << std::endl;
                if (*(mcnpgeo999.element + i - 1) == lenseid + 1 || *(mcnpgeo999.element + i - 1) == 150)
                {
                    *(mcnpgeo999.element + i - 1) = lenseid;
                    numcounttmp++;
                }
                else if (*(mcnpgeo999.element + i + 1) == lenseid + 1 || *(mcnpgeo999.element + i + 1) == 150)
                {
                    *(mcnpgeo999.element + i + 1) = lenseid;
                    numcounttmp++;
                }
            }
            if (numcounttmp >= missnum)
            {
                std::cout << "lense numcounttmp =" << numcounttmp << std::endl;
                break;
            }
        }
    }
    else if (missnum < 0)
    {
        missnum = -missnum;
        for (int i = 0; i < mcnpgeo999.voxelcount; i++)
        {
            if (lenseid == *(mcnpgeo999.element + i))
            {
                //std::cout << *(mcnpgeo999.element + i - 1) << std::endl;
                if (*(mcnpgeo999.element + i + 1) == lenseid + 1)
                {
                    *(mcnpgeo999.element + i) = lenseid + 1;
                    numcounttmp++;
                }
            }
            if (numcounttmp >= missnum)
            {
                std::cout << "lense numcounttmp =" << numcounttmp << std::endl;
                break;
            }
        }
    }
    std::cout << "lense numcounttmp =" << numcounttmp << std::endl;
}

void volAdjust(char* inputpath, McnpFillStruct mcnpgeo999)
{
    std::cout << "Please confirm that file 'OrganRefvolume.txt' located in the same dirtory with ";
    puts(inputpath);
    char* volreffilepath = new char[500]; // store OrganRefvolume.txt path
    for (int i = 0; i < strlen(inputpath); i++)
    {
        if (*(inputpath + strlen(inputpath) - i) == '\\' || *(inputpath + strlen(inputpath) - i) == '/')
        {
            for (int j = 0; j <= strlen(inputpath) - i; j++)
            {
                *(volreffilepath + j) = *(inputpath + j);
            }
            *(volreffilepath + strlen(inputpath) - i + 1) = '\0';
            break;
        }
    }
    strcat(volreffilepath, "OrganRefvolume.txt");
    FILE* volreffile;
    if (0 == (volreffile = fopen(volreffilepath, "r")))
    {
        std::cout << "OrganRefvolume.txt not exist" << std::endl;
        exit(0);
    }
    else
    {
        std::cout << "Begin volume adjust ..." << std::endl;
    }
    float ref[200]; // store ref vol
    for (int i = 0; i < 200; i++)
        ref[i] = -1;
    char* linetmp = new char[100];
    while (0 != fgets(linetmp, 99, volreffile))
    {
        int organid;
        float voltmp;
        sscanf(linetmp, "%d%f", &organid, &voltmp);
        ref[organid] = voltmp;
    }
    //finish organ reference volume store end here
    for (int i = 0; i < 200; i++)
    {
        if (-1 != (ref[i]))
        {
            volAdjustAOrgan(i, ref[i], mcnpgeo999);
        }
    }

    delete[]volreffilepath;
}

void volAdjustAOrgan(int organid, float refvol, McnpFillStruct mcnpgeo999)
{
    float realvol = 0;
    int counttmp = 0;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (organid == *(mcnpgeo999.element + i))
        {
            counttmp++;
        }
    }
    if (0 != counttmp)
    {
        realvol = counttmp * mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey * mcnpgeo999.voxelsizez;
        if (refvol > realvol) //smaller
        {
            counttmp = static_cast<int> ((refvol - realvol) / (mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey * mcnpgeo999.voxelsizez));
            int countreplacenum = 0;
            int circlecount = 0;
            while (countreplacenum < counttmp)
            {
                for (int i = 0; i < mcnpgeo999.voxelcount; i++)
                {
                    if (organid == *(mcnpgeo999.element + i))
                    {
                        if ((*(mcnpgeo999.element + i - 1) >= 106 && *(mcnpgeo999.element + i - 1) <= 109) || 119 == (*(mcnpgeo999.element + i - 1)) ||
                            150 == (*(mcnpgeo999.element + i - 1)))
                        {
                            *(mcnpgeo999.element + i - 1) = organid;
                            countreplacenum++;
                        }
                    }
                    if (countreplacenum >= counttmp)
                    {
                        break;
                    }
                }
                circlecount++;
                if (circlecount >= 10)
                {
                    break;
                }
            }
            std::cout << "finish " << organid;
            std::cout << " and " << countreplacenum << " voxels were replaced";
            std::cout << std::endl;
        }
        else
        {
            counttmp = static_cast<int> ((realvol - refvol) / (mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey * mcnpgeo999.voxelsizez));
            int countreplacenum = 0;
            int circlecount = 0;
            while (countreplacenum < counttmp)
            {
                for (int i = 0; i < mcnpgeo999.voxelcount; i++)
                {
                    if (organid == *(mcnpgeo999.element + i))
                    {
                        if (*(mcnpgeo999.element + i - 1) != organid)
                        {
                            *(mcnpgeo999.element + i) = 150;
                            countreplacenum++;
                            i++;
                        }
                    }
                    if (countreplacenum >= counttmp)
                    {
                        break;
                    }
                }
                circlecount++;
                if (circlecount >= 10)
                {
                    break;
                }
            }
            std::cout << "finish " << organid;
            std::cout << " and " << countreplacenum << " voxels were replaced";
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "zero vol with" << organid << std::endl;
    }
}

void fileHandle(char** dirpath)
{
    std::cout << "Input obj file path:" << std::endl;
	std::cin.getline(*dirpath, 499);
}

int storeGeo(char* filepath, ObjGeo& objgeo)
{
    FILE* objfile;
    char* linetmp = new char[200]; *linetmp = '\0';
    int Vcount = 0, Fcount = 0, VNcount = 0;
    if (0 != (objfile = fopen(filepath, "r")))
    {
        while (0 != fgets(linetmp, 100, objfile)) //count Vnum and Fnum of every parts, no more than 200 organs
        {
            if (1 == strlen(linetmp))
                continue;
            *(linetmp + strlen(linetmp) - 1) = '\0';
            if ('v' == *linetmp && ' ' == *(linetmp + 1))
            {
                Vcount++;
            }
            else if ('v' == *linetmp && 'n' == *(linetmp + 1))
            {
                VNcount++;
            }
            else if ('f' == *linetmp && ' ' == *(linetmp + 1))
            {
                Fcount++;
            }
        }

        //malloc enough space
        objgeo.facecount = Fcount;
        objgeo.pointcount = Vcount;
        objgeo.vncount = VNcount;
        objgeo.point = new Point[Vcount];
        objgeo.pointnormal = new Point[VNcount];
        objgeo.face = new Face[Fcount];

        //back to the begning of file
        fseek(objfile, 0, SEEK_SET);
        int pointtmp = 0; int pointnormaltmp = 0; int facetmp = 0;
        while (0 != fgets(linetmp, 190, objfile)) //count Vnum and Fnum of every parts, no more than 200 organs
        {
            if (1 == strlen(linetmp))
                continue;
            *(linetmp + strlen(linetmp) - 1) = '\0';
            if ('v' == *linetmp && ' ' == *(linetmp + 1))
            {
                float coorX, coorY, coorZ;
                sscanf(linetmp, "%*s %f%f%f", &coorX, &coorY, &coorZ);
                (objgeo.point + pointtmp)->setcoorX(coorX);
                (objgeo.point + pointtmp)->setcoorY(coorY);
                (objgeo.point + pointtmp)->setcoorZ(coorZ);
                pointtmp++;
            }
            else if ('v' == *linetmp && 'n' == *(linetmp + 1))
            {
                float coorX, coorY, coorZ;
                sscanf(linetmp, "%*s %f%f%f", &coorX, &coorY, &coorZ);
                (objgeo.pointnormal + pointnormaltmp)->setcoorX(coorX);
                (objgeo.pointnormal + pointnormaltmp)->setcoorY(coorY);
                (objgeo.pointnormal + pointnormaltmp)->setcoorZ(coorZ);
                pointnormaltmp++;
            }
            else if ('f' == *linetmp && ' ' == *(linetmp + 1))
            {
                int facetmp1, facetmp2, facetmp3;
                sscanf(linetmp, "%*s %d//%*d %d//%*d %d//%*d", &facetmp1, &facetmp2, &facetmp3);
                (objgeo.face + facetmp)->setpointAindex(facetmp1 - 1);
                (objgeo.face + facetmp)->setpointBindex(facetmp2 - 1);
                (objgeo.face + facetmp)->setpointCindex(facetmp3 - 1);
                facetmp++;
            }
        }

        delete[]linetmp;
        fclose(objfile);
        return 0;
    }
    else
    {
        return 1;
    }
}

void freeGeo(ObjGeo& objgeo)
{
    delete[]objgeo.face;
    delete[]objgeo.point;
    delete[]objgeo.pointnormal;
}

float calGeoVol(Point geocenter, ObjGeo objgeo)
{
    //Point geocenter = calGeoCenter(objgeo);
    float geovol = 0;
    for (int i = 0; i < objgeo.facecount; i++)
    {
        Point A, B, C;
        A.setcoorX((objgeo.point + (objgeo.face + i)->getpointBindex())->getcoorX() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorX());
        A.setcoorY((objgeo.point + (objgeo.face + i)->getpointBindex())->getcoorY() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorY());
        A.setcoorZ((objgeo.point + (objgeo.face + i)->getpointBindex())->getcoorZ() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorZ());
        B.setcoorX((objgeo.point + (objgeo.face + i)->getpointCindex())->getcoorX() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorX());
        B.setcoorY((objgeo.point + (objgeo.face + i)->getpointCindex())->getcoorY() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorY());
        B.setcoorZ((objgeo.point + (objgeo.face + i)->getpointCindex())->getcoorZ() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorZ());
        C.setcoorX(geocenter.getcoorX() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorX());
        C.setcoorY(geocenter.getcoorY() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorY());
        C.setcoorZ(geocenter.getcoorZ() - (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorZ());

        geovol += 1.0 / 6 * mixedProduct(A, B, C);
    }

    return geovol;
}

Point calGeoCenter(ObjGeo objgeo)
{
    Point geocenter;
    float xx, yy, zz;
    xx = 0, yy = 0, zz = 0;
    for (int i = 0; i < objgeo.facecount; i++)
    {
        xx += (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorX();
        yy += (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorY();
        zz += (objgeo.point + (objgeo.face + i)->getpointAindex())->getcoorZ();
        xx += (objgeo.point + (objgeo.face + i)->getpointBindex())->getcoorX();
        yy += (objgeo.point + (objgeo.face + i)->getpointBindex())->getcoorY();
        zz += (objgeo.point + (objgeo.face + i)->getpointBindex())->getcoorZ();
        xx += (objgeo.point + (objgeo.face + i)->getpointCindex())->getcoorX();
        yy += (objgeo.point + (objgeo.face + i)->getpointCindex())->getcoorY();
        zz += (objgeo.point + (objgeo.face + i)->getpointCindex())->getcoorZ();
    }
    xx = xx / (objgeo.facecount * 3);
    yy = yy / (objgeo.facecount * 3);
    zz = zz / (objgeo.facecount * 3);

    geocenter.setcoorX(xx);
    geocenter.setcoorY(yy);
    geocenter.setcoorZ(zz);

    return geocenter;
}

//Calculate (A x B)*C
float mixedProduct(Point A, Point B, Point C)
{
    Point D; //A x B = D
    D.setcoorX(A.getcoorY() * B.getcoorZ() - A.getcoorZ() * B.getcoorY());
    D.setcoorY(A.getcoorZ() * B.getcoorX() - A.getcoorX() * B.getcoorZ());
    D.setcoorZ(A.getcoorX() * B.getcoorY() - A.getcoorY() * B.getcoorX());

    return (D.getcoorX() * C.getcoorX() + D.getcoorY() * C.getcoorY() + D.getcoorZ() * C.getcoorZ());
}

//Genereting forms for future analyses.
void genForms(char* inputpath, McnpFillStruct mcnpgeo999)
{
    //std::cout << "All information will be outputted in :";
    char* infofilepath = new char[500]; // store OrganRefvolume.txt path
    for (int i = 0; i < strlen(inputpath); i++)
    {
        if (*(inputpath + strlen(inputpath) - i) == '\\' || *(inputpath + strlen(inputpath) - i) == '/')
        {
            for (int j = 0; j <= strlen(inputpath) - i; j++)
            {
                *(infofilepath + j) = *(inputpath + j);
            }
            *(infofilepath + strlen(inputpath) - i + 1) = '\0';
            break;
        }
    }
    strcat(infofilepath, "OutputInfo.txt");
    //puts(infofilepath);
    FILE* infofile = fopen(infofilepath, "w+");
    if (0 == infofile)
    {
        puts("Fail to create outputinfo.txt");
        exit(0);
    }

    //Cal organ volume for every part.
    float* organvolume = new float[200];
    for (int i = 0; i < 200; i++) *(organvolume + i) = 0;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        *(organvolume + *(mcnpgeo999.element + i)) = *(organvolume + *(mcnpgeo999.element + i)) + 1;
    }
    for (int i = 0; i < 200; i++)
        *(organvolume + i) *= mcnpgeo999.voxelsizex * mcnpgeo999.voxelsizey * mcnpgeo999.voxelsizez;

    //Weight info
    std::cout << "\n=============Information for Phantoms==============" << std::endl;
    std::cout << "Output file located in :";
    puts(infofilepath);

    fprintf(infofile, "================WEIGHT(体重不可用，未更正119和125错误的密度信息，正确体重见文件末尾)===================\n");
    float wholeweight = 0;
    for (int i = 0; i < ORGANNUM; i++)
    {
        wholeweight += *(organvolume + i) * *(ORGANSDESTINY + i);
    }
    printf("Unprocessed Weight = %.3f\n", wholeweight);
    fprintf(infofile, "Unprocessed Weight = %.3f\n", wholeweight);
    float tmpweight = 0; //Weight - redisual - skin
    tmpweight = wholeweight - *(organvolume + 119) * *(ORGANSDESTINY + 119) - *(organvolume + 125) * *(ORGANSDESTINY + 125);
    printf("Unprocessed Tmp Weight (Weight - redisual - skin) = %.3f\n", tmpweight);
    fprintf(infofile, "Unprocessed Tmp (Weight - redisual - skin) Weight = %.3f\n", tmpweight);

    float corticalweight = 0;
    float spongiosaweight = 0;
    float cavityweight = 0;
    for (int i = 0; i < ORGANNUM; i++)
    {
        if (i == 13 || i == 16 || i == 19 || i == 22 || i == 24 || i == 26 || i == 28 || i == 31 || i == 34
            || i == 37 || i == 39 || i == 41 || i == 43 || i == 45 || i == 47 || i == 49 || i == 51 || i == 53 || i == 55)
        {
            corticalweight += *(organvolume + i) * *(ORGANSDESTINY + i);
        }
        else if (i == 14 || i == 17 || i == 20 || i == 23 || i == 25 || i == 27 || i == 29 || i == 32 || i == 35
            || i == 38 || i == 40 || i == 42 || i == 44 || i == 46 || i == 48 || i == 50 || i == 52 || i == 54 || i == 56)
        {
            spongiosaweight += *(organvolume + i) * *(ORGANSDESTINY + i);
        }
        else if (i == 15 || i == 18 || i == 21 || i == 30 || i == 33 || i == 36)
        {
            cavityweight += *(organvolume + i) * *(ORGANSDESTINY + i);
        }
    }

    fprintf(infofile, "\n======Weight For important parts======\n");
    printf("CorticalWeight = %.3f\n", corticalweight);
    fprintf(infofile, "CorticalWeight = %.3f\n", corticalweight);
    printf("SpongiosaWeight = %.3f\n", spongiosaweight);
    fprintf(infofile, "SpongiosaWeight = %.3f\n", spongiosaweight);
    printf("CavityWeight = %.3f\n", cavityweight);
    fprintf(infofile, "CavityWeight = %.3f\n", cavityweight);
    printf("BoneWeight = %.3f\n", corticalweight + spongiosaweight + cavityweight);
    fprintf(infofile, "BoneWeight = %.3f\n", corticalweight + spongiosaweight + cavityweight);

    //Voxelized volume for organs listed in GB

    float testesweight = 0; //129 130
    float ovariesweight = 0; //111 112
    float colonsweight = 0; //76 78 80 82 84
    float lungsweight = 0; //97 99
    float stomachweight = 0; //72
    float urinaryweight = 0; //137
    float breastsweight = 0; //62 63 64 65
    float liverweight = 0; //95
    float oesophagusweight = 0; //110
    float thyroidweight = 0; //132
    float brainweight = 0; //61
    float heartweight = 0; //87
    float kidneysweight = 0; //89 90 91 92 93 94
    float spleenweight = 0; //127
    float salivariesweight = 0; //120 121
    float gallweight = 0; //70
    float smallintweight = 0; //75
    float pancreasweight = 0; //113
    float bulbsweight = 0; //67 69
    float lensesweight = 0; //66 68
    float adrenalsweight = 0; //1 2
    float thymusweight = 0; //131
    float glandweight = 0; //114

    float testesvolume = 0; //129 130
    float ovariesvolume = 0; //111 112
    float colonsvolume = 0; //76 78 80 82 84
    float lungsvolume = 0; //97 99
    float stomachvolume = 0; //72
    float urinaryvolume = 0; //137
    float breastsvolume = 0; //62 63 64 65
    float livervolume = 0; //95
    float oesophagusvolume = 0; //110
    float thyroidvolume = 0; //132
    float brainvolume = 0; //61
    float heartvolume = 0; //87
    float kidneysvolume = 0; //89 90 91 92 93 94
    float spleenvolume = 0; //127
    float salivariesvolume = 0; //120 121
    float gallvolume = 0; //70
    float smallintvolume = 0; //75
    float pancreasvolume = 0; //113
    float bulbsvolume = 0; //67 69
    float lensesvolume = 0; //66 68
    float adrenalsvolume = 0; //1 2
    float thymusvolume = 0; //131
    float glandvolume = 0; //114

    for (int i = 0; i < ORGANNUM; i++)
    {
        if (i == 129 || i == 130)
        {
            testesweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            testesvolume += *(organvolume + i);
        }
        else if (i == 111 || i == 112)
        {
            ovariesweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            ovariesvolume += *(organvolume + i);
        }
        else if (i == 76 || i == 78 || i == 80 || i == 82 || i == 84)
        {
            colonsweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            colonsvolume += *(organvolume + i);
        }
        else if (i == 97 || i == 99)
        {
            lungsweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            lungsvolume += *(organvolume + i);
        }
        else if (i == 72)
        {
            stomachweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            stomachvolume += *(organvolume + i);
        }
        else if (i == 137)
        {
            urinaryweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            urinaryvolume += *(organvolume + i);
        }
        else if (i == 62 || i == 63 || i == 64 || i == 65)
        {
            breastsweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            breastsvolume += *(organvolume + i);
        }
        else if (i == 95)
        {
            liverweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            livervolume += *(organvolume + i);
        }
        else if (i == 110)
        {
            oesophagusweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            oesophagusvolume += *(organvolume + i);
        }
        else if (i == 132)
        {
            thyroidweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            thyroidvolume += *(organvolume + i);
        }
        else if (i == 61)
        {
            brainweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            brainvolume += *(organvolume + i);
        }
        else if (i == 87)
        {
            heartweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            heartvolume += *(organvolume + i);
        }
        else if (i == 89 || i == 90 || i == 91 || i == 92 || i == 93 || i == 94)
        {
            kidneysweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            kidneysvolume += *(organvolume + i);
        }
        else if (i == 127)
        {
            spleenweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            spleenvolume += *(organvolume + i);
        }
        else if (i == 120 || i == 121)
        {
            salivariesweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            salivariesvolume += *(organvolume + i);
        }
        else if (i == 70)
        {
            gallweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            gallvolume += *(organvolume + i);
        }
        else if (i == 75)
        {
            smallintweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            smallintvolume += *(organvolume + i);
        }
        else if (i == 113)
        {
            pancreasweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            pancreasvolume += *(organvolume + i);
        }
        else if (i == 67 || i == 69)
        {
            bulbsweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            bulbsvolume += *(organvolume + i);
        }
        else if (i == 66 || i == 68)
        {
            lensesweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            lensesvolume += *(organvolume + i);
        }
        else if (i == 1 || i == 2)
        {
            adrenalsweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            adrenalsvolume += *(organvolume + i);
        }
        else if (i == 131)
        {
            thymusweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            thymusvolume += *(organvolume + i);
        }
        else if (i == 114)
        {
            glandweight += *(organvolume + i) * *(ORGANSDESTINY + i);
            glandvolume += *(organvolume + i);
        }
    }

    fprintf(infofile, "testesweight    = %.3f\n", testesweight);
    fprintf(infofile, "ovariesweight   = %.3f\n", ovariesweight);
    fprintf(infofile, "colonsweight    = %.3f\n", colonsweight);
    fprintf(infofile, "lungsweight     = %.3f\n", lungsweight);
    fprintf(infofile, "stomachweight   = %.3f\n", stomachweight);
    fprintf(infofile, "urinaryweight   = %.3f\n", urinaryweight);
    fprintf(infofile, "breastsweight   = %.3f\n", breastsweight);
    fprintf(infofile, "liverweight     = %.3f\n", liverweight);
    fprintf(infofile, "oesophagusweight= %.3f\n", oesophagusweight);
    fprintf(infofile, "thyroidweight   = %.3f\n", thyroidweight);
    fprintf(infofile, "brainweight     = %.3f\n", brainweight);
    fprintf(infofile, "heartweight     = %.3f\n", heartweight);
    fprintf(infofile, "kidneysweight   = %.3f\n", kidneysweight);
    fprintf(infofile, "spleenweight    = %.3f\n", spleenweight);
    fprintf(infofile, "salivariesweight= %.3f\n", salivariesweight);
    fprintf(infofile, "gallweight      = %.3f\n", gallweight);
    fprintf(infofile, "smallintweight  = %.3f\n", smallintweight);
    fprintf(infofile, "pancreasweight  = %.3f\n", pancreasweight);
    fprintf(infofile, "bulbsweight     = %.3f\n", bulbsweight);
    fprintf(infofile, "lensesweight    = %.3f\n", lensesweight);
    fprintf(infofile, "adrenalsweight  = %.3f\n", adrenalsweight);
    fprintf(infofile, "thymusweight    = %.3f\n", thymusweight);
    fprintf(infofile, "glandweight     = %.3f\n", glandweight);

    fprintf(infofile, "\n=========================\n");

    fprintf(infofile, "testesvolume    = %.3f\n", testesvolume);
    fprintf(infofile, "ovariesvolume   = %.3f\n", ovariesvolume);
    fprintf(infofile, "colonsvolume    = %.3f\n", colonsvolume);
    fprintf(infofile, "lungsvolume     = %.3f\n", lungsvolume);
    fprintf(infofile, "stomachvolume   = %.3f\n", stomachvolume);
    fprintf(infofile, "urinaryvolume   = %.3f\n", urinaryvolume);
    fprintf(infofile, "breastsvolume   = %.3f\n", breastsvolume);
    fprintf(infofile, "livervolume     = %.3f\n", livervolume);
    fprintf(infofile, "oesophagusvolume= %.3f\n", oesophagusvolume);
    fprintf(infofile, "thyroidvolume   = %.3f\n", thyroidvolume);
    fprintf(infofile, "brainvolume     = %.3f\n", brainvolume);
    fprintf(infofile, "heartvolume     = %.3f\n", heartvolume);
    fprintf(infofile, "kidneysvolume   = %.3f\n", kidneysvolume);
    fprintf(infofile, "spleenvolume    = %.3f\n", spleenvolume);
    fprintf(infofile, "salivariesvolume= %.3f\n", salivariesvolume);
    fprintf(infofile, "gallvolume      = %.3f\n", gallvolume);
    fprintf(infofile, "smallintvolume  = %.3f\n", smallintvolume);
    fprintf(infofile, "pancreasvolume  = %.3f\n", pancreasvolume);
    fprintf(infofile, "bulbsvolume     = %.3f\n", bulbsvolume);
    fprintf(infofile, "lensesvolume    = %.3f\n", lensesvolume);
    fprintf(infofile, "adrenalsvolume  = %.3f\n", adrenalsvolume);
    fprintf(infofile, "thymusvolume    = %.3f\n", thymusvolume);
    fprintf(infofile, "glandvolume     = %.3f\n", glandvolume);

    //生成密度信息表
    float skinweighttmp; float bodyweighttmp;
    std::cout << "\nPlease input weight of skin(125)" << std::endl;
    std::cin >> skinweighttmp;
    std::cout << "Please input weight of whole body" << std::endl;
    std::cin >> bodyweighttmp;

    fprintf(infofile, "\n\n===========destiny & material info===========\n");
    char* dirpath = new char[500]; *dirpath = '\0';// store the dir that name.txt located in 
    for (int i = 0; i < strlen(inputpath); i++)
    {
        if (*(inputpath + strlen(inputpath) - i) == '\\' || *(inputpath + strlen(inputpath) - i) == '/')
        {
            for (int j = 0; j <= strlen(inputpath) - i; j++)
            {
                *(dirpath + j) = *(inputpath + j);
            }
            *(dirpath + strlen(inputpath) - i + 1) = '\0';
            break;
        }
    }
    char* nametxtpath = new char[500];
    strcpy(nametxtpath, dirpath);
    strcat(nametxtpath, "name.txt");
    FILE* nametxt;
    char* linetmp = new char[200];
    int organname;
    if (0 != (nametxt = fopen(nametxtpath, "r")))
    {
        while (0 != fgets(linetmp, 190, nametxt))
        {
            sscanf(linetmp, "%d", &organname);
            if (119 != organname && 125 != organname && organname <= 141)
            {
                if (0 != ORGANMATERIAL[organname])
                {
                    fprintf(infofile, "%-5d", organname);
                    fprintf(infofile, "%5d   ", ORGANMATERIAL[organname]);
                    fprintf(infofile, "-%8f\n", ORGANSDESTINY[organname]);
                }
                else
                {
                    fprintf(infofile, "%-5d", organname);
                    fprintf(infofile, "%5d            \n", ORGANMATERIAL[organname]);
                }
            }
            else if (125 == organname && organname <= 141)
            {
                fprintf(infofile, "%-5d", organname);
                fprintf(infofile, "%5d   ", ORGANMATERIAL[organname]);
                ORGANSDESTINY[125] = skinweighttmp / *(organvolume + 125);
                fprintf(infofile, "-%8f\n", skinweighttmp / *(organvolume + 125));
                std::cout << "destiny of skin" << skinweighttmp / *(organvolume + 125) << std::endl;
            }
            else if (119 == organname && organname <= 141)
            {
                fprintf(infofile, "%-5d", organname);
                fprintf(infofile, "%5d   ", ORGANMATERIAL[organname]);
                ORGANSDESTINY[119] = (bodyweighttmp - tmpweight - skinweighttmp) / *(organvolume + 119);
                fprintf(infofile, "-%8f\n", (bodyweighttmp - tmpweight - skinweighttmp) / *(organvolume + 119));
                std::cout << "destiny of redisual tissue" << (bodyweighttmp - tmpweight - skinweighttmp) / *(organvolume + 119) << std::endl;
            }
            else
            {
                if (0 != ORGANMATERIAL[organname])
                {
                    fprintf(infofile, "%-5d", organname);
                    fprintf(infofile, "%5d   ", ORGANMATERIAL[organname]);
                    fprintf(infofile, "-%8f\n", ORGANSDESTINY[organname]);
                }
                else
                {
                    fprintf(infofile, "%-5d", organname);
                    fprintf(infofile, "%5d            \n", ORGANMATERIAL[organname]);
                }
            }
        }
    }

    fprintf(infofile, "\n\n================WEIGHT(已经更正119和125错误的密度信息)===================\n");
    wholeweight = tmpweight + *(organvolume + 119) * ORGANSDESTINY[119] + *(organvolume + 125) * ORGANSDESTINY[125];
    printf("Processed Weight = %.3f\n", wholeweight);
    fprintf(infofile, "Processed Weight = %.3f\n", wholeweight);

    delete[]infofilepath;
    delete[]nametxtpath;
    delete[]dirpath;
    delete[]organvolume;
    delete[]linetmp;
    fclose(infofile);
}

//return 0, if not exsit
float calOrganVol(unsigned int organid, McnpFillStruct mcnpgeo999)
{
    int numcount = 0;
    for (int i = 0; i < mcnpgeo999.voxelcount; i++)
    {
        if (organid == *(mcnpgeo999.element + i))
        {
            numcount++;
        }
    }

    return numcount*mcnpgeo999.voxelsizex*mcnpgeo999.voxelsizey*mcnpgeo999.voxelsizez;
}

// X方向增加Xnegative(靠近x负方向一侧)和Xpositive(靠近x正方向一侧)，注意数值为负时代表消减一定体素层数，其他方向同理
int geoZoom(McnpFillStruct& mcnpgeo999, int Xnegative, int Xpositive, int Ynegative, int Ypositive, int Znegative, int Zpositive)
{
    geoZoomSingledir(mcnpgeo999, Xnegative, Xpositive, 'X');
    geoZoomSingledir(mcnpgeo999, Ynegative, Ypositive, 'Y');
    geoZoomSingledir(mcnpgeo999, Znegative, Zpositive, 'Z');

    return 1;
}

// dir方向增加negative(靠近dir负方向一侧)和positive(靠近dir正方向一侧),dir可以取'X','Y','Z'或他们的小写
int geoZoomSingledir(McnpFillStruct& mcnpgeo999, int negative, int positive, char dir)
{
    switch (dir)
    {
    case 'X':
    {
        if (negative > 0) // x负方向增加
        {
            int elementtmpnum = mcnpgeo999.voxelcount + negative * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                xyztmp.x = xyztmp.x + negative;
                int sequence = xyztmp.z * (mcnpgeo999.dimxsup + negative - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                    xyztmp.y * (mcnpgeo999.dimxsup + negative - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                *(elementtmp + sequence) = *(mcnpgeo999.element + i);
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimxsup = mcnpgeo999.dimxsup + negative;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        else if (negative < 0)// x负方向减少
        {
            if ((mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) <= abs(negative))
            {
                printf("Wrong layer num!\n");
                return 2;
            }
            int elementtmpnum = mcnpgeo999.voxelcount + negative * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                xyztmp.x = xyztmp.x + negative;
                if (xyztmp.x >= 0)
                {
                    int sequence = xyztmp.z * (mcnpgeo999.dimxsup + negative - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                        xyztmp.y * (mcnpgeo999.dimxsup + negative - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                    *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                }
            }

            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimxsup = mcnpgeo999.dimxsup + negative;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        if (positive > 0) // x正方向增加
        {
            int elementtmpnum = mcnpgeo999.voxelcount + positive * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                int sequence = xyztmp.z * (mcnpgeo999.dimxsup + positive - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                    xyztmp.y * (mcnpgeo999.dimxsup + positive - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimxsup = mcnpgeo999.dimxsup + positive;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        else if (positive < 0)// x正方向减少
        {
            if ((mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) <= abs(positive))
            {
                printf("Wrong layer num!\n");
                return 2;
            }
            int elementtmpnum = mcnpgeo999.voxelcount + positive * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                if ((xyztmp.x - positive) <= mcnpgeo999.dimxsup)
                {
                    int sequence = xyztmp.z * (mcnpgeo999.dimxsup + positive - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                        xyztmp.y * (mcnpgeo999.dimxsup + positive - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                    *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                }
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimxsup = mcnpgeo999.dimxsup + positive;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        break;
    }
    case 'Y':
    {
        if (negative > 0) // y负方向增加
        {
            int elementtmpnum = mcnpgeo999.voxelcount + negative * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                xyztmp.y = xyztmp.y + negative;
                int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup + negative - mcnpgeo999.dimyinf + 1) + \
                    xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimysup = mcnpgeo999.dimysup + negative;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        else if (negative < 0)// y负方向减少
        {
            if ((mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) <= abs(negative))
            {
                printf("Wrong layer num!\n");
                return 2;
            }
            int elementtmpnum = mcnpgeo999.voxelcount + negative * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                xyztmp.y = xyztmp.y + negative;
                if (xyztmp.y >= 0)
                {
                    int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup + negative - mcnpgeo999.dimyinf + 1) + \
                        xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                    *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                }
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimysup = mcnpgeo999.dimysup + negative;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        if (positive > 0) // y正方向增加
        {
            int elementtmpnum = mcnpgeo999.voxelcount + positive * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup + positive - mcnpgeo999.dimyinf + 1) + \
                    xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimysup = mcnpgeo999.dimysup + positive;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        else if (positive < 0)// y正方向减少
        {
            if ((mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) <= abs(positive))
            {
                printf("Wrong layer num!\n");
                return 2;
            }
            int elementtmpnum = mcnpgeo999.voxelcount + positive * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                if ((xyztmp.y - positive) <= mcnpgeo999.dimysup)
                {
                    int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup + positive - mcnpgeo999.dimyinf + 1) + \
                        xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                    *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                }
                
            }
			short int *mctmp = mcnpgeo999.element;
			delete[]mctmp;
            //delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimysup = mcnpgeo999.dimysup + positive;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        break;
    }
    case 'Z':
    {
        if (negative > 0) // z负方向增加
        {
            int elementtmpnum = mcnpgeo999.voxelcount + negative * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                xyztmp.z = xyztmp.z + negative;
                int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                    xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimzsup = mcnpgeo999.dimzsup + negative;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        else if (negative < 0)// z负方向减少
        {
            if ((mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) <= abs(negative))
            {
                printf("Wrong layer num!\n");
                return 2;
            }
            int elementtmpnum = mcnpgeo999.voxelcount + negative * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                xyztmp.z = xyztmp.z + negative;
                if (xyztmp.z >= 0) 
                {
                    int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                        xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                    *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                }
                
            }
            //delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimzsup = mcnpgeo999.dimzsup + negative;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        if (positive > 0) // z正方向增加
        {
            int elementtmpnum = mcnpgeo999.voxelcount + positive * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                    xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimzsup = mcnpgeo999.dimzsup + positive;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        else if (positive < 0)// z正方向减少
        {
            if ((mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1) <= abs(positive))
            {
                printf("Wrong layer num!\n");
                return 2;
            }
            int elementtmpnum = mcnpgeo999.voxelcount + positive * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1);
            short int* elementtmp = new short int[elementtmpnum];
            for (int i = 0; i < elementtmpnum; i++)
            {
                *(elementtmp + i) = 150;
            }
            for (int i = 0; i < mcnpgeo999.voxelcount; i++)
            {
                Coordinate3D xyztmp;
                xyztmp = convertSequenceToXYZ(i, mcnpgeo999);
                xyztmp.z = xyztmp.z;
                if ((xyztmp.z - positive) <= mcnpgeo999.dimzsup)
                {
                    int sequence = xyztmp.z * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) + \
                        xyztmp.y * (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) + xyztmp.x;
                    *(elementtmp + sequence) = *(mcnpgeo999.element + i);
                }
                
            }
            delete[]mcnpgeo999.element;
            mcnpgeo999.element = elementtmp;
            elementtmp = 0; // point to NULL
            mcnpgeo999.dimzsup = mcnpgeo999.dimzsup + positive;
            mcnpgeo999.voxelcount = (mcnpgeo999.dimxsup - mcnpgeo999.dimxinf + 1) * (mcnpgeo999.dimysup - mcnpgeo999.dimyinf + 1) * (mcnpgeo999.dimzsup - mcnpgeo999.dimzinf + 1);
        }
        break;
    }
    default:
        printf("Wrong dir!\n");
        return 1;
    }

    return 0;
}

//计算对称轴，体模朝向Y正方向，计算X方向对称轴
int calSymmetryAxisX(McnpFillStruct mcnpgeo999)
{
	// Use organ head skin (125)
	int counttmp = 0;
	int Xsum = 0;
	Coordinate3D XYZtmp;
	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		if (125 == *(mcnpgeo999.element + i))
		{
			XYZtmp = convertSequenceToXYZ(i, mcnpgeo999);
			if (XYZtmp.z > 460)
			{
				Xsum += XYZtmp.x;
				counttmp++;
			}
		}
	}

	Xsum /= counttmp;

	return Xsum;
}

//copy bulb left
void bulbCopy(unsigned int bulbleft, unsigned int bulbright, McnpFillStruct mcnpgeo999)
{
	int symmetryaxisX = calSymmetryAxisX(mcnpgeo999);
	int checksign = 0;

	//Clean bulb right
	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		if (bulbright == *(mcnpgeo999.element + i))
			*(mcnpgeo999.element + i) = 119;
	}
	std::cout << "organ " << bulbright << " clean" << std::endl;

	int replacecount = 0;
	Coordinate3D XYZtmp;
	for (int i = 0; i < mcnpgeo999.voxelcount; i++)
	{
		if (bulbleft == *(mcnpgeo999.element + i))
		{
			XYZtmp = convertSequenceToXYZ(i, mcnpgeo999);
			XYZtmp.x = 2 * symmetryaxisX - XYZtmp.x;
			if (*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == 119 ||
				*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == 150 ||
				*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == 106 ||
				*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == 61  ||
				*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == 27  ||
				*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == 26  ||
				*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == 142)
			{
				*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) = bulbright;
				replacecount++;
			}
			//else
			//{
			//	std::cout << ' ' << *(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) << ' ';
			//}
			if (*(mcnpgeo999.element + convertXYZToSequence(XYZtmp.x, XYZtmp.y, XYZtmp.z, mcnpgeo999)) == (bulbright - 1))
			{
				checksign++;
			}
		}
	}

	if (0 == checksign)
	{
		std::cout << "Maybe wrong position" << std::endl;
	}
	std::cout << replacecount << " were replaced" << std::endl;
}

void skinRepair(McnpFillStruct& mcnpgeo999)
{
	int counttmp = 0;
	//逐行遍历
	for (int i = mcnpgeo999.dimzinf; i <= mcnpgeo999.dimzsup; i++)
	{
		for (int j = mcnpgeo999.dimyinf; j <= mcnpgeo999.dimysup; j++)
		{
			int x_119_mintmp = 100000; //记录119号器官最先和最后出现的位置
			int x_119_maxtmp = -100000;
			for (int k = mcnpgeo999.dimxinf; k <= mcnpgeo999.dimxsup; k++) // 相同yz，不同x
			{
				int indextmp = convertXYZToSequence(k, j, i, mcnpgeo999);
				if (119 == *(mcnpgeo999.element + indextmp))
				{
					x_119_maxtmp = x_119_maxtmp > k ? x_119_maxtmp : k;
					x_119_mintmp = x_119_mintmp < k ? x_119_mintmp : k;
				}
			}
			if (x_119_maxtmp > x_119_mintmp)
			{
				if (x_119_mintmp > mcnpgeo999.dimxinf && x_119_maxtmp < mcnpgeo999.dimxsup)
				{
					//int indextmpmin1 = convertXYZToSequence(x_119_mintmp, j, i, mcnpgeo999);
					int indextmpmin2 = convertXYZToSequence(x_119_mintmp - 1, j, i, mcnpgeo999);
					//int indextmpmax1 = convertXYZToSequence(x_119_maxtmp, j, i, mcnpgeo999);
					int indextmpmax2 = convertXYZToSequence(x_119_maxtmp + 1, j, i, mcnpgeo999);
					if (150 == *(mcnpgeo999.element + indextmpmin2))
					{
						*(mcnpgeo999.element + indextmpmin2) = 125;
						counttmp++;
					}
					if (150 == *(mcnpgeo999.element + indextmpmax2))
					{
						*(mcnpgeo999.element + indextmpmax2) = 125;
						counttmp++;
					}
				}
			}
		}
	}

	std::cout << "Finish skin repair " << counttmp << " Voxel replaces!" << std::endl;
}