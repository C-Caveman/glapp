// base class for drawable objects

#include "Object.hpp"
#include "GLapp.hpp"
#include "Config.hpp"

#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>

// avoid extra glm:: and std::
using namespace glm;
using namespace std;

#ifdef _WIN32
// don't complain if we use standard IO functions instead of windows-only
#pragma warning( disable: 4996 )
#endif


// create GPU buffers objects
// derived class is responsible for initializing these
Object::Object(const char *texturePPM)
{
    // create buffer objects to be used later
    glGenTextures(NUM_TEXTURES, textureIDs);
    glGenBuffers(NUM_BUFFERS, bufferIDs);
    glGenVertexArrays(1, &varrayID);

    // load color image into a named texture
    loadPPM(texturePPM, textureIDs[COLOR_TEXTURE]);

    // default to position at origin
    objectShaderData = {mat4(1), mat4(1)};
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[OBJECT_UNIFORM_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectShaderData), &objectShaderData, GL_STREAM_DRAW);

    // initial shader load
    shaderParts = {
        {glCreateShader(GL_VERTEX_SHADER  ), "object.vert"},
        {glCreateShader(GL_FRAGMENT_SHADER), "object.frag"}
    };
    shaderID = glCreateProgram();
}


// delete GPU buffers and objects
Object::~Object()
{
    for (auto shader : shaderParts)
       glDeleteShader(shader.id);
    glDeleteProgram(shaderID);
    glDeleteTextures(NUM_TEXTURES, textureIDs);
    glDeleteBuffers(NUM_BUFFERS, bufferIDs);
    glDeleteVertexArrays(1, &varrayID);
}


// load a PPM image into a texture
void Object::loadPPM(const char *imagefile, unsigned int bufferID)
{
    // set active texture for later texture calls
    glBindTexture(GL_TEXTURE_2D, bufferID);

    // can detect 1x1 texture size in shader for missing texture
    if (!imagefile) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        return;
    }

    // open file in project data directory
    string path = string(PROJECT_DATA_DIR).append(string(imagefile));
    FILE *fp = fopen(path.c_str(), "rb");
    assert(fp);

    // check that "magic number" at beginning of file is P6
    if (fgetc(fp) != 'P' || fgetc(fp) != '6') {
        fprintf(stderr, "unknown image format %s\n", path.c_str());
        assert(false);
    }

    // read image size, maximum value, and blank following header
    int width = 0, height = 0, maxval = 0, lf = 0;
    fscanf(fp, " #%*[^\n]");                // skip comment (if there)
    fscanf(fp, "%d %d", &width, &height);   // read image size
    assert(width > 0 && height > 0);

    fscanf(fp, " #%*[^\n]");                // skip comment (if there)
    fscanf(fp, "%d", &maxval);              // skip max value
    assert(maxval == 255);

    lf = fgetc(fp);                         // skip final \n before data
    assert(lf == '\n');

    // check remaining file size matches image size
    // if this fails, you may have checked a ppm file out
    // as text rather than binary
    long headerEnd = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long fileEnd = ftell(fp);
    fseek(fp, headerEnd, SEEK_SET);
    assert(fileEnd - headerEnd == width*height*3);

    // allocate image and read array
    u8vec3 *image = new u8vec3[width * height];
    fread(image, sizeof(u8vec3), width * height, fp);
    fclose(fp);

    // load into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    delete[] image;
}


// load vertex and index arrays to GPU
void Object::initGPUData() 
{
    // update buffer data to GPU
    glBindBuffer(GL_UNIFORM_BUFFER, bufferIDs[OBJECT_UNIFORM_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ObjectShaderData), &objectShaderData, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(vert[0]), &vert[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(norm[0]), &norm[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(uv[0]), &uv[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

    updateShaders();
}


// load or replace object shaders
void Object::updateShaders()
{
    loadShaders(shaderID, shaderParts);
    glUseProgram(shaderID);

    // Bind uniform block #s to their shader names. Indices should match glBindBufferBase in draw
    glUniformBlockBinding(shaderID, glGetUniformBlockIndex(shaderID,"SceneData"),  0);
    glUniformBlockBinding(shaderID, glGetUniformBlockIndex(shaderID,"ObjectData"), 1);

    // Map shader name for texture. 0 says to use GL_TEXTURE0: should match setRenderState
    glUniform1i(glGetUniformLocation(shaderID, "ColorTexture"), 0);

    // bind attribute arrays
    glBindVertexArray(varrayID);

    GLint positionAttrib = glGetAttribLocation(shaderID, "vPosition");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[POSITION_BUFFER]);
    glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttrib);

    GLint normalAttrib = glGetAttribLocation(shaderID, "vNormal");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[NORMAL_BUFFER]);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalAttrib);

    GLint uvAttrib = glGetAttribLocation(shaderID, "vUV");
    glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[UV_BUFFER]);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(uvAttrib);
}


// set shader, textures, etc. for this draw
void Object::setRenderState(GLapp* app, double now)
{
    // enable shader
    glUseProgram(shaderID);

    // select vertex array to render
    glBindVertexArray(varrayID);

    // bind textures
    for (int i=0; i < NUM_TEXTURES; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
    }
    
    // bind uniform buffers to the appropriate uniform block numbers
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, app->sceneUniformsID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferIDs[OBJECT_UNIFORM_BUFFER]);
}


// draw object
void Object::draw(GLapp* app, double now)
{
    // set shader, textures & uniform buffers
    setRenderState(app, now);

    // draw the triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[INDEX_BUFFER]);
    glDrawElements(GL_TRIANGLES, GLsizei(indices.size()), GL_UNSIGNED_INT, 0);
}

void Object::subdivide(int subdivisions) {}

vec3 Object::vec_to_surface(glm::vec3 foot_position) {return vec3(0,0,0);}
