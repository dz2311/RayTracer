#include "plane.h"
bool Plane::intersect (Ray &r)
{
    //cout<<"plane.cc--This surface is plane"<<endl;
	if (r.getDir().dotProduct(normal_)==0)
        return false;
    
    double t = (r.getOrigin()*normal_+scale_)/(r.getDir().dotProduct(normal_));
    if(t>0) return false;
    
    t *= -1.0f;
    mypoint p = r.getOrigin()+(r.getDir()*t);
    
    Intersection_point *ipnt = new Intersection_point(p, normal_, t);

    r.addIntersection(*ipnt);
    delete ipnt;
    //cout<<" the intect is write into class ray: normal x "<<r.getIp().getNormal()[0]<<" y "<<r.getIp().getNormal()[1]<<" z "<<r.getIp().getNormal()[2]<<endl;
    
    
	return true;
}
Materials Plane::getMaterials()
{
    return material_;
}
void Plane::setMaterials(Materials Mater)
{
    material_ = Mater;

}

bool Plane::intersect_bbox (Ray &r)
{
    if (r.getDir().dotProduct(normal_)==0)
        return false;
    
    double t = (r.getOrigin()*normal_+scale_)/(r.getDir().dotProduct(normal_));
    if(t>0) return false;
    
    t *= -1.0f;
    mypoint p = r.getOrigin()+(r.getDir()*t);
    
    Intersection_point ipnt = Intersection_point(p, normal_, t);
    
    r.addIntersection(ipnt);
    //cout<<" the intect is write into class ray: normal x "<<r.getIp().getNormal()[0]<<" y "<<r.getIp().getNormal()[1]<<" z "<<r.getIp().getNormal()[2]<<endl;
    
    
    return true;
}

Boundingbox Plane::getBoundingBox()
{
    return bbox;
}