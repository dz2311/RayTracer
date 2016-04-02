#ifndef TRIANGLE_H_
#define TRIANGLE_H_
#include <iostream>

#include "ray.h"
#include "surface.h"
#include "materials.h"
#include "bbox.h"
#include <stdlib.h>
using namespace std;

//
//
//
class Triangle : public Surface {


public:
    Triangle (mypoint p1, mypoint p2, mypoint p3) {
        a_ = p1;
        b_ = p2;
        c_ = p3;
        double minx, miny, minz;
        double maxx, maxy, maxz;
        minx = std::min(std::min(p1[0],p2[0]),p3[0]);
        miny = std::min(std::min(p1[1],p2[1]),p3[1]);
        minz = std::min(std::min(p1[2],p2[2]),p3[2]);
        maxx = std::max(std::max(p1[0],p2[0]),p3[0]);
        maxy = std::max(std::max(p1[1],p2[1]),p3[1]);
        maxz = std::max(std::max(p1[2],p2[2]),p3[2]);
        if(maxx-minx<0.001f)
        {
            maxx+=0.001;
        }
        if(maxy-miny<0.001f)
        {
            maxy+=0.001;
        }
        if(maxz-minz<0.001f)
        {
            maxz+=0.001;
        }
        mypoint pmin = mypoint(minx, miny, minz);
        mypoint pmax = mypoint(maxx, maxy, maxz);
        mypoint midpoint = mypoint((p1[0]+p2[0]+p3[0])/3, (p1[1]+p2[1]+p3[1])/3, (p1[2]+p2[2]+p3[2])/3);
        //if(abs(midpoint[0])>=250||abs(midpoint[1])>=250||abs(midpoint[2])>=250)
            //midpoint = mypoint(0,0,0);
        bbox = Boundingbox(pmin,pmax,midpoint);
    }
    ~Triangle ();
    
    virtual bool intersect (Ray &r);
    virtual Materials getMaterials();
    virtual void setMaterials(Materials Mater);
    virtual Boundingbox getBoundingBbox();
    virtual bool intersect_bbox (Ray &r);
    mypoint getMypoint1() const {return a_;}
    mypoint getMypoint2() const {return b_;}
    mypoint getMypoint3() const {return c_;}

private:
    
    mypoint a_;
    mypoint b_;
    mypoint c_;

    Materials material_;
    Boundingbox bbox;
};

#endif