#pragma once
#include "mesh.h"
#include <vector>
#include <glm/glm.hpp>
#include <cmath>

class MeshFactory {
public:
    // 创建三角形
    static Mesh createTriangle();
    
    // 创建矩形/四边形
    static Mesh createQuad();
    
    // 创建立方体
    static Mesh createCube();
    
    // 创建球体
    static Mesh createSphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
    
    // 创建平面
    static Mesh createPlane(float width = 2.0f, float height = 2.0f, int widthSegments = 1, int heightSegments = 1);
    
    // 创建圆柱体
    static Mesh createCylinder(float radius = 1.0f, float height = 2.0f, int sectorCount = 36);
    
    // 创建圆锥体
    static Mesh createCone(float radius = 1.0f, float height = 2.0f, int sectorCount = 36);

private:
    static Vertex createVertex(const glm::vec3& pos, const glm::vec3& color = glm::vec3(1.0f), 
                              const glm::vec3& normal = glm::vec3(0.0f, 0.0f, 1.0f), 
                              const glm::vec2& texCoord = glm::vec2(0.0f));
}; 