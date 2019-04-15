/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <limits>
#include "model.h"
#include "render.h"

VertexBuffer::VertexBuffer(Model& model) {
    for (int i = 0; i < model.nverts(); i++) {
        clip.push_back(embed<4>(model.vert(i)));
        screen.push_back(model.vert(i));  // needed?
        norm.push_back(embed<4>(model.normal(i), 0.f));
        tan.push_back(embed<4>(model.tangent(i), 0.f));
        bitan.push_back(embed<4>(model.bitangent(i), 0.f));
        
        nverts = model.nverts();
    }
}

Fragment::Fragment() {}

////////// PIPELINE
VertexProcessor::VertexProcessor(Matrix& view, Matrix& proj, Matrix& viewport) {
    world2clip = proj * view;
    vp_mat = viewport;
}

void VertexProcessor::process(Matrix model_mat, VertexBuffer& vb) {
    Matrix model2clip = world2clip * model_mat;
    Vec4f screen_homog;
    for (int i = 0; i < vb.nverts; i++) {;
        vb.clip[i] = model2clip * vb.clip[i];
        vb.norm[i] = model2clip.invert_transpose() * vb.norm[i];
        vb.tan[i] = model2clip * vb.tan[i];
        vb.bitan[i] = model2clip * vb.bitan[i];
        
        /* Screen coords are usually calculated in the rasterization stage but 
         I'm doing here since we are using a frame buffer and can have separate
         vectors for clip and screen coords.
         */
        screen_homog = vp_mat * vb.clip[i];
        vb.screen[i] = proj<3>(vb.clip[i] / vb.clip[i][3]);
        
    }
}

Rasterizer::Rasterizer(TGAImage& fb) {
    image_dims = {fb.get_width(), fb.get_height()};
}

bool Rasterizer::rasterize(std::vector<Fragment> frag_vec, Triangle triangle, \
        std::vector<std::vector<float>>& zbuffer) {
    
    // discard the triangle if back-facing
    if (face_cull(triangle)) return false;
    
    // raster scan
    frag_vec.clear();
    
    BoundBox bbox(triangle, image_dims);
    Vec2i P;
    Vec3f bc;
    
    Fragment frag;
    
    for (P.x = bbox.min.x; P.x < bbox.max.x; P.x++) {
        for (P.y = bbox.min.y; P.y < bbox.max.y; P.y++) {
            bool inside = barycentric(bc, triangle.screens, P);
            if (inside) {
                Vec3f frag_pos = bc_interp(bc, triangle.screens);
                frag.pos = proj<2>(frag_pos);
                frag.depth = frag_pos[2];
                if (frag.depth < zbuffer[P.x][P.y]) {
                    frag.norm = bc_interp(bc, triangle.norms);
                    frag.tan = bc_interp(bc, triangle.tans);
                    frag.bitan = bc_interp(bc, triangle.bitans);
                    frag.uv = bc_interp(bc, triangle.uvs);
                    
                    frag_vec.push_back(frag);
                }
            }
        }
    }
    return true;
}

FragmentProcessor::FragmentProcessor() {};

void FragmentProcessor::process(std::vector<Fragment>& frag_vec, Model& model) {
    for (auto it = frag_vec.begin(); it != frag_vec.end(); ++it) {
        Fragment frag = *it;
        
    }
}

////////// OTHER FUNCTIONS
BoundBox::BoundBox(Triangle triangle, Vec2i image_dims) {
    const float flt_min = std::numeric_limits<float>::min();
    const float flt_max = std::numeric_limits<float>::max();
    min = {flt_max, flt_max};
    max = {flt_min, flt_min};

    for (int i = 0; i < 3; ++i) {
        min.x = std::min(triangle.screens[i].x, min.x);
        min.y = std::min(triangle.screens[i].y, min.y);
        max.x = std::max(triangle.screens[i].x, max.x);
        max.y = std::max(triangle.screens[i].y, max.y);
    }

    min.x = std::max(0.f, min.x);
    min.y = std::max(0.f, min.y);
    max.x = std::min(image_dims.x - 1.f, max.x);
    max.y = std::min(image_dims.y - 1.f, max.y);
}

Triangle::Triangle(std::vector<int> idx, VertexBuffer& vb, Model& model) {
    for (int i = 0; i < 3; i++) {
        clips[i] = vb.clip[idx[i]];
        screens[i] = vb.screen[idx[i]];
        norms[i] = proj<3>(vb.norm[idx[i]]);
        tans[i] = proj<3>(vb.tan[idx[i]]);
        bitans[i] = proj<3>(vb.bitan[idx[i]]);
        uvs[i] = model.uv(idx[i]);
    }
}

bool barycentric(Vec3f& bc, Vec3f* pts, Vec2i& P) {
    /* Calculates barycentric coordinates of point P.  Returns true if all 
     * barycentric coordinates are positive(indicates that point is inside 
     * triangle).  Otherwise returns false(point is outside triangle and is 
     * rejected.
     */
    
    // pre-compute values that are used multiple times
    float x0_x2 = pts[0].x - pts[2].x;
    float x2_x1 = pts[2].x - pts[1].x;
    float y0_y2 = pts[0].y - pts[2].y;
    float y1_y2 = pts[1].y - pts[2].y;
    
    float inv_det = 1.f / ((y1_y2 * x0_x2) + (x2_x1 * y0_y2));
    
    /* Since any negative barycentric coordinate rejects the point, we can check
     if the point is positive after each calculation, and skip the remaining
     calculations if it is negative. */
    bc[0] = ((y1_y2 * (P.x - pts[2].x)) + (x2_x1 * (P.y - pts[2].y))) * inv_det;
    if (bc[0] >= 0) {
        bc[1]  = ((-y0_y2 * (P.x - pts[2].x)) + (x0_x2 * (P.y - pts[2].y))) * inv_det ;
        if (bc[1] >= 0) {
            bc[2] = 1.f - bc[1] - bc[0];
            if (bc[2] >= 0) {
                return true;
            }   
        }
    }
    
    return false;
}

bool face_cull(Triangle triangle) {
    Vec3f v0 = proj<3>(triangle.clips[1] - triangle.clips[0]);
    Vec3f v1 = proj<3>(triangle.clips[2] - triangle.clips[0]);
    Vec3f face_normal = cross(v0, v1);
    const Vec3f view_vector(0.f, 0.f, 1.f);
    if (!((face_normal * view_vector) > 0)) {
        return false;
    } else {
        return true;
    }
}

ModelMatrix::ModelMatrix() {
    Vec3f zero_vec(0.f, 0.f, 0.f);
    scale(1.f);
    rotate(zero_vec);
    translate(zero_vec);
}

Matrix ModelMatrix::model2world() {
    return scale_mat * rot_mat * trans_mat;
}

void ModelMatrix::scale(float s) {
    scale(Vec3f(s, s, s));
}

void ModelMatrix::scale(Vec3f s_vec) {
    
}

void ModelMatrix::rotate(Vec3f euler_vec) {
    
}

void ModelMatrix::rotate(Vec3f v, float rot) {
    
}

void ModelMatrix::translate(Vec3f t) {
    
}

Render::Render() {
    
}

void render_model(Model& model, ModelMatrix& mod_mat, Render& render, TGAImage& fb) {
    
}