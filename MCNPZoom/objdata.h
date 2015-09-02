#pragma once

class Face
{
private:
    unsigned int pointAindex_;
    unsigned int pointBindex_;
    unsigned int pointCindex_;
public:
    //unsigned int count;
    Face();
    unsigned int getpointAindex();
    unsigned int getpointBindex();
    unsigned int getpointCindex();
    void setpointAindex(unsigned int pointAindex);
    void setpointBindex(unsigned int pointBindex);
    void setpointCindex(unsigned int pointCindex);
};
class Point
{
private:
    float coorX_;
    float coorY_;
    float coorZ_;
public:
    //unsigned int count;
    Point();
    float getcoorX();
    float getcoorY();
    float getcoorZ();
    void setcoorX(float coorX);
    void setcoorY(float coorY);
    void setcoorZ(float coorZ);
};
class ObjGeo
{
public:
    Face* face;
    Point* pointnormal;
    Point* point;
    unsigned int facecount;
    unsigned int pointcount;
    unsigned int vncount;
    ObjGeo();
};

int storeGeo(char* filepath, ObjGeo& objgeo);
void freeGeo(ObjGeo& objgeo);
float calGeoVol(Point geocenter, ObjGeo objgeo);
Point calGeoCenter(ObjGeo objgeo);
float mixedProduct(Point A, Point B, Point C);


//Functions for class "face"
Face::Face()
{
    pointAindex_ = 0;
    pointBindex_ = 0;
    pointCindex_ = 0;
    //count = 0;
}
unsigned int Face::getpointAindex()
{
    return pointAindex_;
}
unsigned int Face::getpointBindex()
{
    return pointBindex_;
}
unsigned int Face::getpointCindex()
{
    return pointCindex_;
}
void Face::setpointAindex(unsigned int pointAindex)
{
    pointAindex_ = pointAindex;
}
void Face::setpointBindex(unsigned int pointBindex)
{
    pointBindex_ = pointBindex;
}
void Face::setpointCindex(unsigned int pointCindex)
{
    pointCindex_ = pointCindex;
}
bool isInside(Point pointnow, ObjGeo objgeo);

//Functions for class "Point"
Point::Point()
{
    coorX_ = 0;
    coorY_ = 0;
    coorZ_ = 0;
    //count = 0;
}
float Point::getcoorX()
{
    return coorX_;
}
float Point::getcoorY()
{
    return coorY_;
}
float Point::getcoorZ()
{
    return coorZ_;
}
void Point::setcoorX(float coorX)
{
    coorX_ = coorX;
}
void Point::setcoorY(float coorY)
{
    coorY_ = coorY;
}
void Point::setcoorZ(float coorZ)
{
    coorZ_ = coorZ;
}
ObjGeo::ObjGeo()
{
    face = 0;
    point = 0;
    facecount = 0;
    pointcount = 0;
    vncount = 0;
}
