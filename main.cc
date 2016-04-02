//
//  main.cc
//  raytra_1
//
//

#include <iostream>
#include <vector>
#include <ctime>
#include "surface.h"
#include "camera.h"
#include "parser.h"
#include "light.h"
#include "materials.h"
#include "bbox.h"
#include "bvhnode.h"

#define PBRT_HAS_OPENEXR
/*void deleteTree(Surface *root){
    if(root){
        deleteTree(dynamic_cast<Bvhnode*>(root->left_));
        deleteTree(dynamic_cast<Bvhnode*>(root->right_));
        delete root;
    }
}*/
int main (int argc, const char * argv[])
{
    
    time_t tstart, tend;
    tstart = clock();
    /*if (argc != 3) {
        // error condition: 
        std::cout << "usage: raytra scenefile outputimage" << std::endl;
    }*/
    std::vector< Surface * > surfaces;
    
    std::vector< Surface * > surfaces_bbox;
    Camera cam;
    
    Parser parser;
    Ambientlight alight;
    std::vector< Light * > lights;
    //Pointlight plight;
    
    parser.parse (argv[1], surfaces, cam, lights, alight, surfaces_bbox);
    srand((unsigned)time(NULL));
    assert (surfaces.size () != 0); // make sure there are some surfaces
    std::cout<<"Tree Created"<<std::endl;
    Surface *head = new Bvhnode();
    head->createBVHTree(surfaces_bbox, 0, head);
    std::cout<<"ready to render"<<std::endl;
    if(argc ==3||(argc==4&&*argv[3]-'0'==3))
    {
        std::cout<<"input argument is 3"<<endl;
        cam.renderScene_BVHTree(surfaces, lights, alight, head);
        cam.writeImage (argv[2]);
    }
    if(argc == 4)
    {
        int renderWay = *argv[3]-'0';
        //std::cout<<"input argument is 3"<<endl;
        switch (renderWay) {
            case 0:
            {
                cam.renderScene (surfaces, lights, alight);
                cam.writeImage (argv[2]);
                break;
            }
            case 1:
            {
                cam.renderScene_bbox(surfaces, lights, alight);
                cam.writeImage (argv[2]);
                break;
            }
            case 2:
            {
                cam.renderScene_BVH(surfaces, lights, alight, head);
                cam.writeImage (argv[2]);
                break;
            }
            case 3:
            {
                break;
            }
            default:
                break;
        }
    }
    if(argc==5)
    {
        int num_primaryRay = *argv[3]-'0';
        int num_shadowRay = *argv[4]-'0';
        cam.renderScene_BVHTree_Sampling(surfaces, lights, alight, head, num_primaryRay, num_shadowRay);
        cam.writeImage (argv[2]);
    }
    for (Surface * s : surfaces)
        delete s;
    
    for(Surface * bs : surfaces_bbox)
        delete bs;
    
    for (Light * l : lights)
        delete l;
    
    //deleteTree(head);
    delete head;
    tend = clock();
    cout<<"The total running time is:"<<(tend-tstart)/CLOCKS_PER_SEC<<" (seconds)  "<<endl;
}
