#ifndef BOUNDINGVOLUME_H
#define BOUNDINGVOLUME_H

#include <glm/glm.hpp>
#include "Objects.h"

class BoundingVolume {
public:
    virtual bool Collision(BoundingVolume &bv) = 0;
    virtual void Calculate(glm::vec3 &pos, float rad) = 0;
};

class BoundingBox : public BoundingVolume {
public:
    glm::vec3 min, max;

    bool Collision(BoundingVolume &bv) { return false; }

    bool Collision(BoundingBox &b) {
        return (min.x <= b.max.x && max.x >= b.min.x) &&
           (min.y <= b.max.y && max.y >= b.min.y) &&
           (min.z <= b.max.z && max.z >= b.min.z);
    }

    void Calculate(glm::vec3 &pos, float rad) {
        min = pos - glm::vec3(rad);
        max = pos + glm::vec3(rad);
    }
};

#endif