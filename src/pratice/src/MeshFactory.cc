#include "MeshFactory.h"
#include <cmath>

const float PI = 3.14159265359f;

Vertex MeshFactory::createVertex(const glm::vec3& pos, const glm::vec3& color, 
                                const glm::vec3& normal, const glm::vec2& texCoord) {
    return Vertex{pos, color, normal, texCoord};
}

Mesh MeshFactory::createTriangle() {
    std::vector<Vertex> vertices = {
        createVertex({0.5f, -0.5f, 0.0f}, {1,0,0}, {0,0,1}, {1,0}),
        createVertex({-0.5f, -0.5f, 0.0f}, {0,1,0}, {0,0,1}, {0,0}),
        createVertex({0.0f, 0.5f, 0.0f}, {0,0,1}, {0,0,1}, {0.5,1})
    };
    std::vector<uint32_t> indices = {0, 1, 2};
    return Mesh(vertices, indices);
}

Mesh MeshFactory::createQuad() {
    std::vector<Vertex> vertices = {
        createVertex({0.5f, 0.5f, 0.0f}, {1,1,1}, {0,0,1}, {1,1}),   // 右上
        createVertex({0.5f, -0.5f, 0.0f}, {1,1,1}, {0,0,1}, {1,0}),  // 右下
        createVertex({-0.5f, -0.5f, 0.0f}, {1,1,1}, {0,0,1}, {0,0}), // 左下
        createVertex({-0.5f, 0.5f, 0.0f}, {1,1,1}, {0,0,1}, {0,1})   // 左上
    };
    std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};
    return Mesh(vertices, indices);
}

Mesh MeshFactory::createCube() {
    std::vector<Vertex> vertices = {
        // 前面
        createVertex({-0.5f, -0.5f,  0.5f}, {1,0,0}, {0,0,1}, {0,0}),
        createVertex({ 0.5f, -0.5f,  0.5f}, {1,0,0}, {0,0,1}, {1,0}),
        createVertex({ 0.5f,  0.5f,  0.5f}, {1,0,0}, {0,0,1}, {1,1}),
        createVertex({-0.5f,  0.5f,  0.5f}, {1,0,0}, {0,0,1}, {0,1}),
        // 后面
        createVertex({-0.5f, -0.5f, -0.5f}, {0,1,0}, {0,0,-1}, {1,0}),
        createVertex({ 0.5f, -0.5f, -0.5f}, {0,1,0}, {0,0,-1}, {0,0}),
        createVertex({ 0.5f,  0.5f, -0.5f}, {0,1,0}, {0,0,-1}, {0,1}),
        createVertex({-0.5f,  0.5f, -0.5f}, {0,1,0}, {0,0,-1}, {1,1}),
        // 左面
        createVertex({-0.5f,  0.5f,  0.5f}, {0,0,1}, {-1,0,0}, {1,0}),
        createVertex({-0.5f,  0.5f, -0.5f}, {0,0,1}, {-1,0,0}, {1,1}),
        createVertex({-0.5f, -0.5f, -0.5f}, {0,0,1}, {-1,0,0}, {0,1}),
        createVertex({-0.5f, -0.5f,  0.5f}, {0,0,1}, {-1,0,0}, {0,0}),
        // 右面
        createVertex({ 0.5f,  0.5f,  0.5f}, {1,1,0}, {1,0,0}, {0,0}),
        createVertex({ 0.5f,  0.5f, -0.5f}, {1,1,0}, {1,0,0}, {0,1}),
        createVertex({ 0.5f, -0.5f, -0.5f}, {1,1,0}, {1,0,0}, {1,1}),
        createVertex({ 0.5f, -0.5f,  0.5f}, {1,1,0}, {1,0,0}, {1,0}),
        // 底面
        createVertex({-0.5f, -0.5f, -0.5f}, {1,0,1}, {0,-1,0}, {0,1}),
        createVertex({ 0.5f, -0.5f, -0.5f}, {1,0,1}, {0,-1,0}, {1,1}),
        createVertex({ 0.5f, -0.5f,  0.5f}, {1,0,1}, {0,-1,0}, {1,0}),
        createVertex({-0.5f, -0.5f,  0.5f}, {1,0,1}, {0,-1,0}, {0,0}),
        // 顶面
        createVertex({-0.5f,  0.5f, -0.5f}, {0,1,1}, {0,1,0}, {0,0}),
        createVertex({ 0.5f,  0.5f, -0.5f}, {0,1,1}, {0,1,0}, {1,0}),
        createVertex({ 0.5f,  0.5f,  0.5f}, {0,1,1}, {0,1,0}, {1,1}),
        createVertex({-0.5f,  0.5f,  0.5f}, {0,1,1}, {0,1,0}, {0,1})
    };
    
    std::vector<uint32_t> indices = {
        0,  1,  2,   0,  2,  3,   // 前面
        4,  5,  6,   4,  6,  7,   // 后面
        8,  9,  10,  8,  10, 11,  // 左面
        12, 13, 14,  12, 14, 15,  // 右面
        16, 17, 18,  16, 18, 19,  // 底面
        20, 21, 22,  20, 22, 23   // 顶面
    };
    
    return Mesh(vertices, indices);
}

Mesh MeshFactory::createSphere(float radius, int sectorCount, int stackCount) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            
            vertices.push_back(createVertex({x, y, z}, {1,1,1}, {nx, ny, nz}, {s, t}));
        }
    }

    int k1, k2;
    for(int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if(i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount-1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return Mesh(vertices, indices);
}

Mesh MeshFactory::createPlane(float width, float height, int widthSegments, int heightSegments) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    float gridX = width / widthSegments;
    float gridY = height / heightSegments;

    for (int iy = 0; iy <= heightSegments; iy++) {
        float y = -halfHeight + iy * gridY;
        for (int ix = 0; ix <= widthSegments; ix++) {
            float x = -halfWidth + ix * gridX;
            vertices.push_back(createVertex(
                {x, y, 0}, 
                {1,1,1}, 
                {0,0,1}, 
                {(float)ix/widthSegments, (float)iy/heightSegments}
            ));
        }
    }

    for (int iy = 0; iy < heightSegments; iy++) {
        for (int ix = 0; ix < widthSegments; ix++) {
            int a = ix + (widthSegments + 1) * iy;
            int b = ix + (widthSegments + 1) * (iy + 1);
            int c = (ix + 1) + (widthSegments + 1) * (iy + 1);
            int d = (ix + 1) + (widthSegments + 1) * iy;

            indices.push_back(a); indices.push_back(b); indices.push_back(d);
            indices.push_back(b); indices.push_back(c); indices.push_back(d);
        }
    }

    return Mesh(vertices, indices);
}

Mesh MeshFactory::createCylinder(float radius, float height, int sectorCount) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float sectorStep = 2 * PI / sectorCount;
    float halfHeight = height * 0.5f;

    // 侧面顶点
    for(int i = 0; i <= sectorCount; ++i) {
        float sectorAngle = i * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);
        float u = (float)i / sectorCount;
        
        // 底部顶点
        vertices.push_back(createVertex({x, -halfHeight, z}, {1,0,0}, {x/radius, 0, z/radius}, {u, 0}));
        // 顶部顶点
        vertices.push_back(createVertex({x, halfHeight, z}, {1,0,0}, {x/radius, 0, z/radius}, {u, 1}));
    }

    // 侧面索引
    for(int i = 0; i < sectorCount; ++i) {
        int k1 = i * 2;
        int k2 = k1 + 2;
        
        // 第一个三角形
        indices.push_back(k1);
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        
        // 第二个三角形
        indices.push_back(k2);
        indices.push_back(k1 + 1);
        indices.push_back(k2 + 1);
    }

    return Mesh(vertices, indices);
}

Mesh MeshFactory::createCone(float radius, float height, int sectorCount) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float sectorStep = 2 * PI / sectorCount;
    float halfHeight = height * 0.5f;

    // 锥顶
    vertices.push_back(createVertex({0, halfHeight, 0}, {1,1,0}, {0,1,0}, {0.5f, 1}));

    // 底面圆心
    vertices.push_back(createVertex({0, -halfHeight, 0}, {0,1,1}, {0,-1,0}, {0.5f, 0}));

    // 底面顶点
    for(int i = 0; i <= sectorCount; ++i) {
        float sectorAngle = i * sectorStep;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);
        float u = (x / radius + 1) * 0.5f;
        float v = (z / radius + 1) * 0.5f;
        
        vertices.push_back(createVertex({x, -halfHeight, z}, {0,0,1}, {0,-1,0}, {u, v}));
    }

    // 侧面索引
    for(int i = 0; i < sectorCount; ++i) {
        indices.push_back(0);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
    }

    // 底面索引
    for(int i = 0; i < sectorCount; ++i) {
        indices.push_back(1);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }

    return Mesh(vertices, indices);
} 