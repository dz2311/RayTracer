//
//  camera.h
//  raytra_1
//

#ifndef CAMERA_H_
#define CAMERA_H_
#include "bbox.h"
#include "bvhnode.h"
#include "ray.h"
#include "light.h"
#include "materials.h"
#include "intersectionpoint.h"
#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
using namespace Imf;
using namespace std;

class Surface;


//
//
//
class Camera {
    
public:
    
    void    init (
                  mypoint pos,
                  myvector dir,
                  double d,
                  double iw,
                  double ih,
                  int pw,
                  int ph);

    Ray     generateRay (const int i, const int j);
    void    renderScene (std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight);
    void    writeImage (const char *sceneFile);
    myvector getReflectRay(myvector d, myvector n);
    mypoint getEye() const {return eye_;}
    myvector L(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights,Light *thisLight, Ambientlight alight);
    void    setPixel (int pix_x, int pix_y, float r, float g, float b) {
        Rgba &px = pixelBuffer_[pix_y][pix_x];
        px.r = r;
        px.g = g;
        px.b = b;
        px.a = 1.0;
    }
    
    //Bounding Boxes
    void renderScene_bbox (std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight);
    myvector L_bbox(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights,Light *thisLight, Ambientlight alight);
   
    void renderScene_BVH(std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head);
    myvector L_BVH(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights,Light *thisLight, Ambientlight alight, Surface *head);
    
    void renderScene_BVHTree(std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head);
    myvector L_BVHTree(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights,Light *thisLight, Ambientlight alight, Surface *head);
    
    void renderScene_BVHTree_Sampling(const std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head, int num_PR, int num_SR);
    Ray generateRay_Sampling (const int i, const int j, const int p, const int q, const int num_PR);
    myvector L_BVHTree_Sampling(Ray ray, int recurse_limit,const std::vector< Surface * > &surfaces,const std::vector< Light * > &lights,Light *thisLight,Ambientlight alight, Surface *head,const int num_SR);
   // myvector returnPixelValue(const int xx, const int yy, const std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head, int num_PR, int num_SR);
private:
    
    // right-handed coordinate system:
    myvector   u_;  // u goes to the "right" with respect to the image plane
    myvector   v_;  // v goes "up" with respect to the image plane
    myvector   w_;  // remember: points in OPPOSITE the viewing direction!
    
    mypoint eye_;   // position of camera, & center of projection 
    
    double   image_width_;
    double   image_height_;
    
    double   focal_length_;
    
    int     pixels_width_;
    int     pixels_height_;
    
    Array2D<Rgba>    pixelBuffer_;  // the image we are rendering
    
};

#endif
