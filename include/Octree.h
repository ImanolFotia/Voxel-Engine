#pragma once

#include <include/Node.h>
#include <include/PointCloud.h>
class Octree {
public:
    Octree(PointSet pointSet) : m_PointSet(pointSet) {
        this->getCenter();
        this->getSize();

        this->m_RootNode = (std::shared_ptr<OctreeNode>) new OctreeNode(this->m_Position, this->m_Scale, m_PointSet, 0);
    }

    Octree(PointSet pointSet, PointSet normals) : m_PointSet(pointSet), m_Normals(normals) {
        this->getCenter();
        this->getSize();

        this->m_RootNode = (std::shared_ptr<OctreeNode>) new OctreeNode(this->m_Position, this->m_Scale, m_PointSet, m_Normals, 0);
    }

    ~Octree() {
        OctreeNode* tmp = m_RootNode.get();}

    void Render(std::vector<glm::vec3>& positions, std::vector<glm::vec3> &normals, std::vector<float>& scales) {
        m_RootNode->RenderNode(positions, normals, scales);
    }

    void addNode(std::shared_ptr<OctreeNode> parent, int key) {
        ///TODO
    }

    void Destroy()
    {
        m_RootNode->Destroy(m_RootNode);
    }
private:

    void getCenter() {
        for(size_t i = 0; i < m_PointSet.size(); ++i) {
            m_Position += m_PointSet[i];
        }

        m_Position /= m_PointSet.size();
    }

    void getSize() {
        float x = 0, y = 0, z = 0;
        float mx = 0, my = 0, mz = 0;
        {
        for(size_t i = 0; i < m_PointSet.size(); ++i) {
            float tmpx = m_PointSet[i].x;
            if( tmpx > x ) {
                x = tmpx;
            }
            if( tmpx < mx ) {
                mx = tmpx;
            }

            float tmpy = m_PointSet[i].y;
            if( tmpy > y ) {
                y = tmpy;
            }
            if( tmpy < my ) {
                my = tmpy;
            }

            float tmpz = m_PointSet[i].z;
            if( tmpz > z ) {
                z = tmpz;
            }
            if( tmpz < mz ) {
                mz = tmpz;
            }
        }
        }

        float scx = glm::length(x - mx);
        float scy = glm::length(y - my);
        float scz = glm::length(z - mz);

        m_Scale = glm::max(scx, glm::max(scy, scz))/1.5;
    }

private:
    std::shared_ptr<OctreeNode> m_RootNode;
    PointSet m_PointSet;
    PointSet m_Normals;

    glm::vec3 m_Position;

    float m_Scale;
};
