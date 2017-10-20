#include <include/Node.h>

/**Constructor for a raw octree*/
OctreeNode::OctreeNode(const glm::vec3 pos,const float s)
    : m_Position(pos), m_HalfSize(s) {

    m_BoundingBox = this->CreateBoundingBox(pos, s);
    BuildTree();
}

/** Constructor for an octree given a set of points*/
OctreeNode::OctreeNode(const glm::vec3 pos, const float s, const PointSet ps, const int lvl)
    : m_Position(pos), m_HalfSize(s), m_PointSet(ps), m_Level(lvl) {

    m_BoundingBox = this->CreateBoundingBox(pos, s);
    BuildTree();
}

/** Constructor for an octree given a set of points*/
OctreeNode::OctreeNode(const glm::vec3 pos, const float s, const PointSet ps, const PointSet nor, const int lvl)
    : m_Position(pos), m_HalfSize(s), m_PointSet(ps), m_Normals(nor), m_Level(lvl) {

    m_BoundingBox = this->CreateBoundingBox(pos, s);
    BuildTree();
}

/** Constructor for an octree given a set of points, plus an AABB*/
OctreeNode::OctreeNode(const glm::vec3 pos,const float s,const PointSet ps,const AABB aabb, int lvl)
    : m_Position(pos), m_HalfSize(s), m_PointSet(ps), m_BoundingBox(aabb), m_Level(lvl) {

    BuildTree();
}

/** Destructor*/

OctreeNode::~OctreeNode() {
    ///Destructor
}

void OctreeNode::Destroy(std::shared_ptr<OctreeNode> node) {

    for(std::unordered_map<int, std::shared_ptr<OctreeNode> >::iterator itr = node->m_Childs.begin(); itr != node->m_Childs.end(); itr++) {
        node->Destroy(itr->second);
    }

    node.reset();
    OctreeNode* tmpNode = node.get();
    tmpNode = nullptr;
}

/** This function will spawn more nodes based on the number of points in the current node*/
void OctreeNode::BuildTree() {
    glm::vec3 CP[8];
    glm::vec3 p = m_Position;
    AABB CAABB[8];
    float Q = m_HalfSize/2.0;
    //std::cout << (m_Level == 0 ? "Root Node" : "") << std::endl;
    if(m_PointSet.size() <= 0) {
        //std::cout << "Empty Point Set" << std::endl;
        return;
    }

    try {
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

        for(int i = 0; i < 8; ++i) {
            CAABB[i] = CreateBoundingBox(CP[i], Q);

            int indexPassed = 0;
            std::vector<glm::vec3> PassedNormals;
            std::vector<glm::vec3> PassedPositions;

            //#define pointCheck

#ifdef pointCheck
            for(size_t j = 0; j < m_PointSet.size(); ++j) {

                /** Check if point is in current node*/
                auto CheckPointinBox = [](AABB aabb, glm::vec3 point) -> bool {

                    return  (aabb.MIN_X <= point.x && point.x <= aabb.MAX_X) &&
                    (aabb.MIN_Y <= point.y && point.y <= aabb.MAX_Y) &&
                    (aabb.MIN_Z <= point.z && point.z <= aabb.MAX_Z);

                };

                auto CheckTriangleinBox = [&]() {};

                if(CheckPointinBox(CAABB[i], m_PointSet[j])) {
                    indexPassed++;
                    PassedPositions.push_back(m_PointSet[j]);
                    PassedNormals.push_back(m_Normals[j]);
                }

            }
#endif // point
#ifndef pointCheck
            glm::vec3 tri[3];
            for(size_t j = 0; j < m_PointSet.size(); j+=3) {
                tri[0] = m_PointSet.at(j);
                tri[1] = m_PointSet.at(j+1);
                tri[2] = m_PointSet.at(j+2);
                if(triBoxOverlap(m_Position, glm::vec3(m_HalfSize), tri)) {
                    indexPassed++;
                    PassedPositions.push_back(tri[0]);
                    PassedPositions.push_back(tri[1]);
                    PassedPositions.push_back(tri[2]);
                    PassedNormals.push_back(m_Normals.at(j));
                    PassedNormals.push_back(m_Normals.at(j+1));
                    PassedNormals.push_back(m_Normals.at(j+2));
                }
            }
#endif // point
            //std::cout << indexPassed << std::endl;
            auto greater_or_equal_to_1 = [](const int x) -> bool {return x >= 1;};

            if(greater_or_equal_to_1(indexPassed) && m_Level+1 < m_MaxLevel) {

                m_Childs[i] = (std::shared_ptr<OctreeNode>) new OctreeNode(CP[i], Q, PassedPositions, PassedNormals, m_Level+1);
                numberOfChilds++;
                cycles++;
                printf("Childs: %d\r", cycles);
                //std::flush(std::cout);
            }
        }


        if(numberOfChilds <= 0 && !this->isLeaf) {
            isLeaf = true;
            leafs++;
            // std::cout << "Leafs: " << leafs << "\n" << "\r";
        }

        for(auto i: m_Normals) {
            m_Normal += i;
        }

        m_Normal /= m_Normals.size();
        m_Normal = glm::normalize(m_Normal);
    }

    catch(std::exception& e) {
        std::cout << "::::::::::WARNING::::::::::" << std::endl;
        std::cout << "Has Ocurred an Exception!" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "Exception at level: " << m_Level << std::endl;
        std::cout << "Position: " << m_Position.x << " " << m_Position.y << " " << m_Position.z << std::endl;
        return;
    }

}

/** Defines a bounding box for the current node*/
AABB OctreeNode::CreateBoundingBox(const glm::vec3 pos,const float s) {

    AABB tmpAABB;

    tmpAABB.MAX_X = pos.x + s;
    tmpAABB.MIN_X = pos.x - s;

    tmpAABB.MAX_Y = pos.y + s;
    tmpAABB.MIN_Y = pos.y - s;

    tmpAABB.MAX_Z = pos.z + s;
    tmpAABB.MIN_Z = pos.z - s;

    return tmpAABB;
}

bool OctreeNode::isSphereinBox(const glm::vec3 pos,const float s) {
    if(glm::length(m_Position - pos) <= s ) {
        return true;
    }
}

/** Recursively render each node and it's children*/
void OctreeNode::RenderNode(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<float> &scales) {
    int guilevel = gui::Level;

    if((this->m_Level == m_MaxLevel-1)) {

        RenderCurrentLevel(positions, normals, scales);
        }else{

        renderCurrentLEvelsChildren(positions, normals, scales);
        }


/*
        if(this->isLeaf)
            RenderCurrentLevel(positions, normals, scales);
        else
            renderCurrentLEvelsChildren(positions, normals, scales);*/
    //}
}

void OctreeNode::renderCurrentLEvelsChildren(std::vector<glm::vec3> &positions, std::vector<glm::vec3>&normals, std::vector<float>& scales) {

    for(auto c: m_Childs) {
        if(c.second == nullptr) {
            continue;
        }
        c.second->RenderNode(positions, normals, scales);
    }

}

void OctreeNode::RenderCurrentLevel(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals, std::vector<float> &scales) {

    positions.push_back(m_Position);
    normals.push_back(glm::normalize(m_Normal));
    scales.push_back(m_HalfSize);
}
