#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <exception>

typedef std::vector<glm::vec3> PointSet;
static int cycles = 0;
static int leafs = 0;

/** Easier identification for the unordered map*/
enum CHILDREN
{
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
struct AABB
{
    float MAX_X;
    float MAX_Y;
    float MAX_Z;

    float MIN_X;
    float MIN_Y;
    float MIN_Z;
};

bool first = true;
GLuint cVAO, cVBO;
static void renderQuad()
{
    if(first)
    {
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

/** Main Octree class, note that it's a recursive class*/
class OctreeNode
{
public:
    /**Constructor for a raw octree*/
    OctreeNode(glm::vec3 pos, float s)
        : m_Position(pos), m_HalfSize(s)
    {
        m_BoundingBox = this->CreateBoundingBox(pos, s);
        BuildTree();
    }

    /** Constructor for an octree given a set of points*/
    OctreeNode(std::shared_ptr<OctreeNode> parent, glm::vec3 pos, float s, PointSet ps, int lvl)
        : m_Parent(parent), m_Position(pos), m_HalfSize(s), m_PointSet(ps), m_Level(lvl)
    {
        m_BoundingBox = this->CreateBoundingBox(pos, s);
        BuildTree();
    }

    /** Constructor for an octree given a set of points, plus an AABB*/
    OctreeNode(std::shared_ptr<OctreeNode> parent, glm::vec3 pos, float s, PointSet ps, AABB aabb, int lvl = 0)
        : m_Parent(parent), m_Position(pos), m_HalfSize(s), m_PointSet(ps), m_BoundingBox(aabb), m_Level(lvl)
    {
        BuildTree();
    }

    /** Destructor*/
    ~OctreeNode()
    {
        ///Destructor
    }

    /** This function will spawn more nodes based on the number of points in the current node*/
    void BuildTree()
    {
        glm::vec3 CP[8];
        glm::vec3 p = m_Position;
        AABB CAABB[8];
        float Q = m_HalfSize / 2.0f;
        std::cout << (m_Parent == nullptr ? "Root Node" : "") << std::endl;
        std::cout << "Number of Points for Node " << m_Level << ": " << m_PointSet.size() << std::endl;
        if(m_PointSet.size() <= 0)
        {
            std::cout << "Empty Point Set" << std::endl;
            return;
        }

        if(m_Level >= m_MaxLevel)
        {
            std::cout << "Reached Max Level" << std::endl;
            this->isLeaf = true;
            leafs++;
            std::cout << "Leafs: " << leafs << std::endl;
            return;
        }

        try
        {
            /** Now we create the new positions for the child nodes*/

            /** Upper part of the tree*/
            CP[UP_TOP_LEFT]         = glm::vec3(p.x - Q, p.y + Q, p.z + Q);
            CP[UP_TOP_RIGHT]        = glm::vec3(p.x - Q, p.y + Q, p.z - Q);
            CP[UP_BOTTOM_RIGHT]     = glm::vec3(p.x + Q, p.y + Q, p.z - Q);
            CP[UP_BOTTOM_LEFT]      = glm::vec3(p.x + Q, p.y + Q, p.z + Q);

            /** Lower part of the tree*/
            CP[DOWN_TOP_LEFT]       = glm::vec3(p.x - Q, p.y - Q, p.z + Q);
            CP[DOWN_TOP_RIGHT]      = glm::vec3(p.x - Q, p.y - Q, p.z - Q);
            CP[DOWN_BOTTOM_RIGHT]   = glm::vec3(p.x + Q, p.y - Q, p.z - Q);
            CP[DOWN_BOTTOM_LEFT]    = glm::vec3(p.x + Q, p.y - Q, p.z + Q);

            for(int i = 0; i < 8; ++i)
            {
                CAABB[i] = CreateBoundingBox(CP[i], Q);

                std::vector<int> indexPassed;
                for(size_t j = 0; j < m_PointSet.size(); ++j)
                {
                    /** Check if point is in current node*/
                    auto CheckPoint = [](AABB aabb, glm::vec3 point) -> bool
                                            {
                                                return  (aabb.MIN_X <= point.x && point.x <= aabb.MAX_X) &&
                                                        (aabb.MIN_Y <= point.y && point.y <= aabb.MAX_Y) &&
                                                        (aabb.MIN_Z <= point.z && point.z <= aabb.MAX_Z);
                                            };

                    if(CheckPoint(CAABB[i], m_PointSet[j]))
                    {
                        indexPassed.push_back(j);
                    }
                }

                std::vector<glm::vec3> PassedPositions;
                if(indexPassed.size() > 0)
                {
                    for(size_t k = 0; k < indexPassed.size(); ++k)
                        PassedPositions.push_back(m_PointSet.at(indexPassed.at(k)));
                }
                if(indexPassed.size() >= 2)
                {
                    m_Childs[i] = (std::shared_ptr<OctreeNode>) new OctreeNode((std::shared_ptr<OctreeNode>)this, CP[i], Q, PassedPositions, m_Level+1);
                    numberOfChilds++;
                    cycles++;
                    std::cout << "Childs: " << cycles << std::endl;
                }
            }
            if(numberOfChilds <= 0)
            {
                isLeaf = true;
                leafs++;
                std::cout << "Leafs: " << leafs << std::endl;
            }
        }

        catch(std::exception& e)
        {
            std::cout << "::::::::::WARNING::::::::::" << std::endl;
            std::cout << "Has Ocurred an Exception!" << std::endl;
            std::cout << e.what() << std::endl;
            std::cout << "Exception at level: " << m_Level << std::endl;
            std::cout << "Position: " << m_Position.x << " " << m_Position.y << " " << m_Position.z << std::endl;
            return;
        }
    }


    /** Defines a bounding box for the current node*/
    AABB CreateBoundingBox(glm::vec3 pos, float s)
    {
        AABB tmpAABB;
        tmpAABB.MAX_X = pos.x + s;
        tmpAABB.MIN_X = pos.x - s;

        tmpAABB.MAX_Y = pos.y + s;
        tmpAABB.MIN_Y = pos.y - s;

        tmpAABB.MAX_Z = pos.z + s;
        tmpAABB.MIN_Z = pos.z - s;

        return tmpAABB;
    }

    bool isSphereinBox(glm::vec3 pos, float s)
    {
        if(glm::length(m_Position - pos) <= s )
            return true;
    }

    /** Recursively render each node and it's children*/
    void RenderNode(GLuint shader, glm::mat4 projection, glm::mat4 view)
    {

        if(isLeaf)
        {
            glm::mat4 model = glm::translate(glm::mat4(), m_Position);
            model = glm::scale(model, glm::vec3((m_HalfSize)));
            glm::mat4 MVP = projection * view * model;
            glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniform1i(glGetUniformLocation(shader, "isQuad"), 1);
            glUniform1f(glGetUniformLocation(shader, "Level"), (float)m_Level);
            renderQuad();
        }

        {
            for(auto c: m_Childs)
            {
                if(c.second == nullptr)
                    continue;

                c.second->RenderNode(shader, projection, view);
            }
        }

    }

private:
    std::unordered_map<int, std::shared_ptr<OctreeNode> > m_Childs;
    std::shared_ptr<OctreeNode> m_Parent;
    glm::vec3 m_Position;
    float m_HalfSize = 0;
    PointSet m_PointSet;
    AABB m_BoundingBox;
    int m_Level;
    int m_MaxLevel = 8;
    int numberOfChilds = 0;

    bool isLeaf = false;
};

#endif // NODE_H_INCLUDED
