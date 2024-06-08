//
// Simple GL example
//

#include "GLapp.hpp"
#include "Asteroid.hpp"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <assert.h>

#ifndef F_PI
#define F_PI 3.1415926f
#endif

// things for handling the file timestamps
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

using namespace glm;  // avoid glm:: for all glm types and functions

int regenerating = 0;
int subdivision = 0;
int noise_amplitude = 0;

///////
// GLFW callbacks must use extern "C"
extern "C" {
    // called for GLFW error
    void error(int error, const char *description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    // called whenever the window size changes
    void reshape(GLFWwindow *win, int width, int height) {
        // save window dimensions
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        glfwGetFramebufferSize(win, &app->width, &app->height);

        // viewport size matches window size
        glViewport(0, 0, app->width, app->height);
    }

    // called when mouse button is pressed
    void mousePress(GLFWwindow *win, int button, int action, int mods) {
        if (button != GLFW_MOUSE_BUTTON_LEFT) return;

        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        app->active = true;
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(win, &app->mouseX, &app->mouseY);
    }

    float sensitivity = 0.25;
    // called when mouse is moved
    void mouseMove(GLFWwindow *win, double x, double y) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        if (!app->active) return;

        // rotation angle, scaled so across the window = one rotation
        app->pan += float(sensitivity*F_PI * float(x - app->mouseX) / app->width);
        app->tilt += float(sensitivity*F_PI * float(y - app->mouseY) / app->height);

        // remember location so next update will be relative to this one
        app->mouseX = x;
        app->mouseY = y;
    }

    // called on any keypress
    void keyPress(GLFWwindow *win, int key, int scancode, int action, int mods) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        // shift key needs special treatment
        if(glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
            && app->dash_cooldown <= 0
        ) {
            app->move_dash = 1; // dash!
            //printf("Dashing!\n");
        }
        else if (app->move_dash == 1) {
            //printf("Dash canceled.\n");
            app->move_dash = 0; // dash!
        }
            
        if (action == GLFW_PRESS) {
            switch (key) {
            // Walking controls
            case 'W':                   // move forwards
                app->move_forward = 1; // wasd movement
                return;
            case 'S':                   // move backwards
                app->move_forward = -1; // wasd movement
                return;
            case 'A':                   // move left
                app->move_side = -1; // wasd movement
                return;
            case 'D':                   // move right
                app->move_side = 1; // wasd movement
                return;
            case GLFW_KEY_SPACE:
                app->move_jump = 1; // jump!
                return;

            // Camera controls
            case GLFW_KEY_LEFT:                   // rotate left
                app->panRate = -F_PI;  // half a rotation/sec
                return;
            case GLFW_KEY_RIGHT:                   // rotate right
                app->panRate = F_PI;   // half a rotation/sec
                return;
            case GLFW_KEY_UP:                   // rotate up
                app->tiltRate = 0.5f * F_PI; // 1/4 rotation/sec
                return;
            case GLFW_KEY_DOWN:                   // rotate down
                app->tiltRate = -0.5f * F_PI; // 1/4 rotation/sec
                return;
             
            // reload shaders
            case 'R':                   
                for (auto object : app->objects)
                    object->updateShaders();
                return;

            // toggle lines or solid
            case 'L':                   
                // example of using input to change OpenGL state
                app->wireframe = !app->wireframe;
                glPolygonMode(GL_FRONT_AND_BACK, app->wireframe ? GL_LINE : GL_FILL);
                return;
                
            // asteroid subdivision
            case '-':
                subdivision--;
                if (subdivision < 0) // no negative subdivisions
                    subdivision = 0;
                regenerating = 1;
                return;
            case '=':
                subdivision++;
                regenerating = 1;
                return;
                
            // asteroid noise tweaking
            case '9':
                noise_amplitude -= 1;
                if (noise_amplitude < 0) // no negative noise_amplitude
                    noise_amplitude = 0;
                regenerating = 1;
                return;
            case '0':
                noise_amplitude += 1;
                regenerating = 1;
                return;

            // quit
            case 'Q':
            case GLFW_KEY_ESCAPE:                    // Escape
                if (app->active) {                   //  1st press, release mouse
                    app->active = false;
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else                                 //  2nd press, exit                 
                    glfwSetWindowShouldClose(win, true);
                return;
            }
        }

        if (action == GLFW_RELEASE) {
            switch (key) {
            // Walking controls
            case 'A': case 'D':         // stop strafing
                app->move_side = 0;
                return;
            case 'W': case 'S':         // stop walking
                app->move_forward = 0;
                return;
                
            // Camera controls
            case GLFW_KEY_LEFT: case GLFW_KEY_RIGHT:      // stop panning
                app->panRate = 0;
                return;
            case GLFW_KEY_UP: case GLFW_KEY_DOWN:         // stop tilting
                app->tiltRate = 0;
                return;
            }
        }
    }
}

// initialize GLFW - windows and interaction
GLapp::GLapp()
{
    // member data initialization
    active = false;                             // not tracking mouse input
    width = 800; height = 600;                  // window size
    distance = 300.f; pan = 0.f; tilt = -1.4f;  // view
    panRate = tiltRate = 0.f;                   // keyboard view control
    mouseX = mouseY = 0.f;                      // mouse view controls
    wireframe = false;                          // solid drawing

    // set error callback before init
    glfwSetErrorCallback(error);
    int ok = glfwInit();
    assert(ok);

    // OpenGL version: YOU MAY NEED TO ADJUST VERSION OR OPTIONS!
    // When figuring out the settings that will work for you, make
    // sure you can see error messages on console output.
    //
    // My driver needs FORWARD_COMPAT, but others will need to comment that out.
    // Likely changes for other versions:
    //   All versions: change VERSION_MAJOR and VERSION_MINOR
    //   OpenGL 3.0 (2008): does not support features we need
    //   OpenGL 3.1 (2009):
    //     comment out GLFW_OPENGL_PROFILE line
    //     Use "140" for the "#version" line in the .vert and .frag files
    //   OpenGL 3.2 (2009): Use "150 core" for the "#version" line in the .vert and .frag files
    //   OpenGL 3.3 (2010): Use "330 core" for the "#version" line in the .vert and .frag files
    //   Any of 4.0-4.6:
    //     Similar to 3.3: #version line in shaders uses <MAJOR><MINOR>0
    //     For example, 4.6 is "#version 460 core" 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // ask for a window with dimensions 843 x 480 (HD 480p)
    win = glfwCreateWindow(width, height, "Simple OpenGL Application", 0, 0);
    const char** desc;
    int error_code = glfwGetError(desc);
    assert(win);

    glfwMakeContextCurrent(win);

    // GLEW handles OpenGL shared library access
    glewExperimental = true;
    glewInit();

    // set callback functions to be called by GLFW
    glfwSetWindowUserPointer(win, this);
    glfwSetFramebufferSizeCallback(win, reshape);
    glfwSetKeyCallback(win, keyPress);
    glfwSetMouseButtonCallback(win, mousePress);
    glfwSetCursorPosCallback(win, mouseMove);

    // tell OpenGL to enable z-buffer for overlapping surfaces
    glEnable(GL_DEPTH_TEST);

    // lock frame rate to display
    glfwSwapInterval(1);

    // initialize buffer for scene shader data
    glGenBuffers(1, &sceneUniformsID);
    glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneData), 0, GL_STREAM_DRAW);

    // initialize scene data
    scene.LightDir = vec4(-1,-2,2,0);
    move_forward =
    move_side =
    move_dash =
    move_jump = 0;
    camera_position = {0,0,0};
    camera_velocity = {0,0,0};
}

///////
// Clean up any context data
GLapp::~GLapp() 
{
    glfwDestroyWindow(win);
    glfwTerminate();
}

void printVec3(vec3 v) { printf("(%.2f, %.2f, %.2f)\n", v.x, v.y, v.z); }

// call before drawing each frame to update per-frame scene state
void GLapp::sceneUpdate(double dTime)
{
    const float move_speed = 700.f;
    const float gravity_strength = 800;//0.08f * (!DEBUG); // no grav in debug mode
    const float push_strength = 1000;
    const float jump_power = 300.f;
    const float player_height = 10.f;
    const float max_dash_duration = 0.24f;
    const float max_dash_cooldown = 2.0f;
    // update view dir
    pan += float(panRate * dTime);
    tilt += float(tiltRate * dTime);
    //float pan_orbit = pan + vec3(0,0,1);
    //float tilt_orbit = tilt + ???;
    vec3 up_dir = vec3{0,0,1};//(camera_position - vec3(0,0,0));
    vec3 forward_dir = vec3(-sin(pan), -cos(pan), 0); //vec3(-sin(pan), -cos(pan), cos(tilt));
    vec3 side_dir = cross(forward_dir, vec3(0,0,1));
    // gravity
    vec3 gravity_dir = -up_dir;
    // Walking
    camera_velocity += float(move_forward * move_speed * dTime) * forward_dir;
    camera_velocity += float(move_side    * move_speed * dTime) * side_dir;
    // collision
    float dist_to_ground = 70.f - glm::length(camera_position) + player_height;
    if (move_jump > 0) {
        camera_velocity += float(move_jump * jump_power) * vec3{0,0,-1};//up_dir;
        move_jump = 0;
    }
    // touching the ground (or flying in debug mode)
    if (camera_position.z > -80 && camera_position.z < -70) {
        //camera_velocity -= gravity_strength * gravity_dir; // cancel out the gravity
        // jump if pressing the jump key
        
    }
    else if (camera_position.z > -70) {
        camera_position.z = -70;
        if (camera_velocity.z > 0)
            camera_velocity.z = 0;
    }
    else if (camera_position.z < -65) {
        camera_velocity -= gravity_strength * gravity_dir * (float)dTime;
    }
    // track dash duration, stop dashing if dash duration exceeded
    if (move_dash == 1)
        dash_time += dTime;
    if (dash_time > max_dash_duration) {
        move_dash = 0;
        dash_time = 0;
        //printf("Dash stopped.\n");
        if (dash_cooldown <= 0)
            dash_cooldown = max_dash_cooldown;
    }
    if (dash_cooldown > 0) {
        dash_cooldown -= dTime;
    }
    // friction
    float tmp = camera_velocity.z;
    camera_velocity *= 0.93;
    camera_velocity.z = tmp;
    // double velocity if dashing
    //if (dash_cooldown <= 0)
    //    camera_velocity *= (move_dash*0.25 + 1);
    camera_position += float(dTime) * camera_velocity;
    // perform camera transformation
    //tilt += acos(gravity_dir.z); // account for changing up_dir
    vec3 forward_pos = camera_position + forward_dir;
    scene.ProjFromWorld = 
        perspective(F_PI/4.f, (float)width/height, 1.f, 10000.f)

        * rotate(mat4(1), tilt, vec3(1,0,0))
        * rotate(mat4(1), pan, vec3(0,0,1))
        * translate(mat4(1), camera_position);
        
        /*
        * lookAt(camera_position, 
                    forward_pos, 
                    vec3(0,0,1));
        */
    scene.WorldFromProj = inverse(scene.ProjFromWorld);

    glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneData), &scene);
}

// render a frame
void GLapp::render()
{
    // consistent time for drawing this frame
    double currTime = glfwGetTime();
    double dTime = currTime - prevTime;
    // clear old screen contents to a sky blue
    glClearColor(0.5, 0.7, 0.9, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw all objects
    sceneUpdate(dTime);
    for (auto object : objects)
        object->draw(this, currTime);

    // show what we drew
    glfwSwapBuffers(win);
    prevTime = currTime;
}

// vars for tracking the shader file timestamps
char filename[] = "../data/asteroid.frag";
struct stat filestats;
double shader_timestamp = 0;
double previous_shader_timestamp = 0;
int main(int argc, char *argv[])
{
    printf("Press '-' and '+' to change the subdivision level.\n");
    printf("Press '(' and ')' to change the noise amplitude.\n");
    // initialize windows and OpenGL
    GLapp app;

    // set the initial subdivision for the asteroid and sky sphere
    subdivision = 3;
    // add the asteroid!
    app.objects.push_back(new Asteroid(3, 3, vec3(50.f,50.f,50.f), "paving-color.ppm"));
    app.objects[0]->subdivide(subdivision);
    // add the sky-sphere
    app.objects.push_back(new Asteroid(3, 3, vec3(500.f,500.f,500.f), "sky-color.ppm"));
    app.objects[1]->subdivide(subdivision);
    
    // set up initial viewport
    reshape(app.win, app.width, app.height);
    
    // set player position
    app.camera_position = vec3(0,80,-72);

    // each frame: render then check for events
    while (!glfwWindowShouldClose(app.win)) {
        app.render();
        glfwPollEvents();
        if (regenerating) { // only subdivide when necessary
            app.objects[0]->subdivide(subdivision);
            regenerating = 0;
        }
        // update shaders if they were modified since the last frame
        if(stat(filename, &filestats)==0)
        {
            shader_timestamp = filestats.st_mtime;
        }
        if(shader_timestamp > previous_shader_timestamp) {
            for (auto object : app.objects)
                object->updateShaders();
        }
        previous_shader_timestamp = shader_timestamp;
    }

    return 0;
}
