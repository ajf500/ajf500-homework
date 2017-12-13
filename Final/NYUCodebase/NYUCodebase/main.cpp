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
std::vector<float> vertexData;
std::vector<float> texCoordData;

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
	Entity(float x, float y, float width, float height, vector<SheetSprite> sprite, float xvelocity, float yvelocity, float xacceleration, float yacceleration, bool isStatic, char entityType) : x(x), y(y), width(width), height(height), sprite(sprite), xvelocity(xvelocity), xacceleration(xacceleration), yvelocity(yvelocity), yacceleration(yacceleration), isStatic(isStatic), entityType(entityType) {}
	void CollidesWithY(const Entity &entity) {	
		if (entity.entityType == 'b' && entity.wounded == true) {
			if (y - height / 2 < entity.y && y + height / 2 > entity.y && x - width / 2 < entity.x + entity.width / 2 && x + width / 2 > entity.x - entity.width / 2) {
				if (y > entity.y) {
					yvelocity = 4.0;
					damagedEnemy = true;
				}
			}
		}
		else if (y - height / 2 < entity.y + entity.height / 2 && y + height / 2 > entity.y - entity.height / 2 && x - width / 2 < entity.x + entity.width / 2 && x + width / 2 > entity.x - entity.width / 2) {
			if (entity.entityType == 'g'){
				if (y > entity.y) {
					collideDown = true;
					if (y - height / 2 > entity.y - 0.01) {
						y = (entity.y + entity.height / 2 + height / 2 + 0.001);
					}
				}
				else if (y < entity.y) {
					collideUp = true;
					y = (entity.y - entity.height / 2 - height / 2 - 0.01);
				}
			}	
			else if (entity.entityType == 'f') {
				collideFlag = true;
			}
			else if (entity.entityType == 'h') {
				collideHazard = true;
			}
			else if (entity.entityType == 'e') {
				collideEnemy = true;
			}
			else if (entity.entityType == 'b' && entity.recovery == false) {
				collideSlug = true;
			}
				
		}
	};
	void CollidesWithX(const Entity &entity) {
		if (y - height / 2 < entity.y + entity.height / 2 && y + height / 2 > entity.y - entity.height / 2 && x - width / 2 < entity.x + entity.width / 2 && x + width / 2 > entity.x - entity.width / 2) {
			if (entity.entityType == 'g') {
				if (entity.y + entity.height / 2 > y || y < entity.y - entity.height / 2) {
					if (x > entity.x + entity.width / 2) {
						collideRight = true;
						x = (entity.x + entity.width / 2 + width / 2 + 0.01);
					}
					else if (x < entity.x - entity.width / 2) {
						collideLeft = true;
						x = (entity.x - entity.width / 2 - width / 2 - 0.01);
					}
				}
			}		
		}
	};
	void CollidesWithS(const Entity &entity) {
		if (y - height / 2 < entity.y + entity.height / 2 && y + height / 2 > entity.y - entity.height / 2 && x - width / 2 < entity.x + entity.width / 2 && x + width / 2 > entity.x - entity.width / 2) {
			if (entity.entityType == 'g') {
				if (entity.y + entity.height / 2 > y || y < entity.y - entity.height / 2) {
					if (x > entity.x + entity.width / 2) {
						collideRight = true;
					}
					else if (x < entity.x - entity.width / 2) {
						collideLeft = true;
					}
				}
				if (y > entity.y) {
					collideDown = true;
				}
			}
		}
	};
	vector<SheetSprite> sprite;
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
	bool collideDown;
	bool collideUp;
	bool collideRight;
	bool collideLeft;
	bool collideFlag;
	bool collideHazard;
	bool collideEnemy;
	bool collideSlug;
	float leftBound;
	float rightBound;
	int aiDirection;
	bool dormant;
	int aniFrame = 0;
	float animation;
	float cooldown;
	bool agressive;
	int attack;
	float targetx;
	bool wounded = false;
	bool recovery = false;
	bool damagedEnemy;
	int health;
};

class GameState {
public:
	Entity player;
	int lifeTotal = 3;
	float playerYoffset = -1.0f;
	vector<Entity> platforms;
	vector<Entity> mobPlatforms;
	vector<Entity> pokers;
	vector<Entity> ghostPlatforms;
	int mode = 1;
	Matrix projectionMatrix;
	Matrix modelviewMatrix;
	Matrix playerMatrix;
	int sprite_index = 0;
	GLuint heartTexture;
	int direction = 1;
	Entity flag;
	int level;
	Entity block;
	Entity slug;
	GLuint fontSheet;
	bool stop = false;
	float shake = 0.23;
	float shaketime = 0.0;
	bool falling = false;
	bool musicStart = false;
	Mix_Music *levelMusic;
	Mix_Music *bossMusic;
	Mix_Music *victoryMusic;
	Mix_Music *overMusic;
	Mix_Chunk *fall;
	Mix_Chunk *bounce;
	Mix_Chunk *damage;

};
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

void RenderGame(GameState &state) {
	GLuint playerSheetTexture = LoadTexture(RESOURCE_FOLDER"playerSprites.png");
	GLuint platformSheetTexture = LoadTexture(RESOURCE_FOLDER"platforms.png");
	GLuint heartTexture = LoadTexture(RESOURCE_FOLDER"heart.png");
	//Add all necessary player sprites into playerAnimations
	//0:Idle, 1-2: Walking Left, 3-4: Walking Right
	vector<SheetSprite> playerAnimations;
	SheetSprite playerSprite = SheetSprite(playerSheetTexture, 97.0f / 192.0f, 64.47f / 128.0f, 13.0f / 192.0f, 13.1f / 128.0f, 0.5f);
	playerAnimations.push_back(playerSprite);
	playerSprite = SheetSprite(playerSheetTexture, 97.0f / 192.0f, 80.47f / 128.0f, 13.0f / 192.0f, 13.1f / 128.0f, 0.5f);
	playerAnimations.push_back(playerSprite);
	playerSprite = SheetSprite(playerSheetTexture, 113.0f / 192.0f, 80.47f / 128.0f, 13.0f / 192.0f, 13.1f / 128.0f, 0.5f);
	playerAnimations.push_back(playerSprite);
	playerSprite = SheetSprite(playerSheetTexture, 97.0f / 192.0f, 96.47f / 128.0f, 13.0f / 192.0f, 13.1f / 128.0f, 0.5f);
	playerAnimations.push_back(playerSprite);
	playerSprite = SheetSprite(playerSheetTexture, 113.0f / 192.0f, 96.47f / 128.0f, 13.0f / 192.0f, 13.1f / 128.0f, 0.5f);
	playerAnimations.push_back(playerSprite);

	
	//Add Flag
	GLuint flagTexture = LoadTexture(RESOURCE_FOLDER"flagBlue.png");
	SheetSprite flagSprite = SheetSprite(flagTexture, 0.0f / 70.0f, 0.0f / 70.0f, 70.0f / 70.0f, 70.0f / 70.0f, 0.5f);
	vector<SheetSprite> flags;
	flags.push_back(flagSprite);
	Entity flag(0.0, 0.0, 0.2f, 0.5f, flags, 0.0f, 0.0f, 0.0f, 0.0f, false, 'f');

	//Add Platforms
	vector<Entity> platforms;
	vector<SheetSprite> platformAnimations;
	vector<Entity> mobPlatforms;
	SheetSprite platformSprite = SheetSprite(platformSheetTexture, 208.0f / 384.0f, 192.0f / 256.0f, 16.0f / 384.0f, 16.0f / 256.0f, 0.5);
	platformAnimations.push_back(platformSprite);

	//Add Poker
	GLuint enemyTexture = LoadTexture(RESOURCE_FOLDER"enemies_spritesheet.png");
	vector<SheetSprite> pokerAnimations;
	vector<Entity> pokers;
	SheetSprite pokerSprite = SheetSprite(enemyTexture, 254.0f / 353.0f, 0.0f / 153.0f, 48.0f / 353.0f, 146.0f / 153.0f, 1.5);
	pokerAnimations.push_back(pokerSprite);
	pokerSprite = SheetSprite(enemyTexture, 302.0f / 353.0f, 0.0f / 153.0f, 48.0f / 353.0f, 146.0f / 153.0f, 1.5);
	pokerAnimations.push_back(pokerSprite);

	//Add Block
	vector<SheetSprite> blockAnimations;
	SheetSprite blockSprite = SheetSprite(enemyTexture, 135.0f / 353.0f, 65.0f / 153.0f, 51.0f / 353.0f, 51.0f / 153.0f, 1.0);
	blockAnimations.push_back(blockSprite);
	blockSprite = SheetSprite(enemyTexture, 188.0f / 353.0f, 65.0f / 153.0f, 51.0f / 353.0f, 51.0f / 153.0f, 1.0);
	blockAnimations.push_back(blockSprite);
	Entity block(0.0f, 0.0f, 0.5f, 1.0f, blockAnimations, 2.0f, 3.0f, 0.0f, -0.098f, false, 'e');

	//Add Slug
	GLuint enemyTexture2 = LoadTexture(RESOURCE_FOLDER"enemies_spritesheet2.png");
	vector<SheetSprite> slugAnimations;
	SheetSprite slugSprite = SheetSprite(enemyTexture, 52.0f / 353.0f, 125.0f / 153.0f, 49.0f / 353.0f, 28.0f / 153.0f, 2.0);
	slugAnimations.push_back(slugSprite);
	slugSprite = SheetSprite(enemyTexture, 0.0f / 353.0f, 125.0f / 153.0f, 50.0f / 353.0f, 25.0f / 153.0f, 2.0);
	slugAnimations.push_back(slugSprite);
	slugSprite = SheetSprite(enemyTexture, 0.0f / 353.0f, 98.0f / 153.0f, 58.0f / 353.0f, 25.0f / 153.0f, 2.0);
	slugAnimations.push_back(slugSprite);
	slugSprite = SheetSprite(enemyTexture2, 252.0f / 353.0f, 125.0f / 153.0f, 49.0f / 353.0f, 28.0f / 153.0f, 2.0);
	slugAnimations.push_back(slugSprite);
	slugSprite = SheetSprite(enemyTexture2, 303.0f / 353.0f, 125.0f / 153.0f, 50.0f / 353.0f, 25.0f / 153.0f, 2.0);
	slugAnimations.push_back(slugSprite);
	slugSprite = SheetSprite(enemyTexture2, 295.0f / 353.0f, 98.0f / 153.0f, 58.0f / 353.0f, 25.0f / 153.0f, 2.0);
	slugAnimations.push_back(slugSprite);
	Entity slug(0.0f, 0.0f, 1.5f, 2.0f, slugAnimations, 2.0f, 0.0f, 1.0f, -0.098f, false, 'b');
	slug.health = 3;
	vector<Entity> ghostPlatforms;

	//Level Specific
	if (state.level == 1) {	
		Mix_PlayMusic(state.levelMusic, -1);
		for (int i = 0; i < 10; i++) {
			platforms.push_back(Entity(-3.55f, -0.5f + 0.5f*i, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 30; i++) {
			platforms.push_back(Entity(-3.55f + 0.3f*i, -0.5f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 10; i++) {
			platforms.push_back(Entity(1.0f + 0.3f*i, 0.8f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 5; i++) {
			platforms.push_back(Entity(7.0f + 0.3f*i, 0.0f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 5; i++) {
			platforms.push_back(Entity(10.0f + 0.3f*i, 0.5f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}

		for (int i = 0; i < 10; i++) {
			platforms.push_back(Entity(22.0f + 0.3f*i, -0.5f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}

		//Add Moving Platforms
		for (int i = 0; i < 5; i++) {
			Entity mobPlatform(16.0f + 0.3f*i, 0.5f, 0.45f, 0.5f, platformAnimations, 1.5f, 0.0f, 0.0f, 0.0f, false, 'g');
			mobPlatform.leftBound = 12.0;
			mobPlatform.rightBound = 20.0;
			mobPlatforms.push_back(mobPlatform);
		}

		for (int i = 0; i < 1; i++) {
			pokers.push_back(Entity(0.0f, -20.0f + 0.5f*i, 0.45f, 0.5f, pokerAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'h'));
		}
		for (int i = 0; i < 1; i++) {
			ghostPlatforms.push_back(Entity(3.9f - 0.3f*i, 55.5f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		flag.x = 24.0;
		block.y = -40.0;
		slug.y = -50.0;
	}
	else if (state.level == 2) {
		for (int i = 0; i < 60; i++) {
			platforms.push_back(Entity(-3.55f + 0.3f*i, -0.5f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 80; i++) {
			platforms.push_back(Entity(-3.4f + 0.3f*i, -1.0f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 1; i++) {
			Entity mobPlatform(0.0f, -10.0f, 0.45f, 0.5f, platformAnimations, 1.5f, 0.0f, 0.0f, 0.0f, false, 'g');
			mobPlatforms.push_back(mobPlatform);
		}
		for (int i = 0; i < 3; i++) {
			Entity poker(2.0f + 1.5*i, -1.0f + 0.2f*i, 0.3f, 1.2f, pokerAnimations, 0.0f, 0.5f, 0.0f, 0.0f, false, 'h');
			poker.aiDirection = 1;
			poker.leftBound = -1.6f;
			poker.rightBound = 0.75f;
			pokers.push_back(poker);
		}
		for (int i = 0; i < 13; i++) {
			Entity poker(8.0f + 0.5*i, -2.0f + 0.01f*i, 0.3f, 1.2f, pokerAnimations, 0.0f, 1.0f, 0.0f, 0.0f, false, 'h');
			poker.aiDirection = 1;
			poker.leftBound = -2.6f;
			poker.rightBound = 0.5f;
			pokers.push_back(poker);
		}

		for (int i = 0; i < 1; i++) {
			Entity platform(9.0f + 0.3*i, 0.8f, 0.3f, 0.5f, platformAnimations, 1.5f, 0.0f, 0.0f, 0.0f, false, 'g');
			platforms.push_back(platform);
		}
		for (int i = 0; i < 1; i++) {
			Entity platform(13.0f + 0.3*i, 0.8f, 0.3f, 0.5f, platformAnimations, 1.5f, 0.0f, 0.0f, 0.0f, false, 'g');
			platforms.push_back(platform);
		}
		for (int i = 0; i < 1; i++) {
			ghostPlatforms.push_back(Entity(3.9f - 0.3f*i, 55.5f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}

		flag.x = 20.0;
		flag.y = -0.5f;
		block.x = 19.0;
		block.y = -0.25;
		block.dormant = true;
		block.aiDirection = -1;
		slug.y = -30.0;
	}
	else if (state.level == 3) {
		Mix_PauseMusic();
		for (int i = 0; i < 10; i++) {
			platforms.push_back(Entity(-3.55f, -0.5f + 0.5f*i, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 10; i++) {
			platforms.push_back(Entity(5.45f, -0.5f + 0.5f*i, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 30; i++) {
			platforms.push_back(Entity(-3.55f + 0.3f*i, -0.5f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 1; i++) {
			Entity mobPlatform(0.0f, -10.0f, 0.45f, 0.5f, platformAnimations, 1.5f, 0.0f, 0.0f, 0.0f, false, 'g');
			mobPlatforms.push_back(mobPlatform);
		}
		for (int i = 0; i < 1; i++) {
			pokers.push_back(Entity(0.0f, -20.0f + 0.5f*i, 0.45f, 0.5f, pokerAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'h'));
		}
		for (int i = 0; i < 3; i++) {
			ghostPlatforms.push_back(Entity(-2.0f + 0.3f*i, 0.6f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 3; i++) {
			ghostPlatforms.push_back(Entity(3.9f - 0.3f*i, 0.6f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 3; i++) {
			ghostPlatforms.push_back(Entity(2.3f - 0.3f*i, 1.3f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 3; i++) {
			ghostPlatforms.push_back(Entity(-0.4f + 0.3f*i, 1.3f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		for (int i = 0; i < 3; i++) {
			ghostPlatforms.push_back(Entity(0.65f + 0.3f*i, 2.0f, 0.45f, 0.5f, platformAnimations, 0.0f, 0.0f, 0.0f, 0.0f, true, 'g'));
		}
		block.y = -10.25;
		block.dormant = true;
		flag.y = -8.5f;
		slug.x = 4.3f;
		slug.y = 5.0f;
		slug.dormant = true;
		slug.aiDirection = -1;
		slug.cooldown = 0;
		slug.agressive = true;
		slug.attack = 0;
		slug.targetx = 0;
	}
	Entity player(0.0, 0.5, 0.2f, 0.5f, playerAnimations, 2.4f, 0.0f, 0.9f, -0.098f, false, 'p');
	player.damagedEnemy = false;
	state.player = player;
	state.platforms = platforms;
	state.mobPlatforms = mobPlatforms;
	state.heartTexture = heartTexture;
	state.flag = flag;
	state.pokers = pokers;
	state.block = block;
	state.slug = slug;
	state.ghostPlatforms = ghostPlatforms;
}

void UpdateGame(GameState &state, ShaderProgram& program, float elapsed) {
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	state.player.collideUp = false;
	state.player.collideDown = false;
	state.player.collideRight = false;
	state.player.collideLeft = false;
	state.player.collideFlag = false;
	state.player.collideHazard = false;
	state.player.collideEnemy = false;
	state.block.collideDown = false;
	state.slug.collideDown = false;
	state.slug.collideRight = false;
	state.slug.collideLeft = false;
	state.player.collideSlug = false;
	//Check collision with platforms
	for (int i = 0; i < state.platforms.size(); i++) {
		state.player.CollidesWithX(state.platforms[i]);
		state.player.CollidesWithY(state.platforms[i]);
	}
	for (int i = 0; i < state.mobPlatforms.size(); i++) {
		state.player.CollidesWithX(state.mobPlatforms[i]);
		state.player.CollidesWithY(state.mobPlatforms[i]);
	}
	for (int i = 0; i < state.ghostPlatforms.size(); i++) {
		state.player.CollidesWithX(state.ghostPlatforms[i]);
		state.player.CollidesWithY(state.ghostPlatforms[i]);
	}

	//Check collision with flag
	state.player.CollidesWithY(state.flag);
	if (state.player.collideFlag == true) {
		state.level ++;
		RenderGame(state);
	}
	//Check collision with pokers
	for (int i = 0; i < state.pokers.size(); i++) {
		state.player.CollidesWithY(state.pokers[i]);
	}
	if (state.player.collideHazard == true) {
		state.player.x = 0;
		state.player.y = 0.5;
		Mix_PlayChannel(-1, state.damage, 0);
		state.lifeTotal--;	
	}
	//Check collision with block
	state.player.CollidesWithY(state.block);
	if (state.player.collideEnemy == true) {
		state.block.x = 19.0f;
		state.block.y = -0.25f;
		state.block.dormant = true;
		state.player.x = 0;
		state.player.y = 0.5;
		Mix_PlayChannel(-1, state.damage, 0);
		state.lifeTotal--;
	}
	//Check block collision with platforms
	for (int i = 0; i < state.platforms.size(); i++) {
		state.block.CollidesWithX(state.platforms[i]);
		state.block.CollidesWithY(state.platforms[i]);
	}
	
	//Check slug collision with platforms
	for (int i = 0; i < state.platforms.size(); i++) {
		state.slug.CollidesWithS(state.platforms[i]);
	}

	//Check player collision with slug
	state.player.CollidesWithY(state.slug);
	if (state.player.collideSlug == true) {
		Mix_PlayChannel(-1, state.damage, 0);
		state.lifeTotal--;
		state.player.x = 0.0;
		state.player.y = 0.5;
		state.slug.recovery = true;
	}

	//Check to see if player is within block sight rage
	if (state.player.x > state.block.x - 3.0f) {
		state.block.dormant = false;
	}

	//Event
	if ((state.player.x > 3.0f) && (state.level == 3) && (state.slug.dormant == true)) {
		state.slug.dormant = false;
		state.falling = true;
	}

	if ((state.falling == true && state.slug.collideDown == true)) {
		Mix_PlayChannel(-1, state.fall, 0);
		state.falling = false;
		state.slug.recovery = true;
		state.slug.aniFrame = 2;
		state.musicStart = true;
	}
	
	 //Slug Cooldown
	if (state.slug.agressive == false && state.slug.wounded == false) {
		state.slug.cooldown += elapsed;
		if (state.slug.cooldown > 1.0f) {
			state.slug.cooldown = 0;
			state.slug.agressive = true;
			if (state.slug.attack == 3) {
				state.slug.attack = 1;
			}
			else {
				state.slug.attack++;
			}
			if (state.slug.attack == 1) {
				state.slug.targetx = state.player.x;
			}
			else if (state.slug.attack == 2) {
				state.slug.targetx = state.player.x;
				state.slug.yvelocity = 4.0;
				state.slug.y += state.slug.yvelocity * elapsed;
			}
			else if (state.slug.attack == 3) {
				state.slug.targetx = state.player.x;
				if (state.slug.targetx < state.slug.x) {
					state.slug.aiDirection = -1;
				}
				else if (state.slug.targetx > state.slug.x) {
					state.slug.aiDirection = 1;
				}
			}
		}
	}
	//Slug Attacks
	if (state.slug.attack == 1 &&  (state.slug.x - state.slug.width/2 < state.slug.targetx && state.slug.x + state.slug.width / 2 > state.slug.targetx)) {
		state.slug.agressive = false;
	}
	else if (state.slug.attack == 2 && (state.slug.x - state.slug.width / 2 < state.slug.targetx && state.slug.x + state.slug.width / 2 > state.slug.targetx) && state.slug.collideDown == true) {
		state.slug.agressive = false;
	}
	//Slug Animations
	state.slug.animation += elapsed;
	if((state.slug.attack == 1 || state.slug.attack == 2) && state.slug.agressive == true && state.slug.wounded == false) {
		if (state.slug.animation > 0.5f) {
			if (state.slug.targetx < state.slug.x) {
				if (state.slug.aniFrame == 0) {
					state.slug.aniFrame = 1;
				}
				else {
					state.slug.aniFrame = 0;
				}
			}
			else if (state.slug.targetx > state.slug.x) {
				if (state.slug.aniFrame == 3) {
					state.slug.aniFrame = 4;
				}
				else {
					state.slug.aniFrame = 3;

				}
			}
			state.slug.animation = 0;
		}
	}
	else if ((state.slug.attack == 3) && state.slug.agressive == true && state.slug.wounded == false) {
		if (state.slug.animation > 0.1f) {
			if (state.slug.aiDirection == -1) {
				if (state.slug.aniFrame == 0) {
					state.slug.aniFrame = 1;
				}
				else {
					state.slug.aniFrame = 0;
				}
			}
			else if (state.slug.aiDirection == 1) {
				if (state.slug.aniFrame == 3) {
					state.slug.aniFrame = 4;
				}
				else {
					state.slug.aniFrame = 3;
				}
			}
			state.slug.animation = 0;
		}
	}

	if (state.slug.attack == 3 && (state.slug.collideLeft == true || state.slug.collideRight == true) && state.slug.agressive == true) {
		Mix_PlayChannel(-1, state.fall, 0);
		state.slug.agressive = false;
		state.slug.wounded = true;
		if (state.slug.collideLeft == true) {
			state.slug.aniFrame = 5;
		}
		else if (state.slug.collideRight == true) {
			state.slug.aniFrame = 2;
		}
	}

	if (state.slug.wounded == true) {
		state.slug.cooldown += elapsed;
		if ((state.slug.cooldown > 3.0f) || (state.player.damagedEnemy == true)) {
			if (state.player.damagedEnemy == true) {
				Mix_PlayChannel(-1, state.bounce, 0);
				state.slug.health--;
			}
			state.slug.cooldown = 0;
			state.player.damagedEnemy = false;
			state.slug.recovery = true;
			state.slug.wounded = false;
		}
	}
	if (state.slug.health == 0) {
		Mix_PauseMusic();
		state.stop = true;
		state.shaketime += elapsed;
		if (state.shaketime > 1.0) {
			Mix_PlayChannel(-1, state.bounce, 0);
			state.shake = (-state.shake);
			state.projectionMatrix.SetOrthoProjection(-3.55f + state.shake, 3.55f + state.shake, -2.0f + state.shake, 2.0f + state.shake, -1.0f + state.shake, 1.0f + state.shake);
		}
		if (state.shaketime > 3.0) {
			state.shake = 0;
			state.slug.y = -50.0;
			state.stop = false;
			state.slug.health = -1;
			state.projectionMatrix.SetOrthoProjection(-3.55f + state.shake, 3.55f + state.shake, -2.0f + state.shake, 2.0f + state.shake, -1.0f + state.shake, 1.0f + state.shake);
			Mix_PlayMusic(state.victoryMusic, -1);
		}
	}

	
	if (state.shake == 0) {
		glClearColor(0.0f, 20.0f, 60.0f, 80.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(program.programID);
		program.SetProjectionMatrix(state.projectionMatrix);
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(-0.8f, 1.5f, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		DrawText(&program, state.fontSheet, "You Win!", 0.25f, 0.00000f);
		program.SetModelviewMatrix(state.modelviewMatrix);
	}
	if (state.stop == false) {
		if (state.slug.recovery == true) {
			state.slug.cooldown += elapsed;
			if (state.slug.cooldown > 1.0f) {
				if (state.player.x > state.slug.x) {
					state.slug.aniFrame = 3;
					state.slug.aiDirection = 1;
				}
				else if (state.player.x < state.slug.x) {
					state.slug.aniFrame = 0;

					state.slug.aiDirection = -1;
				}
				state.slug.cooldown = 0;
				state.slug.recovery = false;
				state.slug.agressive = false;
				state.slug.xvelocity = 2.0;
				state.slug.x += state.slug.aiDirection * state.slug.xvelocity * elapsed * 1.5;
				if (state.musicStart == true) {
					Mix_PlayMusic(state.bossMusic, -1);
					state.musicStart = false;
				}
			}
		}
		//Movement
		if (state.player.collideDown == true) {
			state.player.yvelocity = 0;
		}
		if (state.player.collideDown == false) {
			state.player.yvelocity += state.player.yacceleration;
			state.player.y += state.player.yvelocity * elapsed;
		}

		//Keyboard Inputs
		if (keys[SDL_SCANCODE_LEFT]) {
			if (state.player.xvelocity == 0) {
				state.player.xvelocity = 2.4;
			}
			state.player.x -= state.player.xvelocity * elapsed;
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			if (state.player.xvelocity == 0) {
				state.player.xvelocity = 2.4;
			}
			state.player.x += state.player.xvelocity * elapsed;
		}
		else if (keys[SDL_SCANCODE_UP]) {
			if (state.player.collideDown == true) {
				state.player.yvelocity = 4.0f;
				state.player.y += state.player.yvelocity * elapsed;
			}
		}
		if (state.player.collideUp == true) {
			state.player.yvelocity = 0;
		}
		if (state.player.collideLeft == true || state.player.collideRight == true) {
			state.player.xvelocity = 0;
		}
		if (state.player.y < -2.0) {
			state.player.x = 0;
			state.player.y = 0.5;
			Mix_PlayChannel(-1, state.damage, 0);
			state.lifeTotal--;
		}
		if (state.lifeTotal == 0) {
			Mix_PlayMusic(state.overMusic, -1);
			state.mode = 0;
		}
		if (state.slug.collideLeft == true || state.slug.collideRight == true) {
			state.slug.xvelocity = 0;
		}
		else if (state.slug.collideLeft == false && state.slug.collideRight == false) {
			state.slug.xvelocity = 2.0;
		}
		//AI Movement
		for (size_t i = 0; i < state.mobPlatforms.size(); i++) {
			state.mobPlatforms[i].x += state.direction * state.mobPlatforms[i].xvelocity * elapsed;
			if (state.mobPlatforms[i].x < state.mobPlatforms[i].leftBound || state.mobPlatforms[i].x > state.mobPlatforms[i].rightBound)
				state.direction = -(state.direction);
		}

		for (size_t i = 0; i < state.pokers.size(); i++) {
			state.pokers[i].y += state.pokers[i].aiDirection * state.pokers[i].yvelocity * elapsed;
			if (state.pokers[i].y - state.pokers[i].height / 2 < state.pokers[i].leftBound)
				state.pokers[i].aiDirection = -(state.pokers[i].aiDirection);
			else if (state.pokers[i].y + state.pokers[i].height / 2 > state.pokers[i].rightBound)
				state.pokers[i].aiDirection = -(state.pokers[i].aiDirection);
			state.pokers[i].y += state.pokers[i].aiDirection * 0.1 * elapsed;
		}
		if (state.block.collideDown == true) {
			state.block.yvelocity = 3.0;
			state.block.y += state.block.yvelocity * elapsed;
		}
		if (state.block.collideDown == false) {
			state.block.yvelocity += state.block.yacceleration;
			state.block.y += state.block.yvelocity * elapsed;
		}
		if (state.block.dormant == false) {
			state.block.x += (state.block.aiDirection) * state.block.xvelocity * elapsed;
			if (state.block.collideDown == true) {
				if (state.player.x < state.block.x) {
					state.block.aiDirection = -1;
				}
				else if (state.player.x > state.block.x) {
					state.block.aiDirection = 1;
				}
			}
		}
		if (state.slug.dormant == false && state.slug.collideDown == false) {
			state.slug.yvelocity += state.slug.yacceleration;
			state.slug.y += state.slug.yvelocity * elapsed;
		}
		if ((state.slug.attack == 1 || state.slug.attack == 2) && state.slug.agressive == true) {
			state.slug.x += (state.slug.aiDirection) * state.slug.xvelocity * elapsed;
			if (state.slug.targetx < state.slug.x) {
				state.slug.aiDirection = -1;
			}
			else if (state.slug.targetx > state.slug.x) {
				state.slug.aiDirection = 1;
			}
		}
		else if ((state.slug.attack == 3) && state.slug.agressive == true) {
			state.slug.x += state.slug.aiDirection * state.slug.xvelocity * elapsed;
		}
	}
	

	
	glUseProgram(program.programID);
	//Draw Player
	program.SetModelviewMatrix(state.playerMatrix);
	state.playerMatrix.Identity();
	program.SetProjectionMatrix(state.projectionMatrix);
	state.playerMatrix.Translate(0.0, state.playerYoffset + state.player.y, 0.0f);
	state.player.sprite[state.sprite_index].Draw(&program);

	//Draw Pokers
	for (int i = 0; i < state.pokers.size(); i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(state.pokers[i].x - state.player.x, state.pokers[i].y + state.playerYoffset, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		state.pokers[i].sprite[0].Draw(&program);
		
	}

	//Draw Platforms
	for (int i = 0; i < state.platforms.size(); i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(state.platforms[i].x - state.player.x, state.platforms[i].y + state.playerYoffset, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		state.platforms[i].sprite[0].Draw(&program);
	}

	//Draw MobPlatforms
	for (int i = 0; i < state.mobPlatforms.size(); i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(state.mobPlatforms[i].x - state.player.x, state.mobPlatforms[i].y + state.playerYoffset, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		state.mobPlatforms[i].sprite[0].Draw(&program);
	}

	//Draw GhostPlatforms
	for (int i = 0; i < state.ghostPlatforms.size(); i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(state.ghostPlatforms[i].x - state.player.x, state.ghostPlatforms[i].y + state.playerYoffset, 0.0f);
		program.SetModelviewMatrix(state.modelviewMatrix);
		state.ghostPlatforms[i].sprite[0].Draw(&program);
	}
	
	//Draw Hearts
	for (int i = 0; i < state.lifeTotal; i++) {
		state.modelviewMatrix.Identity();
		state.modelviewMatrix.Translate(-3.3 + 0.4f*i, 1.7, 0.0);
		program.SetModelviewMatrix(state.modelviewMatrix);
		glBindTexture(GL_TEXTURE_2D, state.heartTexture);
		float vertices[] = { -0.15, -0.15, 0.15, -0.15, 0.15, 0.15, -0.15, -0.15, 0.15, 0.15, -0.15, 0.15 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	}
	

	//Draw Flag
	state.modelviewMatrix.Identity();
	state.modelviewMatrix.Translate(state.flag.x - state.player.x, state.flag.y + state.playerYoffset, 0.0f);
	program.SetModelviewMatrix(state.modelviewMatrix);
	state.flag.sprite[0].Draw(&program);

	//Draw Block
	state.modelviewMatrix.Identity();
	state.modelviewMatrix.Translate(state.block.x - state.player.x, state.block.y + state.playerYoffset, 0.0f);
	program.SetModelviewMatrix(state.modelviewMatrix);
	state.block.sprite[0].Draw(&program);

	//Draw Slug
	state.modelviewMatrix.Identity();
	state.modelviewMatrix.Translate(state.slug.x - state.player.x, state.slug.y + state.playerYoffset, 0.0f);
	program.SetModelviewMatrix(state.modelviewMatrix);
	state.slug.sprite[state.slug.aniFrame].Draw(&program);
	

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
	float lastFrameTicks = 0.0f;

	//Music
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Chunk *fall = Mix_LoadWAV("fall.wav");
	state.fall = fall;
	Mix_Chunk *bounce = Mix_LoadWAV("bounce.wav");
	state.bounce = bounce;
	Mix_Chunk *damage = Mix_LoadWAV("damage.wav");
	state.damage = damage;
	Mix_Music *levelMusic = Mix_LoadMUS("level1.mp3");
	Mix_Music *bossMusic = Mix_LoadMUS("boss.mp3");
	Mix_Music *victoryMusic = Mix_LoadMUS("victory.mp3");
	Mix_Music *overMusic = Mix_LoadMUS("over.mp3");
	state.levelMusic = levelMusic;
	state.bossMusic = bossMusic;
	state.victoryMusic = victoryMusic;
	state.overMusic = overMusic;

	glUseProgram(program.programID);
	GLuint fontSheet = LoadTexture(RESOURCE_FOLDER"font2.png");
	state.fontSheet = fontSheet;
	while (!done) {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		float playerAnimationElapsed = 0.0f;
		lastFrameTicks = ticks;
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		glClear(GL_COLOR_BUFFER_BIT);
		while (SDL_PollEvent(&event)) {
				
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}

			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
					if (state.sprite_index != 1 || state.sprite_index != 2) {
						state.sprite_index = 1;
					}
					playerAnimationElapsed += elapsed;
					if (playerAnimationElapsed > 1.0 / 60.0f) {
						if (state.sprite_index == 1) {
							state.sprite_index = 2;
						}
						else {
							state.sprite_index = 1;
						}
						playerAnimationElapsed = 0.0f;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
					if (state.sprite_index != 3 || state.sprite_index != 4) {
						state.sprite_index = 3;
					}
					playerAnimationElapsed += elapsed;
					if (playerAnimationElapsed > 1.0 / 60.0f) {
						if (state.sprite_index == 3) {
							state.sprite_index = 4;
						}
						else {
							state.sprite_index = 3;
						}
						playerAnimationElapsed = 0.0f;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					done = true;
				}
			}
			else {
				state.sprite_index = 0;
			}
		}
		if (state.mode == 0) {
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(program.programID);
			program.SetProjectionMatrix(projectionMatrix);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-1.2f, 1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "Game Over!", 0.25f, 0.00000f);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-3.0f, -1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "Press Enter to try again!", 0.25f, 0.00000f);
			if (keys[SDL_SCANCODE_RETURN]) {
				glClear(GL_COLOR_BUFFER_BIT);
				state.lifeTotal = 3;
				RenderGame(state);
				state.mode = 2;
			}
		}
		if (state.mode == 1) {
			glClearColor(0.0f, 0.0f, 255.0f, 0.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(program.programID);
			program.SetProjectionMatrix(projectionMatrix);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-1.2f, 1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "Jumpy Ghost", 0.25f, 0.00000f);
			modelviewMatrix.Identity();
			modelviewMatrix.Translate(-2.3f, -1.0f, 0.0f);
			program.SetModelviewMatrix(modelviewMatrix);
			DrawText(&program, fontSheet, "Press Enter to play!", 0.25f, 0.00000f);
			if (keys[SDL_SCANCODE_RETURN]) {
				glClear(GL_COLOR_BUFFER_BIT);
				state.level = 1;
				RenderGame(state);
				state.mode = 2;
			}
		}
		if (state.mode == 2) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
			UpdateGame(state, program, elapsed);
		}
		SDL_GL_SwapWindow(displayWindow);
	}
	Mix_FreeChunk(fall);
	Mix_FreeChunk(bounce);
	Mix_FreeChunk(damage);
	Mix_FreeMusic(levelMusic);
	Mix_FreeMusic(bossMusic);
	Mix_FreeMusic(victoryMusic);
	Mix_FreeMusic(overMusic);
	SDL_Quit();
	return 0;
}
