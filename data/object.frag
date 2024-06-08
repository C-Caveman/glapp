#version 410 core
// simple object fragment shader

// per-frame data
layout(std140)                  // standard layout
uniform SceneData {             // like a class name
    mat4 ProjFromWorld, WorldFromProj;
    vec3 LightDir;
    float pad0;
};

// shader settings
uniform sampler2D ColorTexture;

// input from vertex shader
in vec2 texcoord;
in vec3 normal;
in vec4 position;

// output to frame buffer
out vec4 fragColor;

void main() {
    // solid color or from texture
    vec3 color = vec3(.5);
    if (textureSize(ColorTexture,0) != ivec2(1,1))
        color = texture(ColorTexture, texcoord).rgb;

    // apply ambient and diffuse
    vec3 N = normalize(normal);             // surface normal
    vec3 L = normalize(LightDir);           // light direction
    color = color * (max(0., dot(N,L)) + 0.5);

    // final color
    fragColor = vec4(color, 1);
}
