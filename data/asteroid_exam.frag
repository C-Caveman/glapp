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

//----------------------------------------------------------------------
// 3D noise functions
//--------------------------------
// 3D hash function and 3D Perlin simplex noise

// 3D to 3D hash function
uvec3 Hash3(uvec3 v) 
{
    v = v * 1664525u + 1013904223u;      
    v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
    v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
    return v >> 16u;
}

// 3 random floats in [0,1] based on hash of floor(v)
vec3 Rand3(vec3 v)
{
    // cast float to signed int, then to uint for consistency for negatives
    return vec3(Hash3(uvec3(ivec3(floor(v))))) / 0xffff;
}

// support function for Noise3
// 3D simplex weights (& corners)
// based on McEwan et al., Efficient computation of noise in GLSL, JCT 2011
mat4x3 SimplexCoords(vec3 p) 
{
    // skew to tetrahedral coordinates
    vec3 tetbase = floor(p + dot(p, vec3(1./3.)));
    vec3 base = tetbase - dot(tetbase, vec3(1./6.));
    vec3 tf = p - base;

    // One of six tetrahedra: 100, 010, 001, 011, 101, 110
    // since skew is along x=y=z axis, this works in Euclidean space too
    vec3 g = step(tf.yzx, tf.xyz), h = 1. - g.zxy;
    vec3 a1 = min(g, h) - 1./6., a2 = max(g, h) - 1./3.;

    // four corners in Euclidean space
    return mat4x3(base, base + a1, base + a2, base + 0.5);
}

// support function for Noise3
// simplex smoothing function
vec4 Smooth(mat4x3 f)
{
	const float scale = 1024. / 375.;       // scale factor to make noise -1..1
	vec4 d = vec4(dot(f[0], f[0]), dot(f[1], f[1]), dot(f[2], f[2]), dot(f[3], f[3]));
	vec4 s = clamp(2. * d, 0., 1.);
    
	return (1. * scale + s*(-3. * scale + s*(3. * scale - s*scale)));
}

// Simplex noise 3D input, float output
float Noise3(vec3 v)
{
    const uvec3 GMask = uvec3(0x8000, 0x4000, 0x2000);
    const vec3 GScale = 1. / vec3(0x4000, 0x2000, 0x1000);
    
	// corners of tetrahedron
	mat4x3 T = SimplexCoords(v), fv;
	vec4 grad;
    
    for(int i=0; i<4; ++i) {
        fv[i] = v - T[i];
        uvec3 rand = Hash3(uvec3(ivec3(floor(6. * T[i] + 0.5))));
        vec3 gvec = vec3(rand.xxx & GMask) * GScale - 1.;
        grad[i] = dot(gvec, fv[i]);
    }

	// blend gradients
	vec4 sv = Smooth(fv);
	return dot(sv, grad);
}
//----------------------------------------------------------------------


float noisee(vec2 input) {
    return Noise3(input.xyx);
}

float hash(vec2 input) {
    return Hash3( uvec3(floor(input.xyx)) ).x;
}

//
// FRACTAL BROWNIAN MOTION
//
vec2 fbm(vec2 input) {
    const int MAX_OCTAVES = 10; // just in case
    const float EPSILON = 0.001f;
    float lacunarity = 2.0;
    float gain = 0.7;
    float frequency = 4.0f;
    float amplitude = 0.01f;
    for (int i=0; i<MAX_OCTAVES; i++) {
        input += noisee(input*frequency) * amplitude;
        frequency *= lacunarity;
        amplitude *= gain;
        if (amplitude < EPSILON) {
            break;
        }
    }
    return input;
}


void main() {
    // define some features of the bricks
    float W = 0.18;  // brick width
    float H = 0.12;  // brick height
    float M = 0.02;  // mortar width
    float hM = M / 2.0; // half mortar width
    vec3 brick_color = vec3(0.5, 0.5, 0.5);
    vec3 mortar_color = vec3(1, 1, 1);
    vec3 col = mortar_color;
    vec2 cell_size = vec2(W+M, H+M); // size of cell containing a brick surrounded by mortar
    vec2 grid_pos = floor(texcoord / cell_size); // x,y index of brick
    vec2 cell_pos = mod(texcoord, cell_size); // coords inside of current cell
    cell_pos = mix(cell_pos, fbm(cell_pos + noisee(grid_pos)/32.), 
        smoothstep(0.0, 0.5, abs(fbm(cell_pos*2.).x)/2.)
    );
    // warp the brick/mortar boundary
    hM = hM + noisee(grid_pos*64. + cell_pos*32.)/256.;
    // color the bricks
    if (cell_pos.x > hM && cell_pos.x < W - hM &&
        cell_pos.y > hM && cell_pos.y < H - hM
    ) {
        // offset each brick's fbm calculations by a random amount
        col = mix(brick_color, vec3(0,0,0), noisee(cell_pos*32. + noisee(grid_pos)*16.) / 8.);
    }
    // color the mortar
    else {
        col = mortar_color;
    }
    
    // final color of current fragment
    fragColor = vec4(col, 1);
}
