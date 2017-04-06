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

            for(auto i = 0; i < 8; ++i)
            {
                CAABB[i] = CreateBoundingBox(CP[i], Q);

                std::vector<int> indexPassed;
                for(int j = 0; j < m_PointSet.size(); j++)
                {
                    bool passed = false;
                    passed = isPointInBox(m_PointSet[j], CAABB[i]);
                    if(passed)
                    {
                        indexPassed.push_back(j);
                    }
                }
                //std::cout << "Index Passed: " << indexPassed.size() << " For local leaf " << i << std::endl;

                std::vector<glm::vec3> PassedPositions;
                if(indexPassed.size() > 0)
                {
                    for(int k = 0; k < indexPassed.size(); ++k)
                        PassedPositions.push_back(m_PointSet.at(indexPassed.at(k)));
                }
                if(indexPassed.size() > 3)
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

    /** Point/Box collision detection*/
    bool isPointInBox(glm::vec3 point, AABB aabb)
    {
        return (aabb.MIN_X <= point.x && point.x <= aabb.MAX_X) &&
               (aabb.MIN_Y <= point.y && point.y <= aabb.MAX_Y) &&
               (aabb.MIN_Z <= point.z && point.z <= aabb.MAX_Z);
    }

private:
    std::unordered_map<int, std::shared_ptr<OctreeNode> > m_Childs;
    std::shared_ptr<OctreeNode> m_Parent;
    PointSet m_PointSet;
    int m_Level;
    int m_MaxLevel = 5;
    int numberOfChilds = 0;

    glm::vec3 m_Position;
    AABB m_BoundingBox;
    float m_HalfSize;
    bool isLeaf = false;
};

#endif // NODE_H_INCLUDED
