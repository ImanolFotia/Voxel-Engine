#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <exception>
#include <algorithm>
#include <include/Model.h>
#include <cstdio>
#include <include/gui.h>
#include <include/TriBoxCollision.h>

typedef std::vector<glm::vec3> PointSet;
static int cycles = 0;
static int leafs = 0;

static glm::vec3 camPos;

/** Easier identification for the unordered map*/
enum CHILDREN {
    UP_TOP_LEFT = 0,
    UP_TOP_RIGHT,
    UP_BOTTOM_RIGHT,
    UP_BOTTOM_LEFT,
    DOWN_TOP_LEFT,
    DOWN_TOP_RIGHT,
    DOWN_BOTTOM_RIGHT,
    DOWN_BOTTOM_LEFT
};

/** Axis Aligned Bounding Box*/
struct AABB {
    float MAX_X;
    float MAX_Y;
    float MAX_Z;

    float MIN_X;
    float MIN_Y;
    float MIN_Z;
};

static bool first = true;
static GLuint cVAO, cVBO;
static void renderCubeWireframe() {
    if(first) {
        float lines[] = {-1.0, -1.0, -1.0, 1.0, -1.0, -1.0,
                         -1.0, -1.0, -1.0, -1.0, -1.0, 1.0,
                         -1.0, -1.0, 1.0, 1.0, -1.0, 1.0,
                         1.0, -1.0, -1.0, 1.0, -1.0, 1.0,
                         -1.0, 1.0, -1.0, 1.0, 1.0, -1.0,
                         -1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
                         -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                         1.0, 1.0, -1.0, 1.0, 1.0, 1.0,
                         -1.0, -1.0, -1.0, -1.0, 1.0, -1.0,
                         1.0, -1.0, -1.0, 1.0, 1.0, -1.0,
                         -1.0, -1.0, 1.0, -1.0, 1.0, 1.0,
                         1.0, -1.0, 1.0, 1.0, 1.0, 1.0
                        };

        glGenVertexArrays(1, &cVAO);
        glGenBuffers(1, &cVBO);
        glBindVertexArray(cVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 72, &lines, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        first = false;
    }

    glBindVertexArray(cVAO);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
}

static bool csfirst = true;

static GLuint csVAO, csVBO;
static void renderCubeSolid() {
    if(csfirst) {
        GLfloat cubeVertices[] = {
            // Positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &csVAO);
        glGenBuffers(1, &csVBO);
        glBindVertexArray(csVAO);
        glBindBuffer(GL_ARRAY_BUFFER, csVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glBindVertexArray(0);
        csfirst = false;
    }
    glBindVertexArray(csVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

static std::shared_ptr<Model> model;
static int numVerts, numIndices;

static std::vector<glm::vec3> verts;
static std::vector<unsigned int> indices;
static GLuint ssEBO;
static GLuint ssVAO, ssVBO;
static bool ssfirst = true;
static void renderSphereSolid() {
    if(ssfirst) {
        model = (std::shared_ptr<Model>) new Model("esfera.eml");
        std::vector<glm::vec3> verts = model->getVertices();
        std::vector<unsigned int> indices = model->getIndices();
        numVerts = verts.size();
        std::cout << numVerts << std::endl;
        numIndices = indices.size();
        std::cout << numIndices << std::endl;
        glGenVertexArrays(1, &ssVAO);
        glGenBuffers(1, &ssVBO);
        glGenBuffers(1, &ssEBO);
        glBindVertexArray(ssVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ssVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numVerts, &verts[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ssEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
        ssfirst = false;
    }
    glBindVertexArray(ssVAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


/** Main Octree class, note that it's a recursive class*/
class OctreeNode {
public:
    /**Constructor for a raw octree*/
    OctreeNode(const glm::vec3 pos,const float s);

    /** Constructor for an octree given a set of points*/
    OctreeNode(const glm::vec3 pos, const float s, const PointSet ps, const int lvl);

    /** Constructor for an octree given a set of points*/
    OctreeNode(const glm::vec3 pos, const float s, const PointSet ps, const PointSet nor, const int lvl);

    /** Constructor for an octree given a set of points, plus an AABB*/
    OctreeNode(const glm::vec3 pos,const float s,const PointSet ps,const AABB aabb, int lvl = 0);

    /** Destructor*/

    ~OctreeNode();

    void Destroy(std::shared_ptr<OctreeNode> node);

    /** This function will spawn more nodes based on the number of points in the current node*/
    void BuildTree();

    /** Defines a bounding box for the current node*/
    AABB CreateBoundingBox(const glm::vec3 pos,const float s);

    bool isSphereinBox(const glm::vec3 pos,const float s);

    /** Recursively render each node and it's children*/
    void RenderNode(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<float> &scales);

    void renderCurrentLEvelsChildren(std::vector<glm::vec3> &positions, std::vector<glm::vec3>&normals, std::vector<float> &scales);

    void RenderCurrentLevel(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<float> &scales);

private:
    std::unordered_map<int, std::shared_ptr<OctreeNode> > m_Childs;

    glm::vec3 m_Position;
    float m_HalfSize = 0;
    PointSet m_PointSet;
    PointSet m_Normals;
    AABB m_BoundingBox;
    int m_Level;
    int m_MaxLevel = 9;
    int numberOfChilds = 0;
    glm::vec3 m_Normal;
    bool isLeaf = false;
};

#endif // NODE_H_INCLUDED
