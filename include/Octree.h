#pragma once

#include <include/Node.h>

class Octree
{
public:
    Octree(PointSet pointSet) : m_PointSet(pointSet)
    {
        this->getCenter();
        this->getSize();

        this->m_RootNode = (std::shared_ptr<OctreeNode>) new OctreeNode(nullptr, this->m_Position, this->m_Scale, m_PointSet, 0);
    }

    ~Octree() {}

    void Render(GLuint shader, glm::mat4 projection, glm::mat4 view)
    {
        m_RootNode->RenderNode(shader, projection, view);
    }

    void addNode(std::shared_ptr<OctreeNode> parent, int key)
    {
        ///TODO
    }

private:

    void getCenter()
    {
        for(int i = 0; i < m_PointSet.size(); ++i)
        {
            m_Position += m_PointSet[i];
        }

        m_Position /= m_PointSet.size();
    }

    void getSize()
    {
        float x = 0, y = 0, z = 0;
        float mx = 0, my = 0, mz = 0;

        for(int i = 0; i < m_PointSet.size(); ++i)
        {
            float tmpx = m_PointSet[i].x;
            if(tmpx > x)
                x = tmpx;
            if(tmpx < mx)
                mx = tmpx;
            float tmpy = m_PointSet[i].y;
            if(tmpy > y)
                y = tmpy;
            if(tmpy < my)
                my = tmpy;
            float tmpz = m_PointSet[i].z;
            if(tmpz > z)
                z = tmpz;
            if(tmpz < mz)
                mz = tmpz;
        }

        float scx = glm::abs(x) + glm::abs(mx);
        float scy = glm::abs(y) + glm::abs(my);
        float scz = glm::abs(z) + glm::abs(mz);

        m_Scale = glm::max(scx, glm::max(scy, scz));
    }

private:
    std::shared_ptr<OctreeNode> m_RootNode;
    PointSet m_PointSet;

    glm::vec3 m_Position;

    float m_Scale;
};
