#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "Matrix.h"
#include "ShaderProgram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
using namespace std;


void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		int spriteIndex = (int)text[i];
		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 8);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

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
	Entity(float x, float y, float velocity, float height, float width, SheetSprite sprite) : x(x), y(y), velocity(velocity), height(height), width(width), sprite(sprite) {}
	float x;
	float y;
	float velocity;
	float height;
	float width;
	SheetSprite sprite;
};

class GameState {
public:
	Entity Player;
	vector<Entity> enemies;
	vector<Entity> bullets;
	Matrix playerMatrix;
	int bulletIndex;
	int maxBullets;
	float enemyDirection;
	Matrix modelviewMatrix;
	Matrix projectionMatrix;
	int mode;
	int enemyBulletIndex;
	vector<Entity> enemyBullets;
};

//enum GameMode { TITLE_SCREEN, GAME_LEVEL };
//GameMode mode;
GameState state;
SDL_Window* displayWindow;

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
void RenderMainMenu(ShaderProgram& program) {
	
}

void UpdateMainMenu() {

}

void RenderGame(GameState &state) {
	GLuint spriteSheetTexture = LoadTexture("sheet.png");
	SheetSprite playerSprite = SheetSprite(spriteSheetTexture, 325.0f / 1024.0f, 0.0f / 1024.0f, 95.0f / 1024.0f, 75.0f / 1024.0f, 0.3f);
	Entity Player(0.0f, -1.8f, 2.0f, 37.5f / 1024.0f, 0.2f, playerSprite);
	state.Player = Player;
	SheetSprite enemySprite = SheetSprite(spriteSheetTexture, 425.0f / 1024.0f, 730.0f / 1024.0f, 90.0f / 1024.0f, 80.0f / 1024.0f, 0.3f);
	vector<Entity> enemies;
	for (int i = 0; i < 55; i++) {
		float xposition = -3.3f + (i % 11 + 1)*0.5f;
		float yposition = 1.8f - 0.5*(i / 11);
		enemies.push_back(Entity(xposition, yposition, 0.5f, 0.25f, 0.25f, enemySprite));
	}
	state.enemies = enemies;
	vector<Entity> bullets;
	SheetSprite bulletSprite = SheetSprite(spriteSheetTexture, 851.0f / 1024.0f, 684.0f / 1024.0f, 7.0f / 1024.0f, 53.0f / 1024.0f, 0.3f);
	int maxBullets = 2;
	for (int i = 0; i < maxBullets; i++) {
		bullets.push_back(Entity(0.0f, 10.0f, 3.0f, 0.25f, 0.1f, bulletSprite));
	}
	vector<Entity> enemyBullets;
	SheetSprite enemyBulletSprite = SheetSprite(spriteSheetTexture, 856.0f / 1024.0f, 174.0f / 1024.0f, 6.0f / 1024.0f, 56.0f / 1024.0f, 0.3f);
	for (int i = 0; i < 4; i++) {
		enemyBullets.push_back(Entity(0.0f, -10.0f, 3.0f, 0.25f, 0.1f, enemyBulletSprite));
	}
	state.enemyBullets = enemyBullets;
	state.bullets = bullets;
	state.maxBullets = maxBullets;
	state.bulletIndex = 0;
	state.enemyDirection = 1.0;
	state.enemyBulletIndex = 0;
}

void UpdateGame(GameState &state, ShaderProgram& program, float elapsed) {
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT]) {
		if (state.Player.x - state.Player.width > -3.55f) {
			state.Player.x -= state.Player.velocity * elapsed;
		}
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		if (state.Player.x + state.Player.width < 3.55f) {
			state.Player.x += state.Player.velocity * elapsed;
		}
	}
	else if (keys[SDL_SCANCODE_SPACE]) {
		if (state.bullets[state.bulletIndex].y > 2.0) {
			state.bullets[state.bulletIndex].x = state.Player.x;
			state.bullets[state.bulletIndex].y = state.Player.y;
			state.bulletIndex++;
			if (state.bulletIndex > state.maxBullets - 1) {
				state.bulletIndex = 0;
			}
		}
	}
	for (size_t i = 0; i < state.bullets.size(); i++) {
		state.bullets[i].y += state.bullets[i].velocity * elapsed;
	}
	for (size_t i = 0; i < state.enemies.size(); i++) {
		state.enemies[i].x += state.enemyDirection * state.enemies[i].velocity * elapsed;
		int chance = rand() % state.enemies.size();
		if (state.enemies[i].x > 3.55 || state.enemies[i].x < -3.55) {
			state.enemyDirection = -(state.enemyDirection);
			for (size_t i = 0; i < state.enemies.size(); i++) {
				state.enemies[i].y -= state.enemies[i].velocity * elapsed;
			}
		}
		if (state.enemies[i].y < -2.0) {
			state.mode = 3;
		}
		if (state.enemyBullets[state.enemyBulletIndex].y < -2.0 && chance < 3) {
			state.enemyBullets[state.enemyBulletIndex].x = state.enemies[i].x;
			state.enemyBullets[state.enemyBulletIndex].y = state.enemies[i].y;
			state.enemyBulletIndex++;
			if (state.enemyBulletIndex > 3) {
				state.enemyBulletIndex = 0;
			}
		}
		state.enemies[i].velocity += 0.0005;
	}
	for (size_t i = 0; i < state.bullets.size(); i++) {
		for (size_t j = 0; j < state.enemies.size(); j++) {
			if (state.bullets[i].x + state.bullets[i].width <= state.enemies[j].x + state.enemies[j].width && state.bullets[i].x - state.bullets[i].width >= state.enemies[j].x - state.enemies[j].width && state.bullets[i].y + state.bullets[i].height >= state.enemies[j].y - state.enemies[j].height && state.bullets[i].y - state.bullets[i].height <= state.enemies[j].y + state.enemies[j].height) {
				state.bullets[i].y = 10.0f;
				swap(state.enemies[j], state.enemies[state.enemies.size() - 1]);
				state.enemies.pop_back();
			}
		}
	}
	for (size_t i = 0; i < state.enemyBullets.size(); i++) {
		if (state.enemyBullets[i].x + state.enemyBullets[i].width <= state.Player.x + state.Player.width && state.enemyBullets[i].x - state.enemyBullets[i].width >= state.Player.x - state.Player.width && state.enemyBullets[i].y + state.enemyBullets[i].height >= state.Player.y - state.Player.height && state.enemyBullets[i].y - state.enemyBullets[i].height <= state.Player.y + state.Player.height) {
			state.mode = 3;
		}
	}
	for (size_t i = 0; i < state.enemyBullets.size(); i++) {
		state.enemyBullets[i].y -= state.enemyBullets[i].velocity * elapsed;
	}
	glUseProgram(program.programID);
	program.SetModelviewMatrix(state.modelviewMatrix);
	//Draw Player
	state.playerMatrix.Identity();
	state.playerMatrix.Translate(state.Player.x, state.Player.y, 0.0f);
	program.SetModelviewMatrix(state.playerMatrix);
	state.Player.sprite.Draw(&program);
	//Draw Enemies
	for (size_t i = 0; i < state.enemies.size(); i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(state.enemies[i].x, state.enemies[i].y, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		state.enemies[i].sprite.Draw(&program);
	}
	for (size_t i = 0; i < state.bullets.size(); i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(state.bullets[i].x, state.bullets[i].y, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		state.bullets[i].sprite.Draw(&program);
	}
	for (size_t i = 0; i < state.enemyBullets.size(); i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(state.enemyBullets[i].x, state.enemyBullets[i].y, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		state.enemyBullets[i].sprite.Draw(&program);
	}
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
	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	state.projectionMatrix = projectionMatrix;
	glUseProgram(program.programID);
	float lastFrameTicks = 0.0f;
	GLuint fontSheet = LoadTexture(RESOURCE_FOLDER"font1.png");
	RenderGame(state);
	state.mode = 0;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		const Uint8* keys = SDL_GetKeyboardState(NULL);
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		glClear(GL_COLOR_BUFFER_BIT);
		//switch (mode) {
		//case TITLE_SCREEN:
		if (state.mode == 0) {
			glUseProgram(program.programID);
			program.SetProjectionMatrix(projectionMatrix);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-1.55f, 1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "Space Invaders", 0.25f, 0.00000f);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-1.8f, -1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "Press Enter to Start", 0.25f, 0.00000f);
			if (keys[SDL_SCANCODE_RETURN]) {
				glClear(GL_COLOR_BUFFER_BIT);
				state.mode = 1;
			}
			//break;
		}
		else if (state.mode == 1) {
			//case GAME_LEVEL:
			UpdateGame(state, program, elapsed);
			//break;
			if (state.enemies.size() == 0) {
				glClear(GL_COLOR_BUFFER_BIT);
				state.mode = 2;
			}
		}
		else if (state.mode == 2) {
			glUseProgram(program.programID);
			program.SetProjectionMatrix(projectionMatrix);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-1.55f, 1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "You Win!", 0.25f, 0.00000f);
		}
		else if (state.mode == 3) {
			glUseProgram(program.programID);
			program.SetProjectionMatrix(projectionMatrix);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-1.55f, 1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "Game Over!", 0.25f, 0.00000f);
		}
		SDL_GL_SwapWindow(displayWindow);
	}
	//}
	SDL_Quit();
	return 0;
}
