#include "version.h"
#include <cstring>

void Version::addVector(const void* vector, const int vector_id) {
    if (size_ < max_size_) {
        posting_[size_] = new Entity(vector_id, vector, dimension_, element_size_);
        size_++;
    }
}

void Version::calculateCentroid() {
    if (size_ == 0) {
        centroid = nullptr;
        return;
    }

    centroid = new char[dimension_ * element_size_];
    std::memset(centroid, 0, dimension_ * element_size_);

    for (int i = 0; i < size_; ++i) {
        const float* vec = static_cast<const float*>(posting_[i]->getVector());
        for (int j = 0; j < dimension_; ++j) {
            reinterpret_cast<float*>(centroid)[j] += vec[j];
        }
    }

    for (int j = 0; j < dimension_; ++j) {
        reinterpret_cast<float*>(centroid)[j] /= size_;
    }
}