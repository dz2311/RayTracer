#include "light.h"

mypoint Light::getOrigin()
{
    
    std::cout << "error: light::getOrigin should not be called - it's virtual!"
    << std::endl;
    return mypoint(0.0,0.0,0.0);
}

myvector Light::getColor()
{
    
    std::cout << "error: light::getColor should not be called - it's virtual!"
    << std::endl;
    return myvector(0.0,0.0,0.0);
}
myvector Light::getIlumination(Materials &mat, myvector &l, myvector &n, myvector &h)
{
    std::cout << "error: light::getIlumination should not be called - it's virtual!"
    << std::endl;
    return myvector(0.0,0.0,0.0);
}
mypoint Light::generateShadowRay(const int p, const int q, const int num_SR)
{
    std::cout<<"error: light::generateShadowRay should not be called - it's virtual!"<<endl;
    return mypoint(0.0,0.0,0.0);
}
int Light::getLightType()
{

    std::cout<<"error: light::getLightType should not be called - it's virtual!"<<std::endl;
    return -1;
}
myvector Light::getLightSourceNor()
{
    std::cout<<"error: light::getLightSourceNor should not be called - it's virtual!"<<std::endl;
    return myvector(0.0,0.0,0.0);
}


mypoint Pointlight::getOrigin()
{return pl_pos;}

int Pointlight::getLightType()
{return lightType;}

myvector Pointlight::getColor()
{return pl_color;}

myvector Pointlight::getIlumination(Materials &mat, myvector &l, myvector &n, myvector &h)
{
        myvector res;
        myvector diff = mat.getDiffuse();
        myvector spec = mat.getSpecular();
        double n_l = n.dotProduct(l);
        double n_h = n.dotProduct(h);
        double phong = mat.getPhong();
        res.set(diff[0]*pl_color[0]*fmax(0.0,n_l)+spec[0]*pl_color[0]*pow(fmax(0.0,n_h),phong),
                diff[1]*pl_color[1]*fmax(0.0,n_l)+spec[1]*pl_color[1]*pow(fmax(0.0,n_h),phong),
                diff[2]*pl_color[2]*fmax(0.0,n_l)+spec[2]*pl_color[2]*pow(fmax(0.0,n_h),phong));
        
        return res;
}

myvector Ambientlight::getAmbientlight(Materials &mat)
{
    myvector res;
    res.componentWiseMultiple(al_color, mat.getDiffuse());
    return res;
}
mypoint Arealight::getOrigin()
{//return pos_;
    return areal_center ;
}

int Arealight::getLightType()
{return lightType;}

myvector Arealight::getLightSourceNor()
{
    return lnormal;
}

mypoint Arealight::generateShadowRay(const int p, const int q, const int num_SR)
{
    double delta1, delta2;
    //srand((unsigned)time(NULL));
    delta1 = rand() / (double)RAND_MAX;
    delta2 = rand() / (double)RAND_MAX;
    //std::cout<<"delta 1:"<<delta1<<std::endl;
    //std::cout<<"dleta 2:"<<delta2<<std::endl;
    double u = length_ * (p + delta1) / num_SR;
    double v = length_ * (q + delta2) / num_SR;
    
    mypoint cornor = areal_center - udir_*(length_/2)- vdir_*(length_/2);
    mypoint res = cornor + udir_*u + vdir_*v;
    //pos_ = res;
    return res;
}
myvector Arealight::getIlumination(Materials &mat, myvector &l, myvector &n, myvector &h)
{
    myvector res;
    myvector diff = mat.getDiffuse();
    myvector spec = mat.getSpecular();
    double n_l = n.dotProduct(l);
    double n_h = n.dotProduct(h);
    double phong = mat.getPhong();
    res.set(diff[0]*areal_color[0]*fmax(0.0,n_l)+spec[0]*areal_color[0]*pow(fmax(0.0,n_h),phong),
            diff[1]*areal_color[1]*fmax(0.0,n_l)+spec[1]*areal_color[1]*pow(fmax(0.0,n_h),phong),
            diff[2]*areal_color[2]*fmax(0.0,n_l)+spec[2]*areal_color[2]*pow(fmax(0.0,n_h),phong));
    
    return res;
}





