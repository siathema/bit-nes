#include "renderer.h"
#include "lmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"

namespace Render
{

	char* Get_Source_From_File(const char* path)
	{
		char* result;
		FILE* fp = fopen(path, "r");
		if (fp == 0)
		{
			printf("Cannot open file!\n");
			return (char*)"";
		}

		u32 count = 0;
		while (fgetc(fp) != EOF) count++;
		fseek(fp, 0, SEEK_SET);
		assert(count > 0);

		result = (char*)malloc(sizeof(i8) * count + 1);
		assert(result);

		for (u32 i = 0; i < count + 1; i++)
		{
			result[i] = fgetc(fp);
		}
		result[count] = '\0';
		return result;
	}

	i32 Compile_Vertex_Shader(char* source)
	{
		u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// attach shader source and compile
		glShaderSource(vertexShader, 1, &source, 0);
		glCompileShader(vertexShader);

		i32 success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, 0, infoLog);
			printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", infoLog);
		}
		free(source);
		return vertexShader;
	}

	i32 Compile_Fragment_Shader(char* source)
	{
		u32 fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		// attach shader source and compile
		glShaderSource(fragShader, 1, &source, 0);
		glCompileShader(fragShader);

		i32 success;
		char infoLog[512];
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragShader, 512, 0, infoLog);
			printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s\n",
				   infoLog);
		}
		free(source);
		return fragShader;
	}

	i32 Create_Shader_Program(char* vs, char* fs)
	{
		i32 vertexProgram = Compile_Vertex_Shader(vs);
		i32 fragmentProgram = Compile_Fragment_Shader(fs);

		u32 shaderProgram;
		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexProgram);
		glAttachShader(shaderProgram, fragmentProgram);
		glLinkProgram(shaderProgram);

		glDeleteShader(vertexProgram);
		glDeleteShader(fragmentProgram);

		return shaderProgram;
	}

	Texture2D::Texture2D(u8* image, i32 width, i32 height)
		: Width(width), Height(height)
	{
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
					 GL_UNSIGNED_BYTE, image);

		free(image);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::Texture2D(i32 width, i32 height)
		: Width(width), Height(height)
	{
		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//u8* data = new u8[width*height*4];

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
					 GL_UNSIGNED_BYTE, 0);
		//delete[] data;
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::~Texture2D() {}

	void Texture2D::Bind() { glBindTexture(GL_TEXTURE_2D, TextureID); }

	Renderer::Renderer(vec2 view)
	{
		viewport = view;
		Load_Shaders();
		Init_Render_Data();
	}

//TODO(Aria): make this better.
#define RENDER_CHARACTER_WIDTH 8
#define RENDER_CHARACTER_HEIGHT 16
#define RENDER_CHARACTER_MAX_PER_LINE 1024
	void Renderer::Draw_Text_Rect(const char* str,
								  Camera &camera,
								  r32 x,
								  r32 y,
								  r32 width,
								  r32 height,
								  vec4 color)
	{
		u32 strLength = strlen(str);
		u32 numCols = (u32)width/(RENDER_CHARACTER_WIDTH);
		assert(numCols <= RENDER_CHARACTER_MAX_PER_LINE);
		u32 numLines;
		if(strLength < numCols)
		{
			numLines = 1;
			Render_String(str, camera, vec2(x,y), vec2(1.0f, 1.0f), 0.0f, color);
			return;
		}
		else
		{
			numLines = (u32)height/(RENDER_CHARACTER_HEIGHT+4);
		}

		char line[RENDER_CHARACTER_MAX_PER_LINE] = {};
		u32 charIndex = 0;

		for(u32 l=0; l<numLines; l++)
		{
			//strncpy(line, str+charIndex, numCols);
			u32 endIndex = 0;

			for(u32 c=0; c<numCols; c++)
			{
				if(str[charIndex] == ' ' &&
				   c == 0)
					charIndex++;
				if(str[charIndex+c] == 0)
				{
					line[c] = 0;
					break;
				}
				if(str[charIndex+c] == ' ')
					endIndex = c;
				line[c] = str[charIndex+c];
			}
			line[numCols] = 0;
			if(str[charIndex+numCols] != ' ' &&
			   strlen(line)==numCols)
			{
				line[endIndex] = 0;
				charIndex += endIndex+1;
			}
			else
			{
				line[charIndex+numCols] = 0;
				charIndex += numCols;
			}

			Render_String(line, camera, vec2(x,y-(RENDER_CHARACTER_HEIGHT*l)), vec2(1.0f, 1.0f), 0.0f, color);

			if(charIndex > strlen(str) || *line == 0)
				break;
		}
	}

	void Renderer::Render_String(const char *str,
								Camera &camera,
								vec2 pos,
								vec2 scale,
								r32 rot,
								vec4 color)
	{
		i32 len = strlen(str);
		for(i32 i=0; i<len; i++)
		{
			char current = str[i];
			//i32 padding = 6;
			i32 offset = 32;
			iRect texRect;
			texRect.x = (i32)((current-32)%16)*offset;
			texRect.y = (i32)((current-32)/16)*offset;
			texRect.w = 8;
			texRect.h = 13;

			Draw_Texture(fontTex,
						camera,
						texRect,
						vec2(pos.x + ((i*(8))), pos.y),
						vec2(scale.x,-scale.y),
						rot, 0.0f, color);
		}
	}


	void Renderer::Draw_Texture(Texture2D *texture,
							   Camera &camera,
							   iRect &textureRect,
							   vec2 position,
							   vec2 size,
							   r32 rotate,
							   r32 depth,
							   vec4 color,
							   ShaderType shader)
	{
		// Prepare texture coordinates
		if (shader != FILLRECTANGLE)
		{
			r32 texture_pixel_ratio_x = 1.0f / texture->GetWidth();
			r32 texture_pixel_ratio_y = 1.0f / texture->GetHeight();

			r32 texture_coord_offset_x = texture_pixel_ratio_x * textureRect.x;
			r32 texture_coord_offset_y = texture_pixel_ratio_y * textureRect.y;

			r32 texture_coord_offset_width =
				texture_pixel_ratio_x * textureRect.w;
			r32 texture_coord_offset_height =
				texture_pixel_ratio_y * textureRect.h;

			// top left
			TexCoords[0] = texture_coord_offset_x;
			TexCoords[1] = texture_coord_offset_y + texture_coord_offset_height;
			// bottom right
			TexCoords[2] = texture_coord_offset_x + texture_coord_offset_width;
			TexCoords[3] = texture_coord_offset_y;
			// bottom left
			TexCoords[4] = texture_coord_offset_x;
			TexCoords[5] = texture_coord_offset_y;

			// top left
			TexCoords[6] = texture_coord_offset_x;
			TexCoords[7] = texture_coord_offset_y + texture_coord_offset_height;
			// top right
			TexCoords[8] = texture_coord_offset_x + texture_coord_offset_width;
			TexCoords[9] = texture_coord_offset_y + texture_coord_offset_height;
			// bottom right
			TexCoords[10] = texture_coord_offset_x + texture_coord_offset_width;
			TexCoords[11] = texture_coord_offset_y;

			glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TexCoords), TexCoords);
		}
		// Prepare transformations
		glUseProgram(ShaderIDs[shader]);
		mat4 model;
		vec3 scale(textureRect.w * size.x, textureRect.h * size.y, 0.0f);
		model.make_transform(vec3(position.x, position.y, 0.0f),
							 scale,
							 quat(rotate, vec3(0.0f, 0.0f, 1.0f)));
		//model.translation(vec3(position.x, position.y, 0.0f));
		//model = translate(model, vec3(position, 0.0f));

		//model.translate(vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
		//model = translate(model, vec3(0.5f * size.x, 0.5f * size.y, 0.0f));

		//quat rot = quat(rotate, vec3(0.0f, 0.0f, 1.0f));
		//model.rotate(rot);
		//model = rotate(model, rotate, vec3(0.0f, 0.0f, 1.0f));

		//model.translate(vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
		//model = translate(model, vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

		//model.scale(vec3(size.x, size.y, 1.0f));

		u32 modelLoc = glGetUniformLocation(ShaderIDs[shader], "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (GLfloat*)&model._m);

		u32 viewLoc = glGetUniformLocation(ShaderIDs[shader], "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (GLfloat*)&camera.View._m);

		u32 projLoc = glGetUniformLocation(ShaderIDs[shader], "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, (GLfloat*)&camera.Projection._m);

		u32 colorLoc = glGetUniformLocation(ShaderIDs[shader], "spriteColor");
		glUniform4f(colorLoc, color.x, color.y, color.z, color.w);


		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture->TextureID);

		glBindVertexArray(QuadVAO);
		if (shader == DEBUGLINES)
		{
			glDrawArrays(GL_LINE_STRIP, 0, 6);
		}
		else
		{
			if(texture != 0) texture->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			//printf("GLERROR: %d\n", glGetError());
		}
		glBindVertexArray(0);
	}

	void Renderer::Init_Render_Data()
	{

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


		r32 vertices[] = {
			// Pos
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f };

		for (i32 i = 0; i < 12; i++)
		{
			TexCoords[i] = 0.0f;
		}

		glGenBuffers(2, VBOs);

		// vertex VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// tex coords VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords), TexCoords, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &QuadVAO);
		glBindVertexArray(QuadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(r32), (GLvoid *)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(r32), (GLvoid *)0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	void Renderer::Load_Shaders()
	{
		// SIMPLEBLIT
		char* vs = Get_Source_From_File("assets/shaders/SimpleBlit.vs");
		char* fs = Get_Source_From_File("assets/shaders/SimpleBlit.frag");
		u32 id = Create_Shader_Program(vs, fs);
		ShaderIDs[SIMPLEBLIT] = id;

		// DEBUGLINES
		vs = Get_Source_From_File("assets/shaders/DebugLines.vs");
		fs = Get_Source_From_File("assets/shaders/DebugLines.frag");
		id = Create_Shader_Program(vs, fs);
		ShaderIDs[DEBUGLINES] = id;

		// FILLRECTANGLE
		vs = Get_Source_From_File("assets/shaders/FillRectangle.vs");
		fs = Get_Source_From_File("assets/shaders/FillRectangle.frag");
		id = Create_Shader_Program(vs, fs);
		ShaderIDs[FILLRECTANGLE] = id;
	}
}
