// collected state for access in callbacks
// 
#pragma once

#include <glm/glm.hpp>
#include <vector>

#define DEBUG 1

class GLapp {
public:
    struct GLFWwindow *win;      // graphics window from GLFW system

    // uniform buffer data about the scene
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct SceneData {
        glm::mat4 ProjFromWorld, WorldFromProj;  // viewing matrix & inverse
        glm::vec3 LightDir;                      // light direction
        float pad0;                              // padding to fill a vec4
    } scene;
    unsigned int sceneUniformsID;

    // view info
    bool active;                // clicked into window
    int width, height;          // current window dimensions
    float distance;             // distance from 0,0,0
    float pan, tilt;            // horizontal and vertical Euler angles
    float panRate, tiltRate;    // keyboard orbiting rate in radians/sec
    //TODO first person movement
    int move_side, move_forward, move_jump, move_dash; // desired movement direction
    float dash_time; // duration of current dash
    float dash_cooldown; // delay before next dash is allowed
    glm::vec3 camera_position; // can be changed by pressing W, A, S, or D
    glm::vec3 camera_velocity;

    // mouse state
    double mouseX, mouseY;      // location of mouse at last event

    // drawing state
    bool wireframe;

    // time (in seconds) of last frame
    double prevTime;

    // objects to draw
    std::vector<class Object*> objects;

public:
    // initialize and destroy app data
    GLapp();
    ~GLapp();

    // update shader uniform state each frame
    void sceneUpdate(double dTime);

    // main rendering loop
    void render();
};
