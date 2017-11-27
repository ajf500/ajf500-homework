//Aaron Fang, ajf500
//Used project template/class slides code
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
using namespace std;


SDL_Window* displayWindow;

class SheetSprite {
public:
	SheetSprite() {}
	SheetSprite(unsigned int texID, float u, float v, float width, float height, float size) : textureID(texID), u(u), v(v), width(width), height(height), size(size) {}

	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;

	void SheetSprite::Draw(ShaderProgram* program) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		GLfloat texCoords[] = {
			u, v + height,
			u + width, v,
			u, v,
			u + width, v,
			u, v + height,
			u + width, v + height
		};
		float aspect = width / height;
		float vertices[] = {
			-0.5f * size * aspect, -0.5f * size,
			0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, 0.5f * size,
			0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, -0.5f * size ,
			0.5f * size * aspect, -0.5f * size };

		glUseProgram(program->programID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);
		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
};

class Entity {
public:
	Entity() {}
	Entity(float x, float y, float width, float height, SheetSprite sprite, float xvelocity, float yvelocity, float xacceleration, float yacceleration, bool isStatic, char entityType) : x(x), y(y), width(width), height(height), sprite(sprite), xvelocity(xvelocity), xacceleration(xacceleration), yvelocity(yvelocity), yacceleration(yacceleration), isStatic(isStatic), entityType(entityType) {}
	bool CollidesWith(const Entity &entity) {
		if (entity.entityType == 'g') {
			if (y + height / 2 >= entity.y - entity.height / 2 && y - height / 2 <= entity.y + entity.height / 2) {
				return true;
			}
			else {
				return false;
			}
		}
		else if (entity.entityType == 'i') {
			if (x + width / 2 >= entity.x - entity.width / 2 && x - width / 2 <= entity.x + width / 2 && y + height / 2 >= entity.y - entity.height / 2 && y - height / 2 <= entity.y + entity.height / 2) {
				return true;
			}
			else {
				return false;
			}
		}
	};
	SheetSprite sprite;
	float x;
	float y;
	float height;
	float width;
	float xvelocity;
	float xacceleration;
	float yvelocity;
	float yacceleration;
	bool isStatic;
	char entityType;
	bool collidedGround;
	bool collidedJumpy;
};

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}
	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);
	return retTexture;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	SDL_Event event;
	bool done = false;

	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"arne_sprites.png");
	SheetSprite playerSprite = SheetSprite(spriteSheetTexture, 0.0f / 256.0f, 78.0f / 125.0f, 16.5f / 256.0f, 17.0f / 128.0f, 0.8f);
	SheetSprite groundSprite = SheetSprite(spriteSheetTexture, 145.0f / 256.0f, 80.0f / 125.0f, 59.0f / 256.0f, 48.0f / 128.0f, 1.0f);
	SheetSprite jumpySprite = SheetSprite(spriteSheetTexture, 240.0f / 256.0f, 8.0f / 125.0f, 16.0f / 256.0f, 24.0f / 128.0f, 0.8f);
	Entity player(0, 0, 0.2f, 0.2f, playerSprite, 4.0f, 0.0f, 0.0f, -0.098f, false, 'p');
	vector<Entity> ground;
	for (int i = 0; i < 50; i++) {
		ground.push_back(Entity(-3.55f + 0.5f*i, -2.0f, 1.5f, 1.5f, groundSprite, 0, 0, 0, 0, true, 'g'));
	}
	Entity jumpy(6.0f, -1.2f, 1.0f, 1.0f, jumpySprite, 0.0f, 0.0f, 0.0f, 0.0f, true, 'i');
	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	Matrix playerMatrix;
	float lastFrameTicks = 0.0f;
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Chunk *someSound;
	someSound = Mix_LoadWAV("interaction.wav");
	Mix_Music *music;
	music = Mix_LoadMUS("theme.mp3");
	Mix_PlayMusic(music, -1);
	while (!done) {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		player.collidedGround = false;
		for (int i = 0; i < ground.size(); i++) {
			if (player.CollidesWith(ground[i]) == true) {
				player.collidedGround = true;
				break;
			}
		}
		if (player.collidedGround == false) {
			player.yvelocity += player.yacceleration;
			player.y += player.yvelocity * elapsed;
		}
		if (keys[SDL_SCANCODE_LEFT]) {
			if (player.x - player.width > -3.55f) {
				player.x -= player.xvelocity * elapsed;
			}
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			if (player.x + player.width < 7.0f) {
				player.x += player.xvelocity * elapsed;
			}
		}
		else if (keys[SDL_SCANCODE_UP]) {
			if (player.collidedGround == true) {
				player.yvelocity = 4.0f;
				player.y += player.yvelocity * elapsed;
			}
		}
		player.collidedJumpy = player.CollidesWith(jumpy);
		if (player.collidedJumpy == true) {
			Mix_PlayChannel(-1, someSound, 0);
			player.yvelocity = 7;
			player.y += player.yvelocity * elapsed;
		}
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program.programID);

		program.SetModelviewMatrix(playerMatrix);
		playerMatrix.Identity();
		playerMatrix.Translate(0.0f, 0.0f, 0.0f);
		program.SetProjectionMatrix(projectionMatrix);
		player.sprite.Draw(&program);
		for (int i = 0; i < ground.size(); i++) {
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-7.0f + 0.5f*i - player.x, -2.0f - player.y, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			ground[i].sprite.Draw(&program);
		}
		modelviewMatrix.Identity();
		modelviewMatrix.Translate(jumpy.x - player.x, jumpy.y - player.y, 0.0f);
		program.SetModelviewMatrix(modelviewMatrix);
		jumpy.sprite.Draw(&program);

		SDL_GL_SwapWindow(displayWindow);
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

	}
	Mix_FreeChunk(someSound);
	Mix_FreeMusic(music);
	SDL_Quit();
	return 0;
}
