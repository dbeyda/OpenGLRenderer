#define TINYOBJLOADER_IMPLEMENTATION
#include "Obj.h"


Obj::Obj(const std::string& path)
{
    filepath = path;
}

Obj::~Obj()
{}

void Obj::Load()
{
    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str());

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    int totalPosSize = (attrib.vertices.size() / 3) * 8;
    positions = std::vector<float>(totalPosSize, 0);

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                unsigned int vi = (unsigned int)idx.vertex_index;
                indices.push_back(vi);
                positions[8 * vi + 0] = attrib.vertices[3 * idx.vertex_index + 0];
                positions[8 * vi + 1] = attrib.vertices[3 * idx.vertex_index + 1];
                positions[8 * vi + 2] = attrib.vertices[3 * idx.vertex_index + 2];
                positions[8 * vi + 3] = attrib.normals[3 * idx.normal_index + 0];
                positions[8 * vi + 4] = attrib.normals[3 * idx.normal_index + 1];
                positions[8 * vi + 5] = attrib.normals[3 * idx.normal_index + 2];
                positions[8 * vi + 6] = attrib.texcoords[2 * idx.texcoord_index + 0];
                positions[8 * vi + 7] = attrib.texcoords[2 * idx.texcoord_index + 1];
            }
            index_offset += fv;
            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
}


