#ifndef MATERIALS_H_
#define MATERIALS_H_
#include "basicmath.h"
class Materials{
public:
    Materials() {}
    Materials(myvector df, myvector spe, double p_exp, myvector refle)
    {
        diffuse_ = df;
        specular_ = spe;
        phong_exp_ = p_exp;
        reflect_ = refle;
    }
    
    
    
    void set_materials(myvector df, myvector spe, double p_exp, myvector refle)
    {
        diffuse_ = df;
        specular_ = spe;
        phong_exp_ = p_exp;
        reflect_ = refle;
    }
    
    myvector getDiffuse() const {return diffuse_;}
    myvector getSpecular() const {return specular_;}
    double getPhong() const {return phong_exp_;}
    myvector getReflect() const {return reflect_;}
private:
    myvector diffuse_;
    myvector specular_;
    double phong_exp_;
    myvector reflect_;
};


#endif