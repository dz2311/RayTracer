#include "triangle.h"

bool Triangle::intersect (Ray &r)
{
	double a = a_[0] - b_[0];
	double b = a_[1] - b_[1];
	double c = a_[2] - b_[2];
	double d = a_[0] - c_[0];
	double e = a_[1] - c_[1];
	double f = a_[2] - c_[2];
	double g = r.getDir()[0];
	double h = r.getDir()[1];
	double i = r.getDir()[2];
	double j = a_[0] - r.getOrigin()[0];
	double k = a_[1] - r.getOrigin()[1];
	double l = a_[2] - r.getOrigin()[2];
    
    double M = a*(e*i-h*f)+b*(g*f-d*i)+c*(d*h-e*g);
    
    
    double Beta = (j*(e*i-h*f)+k*(g*f-d*i)+l*(d*h-e*g))/M;
    double gamma = (i*(a*k-j*b)+h*(j*c-a*l)+g*(b*l-k*c))/M;
    double t = -(f*(a*k-j*b)+e*(j*c-a*l)+d*(b*l-k*c))/M;
    
    myvector trs1 = c_ - a_;
    myvector trs2 = b_ - a_;
    myvector nor;
    nor.crossProduct(trs1, trs2);
    nor = nor*(-1);
    nor.normalize();
    if(t<=0.001)
        return false;
    if(gamma<0||gamma>1)
        return false;
    
    if(Beta<0||Beta>1-gamma)
        return false;
    mypoint ori = r.getOrigin() + (r.getDir()*t);
    
    Intersection_point *ipnt;
    ipnt  = new Intersection_point(nor,ori, t);
    r.addIntersection(*ipnt);
    return true;
} 


Materials Triangle::getMaterials()
{
    return material_;
}
void Triangle::setMaterials(Materials Mater)
{
    material_ = Mater;
    
}

bool Triangle::intersect_bbox (Ray &r)
{
    double Tminx,Tminy,Tminz;
    double Tmaxx,Tmaxy,Tmaxz;
    mypoint eye = r.getOrigin();
    myvector dir = r.getDir();
    mypoint min = bbox.getBoundingMin();
    //std::cout<<"triangle min x"<<min[0]<<" y "<<min[1]<<" z "<<min[2]<<std::endl;
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
    if(start < 0.001f)
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


Boundingbox Triangle::getBoundingBbox()
{
    return bbox;
}