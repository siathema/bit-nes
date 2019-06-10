#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <GL/glew.h>
#if WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include "bit_nes.h"
#include "b_6502.h"
#include "b_ppu.h"
#include "b_log.h"
#include "b_utils.h"

namespace BITNES
{
#define SCREEN_WIDTH 16 * 8
#define SCREEN_HEIGHT 16 * 8 //240

	struct GL_Data
	{
		u32 VBO;
		u32 VAO;
		u32 ShaderID;
		u32 TextureID;
	};

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

	Internal GL_Data Gen_GL_Data()
	{
		r32 quad[] = {
			// Position    // Texcoords
			-1.0f, 1.0f,   0.0f, 1.0f,
			1.0f, -1.0f,   1.0f, 0.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,

			-1.0f, 1.0f,  0.0f, 1.0f,
			1.0f, 1.0f,   1.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f
		};

		GL_Data result = {};

		glGenVertexArrays(1, &result.VAO);
		glBindVertexArray(result.VAO);

		glGenBuffers(1, &result.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, result.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(r32), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(r32), (void*)(2*sizeof(r32)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);


		char* vertSrc = Get_Source_From_File("./assets/bitnesVertex.glsl");
		char* fragSrc = Get_Source_From_File("./assets/bitnesFragment.glsl");
		result.ShaderID = Create_Shader_Program(vertSrc, fragSrc);

		return result;
	}

	Internal u32 Gen_Texture(i32 width, i32 height, u8* data)
	{
		u32 result = 0;

		glGenTextures(1, &result);
		glBindTexture(GL_TEXTURE_2D, result);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
					 GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);

		return result;
	}

	Internal SDL_Window* Init_Window(SDL_GLContext* context)
	{
		SDL_Window* window = 0;
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("Could not initialize SDL. ERROR: %s\n", SDL_GetError());
			return 0;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
							SDL_GL_CONTEXT_PROFILE_CORE);

		window = SDL_CreateWindow(
			"bit-nes",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		if (window == 0)
		{
			printf("Could not open window! ERROR: %s\n", SDL_GetError());
			return 0;
		}
		else
		{
			*context = SDL_GL_CreateContext(window);
			if (context == 0)
			{
				printf("Could not create opengl context! ERROR: %s\n",
					   SDL_GetError());
				return 0;
			}
			else
			{
				glewExperimental = GL_TRUE;
				if (glewInit() != GLEW_OK)
				{
					printf("Could not initialize GLEW\n");
				}
				if (SDL_GL_SetSwapInterval(0) < 0)
				{
					printf("Warning could not enable v-sync! SDL ERROR: %s\n",
						   SDL_GetError());
				}
			}
			glViewport(0,
					   0,
					   SCREEN_WIDTH,
					   SCREEN_HEIGHT);
		}

		return window;
	}

	Internal void Make_PatternTable_Texture(u8* PatternTables, u8* pixels)
	{
		u8* source = PatternTables;
		u8* dest = pixels;
		i32 pixelWidth = 16*8*4;
		i32 tableWidth = 16*2;
		i32 tileCountCol = 0;
		i32 tileCountRow = 0;

		for(i32 tableRow=0; tableRow<16; tableRow++)
		{
			tileCountRow++;
			for(i32 tableCol=0; tableCol<16; tableCol++)
			{
				dest = pixels + ((tableRow*(pixelWidth*8))+(tableCol*(8*4)));
				tileCountCol++;
                printf("Drawing Tile: Col: %d Row: %d\n", tableCol, tableRow);
				for(i32 tileRow=0; tileRow<8; tileRow++)
				{
                    //if(tableRow==15 && tableCol==15) __builtin_debugtrap();
                    assert(dest <= (pixels+(SCREEN_WIDTH*SCREEN_HEIGHT*4)));
					u8* plane0 = source;
					u8* plane1 = source + 8;
                    if(tableRow==16 && tableCol==16)
                        printf("byte $%02X\n", (*plane0 | *plane1));
					for(i32 tileCol=0; tileCol<8; tileCol++)
					{
						u8 colorPlane0 = ((*plane0 >> (7-tileCol)) & 0x01);
							u8 colorPlane1 = ((*plane1 >> (7-tileCol)) & 0x01);
							colorPlane1 += colorPlane1 == 0 ? 0 : 1;
							u8 color = colorPlane0 + colorPlane1;
							dest[0] = color == 1 ? 255 : 50;
							dest[1] = color == 2 ? 255 : 50;
							dest[2] = color == 3 ? 255 : 50;
							dest[3] = 0xff;
							dest += 4;
					}
					source++;
					dest += (pixelWidth - (8*4));
				}

                source += 8;
			}
		}
		printf("TileCount: Row%d Col:%d\n", tileCountRow, tileCountCol);
	}

	Internal int proc(const char* args, const char* filePath) {
		bool testing = args != 0;

		FILE* romFile = 0;
		if(args)
			romFile = fopen("./roms/nestest.nes", "rb");
		else
			romFile = fopen(filePath, "rb");

		if (romFile == NULL) {
			printf("Could not load file %s\n", filePath);
			return -1;
		}

		int count = 0;

		while (fgetc(romFile) != EOF) {
			count++;
		}

		u8 *romBuffer = (u8*)malloc(sizeof(u8)*count);
		fseek(romFile, 0, SEEK_SET);

		int temp = fgetc(romFile);
		int index = 0;

		while (temp != EOF) {
			romBuffer[index] = temp;
			index++;
			temp = fgetc(romFile);
		}

		Log("Read in rom file.\n");

		//NOTE(matthias): emulation begins here
		//NOTE(matthias): Mapper 000: 16KB or 128KB PRG rom non-switchable banks. If 16KB rom mirror 0x8000-bfff at 0xc000 - 0xffff.
		u8 k16PRGPages = romBuffer[4];
		u32 k8CHRPages = romBuffer[5];

		char debugMessage[64];

		sprintf(debugMessage, "%d 16k pages of program data\n", k16PRGPages);
		Log(debugMessage);

		sprintf(debugMessage, "%d 8k pages of character data\n", k8CHRPages);
		Log(debugMessage);

		int PRGROMIndex = 16;
		int CHRROMIndex = 0;

		int mapperNum = (romBuffer[7] & 0xf0);
		mapperNum |= (romBuffer[6] & 0xf0) >> 4;

		sprintf(debugMessage, "Mapper:%d\n", mapperNum);
		Log(debugMessage);

		if ((romBuffer[6] & 0x04) != 0) {
			printf("Trainer\n");
			PRGROMIndex += 512;
		}

        	CHRROMIndex = PRGROMIndex;
		CHRROMIndex += k16PRGPages * KILOBYTE(16);

		u8* PatternTables = (u8*)malloc(sizeof(u8)*(k8CHRPages*KILOBYTE(8)));
		memcpy(PatternTables, romBuffer + (CHRROMIndex), KILOBYTE(8));

		u8* pixels = (u8*)malloc(sizeof(u8)*(SCREEN_HEIGHT*SCREEN_WIDTH)*4);

		Make_PatternTable_Texture(PatternTables+0x1000, pixels);

		SDL_Window* window = 0;
		SDL_GLContext context;
		window = Init_Window(&context);
		GL_Data bitnesGLData = Gen_GL_Data();
		bitnesGLData.TextureID = Gen_Texture(SCREEN_WIDTH ,SCREEN_HEIGHT, pixels);


		if(window == 0)
		{
			Log("Could not setup SDL and OpenGL!\n");
			return -1;
		}

		MapperType mapper = testing ? M000_16K : M000_32K;
		nes* nes = init_nes(romBuffer + PRGROMIndex, mapper);

		SDL_Event event;
		r64 targetTime = 0.000000000601;
		bool running = true;
		while(running)
		{
			u64 startTime = clock();

			running = run_nes(nes);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(bitnesGLData.ShaderID);
			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D, bitnesGLData.TextureID);

			glBindVertexArray(bitnesGLData.VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			SDL_GL_SwapWindow(window);

			while(SDL_PollEvent(&event) != 0)
			{
				switch(event.type)
				{
				case SDL_QUIT:
				{
					running = false;
				}break;
				case SDL_KEYDOWN:
				{
					i32 keySym = event.key.keysym.sym;
					switch(keySym)
					{
					case SDLK_ESCAPE:
					{
						running = false;
					}break;
					case SDLK_F3:
					{
					}break;
					default:
					{

					}break;
					}
				}break;
				default:
				{

				}break;
				}
			}

			//NOTE(matthias): Timing stuff
			u64 endTime = clock();
			u64 elapsedTime = endTime - startTime;
			r64 elapsedTimed = elapsedTime / CLOCKS_PER_SEC;
			if (elapsedTime < targetTime) {
			   while (1)
			   {
				   elapsedTimed = (r64)(clock() - startTime)/ CLOCKS_PER_SEC;
				   if (elapsedTime >= targetTime) break;
			   }
			}
		}

		if(testing)
		{
			// TODO(matthias): see if this is cross-platform?
			system("./src/test/test.py");
		}
#if 1
		free(romBuffer);
		free(nes->memory);
		free(nes->cpu);
		free(nes->ppu);
		free(nes);
#endif

		LogERROR("killed Emulation!\n");
		return 0;
	}
}

int main(int argc, char** argv) {

	if (argc < 2 || argc > 2) {
		printf("Usage: %s -t(run tests) [Path to rom]\n", argv[0]);
		return -1;
	}
	char* args = 0;
	char* filePath = 0;

	if((argv[1][0] == '-' && argv[1][1] == 't'))
	{
		args = argv[1];
	}
	else
	{
		filePath = argv[1];
	}
	return BITNES::proc(args, filePath);
}
