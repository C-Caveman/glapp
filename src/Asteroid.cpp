// draw a simple octahedron model

#include "Asteroid.hpp"
#include "Noise.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include <cmath>
#include <cstring>
#include "stdio.h"

using namespace glm;  // avoid glm:: for all glm types and functions

#ifndef F_PI
#define F_PI 3.1415926f
#endif

extern int noise_amplitude; // set by the user [0+]
// split each triangle into three triangles -subdivisions- times
// by rebuilding the Asteroid with the appropriate number of vertices
void Asteroid::subdivide(int subdivisions) {
    // clear out the old vertex data
    this->vert.clear();
    this->norm.clear();
    this->uv.clear();
    this->indices.clear();
    this->weighted_tri_norms.clear();
    if (subdivisions < 0) // no negative subdivisions
        subdivisions = 0;
    int divs = subdivisions;//std::pow(2, subdivisions);
    // subdivide!
    int new_width = 3;
    for (int i=0; i<divs; i++) {
        new_width *= 2;
        new_width -= 1;
    }
    int w = new_width;
    int h = new_width;
    vec3 size = this->size;
    // an odd height/width is required
    if (w % 2 == 0) // if even
        w += 1;     // make odd
    if (h % 2 == 0)
        h += 1;
    float grid_width = this->grid_width = 1.0f / (w-1);
    // set the positions of the verticies
    for (int vi=0; vi<h; vi++) {
        float v = (float)vi*grid_width;
        for (int ui=0; ui<w; ui++) {
            float u = (float)ui*grid_width;
            float x = u*2.f-1.f;
            float y = v*2.f-1.f;     // position in square scaled to -1..1
            float z = 1.f - abs(x) - abs(y);  // z computed to make a pyramid
            x += sign(x)*std::min(0.f,z);   // flip bottom of pyramid to make octahedron
            y += sign(y)*std::min(0.f,z);
            vec3 V = vec3(x, y, z);
            V = normalize(V);
            //
            // FRACTAL BROWNIAN MOTION
            //
            const int MAX_OCTAVES = 10; // usually never reached
            const float EPSILON = 0.001f;
            float lacunarity = 2.0;
            float gain = 0.5;
            float frequency = 1.0f; // scale frequency to num verticies
            float amplitude = 0.1f * noise_amplitude; // mutable by pressing '(' and ')'
            float noise = 0;
            for (int i=0; i<MAX_OCTAVES; i++) {
                noise += Noise::Noise1(V*frequency) * amplitude;
                frequency *= lacunarity;
                amplitude *= gain;
                if (amplitude < EPSILON) {
                    break;
                }
            }
            V = V*size + V*(noise*size); // apply the noise
            vert.push_back(V);
            norm.push_back(vec3{0,0,0}); // placeholder for normal vector
            uv.push_back(vec2(ui*grid_width, vi*grid_width));
        }
    }
    // build index array linking sets of three vertices into triangles
    // two triangles per square in the grid. Each vertex index is
    // essentially its unfolded grid array position. Be careful that
    // each triangle ends up in counter-clockwise order
    for(int vi=0; vi<h-1; ++vi) {
        for(int ui=0; ui<w-1; ++ui) {
            unsigned int cur = ui + vi*w;
            unsigned int p1; // triangle 1
            unsigned int p2;
            unsigned int p3;
            unsigned int p4; // triangle 2
            unsigned int p5;
            unsigned int p6;
            int right = 1;
            int down = w;
            int x_sign = 1;
            int y_sign = 1;
            vec3 crossprod1;
            vec3 crossprod2;
            if (ui < w/2) { // horizontal flip
                x_sign = -1;
            }
            if (vi < h/2) { // vertical flip
                y_sign = -1;
            }
            // 4 cases for building triangle pairs depending on the quadrant
            if (x_sign == -1 && y_sign == -1) { // top-left quadrant
                //tri 1
                p1 = cur;
                p2 = cur + down;
                p3 = cur + right;
                //tri 2
                p4 = cur + down + right;
                p5 = cur + right;
                p6 = cur + down;
            }
            else if (x_sign == 1 && y_sign == -1) { // top-right quadrant
                //tri 1
                p1 = cur + right + down;
                p2 = cur + right;
                p3 = cur;
                //tri 2
                p4 = cur;
                p5 = cur + down;
                p6 = cur + down + right;
            }
            else if (x_sign == 1 && y_sign == 1) { // bottom-right quadrant
                //tri 1
                p1 = cur;
                p2 = cur + down;
                p3 = cur + right;
                //tri 2
                p4 = cur + down;
                p5 = cur + down + right;
                p6 = cur + right;
            }
            else if (x_sign == -1 && y_sign == 1) { // bottom-left quadrant
                //tri 1
                p1 = cur;
                p2 = cur + down;
                p3 = cur + down + right;
                //tri 2
                p4 = cur;
                p5 = cur + down + right;
                p6 = cur + right;
            }
            crossprod1 = -cross(vert[p2]-vert[p1],
                                   vert[p3]-vert[p2]);
            crossprod2 = -cross(vert[p5]-vert[p4],
                            vert[p6]-vert[p5]);
            indices.push_back(p1); // triangle 1
            indices.push_back(p2);
            indices.push_back(p3);
            weighted_tri_norms.push_back(crossprod1); // for vertex norm calculation
            indices.push_back(p4); // triangle 2
            indices.push_back(p5);
            indices.push_back(p6);
            weighted_tri_norms.push_back(crossprod2); // for vertex norm calculation
        }
    }
    // obtain the vertex normals
    const float EPSILON = 0.01f;
    int num_indices = indices.size();
    int num_tris = num_indices / 3;
    int num_verts = h * w;
    int num_norms = norm.size();
    for (int v=0; v<num_verts; v++) { // v is the vertex index
        norm[v] = vec3{0,0,0}; // zero-out the old normal data
        // find adjacent triangles
        for (int tri=0; tri<num_indices; tri += 3) {
            // check if the current vertex is in the triangle
            for (int i=0; i<3; i++) {
                if (length(vert[indices[tri+i]] - vert[v]) < EPSILON) {
                    // add the area-weighted normal direction of the triangle
                    vec3 area_weighted_tri_norm = weighted_tri_norms[tri/3];
                    norm[v] = norm[v] + area_weighted_tri_norm;
                }
            }
        }
        norm[v] = normalize(norm[v]);
    }
    initGPUData();
}

// find distance from foot to the asteroid surface
vec3 Asteroid::vec_to_surface(glm::vec3 foot_position) {
    vec3 original_foot_position = foot_position;
    foot_position = glm::normalize(foot_position);
    float cell_width = this->grid_width;
    // foot_position to uv position
    float s = 0.5f/(abs(foot_position.x) 
            + abs(foot_position.y) 
            + abs(foot_position.z));
    float t = min(0.f,foot_position.z);
    vec2 uv_pos = vec2(foot_position.x - sign(foot_position.x)*t, 
                       foot_position.y - sign(foot_position.y)*t)*s + 0.5f;
    float x = uv_pos.x;
    float y = uv_pos.y;
    // uv_grid to cur triangle
    int cell_x = floor(x / cell_width);
    int cell_y = floor(y / cell_width);
    int cur_tri = (cell_y*this->width + cell_x) * 2; // each cell has 2 tris
    if ( (x-cell_x*cell_width) > (y-cell_y*cell_width) )
        cur_tri += 1;
    int tri = indices[cur_tri*3];
    // debug display
    //uv[tri % (this->height * this->width)] = -1.f * uv[tri % (this->height * this->width)];
    vec3 debug_centroid = vert[(tri) % (this->height * this->width)]
                        + vert[(tri+1) % (this->height * this->width)]
                        + vert[(tri+2) % (this->height * this->width)];
    debug_centroid /= 3;
    float distance = glm::length(vert[indices[cur_tri*3] % (this->height * this->width)] - original_foot_position);
    //printf("Triangle %d, distance %f\n", cur_tri, distance);
    return (debug_centroid - original_foot_position);
}

// load the sphere data
Asteroid::Asteroid(int w, int h, vec3 size, const char *texturePPM) :
    Object(texturePPM)
{
    // an odd height/width is required (can't have a pyramid without a point!)
    if (w % 2 == 0) // if even
        w += 1;     // make odd
    if (h % 2 == 0)
        h += 1;
    this->height = h; // store this so we can subdivide later
    this->width = w;
    this->size = size;
    this->texturePPM = texturePPM;
    // use a different fragment shader
    if (strcmp(this->texturePPM, "paving-color.ppm") == 0) {
        this->shaderParts.clear();
        this->shaderParts = {
            {glCreateShader(GL_VERTEX_SHADER  ), "asteroid.vert"},
            {glCreateShader(GL_FRAGMENT_SHADER), "asteroid.frag"}
        };
    }
    // subdivide generates all the vertex information
    Asteroid::subdivide(0);
}

//
// this is called every time the sphere needs to be redrawn 
//
void Asteroid::setRenderState(GLapp *app, double now)
{
    // inherit parent's draw settings
    Object::setRenderState(app, now);

    // update model position; example of per-frame position updates
    objectShaderData.WorldFromModel = 
        translate(mat4(1), 100.f * vec3(0,0,0));
    objectShaderData.ModelFromWorld = inverse(objectShaderData.WorldFromModel);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferIDs[OBJECT_UNIFORM_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ObjectShaderData), &objectShaderData);
}


