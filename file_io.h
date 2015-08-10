#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "SimpleImage.h"

// float lx = 0.1f, lz = -1.0f, ly = -0.15f;
// float x = -0.1f, z = 1.0f, y = 0.15f;
// float angle = 0;

struct Normal{
    float a, b, c;
    Normal() { a = 0.; b = 0.; c = 0.; }
    Normal(float _a, float _b, float _c) { a = _a; b = _b; c = _c; }
};

struct Texture{
    float a, b, c;
    Texture() { a = 0.; b = 0.; c = 0.; }
    Texture(float _a, float _b, float _c) { a = _a; b = _b; c = _c; }
};

struct Vertex {
    float x, y, z;
    Texture t;
    Normal n;
    Vertex() { 
        x = 0. ;  y = 0. ; z = 0. ; 
        t = Texture(); n = Normal(); 
    }
    Vertex(float _x, float _y, float _z, Texture _t, Normal _n) { 
        x = _x; y = _y; z = _z; 
        t = _t; n = _n; 
    }
    Vertex(float _x, float _y, float _z) { 
        x = _x; y = _y; z = _z; 
        t = Texture(); n = Normal(); 
    }
};

struct Triangle_face{
    Vertex a, b, c;
    Triangle_face() { a = Vertex(); b = Vertex(); c = Vertex(); }
    Triangle_face(Vertex _a, Vertex _b, Vertex _c) { a = _a; b = _b; c = _c; }
};

std::vector<Triangle_face> FACES;

std::vector<Triangle_face> read_faces(const std::string& filename) {
    FACES.clear();
    std::ifstream ifs(filename.c_str());

    if (ifs.is_open()) {
        char type;
        std::vector<Vertex> vertices;
        std::vector<Texture>  textures;
        std::vector<Normal>  normals;

        float x0, x1, x2;

        std::string s;

        while (ifs >> type) {

            if(type == '#'){
                getline(ifs, s); 
                continue;

            } else if(type == 'v'){
                getline(ifs, s);
                if(s[0] == 't'){
                    float t0 = 0, t1 = 0, t2 = 0;
                    getline(ifs, s); 
                    std::sscanf ((char*)&(s[2]), "%f %f %f", &t0, &t1, &t2);
                    if(t2 == '\n')
                        t2 = 0;
                    textures.push_back(Texture(t0, t1, t2));
                } else if(s[0] == 'n'){
                    float n0 = 0, n1 = 0, n2 = 0;
                    getline(ifs, s); 
                    std::sscanf ((char*)&(s[2]), "%f %f %f", &n0, &n1, &n2);
                    normals.push_back(Normal(n0, n1, n2));
                } else {
                    std::sscanf ((char*)&(s[1]), "%f %f %f", &x0, &x1, &x2);
                    vertices.push_back(Vertex(x0, x1, x2));
                }

            } else if(type == 'f'){
                int v1 = 0, v2 = 0, v3 = 0, 
                v1t = -1, v2t = -1, v3t = -1,
                v1n = -1, v2n = -1, v3n = -1;

                getline(ifs, s); 
                int args = std::sscanf((char*)&(s[1]), "%d %d %d", &v1, &v2, &v3);
                if(args == 3){
                    FACES.push_back(Triangle_face(vertices[v1 - 1], vertices[v2 - 1], vertices[v3 - 1]));
                    continue;
                }

                std::sscanf ((char*)&(s[1]), " %d/%d/%d %d/%d/%d %d/%d/%d", 
                                &v1, &v1t, &v1n, 
                                &v2, &v2t, &v2n, 
                                &v3, &v3t, &v3n);

                Vertex vert_a   = vertices[v1 - 1];
                if(v1t < textures.size())
                    vert_a.t        = textures[v1t - 1];
                if(v1n < normals.size())
                    vert_a.n        = normals[v1n - 1];

                Vertex vert_b   = vertices[v2 - 1];
                if(v2t < textures.size())
                    vert_b.t        = textures[v2t - 1];
                if(v2n < normals.size())
                    vert_b.n        = normals[v2n - 1];

                Vertex vert_c   = vertices[v3 - 1];
                if(v3t < textures.size())
                    vert_c.t        = textures[v3t - 1];
                if(v3n < normals.size())
                    vert_c.n        = normals[v3n - 1];

                Triangle_face t = Triangle_face(vert_a, vert_b, vert_c);
                FACES.push_back(t);
            }
        }
    }
    ifs.close();
    return FACES;

}

bool load_file(const std::string& filename) {
  // peek at the extension
  std::string::size_type idx;
  idx = filename.rfind('.');
  if (idx != std::string::npos) {
    std::string extension = filename.substr(idx + 1);
    if (extension != "obj") {
        std::cerr << "ERROR: unable to load file " << filename << "  -- unknown extension." << std::endl;
    } else {
        FACES = read_faces(filename);
    }
  }
  return false;
}