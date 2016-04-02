//
//  sphere.cc
//  raytra_1
//


#include "sphere.h"

bool Sphere::intersect (Ray &r)
{
    
    // straight out of the text: p 77
    
    // these first two terms are used a lot, so compute them once:
    mypoint e = r.getOrigin(); //e
    //cout<<"sphere.cc----it's sphere"<<endl;
    //cout<<"sphere.cc---- sphere center: x "<<e[0]<<" y "<<e[1]<<" z "<<e[2]<<endl;
    // (e - c):
    myvector e_c = e - center_;
    
    // D . D
    myvector d = r.getDir ();
    double d_dot_d = d.dotProduct (d);
    
    
    // now compute the two terms under the square root sign:
    
    double discrim_term1 = d.dotProduct (e_c);
    discrim_term1 *= discrim_term1; // square it.
    
    double discrim_term2 = e_c.dotProduct (e_c);
    discrim_term2 -= (radius_ * radius_);
    discrim_term2 *= d_dot_d;
    
    
    // this is what's under the square root sign:
    double discriminant = discrim_term1 - discrim_term2;
    
    // complex root - missed the sphere:
    if (discriminant < 0.0) return false;
    
    // otherwise, we hit it (either once or twice)
    // calculate intersaction point
    double t1 = (d.invert().dotProduct(e_c)+sqrt(discriminant))/d_dot_d;
    double t2 = (d.invert().dotProduct(e_c)-sqrt(discriminant))/d_dot_d;
    if(t1<0.001&&t2<0.001)
       return false;
    
    myvector dir1 = d*t1;
    myvector dir2 = d*t2;
    
    mypoint p1 = e+dir1;
    mypoint p2 = e+dir2;
    //cout<<" t1 "<< t1 <<" t2 "<<t2<<endl;
    //cout<<"sphere.cc----intersect1: x "<<p1[0]<<" y "<<p1[1]<<" z "<<p1[2]<<endl;
    //cout<<"sphere.cc----intersect2: x "<<p2[0]<<" y "<<p2[1]<<" z "<<p2[2]<<endl;
    Intersection_point *ipnt;
    myvector nor1 = p1-center_;
    nor1.normalize();
    myvector nor2 = p2-center_;
    nor2.normalize();
    if(t1>=t2)
        ipnt  = new Intersection_point(center_,p2,radius_, nor2, t2);
    else
        ipnt = new Intersection_point(center_,p1,radius_, nor1, t1);
    r.addIntersection(*ipnt);
    //cout<<"sphere.cc----ray: center: "<<r.getIp().getSurfaceCenter()[0]<<" "<<r.getIp().getSurfaceCenter()[1]<<" "<< r.getIp().getSurfaceCenter()[2]<<endl;
    
    //cout<<"sphere.cc----ray: radius "<<r.getIp().getSurfacePdis()<<endl;
    return true;
    // if you want to know how far along the ray the intersection
    // is, you'll need to fill out the rest of the alg, determine
    // the one or two intersections, and return or store the closest one.
    
}

Materials Sphere::getMaterials()
{
    return material_;
}
void Sphere::setMaterials(Materials Mater)
{
    material_ = Mater;
    
}

Boundingbox Sphere::getBoundingBbox()
{
    return bbox;
}
bool Sphere::intersect_bbox (Ray &r)
{
    double Tminx,Tminy,Tminz;
    double Tmaxx,Tmaxy,Tmaxz;
    mypoint eye = r.getOrigin();
    myvector dir = r.getDir();
    mypoint min = bbox.getBoundingMin();
    mypoint max = bbox.getBoundingMax();
    double a = 1/dir[0];
    double b = 1/dir[1];
    double c = 1/dir[2];
    if(a>=0)
    {
        Tminx = (min[0]-eye[0])*a;
        Tmaxx = (max[0]-eye[0])*a;
    }
    else
    {
        Tmaxx = (min[0]-eye[0])*a;
        Tminx = (max[0]-eye[0])*a;
    }
    if(b>=0)
    {
        Tminy = (min[1]-eye[1])*b;
        Tmaxy = (max[1]-eye[1])*b;
    }
    else
    {
        Tmaxy = (min[1]-eye[1])*b;
        Tminy = (max[1]-eye[1])*b;
    }
    if(c>=0)
    {
        Tminz = (min[2]-eye[2])*c;
        Tmaxz = (max[2]-eye[2])*c;
    }
    else
    {
        Tmaxz = (min[2]-eye[2])*c;
        Tminz = (max[2]-eye[2])*c;
    }
    double start = std::max(std::max(Tminx,Tminy),Tminz);
    if(start<0.001)
        return false;
    double end = std::min(std::min(Tmaxx,Tmaxy),Tmaxz);
    if(start<end)
    {
        Intersection_point *ipnt;
        myvector nor;
        mypoint inters = eye+dir*start;
        if(start==Tminz&&dir[2]<0)
            nor = myvector(0,0,1);
        else if(start==Tminz&&dir[2]>0)
            nor = myvector(0,0,-1);
        else if(start==Tminx&&dir[0]>0)
            nor=myvector(-1, 0 ,0);
        else if(start==Tminx&&dir[0]<0)
            nor = myvector(1, 0, 0);
        else if(start==Tminy&&dir[1]>0)
            nor = myvector(0,-1,0);
        else
            nor = myvector(0,1,0);
        
        ipnt  = new Intersection_point(nor, inters, start , end);
        r.addIntersection(*ipnt);
        delete ipnt;
        return true;
    }
    else
        return false;
}
