#pragma once
#include"structure/HalfEdge.h"
#include"glm/glm.hpp"
#include"Earcutting.h"
#include<unordered_map>
#include<vector>
#include<array>
#include"Eigen/eigen"
#include<iostream>
#include<fstream>
namespace triangulation
{
    class TriangleIntices
    {
    public:
        Vertex* vertex3[3];
        TriangleIntices();
        TriangleIntices(Vertex *v1, Vertex *v2, Vertex *v3);
        ~TriangleIntices();

        inline void print()
        {
            std::ofstream data;
            data.open("dataFile.txt", std::ofstream::app);
            data << "{" << vertex3[0]->m_Id << ", " << vertex3[1]->m_Id << ", " << vertex3[2]->m_Id << "}" << std::endl;
        }
    };
    class VPMapping {
    public:
        Vertex* v;
        glm::vec3 p3d = { 0.0f,0.0f,0.0f };
        glm::vec2 p2d = { 0.0f,0.0f};
    };

    bool IsPointInTriangle(Vertex v, Vertex tv1, Vertex tv2, Vertex tv3);
    float CrossProduct(glm::vec3 vecter1, glm::vec3 vector2);
    std::vector<TriangleIntices> MyTrangulation(Face* f,int size);
    glm::vec2  Translate2d(glm::vec3 point3d,Eigen::Matrix4f mat);
    Eigen::Matrix4f TranslationMatrixTo2d(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

}