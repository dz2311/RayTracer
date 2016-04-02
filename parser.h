#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <vector>
#include "light.h"
#include "materials.h"
#include "camera.h"
#include "triangle.h"
#include "bvhnode.h"
//
// Basic parser for the scene files:
//
class Parser {
public:
    virtual void parse(
            const char *file,
            std::vector< Surface * > &surfaceVector,
                       Camera &cam, std::vector< Light * > &lightVector, Ambientlight &alight, std::vector< Surface * > &surfaceVector_bbox);
    
    
    void read_wavefront_file (
                                      const char *file,
                                      std::vector< int > &tris,
                              std::vector< float > &verts);

};

#endif
