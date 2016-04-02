#ifndef BVHNODE_H_
#define BVHNODE_H_
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include "surface.h"
#include "bbox.h"
#include "materials.h"
#include "ray.h"
#include "light.h"
#include "ray.h"
class Bvhnode : public Surface {
public:
    Surface * left_;
    Surface * right_;
    
    
    Bvhnode () {};
	Bvhnode (Boundingbox bounding, int num)
	{
		bbox = bounding;
		left_ = NULL;
		right_ = NULL;
        num_child = 0;
        //surfaceID = num;
        bbox.setNumSurface(num);
	}
    virtual Materials getMaterials();
    virtual void setMaterials(Materials Mater);
    virtual bool intersect (Ray &r);
    virtual void createBVHTree(const std::vector< Surface * > bboxSur, int Axis, Surface *treeroot);
    virtual Boundingbox getBoundingBbox();
   
    virtual bool intersect_bbox (Ray &ray);
    virtual bool intersect_BVH(Ray &ray, const std::vector< Surface * > &surfaces);
    
    //virtual bool intersect (Ray &r);
    Boundingbox combineBbox(Boundingbox bbox1, Boundingbox bbox2);
    void setLeftChild(Surface *leftchild) {left_ = leftchild;}
    void setRightChild(Surface *rightchild) {right_ = rightchild;}
    Surface * getLeftChild() const {return left_;}
    Surface * getRightChild() const {return right_;}
    void setNumChild(const int num) {num_child = num;}
    void setSurfaceID(const int identifier) {surfaceID = identifier;}
    void setBoundingBox(const Boundingbox bounding) {bbox = bounding;}
    mypoint getBoundingCenter() const {return bbox.getBoundingCenter();}
    myvector getReflectRay(myvector d, myvector n);

private:

Surface * head_;
Boundingbox bbox;
int num_child;
int surfaceID;
Materials material_;
};
//bool operator<(const Bvhnode &p1, const Bvhnode &p2, const int Axis);
struct EntityComp {
    int Axis;
    EntityComp(int ax) {this->Axis = ax;}
    bool operator()(Surface *s1, Surface *s2) const {
            return s1->getBoundingBbox().getBoundingCenter()[Axis] < s2->getBoundingBbox().getBoundingCenter()[Axis];
    }
};

#endif