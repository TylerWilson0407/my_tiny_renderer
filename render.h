/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   render.h
 * Author: tylerw
 *
 * Created on April 13, 2019, 4:12 PM
 */

#ifndef RENDER_H
#define RENDER_H

#include "geometry.h"
#include "model.h"

////////// BUFFERS
class VertexBuffer {
private:
public:
    VertexBuffer(Model& model);
    std::vector<Vec4f> clip;
    std::vector<Vec4f> norm;
    std::vector<Vec4f> tan;
    std::vector<Vec4f> bitan;
    std::vector<Vec3f> screen;
    int nverts;
};

// Needed?
class PrimitiveBuffer {
private:
public:
};

////////// CLASSES/STRUCTS/CONTAINERS

struct Fragment {
    Fragment();
    Vec2i pos;
    float depth;
    Vec3f norm;
    Vec3f tan;
    Vec3f bitan;
    Vec2f uv;
};

class Triangle {
private:
public:
    Triangle(std::vector<int> idx, VertexBuffer& vb, Model& model);
    Vec4f clips[3];
    Vec3f screens[3];
    Vec3f norms[3];
    Vec3f tans[3];
    Vec3f bitans[3];
    Vec2f uvs[3];
};

////////// PIPELINE
class VertexProcessor {
private:
    Matrix world2clip;
    Matrix vp_mat;
public:
    VertexProcessor(Matrix& view, Matrix& proj, Matrix& viewport);
    void process(Matrix model_mat, VertexBuffer& vb);
};

class Rasterizer {
private:
    Vec2i image_dims;
public:
    Rasterizer(TGAImage& framebuffer);
    bool rasterize(std::vector<Fragment> frag_vec, Triangle triangle, std::vector<std::vector<float>>& zbuffer);
};

class FragmentProcessor {
private:
public:
    FragmentProcessor();
    void process(std::vector<Fragment>& frag_vec, Model& model);
};

////////// UTILITY FUNCTIONS/CLASSES
class BoundBox {
private:
public:
    BoundBox(Triangle triangle, Vec2i image_dims);
    Vec2f min;
    Vec2f max;
};

Vec3f bc_coords(const Vec3f* pts, const Vec2f& P);
bool barycentric(Vec3f& bc, Vec3f* pts, Vec2i& P);
bool face_cull(Triangle triangle);

template <class T>
T bc_interp(Vec3f bc, T* vert_vals) {
    
    T result;
    
    for (int i = 0; i < 3; i++) {
        result = result + vert_vals[i] * bc[i];
    }
    
    return result;
}

class ModelMatrix {
private:
    Matrix scale_mat;
    Matrix rot_mat;
    Matrix trans_mat;
    void update();
public:
    ModelMatrix();
    Matrix model2world();
    void scale(float s);
    void scale(Vec3f s_vec);
    void rotate(Vec3f euler_vec);
    void rotate(Vec3f v, float rot);
    void translate(Vec3f t);
};

class Scene {
private:
public:
    Scene();
};

void render_model(Model& model, ModelMatrix& mod_mat, Scene& scene, TGAImage& fb);

#endif /* RENDER_H */

