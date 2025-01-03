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

    Campus(int dimension, int posting_limit, int connection_limit, DistanceType distance_type, size_t element_size)
        : dimension_(dimension), posting_limit_(posting_limit), connection_limit_(connection_limit), node_num_(0),
            update_counter_(0), distance_type_(distance_type), element_size_(element_size), entry_point_(nullptr){}

    ~Campus() {

    }

    int getNodeNum() const { return node_num_; }
    int getUpdateCounter() { return update_counter_++;}
    int getPositingLimit() const { return posting_limit_; }
    int getDimension() const { return dimension_; }
    size_t getElementSize() const { return element_size_; }
    int getConnectionLimit() const { return connection_limit_; }

    Node *findExactNearestNode(const void *query_vector, Distance *distance);
    std::vector<Node*> findNearestNodes(const void *query_vector, Distance *distance, int n);
    std::vector<int> topKSearch(const void *query_vector, int top_k, Distance *distance, int n);
    DistanceType getDistanceType() const { return distance_type_; }
    bool validationLock() { return validation_lock_.w_trylock(); }
    void validationUnlock() { return validation_lock_.w_unlock(); }
    void switchVersion(Node *node, Version *new_version);
    void setEntryPoint(Node *node) { entry_point_ = node; }
    void incrementNodeNum() { node_num_++; }

private:
    const int dimension_;
    const int posting_limit_;
    const int connection_limit_;
    int node_num_;
    int update_counter_;
    size_t element_size_;
    Lock validation_lock_;
    Node *entry_point_;
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
    std::vector<Version*> changed_versions_;
    // std::vector<Node*> changed_nodes_;
    std::vector<Node*> new_nodes_; // Newly created nodes with split
    std::vector<Version*> new_versions_; // Newly created versions without split
    void splitCalculation(Version *spliting_version);
    void assignCalculation(Node *new_node1, Node *new_node2);
    void reassignCalculation(Version *spliting_version, Node *new_node1, Node *new_node2);
    void connectNeighbors(Version *spliting_version, Node *new_node1, Node *new_node2, int connection_limit);
    void updateNeighbors(Version *spliting_version, Node *new_node1, Node *new_node2, int connection_limit);
    bool validation();
    void commit();
    void abort();
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

#endif // CAMPUS_H