#include "bvhnode.h"
#include "ray.h"
#include <limits>
#include <cmath>


#define CAMERARAY 1
#define SHADOWRAY 2
Boundingbox Bvhnode::combineBbox(Boundingbox bbox1, Boundingbox bbox2)
{
    double minx,miny,minz;
    double maxx,maxy,maxz;
    
    minx = std::min(bbox1.getBoundingMin()[0],bbox2.getBoundingMin()[0]);
    miny = std::min(bbox1.getBoundingMin()[1],bbox2.getBoundingMin()[1]);
    minz = std::min(bbox1.getBoundingMin()[2],bbox2.getBoundingMin()[2]);
    
    maxx = std::max(bbox1.getBoundingMax()[0],bbox2.getBoundingMax()[0]);
    maxy = std::max(bbox1.getBoundingMax()[1],bbox2.getBoundingMax()[1]);
    maxz = std::max(bbox1.getBoundingMax()[2],bbox2.getBoundingMax()[2]);
    
    mypoint minp = mypoint(minx, miny, minz);
    mypoint maxp = mypoint(maxx, maxy, maxz);
    //mypoint center = mypoint((bbox1.getBoundingCenter()[0]+bbox2.getBoundingCenter()[0])/2, (bbox1.getBoundingCenter()[1]+bbox2.getBoundingCenter()[1])/2, (bbox1.getBoundingCenter()[2]+bbox2.getBoundingCenter()[2])/2);
    mypoint center = mypoint((minp[0]+maxp[0])/2, (minp[1]+maxp[1])/2, (minp[2]+maxp[2])/2);
    
    Boundingbox res = Boundingbox(minp, maxp, center);
    return res;
    
}


Materials Bvhnode::getMaterials()
{
    return material_;
}
void Bvhnode::setMaterials(Materials Mater)
{
   material_ = Mater;
    
};

Boundingbox Bvhnode::getBoundingBbox()
{
    return bbox;
}

myvector Bvhnode::getReflectRay(myvector d, myvector n)
{
    myvector res;
    double d_dot_n = d.dotProduct(n);
    res = n * d_dot_n * -2;
    res+= d;
    return res;
}

void Bvhnode::createBVHTree(std::vector< Surface * > bvhSur, int Axis, Surface *treeroot)
{
    int len = (int)bvhSur.size();
    Boundingbox bounding;
    Boundingbox originBound;
    left_ = new Bvhnode();
    right_ = new Bvhnode();
    if(len==0)
    {
        left_ = NULL;
        right_ = NULL;
        return;
    }
 	else if(len==1)
    {
        left_ = bvhSur[0];
        right_ = NULL;
        bounding = bvhSur[0]->getBoundingBbox();
        bounding.setNumSurface(-2);
    }
    else if(len == 2)
    {
        left_ = bvhSur[0];
        right_ = bvhSur[1];
        bounding = combineBbox(bvhSur[0]->getBoundingBbox(), bvhSur[1]->getBoundingBbox());
        bounding.setNumSurface(-2);
    }
    else
    {
        //std::cout<<"len>=3"<<std::endl;
        std::vector< Surface * > bvhSur_left;
        std::vector< Surface * > bvhSur_right;
        //std::cout<<"ready to sort"<<std::endl;
        std::sort(bvhSur.begin(), bvhSur.end(), EntityComp(Axis));
        int i;
        for(i=0;i<len;i++)
        {
            if(i<=int(len/2))
                bvhSur_left.push_back(bvhSur[i]);
            else
                bvhSur_right.push_back(bvhSur[i]);
        }
        
        left_->createBVHTree(bvhSur_left, (Axis+1)%3, treeroot);
        right_->createBVHTree(bvhSur_right, (Axis+1)%3, treeroot);
        bounding = combineBbox(left_->getBoundingBbox(), right_->getBoundingBbox());
        bounding.setNumSurface(-3);
    }
    //head_ = treeroot;
    //std::cout<<"Bounding box min: x"<<Bounding.getBoundingMin()[0]<<" y "<<Bounding.getBoundingMin()[1]<<" z "<<Bounding.getBoundingMin()[2]<<std::endl;
    //std::cout<<"Bounding box min: x"<<Bounding.getBoundingMax()[0]<<" y "<<Bounding.getBoundingMax()[1]<<" z "<<Bounding.getBoundingMax()[2]<<std::endl;
    setBoundingBox(bounding);
}

bool Bvhnode::intersect_bbox (Ray &r)
{
    if(r.getNumSurface()==bbox.getNumSurface()&&r.getNumSurface()>=0)
    {
        //std::cout<<"This is the origin surface"<<std::endl;
        return false;
    }
    double Tminx,Tminy,Tminz;
    double Tmaxx,Tmaxy,Tmaxz;
    mypoint eye = r.getOrigin();
    myvector dir = r.getDir();
    mypoint min = bbox.getBoundingMin();
    mypoint max = bbox.getBoundingMax();
    
    double a = 1/dir[0];
    double b = 1/dir[1];
    double c = 1/dir[2];
    if(a>=0)
    {
        Tminx = (min[0]-eye[0])*a;
        Tmaxx = (max[0]-eye[0])*a;
    }
    else
    {
        Tmaxx = (min[0]-eye[0])*a;
        Tminx = (max[0]-eye[0])*a;
    }
    if(b>=0)
    {
        Tminy = (min[1]-eye[1])*b;
        Tmaxy = (max[1]-eye[1])*b;
    }
    else
    {
        Tmaxy = (min[1]-eye[1])*b;
        Tminy = (max[1]-eye[1])*b;
    }
    if(c>=0)
    {
        Tminz = (min[2]-eye[2])*c;
        Tmaxz = (max[2]-eye[2])*c;
    }
    else
    {
        Tmaxz = (min[2]-eye[2])*c;
        Tminz = (max[2]-eye[2])*c;
    }
    double start = std::max(std::max(Tminx,Tminy),Tminz);
    //if(start<0.001)
       // return false;
    double end = std::min(std::min(Tmaxx,Tmaxy),Tmaxz);
    if(start<end)
    {
        Intersection_point *ipnt;
        myvector nor;
        mypoint inters = eye+dir*start;
        if(start==Tminz&&dir[2]<0)
            nor = myvector(0,0,1);
        else if(start==Tminz&&dir[2]>0)
            nor = myvector(0,0,-1);
        else if(start==Tminx&&dir[0]>0)
            nor=myvector(-1, 0 ,0);
        else if(start==Tminx&&dir[0]<0)
            nor = myvector(1, 0, 0);
        else if(start==Tminy&&dir[1]>0)
            nor = myvector(0,-1,0);
        else
            nor = myvector(0,1,0);
        //myvector rayinter = inters - r.getOrigin();
        //if(rayinter.dotProduct(nor)>0)
            //return false;
        ipnt  = new Intersection_point(nor, inters, start , end);
        r.addIntersection(*ipnt);
        r.setMaterials(material_);
        r.setNumSurface(bbox.getNumSurface());
        delete ipnt;
        return true;
    }
    else
        return false;

}

bool Bvhnode::intersect (Ray &r)
{
    bool leftRes = false;
    bool rightRes = false;
    Ray rleft = r;
    Ray rright = r;
    if(bbox.isInBbox(r.getOrigin()))
    {
        if(left_!=NULL)
            leftRes = left_->intersect(rleft);
        if(right_!=NULL)
            rightRes = right_->intersect(rright);
        if(leftRes&&rightRes)
        {
            //if(rleft.getIp().getT()>0.001&&rright.getIp().getT()>0.001)
                r = rleft.getIp().getT()>rright.getIp().getT()?rright:rleft;
            //else if(rleft.getIp().getT()>0.001&&rright.getIp().getT()<0.001)
                //r = rleft;
            //else if(rright.getIp().getT()>0.001&&rleft.getIp().getT()<0.001)
               // r = rright;
            //else return false;
            
            return true;
        }
        else if(leftRes&&!rightRes)
            r = rleft;
        else if(rightRes&&!leftRes)
            r = rright;
        else
            return false;
        return true;
    }

    else if(this->intersect_bbox(r)&&!bbox.isInBbox(r.getOrigin()))
    {
        //if (r.getIp().getT()<0.001)
            //return false;
        //std::cout<<"intersect_bbox"<<std::endl;
        if(left_==NULL&&right_==NULL)
            return true;
        if(left_!=NULL)
            leftRes = left_->intersect(rleft);
        if(right_!=NULL)
            rightRes = right_->intersect(rright);
        
        if(leftRes&&rightRes)
            r = rleft.getIp().getT()>rright.getIp().getT()?rright:rleft;
        else if(leftRes&&!rightRes)
            r = rleft;
        else if(rightRes&&!leftRes)
            r = rright;
        else return false;
        
        return true;
    }
    else
        return false;
}



bool Bvhnode::intersect_BVH(Ray &r, const std::vector< Surface * > &surfaces)
{
    bool leftRes = false;
    bool rightRes = false;
    Ray rleft = r;
    Ray rright = r;
    Ray rr=r;
    if(bbox.isInBbox(r.getOrigin()))
    {
        if(left_!=NULL)
                leftRes = left_->intersect_BVH(rleft, surfaces);
        if(right_!=NULL)
                rightRes = right_->intersect_BVH(rright, surfaces);
        if(leftRes&&rightRes)
        {
            r = rleft.getIp().getT()>rright.getIp().getT()?rright:rleft;
        }
        else if(leftRes&&!rightRes)
            r = rleft;
        else if(rightRes&&!leftRes)
            r = rright;
        else
            return false;
        //if(r.getIp().getT()<0.001)
            //return false;
        return true;
    }
    
    else if(this->intersect_bbox(rr)&&!bbox.isInBbox(r.getOrigin()))
    {
        //std::cout<<"intersect_bbox"<<std::endl;
        if(left_==NULL&&right_==NULL)
        {
            if(surfaces[bbox.getNumSurface()]->intersect(rr))
            {
                //if(rr.getIp().getT()<0.001)
                    //return false;
                //else
                    r = rr;
                return true;
            }
            else
                return false;
        }
        if(left_!=NULL)
            leftRes = left_->intersect_BVH(rleft, surfaces);
        if(right_!=NULL)
            rightRes = right_->intersect_BVH(rright, surfaces);
        
        if(leftRes&&rightRes)
        {
            r = rleft.getIp().getT()>rright.getIp().getT()?rright:rleft;
            //if(r.getIp().getT()<0.001)
                //return false;
        }
        else if(leftRes&&!rightRes)
            r = rleft;
        else if(rightRes&&!leftRes)
            r = rright;
        else return false;
        //if(r.getIp().getT()<0.001)
            //return false;
        return true;
    }
    else
        return false;
}










