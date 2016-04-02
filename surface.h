//
//  surface.h
//  raytra_1
//

#ifndef SURFACE_H_
#define SURFACE_H_



#include <iostream>
#include <vector>
#include "materials.h"
#include "ray.h"
#include "bbox.h"
#include "light.h"

//
// Abstract surface class: all geometry should derive from this
// class so that we can put them in lists, etc.
//
class Surface {
    
public:
    
    virtual bool intersect (Ray &ray);
    virtual Materials getMaterials();
    virtual void setMaterials(Materials Mater);
    virtual Boundingbox getBoundingBbox();
    virtual bool intersect_bbox (Ray &ray);
    virtual void createBVHTree(std::vector< Surface * > bboxSur, int Axis, Surface *treeroot);
    virtual Surface* getLeftChild();
    virtual Surface* getRightChild();
    virtual bool intersect_BVH(Ray &ray, const std::vector< Surface * > &surfaces);
    virtual myvector L_BVHTree_Sampling(Ray& ray, int recurse_limit,const std::vector< Surface * > &surfaces, const std::vector< Light * > &lights, Light *thisLight,Ambientlight alight,const int num_SR);
private:
    Materials materials_;
    //Boundingbox bbox;
};



#endif
