//Aaron Fang, ajf500
//Used project template/class slides code
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif



SDL_Window* displayWindow;

class Entity {
public:
	void Draw(ShaderProgram *p);
	float x;
	float y;
	float rotation;
	int textureID;
	float width;
	float height;
	float velocity;
	float direction_x;
	float direction_y;
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

	GLuint lineTexture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Puzzle assets (80 assets)/PNG/element_blue_square.png");
	GLuint ballTexture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Puzzle assets (80 assets)/PNG/ballGrey.png");
	GLuint p1Texture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Puzzle assets (80 assets)/PNG/element_purple_square.png");
	GLuint p2Texture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Puzzle assets (80 assets)/PNG/element_purple_square.png");
	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	float lastFrameTicks = 0.0f;
	float angle = 0.0f;
	Entity Player1;
	Entity Player2;
	Entity ball;
	Player1.width = 0.3;
	Player2.width = 0.3;
	Player1.height = 1.0;
	Player2.height = 1.0;
	Player1.textureID = p1Texture;
	Player2.textureID = p2Texture;
	ball.width = 0.3;
	ball.height = 0.3;
	ball.x = 0;
	ball.y = 0;
	ball.velocity = 1;
	angle = rand() % 360;
	while (!done) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		ball.x += cos(angle)*elapsed*ball.velocity;
		ball.y += sin(angle)*elapsed*ball.velocity;
		glClear(GL_COLOR_BUFFER_BIT);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_UP]) {
			//Player 2 entity moves up
			Player2.y += 0.5;
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			//Player 2 entity moves down
			Player2.y -= 0.5;
		}
		else if (keys[SDL_SCANCODE_W]) {
			//Player 1 entity moves up
			Player1.y += 0.5;
		}
		else if (keys[SDL_SCANCODE_S]) {
			//Player 1 entity moves down
			Player1.y -= 0.5;
		}

		if ((ball.x - (ball.width / 2)) < -3.55 < (ball.x + (ball.width / 2))) {
			printf("Player 2 wins!");
		}
		else if ((ball.x - (ball.width / 2)) < 3.55 < (ball.x + (ball.width / 2))) {
			printf("Player 1 wins!");
		}
		else if ((ball.y - (ball.height / 2)) < -2.0 < (ball.y + (ball.height / 2)) || (ball.y - (ball.height / 2)) < 2.0 < (ball.y + (ball.height / 2))) {
			angle = 360 - angle;
		}
		//Checks for collisions between ball and either player entity
		else if ((ball.y - (ball.height / 2) < (Player1.y + (Player1.height / 2))) && (ball.y + (ball.height / 2) > (Player1.y - (Player1.height / 2))) && (ball.x - (ball.width / 2) < (Player1.x + (Player1.width / 2))) && (ball.x + (ball.width / 2) > (Player1.x + (Player1.width / 2))) || (ball.y - (ball.height / 2) < (Player2.y + (Player2.height / 2))) && (ball.y + (ball.height / 2) > (Player2.y - (Player2.height / 2))) && (ball.x - (ball.width / 2) < (Player2.x + (Player2.width / 2))) && (ball.x + (ball.width / 2) > (Player2.x + (Player2.width / 2)))) {
			if (0 <= angle <= 180) {
				angle = 180 - angle;
			}
			else if (180 < angle <= 360) {
				angle = 540 - angle;
			}
		}

		

		glUseProgram(program.programID);

		program.SetModelviewMatrix(modelviewMatrix);
		program.SetProjectionMatrix(projectionMatrix);

		glBindTexture(GL_TEXTURE_2D, lineTexture);

		float lineVertices[] = { -.25, -3, .25, -3, .25, 3, -.25, -3, .25, 3, -.25, 3 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, lineVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float lineTexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, lineTexCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, ballTexture);

		float ballVertices[] = { (ball.x-ball.width/2), (ball.y - ball.height / 2), (ball.x + ball.width / 2), (ball.y - ball.height / 2), (ball.x + ball.width / 2), (ball.y + ball.height / 2), (ball.x - ball.width / 2), (ball.y - ball.height / 2), (ball.x + ball.width / 2), (ball.y + ball.height / 2), (ball.x - ball.width / 2), (ball.y + ball.height / 2) };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, lineVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float ballTexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, lineTexCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, p1Texture);

		float p1Vertices[] = { -3.55, (Player1.x-Player1.height/2), -3.25, (Player1.x - Player1.height / 2), -3.25, (Player1.x + Player1.height / 2), -3.55, (Player1.x - Player1.height / 2), -3.25, (Player1.x + Player1.height / 2), -3.55, (Player1.x + Player1.height / 2) };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, lineVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float p1TexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, lineTexCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, p2Texture);

		float p2Vertices[] = { 3.25, (Player2.x - Player2.height / 2), 3.55, (Player2.x - Player2.height / 2), 3.55, (Player2.x + Player2.height / 2), 3.25, (Player2.x - Player2.height / 2), 3.55, (Player2.x + Player2.height / 2), 3.25, (Player2.x + Player2.height / 2) };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, lineVertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float p2TexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, lineTexCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		SDL_GL_SwapWindow(displayWindow);
	}
	
	SDL_Quit();
	return 0;
}
