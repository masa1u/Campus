#ifndef CAMPUS_VERSION_H
#define CAMPUS_VERSION_H

#include "entity.h"
#include <vector>
#include <algorithm>

class Node;

class Version {
public:
    Version(int version, Node *node, Version *prev_version, int max_num, int dimension, size_t element_size)
        : version_(version), node_(node), prev_version_(prev_version), max_num_(max_num), vector_num_(0),
            dimension_(dimension), element_size_(element_size) {
        posting_ = new Entity*[max_num_];
        centroid = new char[dimension_ * element_size_];
    }

    ~Version() {
        for (int i = 0; i < vector_num_; ++i) {
            delete posting_[i];
        }
        delete[] posting_;
        delete[] static_cast<char*>(centroid);
    }

    int getVersion() const { return version_; }
    Node *getNode() const { return node_; }
    int getVectorNum() const { return vector_num_; }
    const void* getCentroid() const { return centroid; }
    std::vector<Node*> getInNeighbors() const { return in_neighbors_; }
    std::vector<Node*> getOutNeighbors() const { return out_neighbors_; }
    Entity **getPosting() const { return posting_; }
    void calculateCentroid();
    bool canAddVector() const { return vector_num_ < max_num_; }
    void addVector(const void* vector, const int vector_id);
    void deleteVector(int vector_id);
    void copyPostingFromPrevVersion();
    void copyNeighborFromPrevVersion();
    void copyCentroidFromPrevVersion() {
        std::memcpy(centroid, prev_version_->getCentroid(), dimension_ * element_size_);
    }
    void addInNeighbor(Node* neighbor) { in_neighbors_.push_back(neighbor); }
    void deleteInNeighbor(Node* neighbor) {
        in_neighbors_.erase(std::remove(in_neighbors_.begin(), in_neighbors_.end(), neighbor), in_neighbors_.end());
    }
    void addOutNeighbor(Node* neighbor) { out_neighbors_.push_back(neighbor); }
    void deleteOutNeighbor(Node* neighbor) {
        out_neighbors_.erase(std::remove(out_neighbors_.begin(), out_neighbors_.end(), neighbor), out_neighbors_.end());
    }
    void setUpdaterId(int updater_id) { updater_id_ = updater_id; }

private:
    const int version_;
    Node *node_;
    Version *prev_version_;
    const int max_num_;
    int vector_num_;
    const int dimension_;
    int updater_id_;
    const size_t element_size_;
    std::vector<Node*> in_neighbors_;
    std::vector<Node*> out_neighbors_;
    void *centroid;
    Entity **posting_;
};

#endif //CAMPUS_VERSION_H