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

// colors used in the procedural texture
vec3 black = vec3(0.0, 0.0, 0.0);
vec3 white = vec3(1.0, 1.0, 1.0);
vec3 grass_color = vec3(0.4, 0.5, 0.3) * 1.1;
vec3 brick_color = vec3(0.7, 0.7, 0.67) * 1.05;
vec3 brick_color_2 = vec3(0.7, 0.7, 0.71) * 1.05;
vec3 dirt_color = vec3(0.30, 0.20, 0.15);

// modes for which texture is drawn
#define PROCEDURAL 0
#define ORIGINAL 1
#define BLEND 2

// set which mode to use
#define MODE PROCEDURAL

// toggle lighting
#define DO_LIGHTING 1

void main() {
vec3 col = vec3(0.0);
// procedural texture
#if MODE == PROCEDURAL || MODE == BLEND
    // Slide and stretch uv to better match the original texture
    vec2 uv = (texcoord + vec2(-0.002, -0.002)) * 1.003;
    
    // Randomize uv position
    uv = mix(uv, uv + vec2(Noise3(vec3(position.xyz)*4.0) * 0.0025), 0.3);
    uv = texcoord;
    
    // Brick coords (pairs of squares)
    float bricksize = 0.25;
    vec2 bc = fract(uv / vec2(bricksize, bricksize/2.25));

    // Square coords
    vec2 sc = fract(bc * vec2(2.0, 1.0));
    sc.x = abs(sc.x - 0.5) * 0.8;
    sc.y = abs(sc.y - 0.5) * 0.8;
    float dist = abs(sc.x) + abs(sc.y);
    
    // Set initial color to grass color
    col = grass_color;
 
    // clip the tile shape in the square coordinates
    float in_tile = 0.0;
    if (dist > 0.2 && dist < 0.5) {
        col = brick_color;
        in_tile = 1.0;
    }
    else if (dist > 0.8) {
        col = brick_color;
        in_tile = 1.0;
    }

    // add the seams using the brick coordinates
    float seam_thickness = 0.01;
    float in_seam = 0.0;
    // top and bottom seams
    if (bc.y > 1.0 - seam_thickness || 
        bc.y < 0.0 + seam_thickness) {
        in_seam = 1.0;
        col = grass_color;
    }
    // middle horizontal seam
    if (bc.y > 0.5 - seam_thickness && 
        bc.y < 0.5 + seam_thickness) {
        in_seam = 1.0;
        col = grass_color;
    }
    // vertical seams
    if (bc.x > 1.0 - seam_thickness || 
        bc.x < 0.0 + seam_thickness) {
        in_seam = 1.0;
        col = grass_color;
    }
    
    // Apply noise for grass
    if (col.x == grass_color.x) {
        col += Noise3(vec3(position.xyz)*4.0) * 0.1 * uv.x;
        col += Noise3(vec3(position.xyz)*5.0) * 0.075;
        col += Noise3(vec3(position.xyz)*6.0) * 0.05;
        
        // add in the dirt
        //col += mix(black, dirt_color, Rand3(vec3(position.xyz*2.0)) *0.5);
        //col += mix(black, dirt_color, Rand3(vec3(position.xyz)) *0.4);
        col = mix(col, dirt_color, (Noise3(vec3(position.xyz*0.05)) *0.6));
        col = mix(col, dirt_color, (Noise3(vec3(position.xyz*0.25)) *0.4));
        col = mix(col, dirt_color, (Noise3(vec3(position.xyz*0.5)) *0.1));
        col += dirt_color * (Noise3(vec3(position.xyz)) *0.5);
        col += dirt_color * (Noise3(vec3(position.xyz*2.0)) *0.4);
        col += dirt_color * (Noise3(vec3(position.xyz*4.0)) *0.3);
    }
    // Apply noise for tiles
    else {
        // Add bright splotches to the tiles
        col = mix(col, brick_color_2, (Noise3(vec3(position.xyz*0.1 + 0.2)) *0.35));
        col = mix(col, brick_color_2, (Noise3(vec3(position.xyz*0.2 + 0.5)) *0.7));
        col = mix(col, brick_color_2, (Noise3(vec3(position.xyz*0.05 + 0.6)) *0.85));
        col = mix(col, brick_color_2, (Noise3(vec3(position.xyz*0.05 + 0.2)) *0.65));
        col += Noise3(vec3(position.xyz)*1.2) * 0.04;
        col -= Noise3(vec3(position.xyz)*8.0 + 0.25) * 0.1;
        //col += Noise3(vec3(position.xyz)*16.0) * 0.01;
        col += Noise3(vec3(position.xyz)*4.0 + 0.5) * 0.01;
    }
    
    // Darken the seams between tiles
    if (in_seam > 0.0 && 
        (
        ( // top and bottom seams
        (
            bc.y > 1.0 - seam_thickness || 
            bc.y < 0.0 + seam_thickness) &&
            dist < 0.5
        ) ||
        ( // middle seam joints and vertical seam
            bc.y < 1.0 - seam_thickness || 
            bc.y > 0.0 + seam_thickness) &&
            dist < 0.5 &&
            (sc.x > 0.19)
        )
        ) {
        col = mix(col, black, 0.2);
    }
    // Darken the edges around the tiles
    if (dist > 0.2 - seam_thickness*2.0 && dist < 0.2)
        col = mix(col, black, 0.4);
    else if (dist > 0.8 && dist < 0.8) // not an edge
        ;
    if (dist > 0.5 && dist < 0.5 + seam_thickness*2.0)
        col = mix(col, black, 0.4);
#endif

    // Bomb the texture with weeds!
    float r = 0.2;
    vec2 tc = 10.f * texcoord;
    vec2 dx = dFdx(tc/r), dy = dFdy(tc/r);
    vec2 cell;
    for (cell.x = floor(tc.x-r); cell.x <= floor(tc.x+r); ++cell.x) {
        for (cell.y = floor(tc.y-r); cell.y <= floor(tc.y+r); ++cell.y) {
            vec2 center = cell + Rand3(vec3(cell,0)).xy;
            vec2 diff = tc - center;
            float d = 0.002/length(diff+Noise3(position.xyz)*0.1);
            if (max(abs(diff.x), abs(diff.y)) < r && in_tile < 1.0) {
                col += grass_color*(8.0)*d;
            }
        }
    }

// the original texture map
#if MODE == ORIGINAL
    // solid color or from texture
    if (textureSize(ColorTexture,0) != ivec2(1,1))
        texture(ColorTexture, texcoord).rgb;
#endif

// the blend between procedural and original texture
#if MODE == BLEND
    // solid color or from texture
    if (textureSize(ColorTexture,0) != ivec2(1,1))
        col += mix(col, texture(ColorTexture, texcoord).rgb, 0.7);
#endif
  
#if DO_LIGHTING
    // apply ambient and diffuse lighting
    vec3 N = normalize(normal);             // surface normal
    vec3 L = normalize(LightDir);           // light direction
    col = col * (max(0., dot(N,L)) + 0.5);
#endif    
    // final color
    fragColor = vec4(col, 1);
}
