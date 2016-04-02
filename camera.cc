    //
//  camera.cc
//  raytra_1
//

#include <vector>  // std::vector - template for array-like structures.
#include <cmath>

#include "camera.h"
#include "surface.h"
#include "ray.h"
#include <limits>
#include "plane.h"

#define CAMERARAY 1
#define SHADOWRAY 2
//
// Initialize a camera, given its paraemters.
// Called from the parser, after it has read in the description from
// the scene file.
//
void Camera::init (
    mypoint pos,
    myvector dir,
    double d,
    double iw,
    double ih,
    int pw,
    int ph)
{
    eye_ = pos;
    w_ = dir.invert ();
    w_.normalize ();
    
    focal_length_ = d;
    
    image_width_ = iw;
    image_height_ = ih;
    
    pixels_width_ = pw;
    pixels_height_ = ph;
    
    // make an orthonormal frame for the camera:
    if (1.0 - fabs (w_[1]) < .0001) {
        // camera is pointing up or down - make an arbitrary
        // right-handed coordinate system:
        u_.set (1.0, 0.0, 0.0);
        v_.crossProduct (w_, u_);
    }
    else {
        myvector up (0., 1., 0.);
        u_.crossProduct (up, w_);
        v_.crossProduct (w_, u_);
    }

    u_.normalize ();
    v_.normalize ();
    
    // set up the image:
    // OpenEXR uses exceptions - catch and print on error

    try
    {
    pixelBuffer_.resizeErase(pixels_height_, pixels_width_);    
    }
    catch (const std::exception &exc) {
        std::cerr << exc.what() << std::endl;
        return;
    }
   
    
}


//
// Generate a ray for a specific pixel:
//
Ray Camera::generateRay (
    const int i, // i is in the u direction (similar to X in cartesian)
    const int j) // j is in the v direction (similar to Y in cartesian)
{
    
    
    // get the u and v values that correspond to i and j:
    
    // we are assuming a centered image here - but maybe that
    // will change in the future, so let's calculate u,v from
    // the more general image plane description:
    double r = image_width_ / 2.0;
    double l = -1. * r;
    double t = image_height_ / 2.0;
    double b = -1. * t;

    double u = l + (r - l) * (i + .5) / pixels_width_;
    double v = t + (b - t) * (j + .5) / pixels_height_;
    
    
    // compute the direction vector d based on this:
    myvector d = w_ * -1.0 * focal_length_;
    d += u_ * u;
    d += v_ * v;
        
    d.normalize ();
    
    Ray retRay (eye_, d);
    
    return retRay;
    
}
myvector Camera::getReflectRay(myvector d, myvector n)
{
    myvector res;
    double d_dot_n = d.dotProduct(n);
    res = n * d_dot_n * -2;
    res+= d;
    return res;
}

//I don't use t to calculate the distance. I get the distance directly in the code
// mindis = 0; maxdis = distance(intersection -> light)
myvector Camera::L(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Light *thisLight, Ambientlight alight)
{
    if(recurse_limit==0)
        return myvector(0.0,0.0,0.0);
    if(ray.getRayType()==SHADOWRAY)
    {
        double min_dis_inter_to_surface = numeric_limits<double>::max();
        bool foundShadowHit = false;
        mypoint trs = ray.getOrigin();
        //if the shadow ray hits an object
        for (int whichShadowSurf = 0;whichShadowSurf <(int)surfaces.size ();++whichShadowSurf)
        {
            if(whichShadowSurf!=ray.getNumSurface())
            {
                Surface *sShadow = surfaces[whichShadowSurf];
                
                if(sShadow->intersect (ray))
                {
                    //find the nearest intersection point
                    foundShadowHit = true;
                    mypoint shadowTrs = ray.getIp().getIntersect();
                    if((shadowTrs-trs).valueOf()<min_dis_inter_to_surface)
                    {
                        min_dis_inter_to_surface = (shadowTrs-trs).valueOf();
                    }
                }
            }
        }
        /*if the light is in front of the obstacle, the intersection point is still bright*/
        double dis_inter_lp = (thisLight->getOrigin() - trs).valueOf();
        if(dis_inter_lp>min_dis_inter_to_surface)
            foundShadowHit = true;
        else foundShadowHit = false;
        
        if(!foundShadowHit) //if there is no object lying between the intersection point and the light.
        {
            return myvector(1.0,1.0,1.0);
        }
        else
            return myvector(0.0,0.0,0.0);
    }
    
    Materials mat; //materials
    mypoint origin = ray.getOrigin();
    myvector dir = ray.getDir();
    myvector n;
    
    bool foundIntersection = false;
    double min_len = numeric_limits<double>::max();
    int surface_no = 0;
    myvector sumIlumination = myvector(0.0,0.0,0.0);
    // If we intersect anything, stop since we don't need to test
    // anymore. To render with shading, however, we will have to test
    // all the objects and select the closest one.   !foundIntersection &&
    Ray r;
    mypoint trs1;
    for (int whichSurf = 0;whichSurf <(int)surfaces.size ();++whichSurf) {
        Surface *s = surfaces[whichSurf];
        Materials trs_mat = s->getMaterials();
        Ray r1 = ray;
        if(whichSurf!=r1.getNumSurface())
        {
        if (s->intersect (r1))
        {
            foundIntersection = true;
            mypoint inters = r1.getIp().getIntersect();
            myvector dis = origin-inters;//eye origin - intersect
            double len = dis.valueOf();
            if(len<min_len)
            {
                min_len = len;
                surface_no = whichSurf;
                r = r1;
                trs1 = inters;
                mat = trs_mat;
            }
        }
        }
    } // end for finding the nearest point
    if(!foundIntersection)
        return myvector(0.0, 0.0, 0.0); //if there is no intersection, return (0,0,0)
    
    
    Ray ref_ray;
    myvector v = dir.invert();
    n = r.getIp().getNormal();
    n.normalize();
        for(int whichlight = 0; whichlight<(int)lights.size (); ++whichlight)
        {
            //myvector L_rgb = myvector(0.0,0.0,0.0);
            Light *thisl = lights[whichlight];
            myvector l = (thisl->getOrigin())-trs1;
            l.normalize();
            Ray shadowRay (trs1, l);
            shadowRay.setRayType(SHADOWRAY);
            shadowRay.setNumSurface(surface_no);
            myvector lumination = myvector(0.0,0.0,0.0);
            lumination =  L(shadowRay, 1, surfaces, lights, thisl, alight);
            //cout<<" L return x "<<lumination[0]<<" y "<<lumination[1]<<" z "<<lumination[2]<<endl;
            if(lumination.valueOf()!=0.0f) //if there is no object lying between the intersection point and the light.
            {
                myvector h;
                h.bisector(v,l);
                h.normalize();
                lumination = thisl->getIlumination(mat, l, n, h);
            }//end if shadow ray hits obj
            sumIlumination+=lumination;
        }//end for
        sumIlumination += alight.getAmbientlight(mat);
        //get reflect light
       if(mat.getReflect().valueOf()!=0)
        {
            myvector ref = getReflectRay(dir, n);
            ref.normalize();
            ref_ray = Ray(trs1, ref);
            ref_ray.setNumSurface(surface_no);
            ref_ray.setRayType(CAMERARAY);
            myvector refl = L(ref_ray, recurse_limit-1, surfaces, lights, NULL, alight);
            myvector kr = mat.getReflect();
            refl.componentWiseMultiple(kr,refl);
            sumIlumination += refl;
        }
    
       return sumIlumination;
}

//
// Render the scene:
//
void Camera::renderScene (
                          std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight)
{
    
    std::cout << "rendering"<<std::endl;
    
    //int printProgress = pixels_height_ * pixels_width_ / 10.0;
    
    // for each of the pixels, generate a ray, then test against the
    // scene for intersection:
    cout<<"alight color x"<<alight.getColor()[0]<<" y "<<alight.getColor()[1]<<" z "<<alight.getColor()[2]<<endl;
    for (int yy = 0; yy < pixels_height_; ++yy) {
        for (int xx = 0; xx < pixels_width_; ++xx) {
           
            Ray r=generateRay (xx, yy);
            r.setRayType(CAMERARAY);
            r.setNumSurface(-1);
            myvector R = L(r, 20, surfaces, lights, NULL, alight);
            setPixel (xx, yy, R[0],R[1], R[2]);
        
        }//end for x pixles
       
        
    }//end for y pixels
            
    
    cout << "done!" << endl;
    
}

myvector Camera::L_bbox(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Light *thisLight, Ambientlight alight)
{
    if(recurse_limit==0)
        return myvector(0.0,0.0,0.0);
    if(ray.getRayType()==SHADOWRAY)
    {
        double min_dis_inter_to_surface = numeric_limits<double>::max();
        bool foundShadowHit = false;
        mypoint trs = ray.getOrigin();
        //if the shadow ray hits an object
        for (int whichShadowSurf = 0;whichShadowSurf <(int)surfaces.size ();++whichShadowSurf)
        {
            if(whichShadowSurf!=ray.getNumSurface())
            {
                Surface *sShadow = surfaces[whichShadowSurf];
                
                if(sShadow->intersect_bbox (ray))
                {
                    //find the nearest intersection point
                    foundShadowHit = true;
                    mypoint shadowTrs = ray.getIp().getIntersect();
                    if((shadowTrs-trs).valueOf()<min_dis_inter_to_surface)
                    {
                        min_dis_inter_to_surface = (shadowTrs-trs).valueOf();
                    }
                }
            }
        }
        /*if the light is in front of the obstacle, the intersection point is still bright*/
        double dis_inter_lp = (thisLight->getOrigin() - trs).valueOf();
        if(dis_inter_lp>min_dis_inter_to_surface)
            foundShadowHit = true;
        else foundShadowHit = false;
        
        if(!foundShadowHit) //if there is no object lying between the intersection point and the light.
        {
            return myvector(1.0,1.0,1.0);
        }
        else
            return myvector(0.0,0.0,0.0);
    }
    
    Materials mat; //materials
    mypoint origin = ray.getOrigin();
    myvector dir = ray.getDir();
    myvector n;
    
    bool foundIntersection = false;
    double min_len = numeric_limits<double>::max();
    int surface_no = 0;
    myvector sumIlumination = myvector(0.0,0.0,0.0);
    // If we intersect anything, stop since we don't need to test
    // anymore. To render with shading, however, we will have to test
    // all the objects and select the closest one.   !foundIntersection &&
    Ray r;
    mypoint trs1;
    for (int whichSurf = 0;whichSurf <(int)surfaces.size ();++whichSurf) {
        Surface *s = surfaces[whichSurf];
        Materials trs_mat = s->getMaterials();
        Ray r1 = ray;
        if(whichSurf!=r1.getNumSurface())
        {
            if (s->intersect_bbox (r1))
            {
                foundIntersection = true;
                mypoint inters = r1.getIp().getIntersect();
                myvector dis = origin-inters;//eye origin - intersect
                double len = dis.valueOf();
                if(len<min_len)
                {
                    min_len = len;
                    surface_no = whichSurf;
                    r = r1;
                    trs1 = inters;
                    mat = trs_mat;
                }
            }
        }
    } // end for finding the nearest point
    if(!foundIntersection)
        return myvector(0.0, 0.0, 0.0); //if there is no intersection, return (0,0,0)
    //std::cout<<"camera.cc intersect point x"<<trs1[0]<<" y "<<trs1[1]<<" z "<<trs1[2]<<std::endl;
    
    Ray ref_ray;
    myvector v = dir.invert();
    n = r.getIp().getNormal();
    
    n.normalize();
    for(int whichlight = 0; whichlight<(int)lights.size (); ++whichlight)
    {
        //myvector L_rgb = myvector(0.0,0.0,0.0);
        Light *thisl = lights[whichlight];
        myvector l = (thisl->getOrigin())-trs1;
        l.normalize();
        Ray shadowRay (trs1, l);
        shadowRay.setRayType(SHADOWRAY);
        shadowRay.setNumSurface(surface_no);
        myvector lumination = myvector(0.0,0.0,0.0);
        lumination =  L_bbox(shadowRay, 1, surfaces, lights, thisl, alight);
        //cout<<" L return x "<<lumination[0]<<" y "<<lumination[1]<<" z "<<lumination[2]<<endl;
        if(lumination.valueOf()!=0.0f) //if there is no object lying between the intersection point and the light.
        {
            myvector h;
            h.bisector(v,l);
            h.normalize();
            lumination = thisl->getIlumination(mat, l, n, h);
        }//end if shadow ray hits obj
        sumIlumination+=lumination;
    }//end for
    sumIlumination += alight.getAmbientlight(mat);
    //get reflect light
    if(mat.getReflect().valueOf()!=0)
    {
        myvector ref = getReflectRay(dir, n);
        ref.normalize();
        ref_ray = Ray(trs1, ref);
        ref_ray.setNumSurface(surface_no);
        ref_ray.setRayType(CAMERARAY);
        myvector refl = L_bbox(ref_ray, recurse_limit-1, surfaces, lights, NULL, alight);
        myvector kr = mat.getReflect();
        refl.componentWiseMultiple(kr,refl);
        sumIlumination += refl;
    }
    
    return sumIlumination;
}


void Camera::renderScene_bbox (std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight)
{
    
    std::cout << "rendering Bounding Boxes only"<<std::endl;
    
    //int printProgress = pixels_height_ * pixels_width_ / 10.0;
    
    // for each of the pixels, generate a ray, then test against the
    // scene for intersection:
    for (int yy = 0; yy < pixels_height_; ++yy) {
        for (int xx = 0; xx < pixels_width_; ++xx) {
            
            Ray r=generateRay (xx, yy);
            r.setRayType(CAMERARAY);
            r.setNumSurface(-1);
            myvector R = L_bbox(r, 20, surfaces, lights, NULL, alight);
            setPixel (xx, yy, R[0],R[1], R[2]);
        }//end for x pixles
        
        
    }//end for y pixels
    
    
    cout << "done!" << endl;
    
}


myvector Camera::L_BVH(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Light *thisLight, Ambientlight alight, Surface *head)
{
    Surface *root = head;
    //std::cout<<"caculate L_bvh"<<endl;
    if(recurse_limit==0)
        return myvector(0.0,0.0,0.0);
    if(ray.getRayType()==SHADOWRAY)
    {
        Ray r2 = ray;
        bool foundShadowHit = false;
        //if the shadow ray hits an object
        if(root->intersect(r2))
        {
            foundShadowHit = true;
        }
                    /*if the light is in front of the obstacle, the intersection point is still bright*/
        if(!foundShadowHit) //if there is no object lying between the intersection point and the light.
        {
            //std::cout<<"calculate shadow light"<<endl;
            return myvector(1.0,1.0,1.0);
        }
        else
        {
            //std::cout<<"shadow!!!"<<endl;
            return myvector(0.0,0.0,0.0);
        }
    }
    
    Materials mat; //materials
    myvector dir = ray.getDir();
    myvector n;
    
    bool foundIntersection = false;
    myvector sumIlumination = myvector(0.0,0.0,0.0);
    // If we intersect anything, stop since we don't need to test
    // anymore. To render with shading, however, we will have to test
    // all the objects and select the closest one.   !foundIntersection &&
    Ray r= ray;
    mypoint trs1;
    
    if (root->intersect(r))
    {
            foundIntersection = true;
            mat = r.getMaterials();
            trs1 = r.getIp().getIntersect();
    } // end for finding the nearest point
    if(!foundIntersection)
    {
        //std::cout<<"no intersect"<<endl;
        return myvector(0.0, 0.0, 0.0);
    }
    //if there is no intersection, return (0,0,0)
    //std::cout<<"camera.cc intersect point x"<<trs1[0]<<" y "<<trs1[1]<<" z "<<trs1[2]<<std::endl;
    //std::cout<<"basic hit"<<endl;
    Ray ref_ray;
    myvector v = dir.invert();
    v.normalize();
    n = r.getIp().getNormal();
    n.normalize();
    for(int whichlight = 0; whichlight<(int)lights.size (); ++whichlight)
    {
        //myvector L_rgb = myvector(0.0,0.0,0.0);
        // std::cout<<"iterate light"<<endl;
        Light *thisl = lights[whichlight];
        myvector l = (thisl->getOrigin())-trs1;
        l.normalize();
        Ray shadowRay (trs1, l);
        shadowRay.setNumSurface(r.getNumSurface());
        shadowRay.setRayType(SHADOWRAY);
        //std::cout<<"shadow ray Origin   x"<<shadowRay.getOrigin()[0]<<" y "<<shadowRay.getOrigin()[1]<<" z "<<shadowRay.getOrigin()[2]<<endl;
        myvector lumination = myvector(0.0,0.0,0.0);
        lumination =  L_BVH(shadowRay, 1, surfaces, lights, thisl, alight, head);
        //cout<<" L return x "<<lumination[0]<<" y "<<lumination[1]<<" z "<<lumination[2]<<endl;
        if(lumination.valueOf()!=0.0f) //if there is no object lying between the intersection point and the light.
        {
            myvector h;
            h.bisector(v,l);
            h.normalize();
            lumination = thisl->getIlumination(mat, l, n, h);
        }//end if shadow ray hits obj
        sumIlumination+=lumination;
    }//end for
    sumIlumination += alight.getAmbientlight(mat);
    //get reflect light
    if(mat.getReflect().valueOf()!=0)
    {
        myvector ref = getReflectRay(dir, n);
        ref.normalize();
        ref_ray = Ray(trs1, ref);
        ref_ray.setRayType(CAMERARAY);
        ref_ray.setNumSurface(r.getNumSurface());
        //std::cout<<"calculate reflec light"<<endl;
        myvector refl = L_BVH(ref_ray, recurse_limit-1, surfaces, lights, NULL, alight, head);
        myvector kr = mat.getReflect();
        refl.componentWiseMultiple(kr,refl);
        sumIlumination += refl;
    }
    
    return sumIlumination;
}


void Camera::renderScene_BVH(std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head)
{
    
    std::cout << "rendering with BVH tree"<<std::endl;
    
    //int printProgress = pixels_height_ * pixels_width_ / 10.0;
    
    // for each of the pixels, generate a ray, then test against the
    // scene for intersection:
    for (int yy = 0; yy < pixels_height_; ++yy) {
        for (int xx = 0; xx < pixels_width_; ++xx) {
            
            Ray r=generateRay (xx, yy);
            r.setRayType(CAMERARAY);
            r.setNumSurface(-1);
            
            //bool res = false;
            //res =true;
            myvector R = L_BVH(r, 20, surfaces, lights, NULL, alight, head);
            //if(res)
            //setPixel(xx,yy,1,0,0);
            // else
            setPixel (xx, yy, R[0],R[1], R[2]);
            //setPixel(xx,yy,0,0,0);
        }//end for x pixles
        
        
    }//end for y pixels
    
    
    cout << "done!" << endl;
    
}

myvector Camera::L_BVHTree(Ray ray, int recurse_limit, std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Light *thisLight, Ambientlight alight, Surface *head)
{
    Surface *root = head;
    //std::cout<<"caculate L_bvh"<<endl;
    if(recurse_limit==0)
        return myvector(0.0,0.0,0.0);
    if(ray.getRayType()==SHADOWRAY)
    {
        Ray r2 = ray;
        bool foundShadowHit = false;
        //double dis_inter_obj = numeric_limits<double>::max();
        //if the shadow ray hits an object
        if(root->intersect_BVH(r2, surfaces))
        {
            foundShadowHit = true;
            double dis_inter_lp = (thisLight->getOrigin()-r2.getOrigin()).valueOf();
            double dis_inter_obj = (r2.getIp().getIntersect()-r2.getOrigin()).valueOf();
            if(dis_inter_lp>dis_inter_obj)
                foundShadowHit = true;
            else
                foundShadowHit = false;
            if(r2.getIp().getT()<0.001)
                foundShadowHit=false;
        }
        
        /*if the light is in front of the obstacle, the intersection point is still bright*/
        if(!foundShadowHit) //if there is no object lying between the intersection point and the light.
        {
            //std::cout<<"calculate shadow light"<<endl;
            return myvector(1.0,1.0,1.0);
        }
        else
        {
            //std::cout<<"shadow!!!"<<endl;
            return myvector(0.0,0.0,0.0);
        }
    }
    
    Materials mat; //materials
    myvector dir = ray.getDir();
    myvector n;
    
    bool foundIntersection = false;
    myvector sumIlumination = myvector(0.0,0.0,0.0);
    // If we intersect anything, stop since we don't need to test
    // anymore. To render with shading, however, we will have to test
    // all the objects and select the closest one.   !foundIntersection &&
    Ray r= ray;
    mypoint trs1;
    
    if (root->intersect_BVH(r, surfaces))
    {
            foundIntersection = true;
            mat = r.getMaterials();
            trs1 = r.getIp().getIntersect();
    }
    
    // end for finding the nearest point
    if(!foundIntersection)
    {
        //std::cout<<"no intersect"<<endl;
        return myvector(0.0, 0.0, 0.0);
    }
    //if there is no intersection, return (0,0,0)
    //std::cout<<"camera.cc intersect point x"<<trs1[0]<<" y "<<trs1[1]<<" z "<<trs1[2]<<std::endl;
    //std::cout<<"basic hit"<<endl;
    Ray ref_ray;
    myvector v = dir.invert();
    v.normalize();
    n = r.getIp().getNormal();
    n.normalize();
    for(int whichlight = 0; whichlight<(int)lights.size (); ++whichlight)
    {
        //myvector L_rgb = myvector(0.0,0.0,0.0);
        // std::cout<<"iterate light"<<endl;
        Light *thisl = lights[whichlight];
        myvector l = (thisl->getOrigin())-trs1;
        l.normalize();
        Ray shadowRay (trs1, l);
        shadowRay.setNumSurface(r.getNumSurface());
        shadowRay.setRayType(SHADOWRAY);
        //std::cout<<"shadow ray Origin   x"<<shadowRay.getOrigin()[0]<<" y "<<shadowRay.getOrigin()[1]<<" z "<<shadowRay.getOrigin()[2]<<endl;
        myvector lumination = myvector(0.0,0.0,0.0);
        lumination =  L_BVHTree(shadowRay, 1, surfaces, lights, thisl, alight, head);
        //cout<<" L return x "<<lumination[0]<<" y "<<lumination[1]<<" z "<<lumination[2]<<endl;
        if(lumination.valueOf()!=0.0f) //if there is no object lying between the intersection point and the light.
        {
            myvector h;
            h.bisector(v,l);
            h.normalize();
            lumination = thisl->getIlumination(mat, l, n, h);
        }//end if shadow ray hits obj
        sumIlumination+=lumination;
    }//end for
    //sumIlumination += alight.getAmbientlight(mat);
    //get reflect light
    if(mat.getReflect().valueOf()!=0)
    {
        myvector ref = getReflectRay(dir, n);
        ref.normalize();
        ref_ray = Ray(trs1, ref);
        ref_ray.setRayType(CAMERARAY);
        ref_ray.setNumSurface(r.getNumSurface());
        //std::cout<<"calculate reflec light"<<endl;
        myvector refl = L_BVHTree(ref_ray, recurse_limit-1, surfaces, lights, NULL, alight, head);
        myvector kr = mat.getReflect();
        refl.componentWiseMultiple(kr,refl);
        sumIlumination += refl;
    }
    
    return sumIlumination;
}


void Camera::renderScene_BVHTree(std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head)
{
    
    std::cout << "rendering with BVH tree"<<std::endl;
    
    //int printProgress = pixels_height_ * pixels_width_ / 10.0;
    
    // for each of the pixels, generate a ray, then test against the
    // scene for intersection:
    for (int yy = 0; yy < pixels_height_; ++yy) {
        for (int xx = 0; xx < pixels_width_; ++xx) {
            
            Ray r=generateRay (xx, yy);
            r.setRayType(CAMERARAY);
            r.setNumSurface(-1);
            
            //bool res = false;
            //res =true;
            myvector R = L_BVHTree(r, 20, surfaces, lights, NULL, alight, head);
            //if(res)
            //setPixel(xx,yy,1,0,0);
            // else
            setPixel (xx, yy, R[0],R[1], R[2]);
            //setPixel(xx,yy,0,0,0);
        }//end for x pixles
        
        
    }//end for y pixels
    
    
    cout << "done!" << endl;
    
}

Ray Camera::generateRay_Sampling (
                         const int i, // i is in the u direction (similar to X in cartesian)
                         const int j, const int p, const int q, const int num_PR) // j is in the v direction (similar to Y in cartesian)
{
    
    
    // get the u and v values that correspond to i and j:
    
    // we are assuming a centered image here - but maybe that
    // will change in the future, so let's calculate u,v from
    // the more general image plane description:
    double r = image_width_ / 2.0;
    double l = -1. * r;
    double t = image_height_ / 2.0;
    double b = -1. * t;
    double delta1, delta2;
    //srand((unsigned)time(NULL));
    delta1 = rand() / (double)RAND_MAX;
    delta2 = rand() / (double)RAND_MAX;
    //cout<<"delta 1:"<< delta1<<endl;
    //cout<<"delta 2:"<<delta2<<endl;
    double u = l + (r - l) * (i + (p+delta1)/num_PR) / pixels_width_;
    double v = t + (b - t) * (j + (q+delta2)/num_PR) / pixels_height_;
    
    
    // compute the direction vector d based on this:
    myvector d = w_ * -1.0 * focal_length_;
    d += u_ * u;
    d += v_ * v;
    
    d.normalize ();
    
    Ray retRay (eye_, d);
    
    return retRay;
    
}


myvector Camera::L_BVHTree_Sampling(Ray ray, int recurse_limit,const std::vector< Surface * > &surfaces, const std::vector< Light * > &lights, Light *thisLight,Ambientlight alight,Surface *head,const int num_SR)
{
    if(recurse_limit==0)
        return myvector(0.0,0.0,0.0);
    if(ray.getRayType()==SHADOWRAY)
    {
        Ray r2 = ray;
        bool foundShadowHit = false;
        if(head->intersect_BVH(r2, surfaces))
        {
            foundShadowHit = true;
            double dis_inter_lp = (thisLight->getOrigin()-r2.getOrigin()).valueOf();
            double dis_inter_obj = (r2.getIp().getIntersect()-r2.getOrigin()).valueOf();
            if(dis_inter_lp<dis_inter_obj)
                foundShadowHit = false;
            if(r2.getIp().getT()<0.001)
                foundShadowHit=true;
        }
        
        if(!foundShadowHit) //if there is no object lying between the intersection point and the light.
        {
            return myvector(1.0,1.0,1.0);
        }
        else
        {
            return myvector(0.0,0.0,0.0);
        }
    }
    
    Materials mat; //materials
    myvector dir = ray.getDir();
    myvector n;
    
    bool foundIntersection = false;
    myvector sumIlumination = myvector(0.0,0.0,0.0);
    Ray r= ray;
    mypoint trs1;
    
    if (head->intersect_BVH(r, surfaces))
    {
        foundIntersection = true;
        mat = r.getMaterials();
        trs1 = r.getIp().getIntersect();
    }
    
    if(!foundIntersection)
    {
        return myvector(0.0, 0.0, 0.0);
    }
    Ray ref_ray;
    myvector v = dir.invert();
    v.normalize();
    n = r.getIp().getNormal();
    n.normalize();
    for(int whichlight = 0; whichlight<(int)lights.size (); ++whichlight)
    {
        Light *thisl = lights[whichlight];
        if(thisl->getLightType()==1)
        {
            myvector l = (thisl->getOrigin())-trs1;
            l.normalize();
            Ray shadowRay (trs1, l);
            shadowRay.setNumSurface(r.getNumSurface());
            shadowRay.setRayType(SHADOWRAY);
            myvector lumination = myvector(0.0,0.0,0.0);
            lumination =  L_BVHTree_Sampling(shadowRay, 1, surfaces, lights, thisl, alight, head, num_SR);
            if(lumination.valueOf()!=0.0f) //if there is no object lying between the intersection point and the light.
            {
                myvector h;
                h.bisector(v,l);
                h.normalize();
                lumination = thisl->getIlumination(mat, l, n, h);
            }//end if shadow ray hits obj
            sumIlumination+=lumination;
        }
        else
        {
            myvector lumination1 = myvector(0.0,0.0,0.0);
            for(int p=0;p<num_SR;++p){
                for(int q=0;q<num_SR;++q){
                    mypoint lsample = thisl->generateShadowRay(p, q, num_SR);
                    myvector l = lsample - trs1;
                    l.normalize();
                    Ray shadowRay (trs1, l);
                    shadowRay.setNumSurface(r.getNumSurface());
                    shadowRay.setRayType(SHADOWRAY);
                    myvector lumination = myvector(0.0,0.0,0.0);
                    lumination =  L_BVHTree_Sampling(shadowRay, 1, surfaces, lights, thisl, alight, head, num_SR);
                    if(lumination.valueOf()!=0.0f) //if there is no object lying between the intersection point and the light.
                    {
                        myvector h;
                        h.bisector(v,l);
                        h.normalize();
                        lumination = thisl->getIlumination(mat, l, n, h);
                        double d = (lsample - trs1).valueOf();
                        lumination =lumination/((d+1)*(d+1));
                        //lumination = lumination * max((double)0.0, l.dotProduct(thisl->getLightSourceNor()));
                        lumination = lumination * max((double)0,n.dotProduct(l));
                    }
                    lumination1+=lumination;
                }
            }
            lumination1 = lumination1/(num_SR*num_SR);
            sumIlumination+=lumination1;
        }
        thisl = NULL;
        delete thisl;
    }//end for
    sumIlumination += alight.getAmbientlight(mat);
    if(mat.getReflect().valueOf()!=0)
    {
        myvector ref = getReflectRay(dir, n);
        ref.normalize();
        ref_ray = Ray(trs1, ref);
        ref_ray.setRayType(CAMERARAY);
        ref_ray.setNumSurface(r.getNumSurface());
        myvector refl = L_BVHTree_Sampling(ref_ray, recurse_limit-1, surfaces, lights, NULL, alight, head, num_SR);
        myvector kr = mat.getReflect();
        refl.componentWiseMultiple(kr,refl);
        sumIlumination += refl;
    }
    
    return sumIlumination;
}
/*
myvector Camera::returnPixelValue(const int xx, const int yy, const std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head, int num_PR, int num_SR)
{
    myvector R = myvector(0.0,0.0,0.0);
    for(int i = 0; i< num_PR; ++i){
        for(int j=0; j< num_PR; ++j){
            Ray r=generateRay_Sampling (xx, yy, i, j, num_PR);
            r.setRayType(CAMERARAY);
            r.setNumSurface(-1);
            R+=head->L_BVHTree_Sampling(r, 2, surfaces, lights, NULL, alight, num_SR);
        }
    }
    return R = R/(num_PR*num_PR);
}*/
void Camera::renderScene_BVHTree_Sampling(const std::vector< Surface * > &surfaces, std::vector< Light * > &lights, Ambientlight alight, Surface *head, int num_PR, int num_SR)
{
    
    std::cout << "rendering with BVH tree"<<std::endl;
    for (int yy = 0; yy < pixels_height_; ++yy) {
        for (int xx = 0; xx < pixels_width_; ++xx) {
            myvector R = myvector(0.0,0.0,0.0);
            for(int i = 0; i< num_PR; ++i){
                for(int j=0; j< num_PR; ++j){
                    Ray r=generateRay_Sampling (xx, yy, i, j, num_PR);
                    r.setRayType(CAMERARAY);
                    r.setNumSurface(-1);
                    R += L_BVHTree_Sampling(r, 20, surfaces, lights, NULL, alight, head, num_SR);
                }
            }
             R = R/(num_PR*num_PR);
            //R += returnPixelValue(xx, yy, surfaces, lights, alight, head,num_PR, num_SR);
            setPixel (xx, yy, R[0],R[1], R[2]);
           
            //setPixel(xx,yy,0,0,0);
        }//end for x pixles
        
        
    }//end for y pixels
    
    
    cout << "done!" << endl;
    
}



//
// Save out the final image:
//
void Camera::writeImage (
    const char *sceneFile)
{
    
    // write the image data as an openEXR file
    // OpenEXR uses exceptions - catch and print on error
    try
    {
    RgbaOutputFile file (sceneFile, pixels_width_, pixels_height_, WRITE_RGBA);
    file.setFrameBuffer (&pixelBuffer_[0][0], 1, pixels_width_);
    file.writePixels (pixels_height_);
    }
    
    catch (const std::exception &exc) {
        std::cerr << exc.what() << std::endl;
        return;
    }

}


