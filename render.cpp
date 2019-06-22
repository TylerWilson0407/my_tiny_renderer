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
        screen.push_back(model.vert(i));
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
    view_matrix = view;
    vp_mat = viewport;
}

void VertexProcessor::process(Matrix& model_mat, VertexBuffer& vb) {
    Matrix model2clip = world2clip * model_mat;
    
    for (int i = 0; i < vb.nverts; i++) {
        vb.clip[i] = model2clip * vb.clip[i];
        
        vb.norm[i] = model2clip.invert_transpose() * vb.norm[i];
        vb.tan[i] = model2clip * vb.tan[i];
        vb.bitan[i] = model2clip * vb.bitan[i];
        
        Vec4f screen_homog = vp_mat * vb.clip[i];
        vb.screen[i] = proj<3>(screen_homog / screen_homog[3]);
    }
}

Rasterizer::Rasterizer(TGAImage& fb) {
    image_dims = {fb.get_width(), fb.get_height()};
}

bool Rasterizer::rasterize(std::vector<Fragment>& frag_vec, Triangle& triangle, \
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
                if (depth_check(frag.depth, frag.pos, zbuffer)) {
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

void FragmentProcessor::process(std::vector<Fragment>& frag_vec, Model& model, Render& render) {
    for (int i = 0; i < frag_vec.size(); i++) {
        Fragment frag = frag_vec[i];
        
        //tangent space normal
        Vec3f norm_ts = model.normal(frag.uv);
        
        Vec3f normal = frag.tan * norm_ts[0] + frag.bitan * norm_ts[1] + frag.norm * norm_ts[2];
        normal.normalize();
        
        //diffuse lighting
        float diff_intensity = std::max(normal * render.light_vec, 0.f);
        
        //specular lighting
        Vec3f refl = normal * 2 * (render.light_vec * normal) - render.light_vec;
        float spec_intensity = std::pow(normal * refl, model.specular(frag.uv));
        
        float total_intensity = render.ambient + render.diffuse * diff_intensity + render.specular * spec_intensity;
        
        TGAColor color = model.diffuse(frag.uv) * total_intensity;
        render.framebuffer.set(frag.pos.x, frag.pos.y, color);
    }
}

////////// OTHER FUNCTIONS
BoundBox::BoundBox(Triangle& triangle, Vec2i image_dims) {
    const float flt_max = std::numeric_limits<float>::max();
    min = {flt_max, flt_max};
    max = {-flt_max, -flt_max};

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

Triangle::Triangle(int i_face, VertexBuffer& vb, Model& model) {
    std::vector<int> face = model.face(i_face);
    for (int j = 0; j < 3; j++) {
        clips[j] = vb.clip[face[j]];
        screens[j] = vb.screen[face[j]];
        norms[j] = proj<3>(vb.norm[face[j]]).normalize();
        tans[j] = proj<3>(vb.tan[face[j]]).normalize();
        bitans[j] = proj<3>(vb.bitan[face[j]]).normalize();
        uvs[j] = model.uv(i_face, j);
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

bool face_cull(Triangle& triangle) {
    Vec3f v0 = proj<3>(triangle.clips[1] - triangle.clips[0]);
    Vec3f v1 = proj<3>(triangle.clips[2] - triangle.clips[0]);
    Vec3f face_normal = cross(v0, v1);
    const Vec3f view_vector(0.f, 0.f, -1.f);
    if (!((face_normal * view_vector) > 0)) {
        return false;
    } else {
        return true;
    }
}

Matrix view_matrix(const Vec3f& from, const Vec3f& to, Vec3f& up) {
    
    
    Vec3f forward = from - to;
    forward.normalize();
    Vec3f right = cross(up.normalize(), forward);
    right.normalize();
    Vec3f up_ortho = cross(forward, right);
    
    Vec3f vec_arr[] = {right, up_ortho, forward};
    
    Matrix view_matrix = Matrix::identity();
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            view_matrix[i][j] = (j!=3) ? vec_arr[i][j]: -(vec_arr[i] * from);
        }
    }
    
    return view_matrix;
}

Matrix projection_matrix(float fovx, float fovy, float n, float f) {
    /* Maps 3D points from eye coordinates(viewing frustum) to normalized device
     coordinates (NDC space). This matrix uses reversed-Z convention, mapping to
     zero on the far clipping plane and positive 1 on the near clipping plane, 
     to take advantage of increased floating-point precision near zero to 
     attempt to cancel out the decreased depth value distribution as Z moves 
     away from the near plane.  See this blog post:
     * 
     https://developer.nvidia.com/content/depth-precision-visualized
     * 
     */
    
    float l = -n * std::tan((fovx / 2) * (M_PI / 180));
    float r = -l;
    float b = -n * std::tan((fovy / 2) * (M_PI / 180));
    float t = -b;
    
    Matrix perspective_matrix;
    
    perspective_matrix[0][0] = (2 * n) / (r -l);
    perspective_matrix[0][1] = 0;
    perspective_matrix[0][2] = (r + l) / (r - l);
    perspective_matrix[0][3] = 0;
    
    perspective_matrix[1][0] = 0;
    perspective_matrix[1][1] = (2 * n) / (t - b);
    perspective_matrix[1][2] = (t + b) / (t - b);
    perspective_matrix[1][3] = 0;
    
    perspective_matrix[2][0] = 0;
    perspective_matrix[2][1] = 0;
    perspective_matrix[2][2] = n / (f - n);
    perspective_matrix[2][3] = (n * f) / (f - n);
    
    perspective_matrix[3][0] = 0;
    perspective_matrix[3][1] = 0;
    perspective_matrix[3][2] = -1;
    perspective_matrix[3][3] = 0;
    
    return perspective_matrix;
}

Matrix viewport_matrix(int l, int r, int b, int t) {
    
    Matrix viewport_matrix = Matrix::identity();
    
    viewport_matrix[0][0] = (r - l) / 2.f;
    viewport_matrix[0][3] = (r + l) / 2.f;
    viewport_matrix[1][1] = (t - b) / 2.f;
    viewport_matrix[1][3] = (t + b) / 2.f;
    
    return viewport_matrix;
}

ModelMatrix::ModelMatrix() {
    _model_mat = Matrix::identity();
}

Matrix ModelMatrix::model_mat() {
    return _model_mat;
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

Render::Render(TGAImage& fbuffer) {
    framebuffer = fbuffer;
    z_buffer.resize(fbuffer.get_width(), std::vector<float>(fbuffer.get_height(), -std::numeric_limits<float>::max()));
    viewport = viewport_matrix(0, fbuffer.get_width(), 0, fbuffer.get_height());
}

void render_model(Model& model, Matrix& mod_mat, Render& render) {
    
    render.light_vec = proj<3>(render.view * embed<4>(render.light_vec, 0.f));
    
    VertexBuffer vb(model);
    
    VertexProcessor vp(render.view, render.proj, render.viewport);
    vp.process(mod_mat, vb);
    
    Rasterizer rast(render.framebuffer);
    FragmentProcessor frag_proc;
    
    for (int i = 0; i < model.nfaces(); i++) {
        Triangle triangle(i, vb, model);
        std::vector<Fragment> frag_vec;
        if (rast.rasterize(frag_vec, triangle, render.z_buffer)) {
            frag_proc.process(frag_vec, model, render);
        }
    }
    
    render.framebuffer.flip_vertically();
}

bool depth_check(float depth, Vec2i pos, std::vector<std::vector<float>>& z_buffer) {
    /* assumes reverse-Z(0 at far plane, 1 at near plane), so checks if depth 
     * is greater than z_buffer value. */
    if (depth > z_buffer[pos.x][pos.y]) {
        z_buffer[pos.x][pos.y] = depth;
        return true;
    } else {
        return false;
    }
}
