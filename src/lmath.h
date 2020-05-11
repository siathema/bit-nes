#pragma once
#include "bit_nes.h"

struct vec2 {
	r32 x, y;
	vec2() : x(0), y(0) {}
	vec2(r32 x, r32 y) : x(x), y(y) {}
};

struct vec3 {
	r32 x, y, z;
	vec3() : x(0), y(0), z(0) {}
	vec3(r32 x, r32 y, r32 z) : x(x), y(y), z(z) {}
};

struct vec4 {
	r32 x, y, z, w;
	vec4() : x(0), y(0), z(0), w(0) {}
	vec4(r32 x, r32 y, r32 z, r32 w) : x(x), y(y), z(z), w(w) {}
};

struct mat3 {

};

struct mat4 {
	union {
		r32 _m[16];
		r32 m[4][4];
	};

	void orthographic(const r32 width, const r32 height, const r32 near, const r32 far) {
		m[0][0] = 2.0f / width;
		m[1][1] = 2.0f / height
		r32 inverse_depth = 1.0f / (near -far);
		m[2][2] = inverse_depth;
		m[3][2] = near * inverse_depth;
		m[3][3] = 1.0f;
	}
};

struct iRect {
	i32 x, y, w, h;
};
