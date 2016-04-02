
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include "parser.h"
#include "basicmath.h"
#include "surface.h"
#include "sphere.h"
#include "plane.h"



// In this code, you can see how Sphere and Camera are set up - you
// will need to do similar for the other classes.
//
// You will also need to keep track of the last material defined, so
// that you can connect it to any surfaces that are subsequently defined.
//
void Parser::parse(
    const char *file,
    std::vector< Surface * > &surfaceVector,
                   Camera &cam, std::vector< Light * > &lightVector, Ambientlight &alight, std::vector< Surface * > &surfaceVector_bbox)
{
    Materials *mat=NULL;
    ifstream in(file);
    char buffer[1025];
    string cmd;
    int num_cams = 0; // keep track of how many cameras we read in
    
    for (int line=1; in.good(); line++) {
        in.getline(buffer,1024);
        buffer[in.gcount()]=0;
        
        Surface *thisSurface = 0;
        Surface *bboxSurface = 0;
        Light *thisLight = 0;
        
        cmd="";
        
        istringstream iss(buffer);
        
        iss >> cmd;
        
        if (cmd[0]=='/' or cmd.empty() or cmd==" ") {
            // ignore comments or blank lines
            continue;
        } 
        else if (cmd=="s") {
            // Sphere:
            
            // read in the parameters:
            mypoint pos; 
            double r;
            iss >> pos >> r;
            
            // construct it:
            // at the end of this loop, we put any newly-constructed surfaces
            // into the surfaceVector.
            thisSurface = new Sphere(pos+mypoint(0.0,0.001,0.0), r);
            thisSurface->setMaterials(*mat);
            
            Boundingbox bounding = thisSurface->getBoundingBbox();
            bboxSurface = new Bvhnode(bounding, (int)surfaceVector.size());
            bboxSurface->setMaterials(*mat);
            //bboxSurface->getBoundingBbox().setNumSurface((int)surfaceVector.size());
            //std::cout<<"sphere surface num"<<bboxSurface->getBoundingBbox().getNumSurface()<<endl;
            //cout<<"parse.cc  sphere get material diff:x  "<<thisSurface->getMaterials().getDiffuse()[0]<< " y "<< thisSurface->getMaterials().getDiffuse()[1]<<" z "<< thisSurface->getMaterials().getDiffuse()[2]<<endl;
            //cout<<"parse.cc sphere phong"<<thisSurface->getMaterials().getPhong()<<endl;
        } 
        else if (cmd=="t") {
            // Triangle
            
            // read in the parameters:
            mypoint pa,pb,pc;
            iss >> pa >> pb >> pc;
            thisSurface = new Triangle(pa,pb,pc);
            thisSurface->setMaterials(*mat);
            Boundingbox bounding = thisSurface->getBoundingBbox();
            bboxSurface = new Bvhnode(bounding, (int)surfaceVector.size());
            //std::cout<<"triangle surface num"<<bboxSurface->getBoundingBbox().getNumSurface()<<endl;
            bboxSurface->setMaterials(*mat);
            
            // construct it:
//            triangle(a,b,c);
        } 
        else if (cmd=="p") {
            // Plane
            
            // read in the parameters:
            myvector n; 
            double d;
            iss >> n >> d;
            thisSurface = new Plane(n, d);
            thisSurface->setMaterials(*mat);
        } 
        else if (cmd=="c") {
            // Camera:
            ++num_cams; // keep track of how many we read in
 
            
            // read in the parameters:
            mypoint pos; myvector dir; 
            double d,iw,ih; 
            int pw,ph;
            iss >> pos >> dir >> d >> iw >> ih >> pw >> ph;
            
            // construct it:
            cam.init (pos,dir,d,iw,ih,pw,ph);
            //cout<<" camera: eye: " <<cam.getEye()[0]<<" "<<cam.getEye()[1]<<" "<< cam.getEye()[2]<<endl;
        } 
        else if (cmd=="l") {
            //cout<<"start read light"<<endl;
            iss >> cmd;
            if (cmd=="p") {
                //cout<<"start read point light"<<endl;
                mypoint pos;
                myvector rgb;
                iss >> pos >> rgb;
                //cout<<"start write light"<<endl;
                //construct it:
                //plight.set_pllight(pos,rgb);
                thisLight = new Pointlight(pos, rgb);
//                pointLight(pos,rgb);
                //cout<<"light origin: x "<<thisLight->getOrigin()[0]<<"y "<<thisLight->getOrigin()[1]<<"z "<<thisLight->getOrigin()[2]<<endl;
            } 
            else if (cmd=="d") {
                myvector dir,rgb;
                iss >> dir >> rgb;
//                directionalLight(dir,rgb);
            } 
            else if (cmd=="a") {
                myvector rgb;
                iss >> rgb;
                alight = Ambientlight(rgb);
//                ambientLight(rgb);
            }
            else if(cmd=="s") {
                mypoint center;
                myvector dir, udir, rgb;
                double length;
                iss >> center >> dir >> udir >> length >> rgb;
                thisLight = new Arealight(center, dir, udir, length, rgb);
            }
            else {
                cout << "Parser error: invalid light at line " << line << endl;
            }
        } 
        else if (cmd=="m") {
            // Material:
            
            // read in the parameters:
                myvector diff,spec,refl;
                double r;
                iss >> diff >> spec >> r >> refl;
            
            // construct it:
            mat = new Materials(diff,spec,r,refl);
            
            cout<<"phong"<<mat->getPhong()<<endl;
            
        }
        else if (cmd=="w") {
            // alias-wavefront file of triangles
            string aw_file;
            iss >> aw_file;
            std::vector<int> tri_ids;
            std::vector<float> tri_verts;
            read_wavefront_file(aw_file.c_str(), tri_ids, tri_verts);
            
            // build all the surfaces:
            for (int k = 0; k < (int)tri_ids.size() / 3; ++k) {
                mypoint pa(tri_verts[3*tri_ids[3*k]],
                           tri_verts[3*tri_ids[3*k]+1],
                           tri_verts[3*tri_ids[3*k]+2]);
                
                mypoint pb(tri_verts[3*tri_ids[3*k+1]],
                           tri_verts[3*tri_ids[3*k+1]+1],
                           tri_verts[3*tri_ids[3*k+1]+2]);
                
                mypoint pc(tri_verts[3*tri_ids[3*k+2]],
                           tri_verts[3*tri_ids[3*k+2]+1],
                           tri_verts[3*tri_ids[3*k+2]+2]);
                
                thisSurface = new Triangle(pa,pb,pc);
                thisSurface->setMaterials(*mat);
                Boundingbox bounding = thisSurface->getBoundingBbox();
                bboxSurface = new Bvhnode(bounding, (int)surfaceVector.size());
                bboxSurface->setMaterials(*mat);
               // std::cout<<"surface num"<<bboxSurface->getBoundingBbox().getNumSurface()<<endl;
                surfaceVector.push_back (thisSurface);
                surfaceVector_bbox.push_back(bboxSurface);
            }
        }
        else {
            std::cerr << "Parser error: invalid command at line " << line << endl;
        }
        
        // if we found a surface, add it to the list of surfaces:
        if (thisSurface && cmd!="w") {
            surfaceVector.push_back (thisSurface);
            surfaceVector_bbox.push_back(bboxSurface);
        }
        if(thisLight)
            lightVector.push_back (thisLight);
    }
    
    in.close();
    
    // make sure we read in 1 camera, no more no less 8).
    if (num_cams != 1) {
        std::cerr << "scene file error: exactly ONE camera must be defined." << endl;
    }
    delete mat;
}

void Parser::read_wavefront_file (
                                  const char *file,
                                  std::vector< int > &tris,
                                  std::vector< float > &verts)
{
    
    // clear out the tris and verts vectors:
    tris.clear ();
    verts.clear ();
    
    ifstream in(file);
    char buffer[1025];
    string cmd;
    
    
    for (int line=1; in.good(); line++) {
        in.getline(buffer,1024);
        buffer[in.gcount()]=0;
        
        cmd="";
        
        istringstream iss (buffer);
        
        iss >> cmd;
        
        if (cmd[0]=='#' or cmd.empty()) {
            // ignore comments or blank lines
            continue;
        }
        else if (cmd=="v") {
            // got a vertex:
            
            // read in the parameters:
            double pa, pb, pc;
            iss >> pa >> pb >> pc;
            
            verts.push_back (pa);
            verts.push_back (pb);
            verts.push_back (pc);
        }
        else if (cmd=="f") {
            // got a face (triangle)
            
            // read in the parameters:
            int i, j, k;
            iss >> i >> j >> k;
            
            // vertex numbers in OBJ files start with 1, but in C++ array
            // indices start with 0, so we're shifting everything down by
            // 1
            tris.push_back (i-1);
            tris.push_back (j-1);
            tris.push_back (k-1);
        }
        else {
            std::cerr << "Parser error: invalid command at line " << line << std::endl;
        }
        
    }
    
    in.close();
    
    std::cout << "found this many tris, verts: " << tris.size () / 3.0 << "  "
    << verts.size () / 3.0 << std::endl;
}


