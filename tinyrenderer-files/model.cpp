#include <iostream>
#include <fstream>
#include <sstream>
#include "model.h"

Model::Model(const char *filename, Matrix object2world) : verts_(), faces_(), \
        norms_(), uv_(), tans_(), bitans_(), diffusemap_(), normalmap_(), \
        specularmap_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f n;
            for (int i=0;i<3;i++) iss >> n[i];
            norms_.push_back(n);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f uv;
            for (int i=0;i<2;i++) iss >> uv[i];
            uv_.push_back(uv);
        }  else if (!line.compare(0, 2, "f ")) {
            std::vector<Vec3i> f;
            Vec3i tmp;
            iss >> trash;
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    
    //////////////////// Added by TylerW
    
    // Transform all vertices into world space
    for (int i = 0; i < nverts(); i++) {
        Vec4f ws_vert = object2world * embed<4>(verts_[i]);
        verts_[i] = proj<3>(ws_vert / ws_vert[3]);
    }
    
    // Compute tangent/bitangent vectors at each vertex
    std::vector<int> shared_faces[nverts()];
    float face_areas[nfaces()];
    mat<2, 3, float> face_tangents[nfaces()];
    
    for (int i = 0; i < nfaces(); i++) {
        
        std::vector<int> current_face = face(i);
        
        for (int j = 0; j < 3; j++) {
            int vertex = current_face[j];
            shared_faces[vertex].push_back(i);
        }
        
        face_areas[i] = get_face_area(i);
        face_tangents[i] = get_face_tangents(i);
    }
    
    for (int i = 0; i < nverts(); i++) {
        
       std::vector<int> faces = shared_faces[i];
       Vec3f vert_tan;
       Vec3f vert_bitan;
        
       std::vector<int>::iterator itr;
        for (itr = faces.begin(); itr != faces.end(); ++itr) {
            vert_tan = vert_tan + face_tangents[*itr][0] * face_areas[*itr];
            vert_bitan = vert_bitan + face_tangents[*itr][1] * face_areas[*itr];
            
        }
       
       tans_.push_back(vert_tan.normalize());
       bitans_.push_back(vert_bitan.normalize());
    }
    
    //////////////////// end Added section
    
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap_);
    load_texture(filename, "_nm_tangent.tga",      normalmap_);
    load_texture(filename, "_spec.tga",    specularmap_);
}

Model::~Model() {}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i=0; i<(int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::vert(int iface, int nthvert) {
    return verts_[faces_[iface][nthvert][0]];
}

void Model::load_texture(std::string filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

//////////////////// Added by TylerW
// Functions used in tangent/bitangent calculations

float Model::get_face_area(int i_face) {
    Vec3f AB = vert(i_face, 1) - vert(i_face, 0);
    Vec3f AC = vert(i_face, 2) - vert(i_face, 0);
    return (0.5 * cross(AB, AC).norm());
}

mat<2, 3, float> Model::get_face_tangents(int i_face) {
    /* Computes tangent space basis based on method found here:
     * Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary 
     * Mesh”. Terathon Software, 2001. http://terathon.com/code/tangent.html
     */
    
    Vec3f Q1 = vert(i_face, 1) - vert(i_face, 0);
    Vec3f Q2 = vert(i_face, 2) - vert(i_face, 0);
    
    Vec2f s1t1 = uv(i_face, 1) - uv(i_face, 0);
    Vec2f s2t2 = uv(i_face, 2) - uv(i_face, 0);
    
    mat<2, 3, float> Q;
    Q[0] = Q1;
    Q[1] = Q2;
    
    mat<2, 2, float> st;
    st[0] = s1t1;
    st[1] = s2t2;
    
    mat<2, 3, float> TB;
    TB = st.invert() * Q;
    TB[0].normalize();
    TB[1].normalize();
    
    return TB;
}

//////////////////// end Added section

TGAColor Model::diffuse(Vec2f uvf) {
    Vec2i uv(uvf[0]*diffusemap_.get_width(), uvf[1]*diffusemap_.get_height());
    return diffusemap_.get(uv[0], uv[1]);
}

Vec3f Model::normal(Vec2f uvf) {
    Vec2i uv(uvf[0]*normalmap_.get_width(), uvf[1]*normalmap_.get_height());
    TGAColor c = normalmap_.get(uv[0], uv[1]);
    Vec3f res;
    for (int i=0; i<3; i++)
        res[2-i] = (float)c[i]/255.f*2.f - 1.f;
    return res;
}

Vec2f Model::uv(int iface, int nthvert) {
    return uv_[faces_[iface][nthvert][1]];
}

float Model::specular(Vec2f uvf) {
    Vec2i uv(uvf[0]*specularmap_.get_width(), uvf[1]*specularmap_.get_height());
    return specularmap_.get(uv[0], uv[1])[0]/1.f;
}

Vec3f Model::normal(int iface, int nthvert) {
    int idx = faces_[iface][nthvert][2];
    return norms_[idx].normalize();
}

//////////////////// Added by TylerW
// Getter methods for tangent/bitangent

Vec3f Model::tangent(int iface, int nthvert) {
    int idx = faces_[iface][nthvert][2];
    return tans_[idx].normalize();
}

Vec3f Model::bitangent(int iface, int nthvert) {
    int idx = faces_[iface][nthvert][2];
    return bitans_[idx].normalize();
}

//////////////////// end Added section