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

