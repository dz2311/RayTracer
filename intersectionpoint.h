#ifndef INTERSECTIONPOINT_H_
#define INTERSECTIONPOINT_H_
#include "ray.h"


class Intersection_point {
public:
    Intersection_point(){}
    
    
    
    Intersection_point(mypoint& center, mypoint& intersection, double dis, myvector& nor, double t_value)
    {
        surface_center_ = center;
        intersect_ = intersection;
        surface_ip_dis = dis;
        normal = nor;
        surface_type = 1;
        t_ = t_value;
    }
    
    
    Intersection_point(mypoint& inters, myvector& mor, double t_value)
    {
        intersect_ = inters;
        normal = mor;
        surface_center_.set(0,0,0);
        surface_ip_dis = 0;
        surface_type = 2;
        t_ = t_value;
        tmax_ = 0.0;
    }
    
    Intersection_point(myvector& mor, mypoint& inters, double t_value)
    {
        intersect_ = inters;
        normal = mor;
        surface_center_.set(0,0,0);
        surface_ip_dis = 0;
        surface_type = 3;
        t_ = t_value;
        tmax_ = 0.0;
    }
    
    Intersection_point(myvector& mor, mypoint& inters, double t_value, double tmax)
    {
        intersect_ = inters;
        normal = mor;
        surface_center_.set(0,0,0);
        surface_ip_dis = 0;
        surface_type = 4;
        t_ = t_value;
        tmax_ = tmax;
    }
    
    
    mypoint getIntersect() const {return intersect_;}
    double getSurfacePdis() const {return surface_ip_dis;}
    mypoint getSurfaceCenter() const {return surface_center_;}
    myvector getNormal() const {return normal;}
    int getSurfaceType() const {return surface_type;}
    double getT() const {return t_;};
    void setT(const double tvalue) {t_ = tvalue;}
    double getTmax () const {return tmax_;}
private:
    mypoint surface_center_;
    double surface_ip_dis;
    mypoint    intersect_; //intersection point with object surface
    myvector normal;
    int surface_type;
    double t_;
    double tmax_;
};

#endif