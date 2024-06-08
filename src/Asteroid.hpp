// data and functions to draw a sphere
#pragma once

#include "Object.hpp"

// octahedron object
class Asteroid : public Object {
private:
    int width;
    int height;
    glm::vec3 size;
    const char *texturePPM;
    std::vector<glm::vec3> weighted_tri_norms;
    float grid_width;
public:
    // create an octahedron given latitude and longitude sizes and color texture
    Asteroid(int width, int height, glm::vec3 size, const char *texturePPM);

    // update render state, overridden to move object around
    virtual void setRenderState(GLapp *app, double now) override;
    
    // increase mesh resolution
    virtual void subdivide(int subdivisions) override;
    
    // obtain distance from p to the surface below p
    virtual glm::vec3 vec_to_surface(glm::vec3 foot_position);
};
