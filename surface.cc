
//  surface.cc
//  raytra_1
//


#include "surface.h"

bool Surface::intersect (Ray &ray)
{
    std::cout << "error: surface::intersect should not be called - it's virtual!"
    << std::endl;
    
    return false;
}
Materials Surface::getMaterials()
{
    std::cout << "error: surface::getMaterials should not be called - it's virtual!"
    << std::endl;
    return materials_;
}
void Surface::setMaterials(Materials Mater)
{
    std::cout << "error: surface::setMaterials should not be called - it's virtual!"
    << std::endl;
    
}
bool Surface::intersect_bbox (Ray &ray)
{
    std::cout << "error: surface::intersect_bbox should not be called - it's virtual!"
    << std::endl;
    
    return false;
}

Boundingbox Surface::getBoundingBbox()
{
    std::cout << "error: surface::getBoundingBbox should not be called - it's virtual!"
    << std::endl;
    Boundingbox *res = new Boundingbox();
    return *res;
}

void Surface::createBVHTree(std::vector< Surface * > bboxSur, int Axis, Surface *treeroot)
{
    std::cout << "error: surface::CreateBVHTree should not be called - it's virtual!"
    << std::endl;
}
Surface* Surface::getLeftChild()
{
    std::cout << "error: surface::getLeftChild should not be called - it's virtual!"
    << std::endl;
    Surface *res = new Surface();
    return res;
}
Surface* Surface::getRightChild()
{
    std::cout << "error: surface::getRightChild should not be called - it's virtual!"
    << std::endl;
    Surface *res = new Surface();
    return res;
}

bool Surface::intersect_BVH(Ray &ray, const std::vector< Surface * > &surfaces)
{
    std::cout << "error: surface::intersect_BVH should not be called - it's virtual!"
    << std::endl;
    return true;
}

myvector Surface::L_BVHTree_Sampling(Ray& ray, int recurse_limit,const std::vector< Surface * > &surfaces, const std::vector< Light * > &lights, Light *thisLight,Ambientlight alight,const int num_SR)
{
    std::cout << "error: surface::L_BVHTree_Sampling should not be called - it's virtual!"
    << std::endl;
    return myvector(0.0,0.0,0.0);
}

