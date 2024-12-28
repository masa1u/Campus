#ifndef CAMPUS_H
#define CAMPUS_H

#include "node.h"
#include "../utils/distance.h"
#include "../utils/lock.h"
#include <vector>

class Campus {
public:
    enum DistanceType {
        L2,
        Angular
    };

    Campus(int dimension, int posting_limit, DistanceType distance_type, size_t element_size)
        : dimension_(dimension), posting_limit_(posting_limit), node_size_(0), distance_type_(distance_type), element_size_(element_size), entry_point_(nullptr), link_list_(nullptr) {}

    ~Campus() {
        delete[] link_list_;
    }

    int getNodeSize() const { return node_size_; }
    int getPositionLimit() const { return posting_limit_; }
    int getDimension() const { return dimension_; }
    Node *findExactNearestNode(const void *query_vector, Distance *distance);
    std::vector<Node*> findNearestNodes(const void *query_vector, Distance *distance, int n);
    std::vector<int> topKSearch(const void *query_vector, int top_k, Distance *distance, int n);
    std::vector<int> getAllVectorIds(); // for Debug
    DistanceType getDistanceType() const { return distance_type_; }
    bool validationLock() { return validation_lock_.w_trylock(); }
    void validationUnlock() { return validation_lock_.w_unlock(); }
    void addNodeToGraph(Node *new_node);

private:
    const int dimension_;
    const int posting_limit_;
    int node_size_;
    size_t element_size_;
    Lock validation_lock_;
    Node *entry_point_;
    Node **link_list_;
    DistanceType distance_type_;
};

class CampusInsertExecutor {
public:
    CampusInsertExecutor(Campus *campus, const void *insert_vector, int vector_id)
        : campus_(campus), insert_vector_(insert_vector), vector_id_(vector_id) {
        switch (campus_->getDistanceType()) {
            case Campus::L2:
                distance_ = new L2Distance();
                break;
            case Campus::Angular:
                distance_ = new AngularDistance();
                break;
        }
    }

    ~CampusInsertExecutor() {
        delete distance_;
    }

    void insert();

private:
    Campus *campus_;
    Distance *distance_;
    const void *insert_vector_;
    const int vector_id_;
};

class CampusQueryExecutor {
public:
    CampusQueryExecutor(Campus *campus, const void *query_vector, int top_k)
        : campus_(campus), query_vector_(query_vector), top_k_(top_k) {
        switch (campus_->getDistanceType()) {
            case Campus::L2:
                distance_ = new L2Distance();
                break;
            case Campus::Angular:
                distance_ = new AngularDistance();
                break;
        }
    }

    ~CampusQueryExecutor() {
        delete distance_;
    }

    std::vector<int> query();

private:
    Campus *campus_;
    const void *query_vector_;
    const int top_k_;
    Distance *distance_;
};

#endif //CAMPUS_H