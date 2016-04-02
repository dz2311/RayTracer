#ifndef BBOX_H_
#define BBOX_H_
#include "basicmath.h"

class Boundingbox {
public:
    Boundingbox() {}
    Boundingbox(const mypoint p1, const mypoint p2, const mypoint center) {min_ = p1; max_ = p2;central_ = center;num_surface = -2;}

    mypoint getBoundingCenter() const {return central_;}
    mypoint getBoundingMin() const {return min_;}
    mypoint getBoundingMax() const {return max_;}
    int getNumSurface() const {return num_surface;}
    void setNumSurface(const int Num) {num_surface = Num;}
    bool isInBbox(const mypoint p)
    {
        if(p[0]<=max_[0]&&p[0]>=min_[0]&&p[1]<=max_[1]&&p[1]>=min_[1]&&p[2]<=max_[2]&&p[2]>=min_[2])
            return true;
        else
            return false;
    }
    
private:
    mypoint min_;
    mypoint max_;
    mypoint central_;
    int num_surface;
};

#endif