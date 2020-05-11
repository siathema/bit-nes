#pragma once
#include "bit_nes.h"
#include "lmath.h"

namespace Render
{
	struct Texture2D
	{
		u32 TextureID;
		i32 Width, Height;

		Texture2D(u8* image, i32 width, i32 height);
		Texture2D(i32 width, i32 height);
		~Texture2D();

		void Bind();

		i32 GetWidth() { return Width; }
		i32 GetHeight() { return Height; }
	};

	enum ShaderType{
		SIMPLEBLIT = 0,
		DEBUGLINES,
		FILLRECTANGLE,
		SHADERCOUNT
	};

	struct Renderer
	{
		struct Camera {
			vec3 pos;
			vec3 scale;
			r32 rot;
			mat4 View, Projection;
		}camera;
		u32 ShaderIDs[4];
		u32 QuadVAO;
		u32 VBOs[2];
		r32 TexCoords[12];
		vec2 viewport;
		Texture2D* fontTex;

		Renderer(vec2 view);

		void Draw_Text_Rect(const char* str,
							Camera &camera,
							r32 x,
							r32 y,
							r32 width,
							r32 height,
							vec4 color);
		void Render_String(const char* str,
						  Camera &camera,
						  vec2 pos,
						  vec2 scale,
						  r32 rot,
						  vec4 color);
		void Draw_Texture(Texture2D *texture,
						 Camera &camera,
						 iRect &textureRect,
						 vec2 position,
						 vec2 size,
						 r32 rotate,
						 r32 depth,
						 vec4 color,
						 ShaderType shader=SIMPLEBLIT);
		void Init_Render_Data();
		void Load_Shaders();
	};
}
