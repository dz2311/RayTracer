//
//  sphere.h
//  raytra_1
//

#ifndef SPHERE_H_
#define SPHERE_H_

#include <iostream>
#include "ray.h"
#include "surface.h"
#include "materials.h"
#include "basicmath.h"

//
//
//
class Sphere : public Surface {


public:
    Sphere (mypoint pos, double r) {
        center_ = pos;
        radius_ = r;
        mypoint bbox_min = mypoint(pos[0]-r, pos[1]-r, pos[2]-r);
        mypoint bbox_max = mypoint(pos[0]+r, pos[1]+r, pos[2]+r);
        bbox = Boundingbox(bbox_min, bbox_max, center_);
    }
    ~Sphere ();
    
    virtual bool intersect (Ray &r);
    virtual Materials getMaterials();
    virtual void setMaterials(Materials Mater);
    double getRadius() const {return radius_;}
    mypoint getCenter() const {return center_;}
    virtual Boundingbox getBoundingBbox();
    virtual bool intersect_bbox (Ray &r);
    
private:
    
    mypoint center_;
    double  radius_;
    Materials material_;
    Boundingbox bbox;
};

#endif
