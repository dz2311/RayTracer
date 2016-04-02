#ifndef PLANE_H_
#define PLANE_H_
#include "ray.h"
#include "basicmath.h"
#include "intersectionpoint.h"
#include "surface.h"
#include "materials.h"
#include "bbox.h"
#include <limits>
#include <cmath>
class Plane : public Surface {
    
    
public:
    Plane (myvector normal, double scale) {
        normal_ = normal;
        normal_.normalize();scale_ = scale;
        mypoint bbox_min = mypoint(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
        mypoint bbox_max = mypoint(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
        mypoint center = mypoint(0,0,0);
        bbox = Boundingbox(bbox_min, bbox_max, center);
    }
  
    
    virtual bool intersect (Ray &r);
    virtual Materials getMaterials();
    virtual void setMaterials(Materials Mater);
    //void setMaterials(Materials Mater);
    double getScale() const {return scale_;}
    myvector getNormal() const {return normal_;}
    virtual Boundingbox getBoundingBox();
    
    virtual bool intersect_bbox (Ray &r);
    //int getSurfaceType() const {return surface_type;}
private:
    
    myvector normal_;
    double  scale_;
    Materials material_;
    Boundingbox bbox;
};



#endif