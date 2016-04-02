//
//  ray.h
//  raytra_1
//

#ifndef RAY_H_
#define RAY_H_

#include "basicmath.h"
#include "intersectionpoint.h"
#include "materials.h"
class Ray {
    
public:
    

    
    Ray () {}
    Ray (const mypoint &p, const myvector &d) {
        p_ = p;
        d_ = d;
    }
        //
    // Get and set functions, for the direction and origin:
    void setDir (const myvector dir)   { d_ = dir; }
    void setOrigin (const mypoint pnt) { p_ = pnt; }
  
    void addIntersection(Intersection_point inter)
    {intersect = inter;}
    void setRayType(int i) {ray_type = i;}
    void setNumSurface(int i){numSurface = i;}
    //void setIntersection(const mypoint inters) {intersect_ = inters;}
    //void setSurfaceCenter(const mypoint pnt) {surface_center_ = pnt;}
    //void setDis(const double dis) {surface_intersection_ = dis;}
    
    //mypoint getIntersection() const {return intersect_;}
    //mypoint getSurfaceCenter() const {return surface_center_;}
    //double getDis() const {return surface_intersection_;}

    int getNumSurface() const {return numSurface;}
    myvector getDir () const { return d_; }
    mypoint  getOrigin () const { return p_; }
    Intersection_point getIp() const {return intersect;}
    int getRayType() const {return ray_type;}
    void setMaterials(const Materials mat) {material_ = mat;}
    Materials getMaterials() const {return material_;}
private:
    int numSurface; //the surface the ray is generated
    myvector    d_; // direction of ray
    mypoint     p_; // origin of ray
    Intersection_point intersect;
    int ray_type; //1-camera ray 2-shadow ray
    Materials material_;
    //mypoint surface_center_;
    //double surface_intersection_;
    //mypoint    intersect_; //intersection point with object surface
    
    
    
};


#endif
