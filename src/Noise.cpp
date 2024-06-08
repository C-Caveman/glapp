// implementation for simplex Perlin noise function
// C++ adaptation of code at https://www.shadertoy.com/view/wds3zr

#include "Noise.hpp"

using namespace glm;

// 3D to 3D hash function
// see Jarzynski and Olano, Hash Functions for GPU Rendering
// https://jcgt.org/published/0009/03/02/
uvec3 Noise::Hash3(uvec3 v) 
{
	v = v * 1664525u + 1013904223u;      
    v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
    v.x += v.y*v.z; v.y += v.z*v.x; v.z += v.x*v.y;
    return v >> 16u;
}

// 3D simplex weights (& corners)
// based on McEwan et al., Efficient computation of noise in GLSL, JCT 2011
static mat4x3 simplexCoords(vec3 p) 
{
    // skew to tetrahedral coordinates
    vec3 tetbase = floor(p + dot(p, vec3(1.f/3.f)));
    vec3 base = tetbase - dot(tetbase, vec3(1.f/6.f));
    vec3 tf = p - base;

    // One of six tetrahedra: 100, 010, 001, 011, 101, 110
    // since skew is along x=y=z axis, this works in Euclidean space too
    vec3 g = step(vec3(tf.y, tf.z, tf.x), tf);
    vec3 h = 1.f - vec3(g.z, g.x, g.y);
    vec3 a1 = min(g, h) - 1.f/6.f, a2 = max(g, h) - 1.f/3.f;

    // four corners in Euclidean space
    return mat4x3(base, base + a1, base + a2, base + 0.5f);
}

// simplex smoothing function
vec4 smooth(mat4x3 f)
{
	const float scale = 1024.f / 375.f;       // scale factor to make noise -1..1
	vec4 d = vec4(dot(f[0], f[0]), dot(f[1], f[1]), dot(f[2], f[2]), dot(f[3], f[3]));
	vec4 s = clamp(2.f * d, 0.f, 1.f);
    
	return (1.f * scale + s*(-3.f * scale + s*(3.f * scale - s*scale)));
}

// Simplex noise with 3D input and 1D output
float Noise::Noise1(vec3 v)
{
    const uvec3 GMask = uvec3(8,4,2);
    const vec3 GScale = 1.f / vec3(4,2,1);
    
	// corners of tetrahedron
	mat4x3 T = simplexCoords(v), fv;
	vec4 grad;
    
	// processing of tetrahedral vertices
    for(int i=0; i<4; ++i) {
        fv[i] = v - T[i];
        uvec3 rand = Noise::Hash3(uvec3(6.f * T[i] + 0.5f));
        vec3 gvec = vec3(uvec3(rand.x) & GMask) * GScale - 1.f;
        grad[i] = dot(gvec, fv[i]);
    }

	// blend gradients
	return dot(smooth(fv), grad);
}


// Simplex noise with 3D input and 3D output
vec3 Noise::Noise3(vec3 v)
{
    const uvec3 GMask = uvec3(8,4,2);
    const vec3 GScale = 1.f / vec3(4,2,1);
    
	// corners of tetrahedron
	mat4x3 T = simplexCoords(v), fv;
	mat3x4 grad;

	// processing of tetrahedral vertices
	for (int i=0; i<4; ++i){
		fv[i] = v - T[i];
		uvec3 rand = Noise::Hash3(ivec3(floor(6.f * T[i] + 0.5f)));
		for (int j=0; j<3; ++j) {
			vec3 gvec = vec3(uvec3(rand[j]) & GMask) * GScale - 1.f;
			grad[j][i] = dot(gvec, fv[i]);
		}
	}

	// blend gradients
	return smooth(fv) * grad;
}
