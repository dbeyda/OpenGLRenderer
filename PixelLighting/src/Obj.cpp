#define TINYOBJLOADER_IMPLEMENTATION
#include "Obj.h";
#include "glm/glm.hpp"
#include "Renderer.h"


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

    int stride = 11;
    int totalPosSize = (attrib.vertices.size() / 3) * stride;
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
                // positions
                positions[stride * vi + 0] = attrib.vertices[3 * idx.vertex_index + 0];
                positions[stride * vi + 1] = attrib.vertices[3 * idx.vertex_index + 1];
                positions[stride * vi + 2] = attrib.vertices[3 * idx.vertex_index + 2];
                // normal vector
                positions[stride * vi + 3] = attrib.normals[3 * idx.normal_index + 0];
                positions[stride * vi + 4] = attrib.normals[3 * idx.normal_index + 1];
                positions[stride * vi + 5] = attrib.normals[3 * idx.normal_index + 2];
                // texture coords
                positions[stride * vi + 6] = attrib.texcoords[2 * idx.texcoord_index + 0];
                positions[stride * vi + 7] = attrib.texcoords[2 * idx.texcoord_index + 1];
            }

            // here, the last 'fv' vertices on indices[] are the vertices of this face
            // as we are importing faces with triangulation, we expect every face to have
            // exactly 3 vertices
            ASSERT(fv == 3);
            int zero_idx = indices[indices.size() - 3];
            int one_idx = indices[indices.size() - 2];
            int two_idx = indices[indices.size() - 1];

            // Shortcuts for vertices
            glm::vec3 v0 = glm::vec3(positions[stride * zero_idx], positions[stride * zero_idx + 1], positions[stride * zero_idx + 2]);
            glm::vec3 v1 = glm::vec3(positions[stride * one_idx], positions[stride * one_idx + 1], positions[stride * one_idx + 2]);
            glm::vec3 v2 = glm::vec3(positions[stride * two_idx], positions[stride * two_idx + 1], positions[stride * two_idx + 2]);
            
            // Shortcuts for UVs
            glm::vec2 uv0 = glm::vec2(positions[stride * zero_idx+6], positions[stride * zero_idx + 7]);
            glm::vec2 uv1 = glm::vec2(positions[stride* one_idx+6], positions[stride* one_idx + 7]);
            glm::vec2 uv2 = glm::vec2(positions[stride * two_idx +6], positions[stride * two_idx + 7]);

            // Edges of the triangle : position delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            tangent = normalize(tangent);
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                unsigned int vi = (unsigned int)idx.vertex_index;

                // sum in tangent vector, that will be normalized afterwards
                positions[stride * vi + 8]  += tangent.x;
                positions[stride * vi + 9]  += tangent.y;
                positions[stride * vi + 10] +=  tangent.z;

                FixNegativeZeros(positions, stride * vi + 8, 3);
            }

            index_offset += fv;
            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
    NormalizeTangents(stride, 8);
}

void Obj::FixNegativeZeros(std::vector<float>& vec, int s, int n)
{
    for (int i=s; i < s+n; i++)
        if (abs(vec[i] - 0.0) < 0.0001)
            vec[i] = 0.0f;
}

void Obj::NormalizeTangents(int stride, int firstPos)
{
    for (int i = 0; i < positions.size(); i+=stride)
    {
        glm::vec3 tangent = glm::vec3(
            positions[i + firstPos],
            positions[i + firstPos + 1],
            positions[i + firstPos + 2]
            );
        tangent = glm::normalize(tangent);
        positions[i + firstPos]     = tangent.x;
        positions[i + firstPos + 1] = tangent.y;
        positions[i + firstPos + 2] = tangent.z;
    std::cout << "new tangent for vertex " << i / stride << ": " << tangent.x
        << ", " << tangent.y
        << ", " << tangent.z << std::endl;

    }
}

/*
template <typename T>
glm::vec3<T>* toGlmVec(T arr)
{
    return reinterpret_cast<vec3<T>*>(arr);
}
*/

