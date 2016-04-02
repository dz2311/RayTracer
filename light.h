#ifndef LIGHT_H_
#define LIGHT_H_

#include "basicmath.h"
#include "materials.h"
#include <cmath>
using namespace std;

class Light {
public:
    virtual mypoint getOrigin();
    virtual myvector getColor();
    virtual myvector getIlumination(Materials &, myvector &, myvector &, myvector &);
    virtual int getLightType();
    virtual mypoint generateShadowRay(const int p, const int q, const int num_SR);
    virtual myvector getLightSourceNor();
protected:
    mypoint pos_;
};
class Pointlight: public Light{      //point light

public:
    Pointlight(mypoint lorigin, myvector rgb_color) { pl_pos = lorigin; pl_color = rgb_color; lightType = 1;}
    ~Pointlight ();
    virtual mypoint getOrigin();
    virtual myvector getColor();
    virtual int getLightType();
    virtual myvector getIlumination(Materials &mat, myvector &l, myvector &n, myvector &h);
    void set_pllight(mypoint pos, myvector rgb){
        pl_pos = pos;
        pl_color = rgb;
    }


private:
   
    mypoint pl_pos;
    myvector pl_color; //[0..1] for each channel.
    int lightType;
};

class Ambientlight{
public:
    Ambientlight(myvector rgb_color) {al_color = rgb_color; }
    Ambientlight () {al_color = myvector(0.0,0.0,0.0);}
    myvector getColor() {return al_color;};
    myvector getAmbientlight(Materials &mat);
private:
    myvector al_color;
};

class Arealight: public Light{   //area light
    
public:
    Arealight(mypoint lorigin, myvector normal, myvector udir, double len, myvector rgb_color) { areal_center = lorigin; lnormal = normal; udir_ = udir; length_ = len; areal_color = rgb_color; lightType = 2; vdir_.crossProduct(udir, normal); vdir_.normalize();}
    ~Arealight ();
    virtual int getLightType();
    virtual mypoint getOrigin();
    virtual mypoint generateShadowRay(const int p, const int q, const int num_SR);
    virtual myvector getLightSourceNor();
    //virtual myvector getColor();
    virtual myvector getIlumination(Materials &mat, myvector &l, myvector &n, myvector &h);
    
private:
    
    mypoint areal_center;
    myvector lnormal;
    myvector udir_;
    double length_;
    myvector areal_color; //[0..1] for each channel.
    int lightType;
    myvector vdir_;
    mypoint pos_;   //used to calculate the distance from light sample to the intersection on the object
};
#endif