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
#include <cstdlib>
#include <time.h>
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

	GLuint ballTexture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Puzzle assets (80 assets)/PNG/ballGrey.png");
	GLuint p1Texture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Puzzle assets (80 assets)/PNG/element_purple_square.png");
	GLuint p2Texture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Puzzle assets (80 assets)/PNG/element_purple_square.png");
	GLuint winTexture = LoadTexture(RESOURCE_FOLDER"C:/Program Files/Git/CS3113/assets/graphics/Medals (30 assets)/PNG/shaded_medal1.png");
	Matrix projectionMatrix;
	projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	Matrix modelviewMatrix;
	float lastFrameTicks = 0.0f;
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
	ball.velocity = 1.5;
	srand(time(NULL));
	float angle = (rand() % 360 + 1);
	Player1.x = -3.4;
	Player2.x = 3.4;
	Player1.y = 0;
	Player2.y = 0;
	Player1.velocity = 3.0;
	Player2.velocity = 3.0;
	Matrix p1matrix;
	Matrix p2matrix;
	Matrix ballmatrix;
	Matrix winmatrix;
	bool end = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}

		}
		if (end == false) {
			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;
			ball.x += cos(angle*3.1415/180)*elapsed*ball.velocity;
			ball.y += sin(angle*3.1415/180)*elapsed*ball.velocity;
			ballmatrix.Identity();
			ballmatrix.Translate(ball.x, ball.y, 0.0f);

			glClear(GL_COLOR_BUFFER_BIT);

			const Uint8 *keys = SDL_GetKeyboardState(NULL);

			if (keys[SDL_SCANCODE_UP]) {
				//Player 2 entity moves up
				p2matrix.Translate(0.0f, Player2.velocity * elapsed, 0.0f);
				Player2.y += Player2.velocity * elapsed;
			}
			else if (keys[SDL_SCANCODE_DOWN]) {
				//Player 2 entity moves down
				p2matrix.Translate(0.0f, -(Player2.velocity * elapsed), 0.0f);
				Player2.y -= Player2.velocity * elapsed;
			}
			else if (keys[SDL_SCANCODE_W]) {
				//Player 1 entity moves up
				p1matrix.Translate(0.0f, Player1.velocity * elapsed, 0.0f);
				Player1.y += Player1.velocity * elapsed;
			}
			else if (keys[SDL_SCANCODE_S]) {
				//Player 1 entity moves down
				p1matrix.Translate(0.0f, -(Player1.velocity * elapsed), 0.0f);
				Player1.y -= Player1.velocity * elapsed;
			}

			if ((ball.x - (ball.width / 2)) < -3.55) {
				winmatrix.Translate(1.5f, 0.0f, 0.0f);
				program.SetModelviewMatrix(winmatrix);
				glBindTexture(GL_TEXTURE_2D, winTexture);
				float vertices[] = { -0.5, -1.0, 0.5, -1.0, 0.5, 1.0, -0.5, -1.0, 0.5, 1.0, -0.5, 1.0 };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
				glEnableVertexAttribArray(program.positionAttribute);
				float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);
				end = true;
			}
			else if (3.55 < ball.x + (ball.width / 2)) {
				winmatrix.Translate(-1.5f, 0.0f, 0.0f);
				program.SetModelviewMatrix(winmatrix);
				glBindTexture(GL_TEXTURE_2D, winTexture);
				float vertices[] = { -0.5, -1.0, 0.5, -1.0, 0.5, 1.0, -0.5, -1.0, 0.5, 1.0, -0.5, 1.0 };
				glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
				glEnableVertexAttribArray(program.positionAttribute);
				float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
				glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
				glEnableVertexAttribArray(program.texCoordAttribute);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(program.positionAttribute);
				glDisableVertexAttribArray(program.texCoordAttribute);
				end = true;
			}
			if (ball.y - (ball.height / 2) < -2.0) {
				ball.y = -2.0 + (ball.height / 2);
				angle = -(angle);
			}
			else if (ball.y + (ball.height / 2) > 2.0) {
				ball.y = 2.0 - (ball.height / 2);
				angle = -(angle);

			}

			if ((ball.x -(ball.width / 2) < Player1.x + (Player1.width / 2)) && (ball.y + ball.height / 2 < Player1.y + Player1.height / 2) && (ball.y - ball.height / 2 > Player1.y - Player1.height / 2)) {
				ball.x = Player1.x + ball.width / 2 + 0.15;
				if (0 <= angle <= 180) {
					angle = 180 - angle;
				}
				else if (180 < angle <= 360) {
					angle = 540 - angle;
				}
			}

			if ((ball.x + (ball.width / 2) > Player2.x - (Player2.width / 2)) && (ball.y + ball.height / 2 < Player2.y + Player2.height / 2) && (ball.y - ball.height / 2 > Player2.y - Player2.height / 2)) {
				ball.x = Player2.x - ball.width / 2 - 0.15;
				if (0 <= angle <= 180) {
					angle = 180 - angle;
				}
				else if (180 < angle <= 360) {
					angle = 540 - angle;
				}
			}

			if (Player1.y + (Player1.height / 2) > 2.0) {
				Player1.y = 2.0 - (Player1.height / 2);
				p1matrix.Identity();
				p1matrix.Translate(0.0f, Player1.y, 0.0f);
			}
			else if (Player1.y - (Player1.height / 2) < -2.0) {
				Player1.y = -2.0 + (Player1.height / 2);
				p1matrix.Identity();
				p1matrix.Translate(0.0f, Player1.y, 0.0f);
			}

			if (Player2.y + (Player2.height / 2) > 2.0) {
				Player2.y = 2.0 - (Player2.height / 2);
				p2matrix.Identity();
				p2matrix.Translate(0.0f, Player2.y, 0.0f);
			}
			else if (Player2.y - (Player2.height / 2) < -2.0) {
				Player2.y = -2.0 + (Player2.height / 2);
				p2matrix.Identity();
				p2matrix.Translate(0.0f, Player2.y, 0.0f);
			}

			glUseProgram(program.programID);

			program.SetModelviewMatrix(modelviewMatrix);
			program.SetProjectionMatrix(projectionMatrix);

			program.SetModelviewMatrix(ballmatrix);
			glBindTexture(GL_TEXTURE_2D, ballTexture);

			float ballVertices[] = { -ball.width / 2, -ball.height / 2, ball.width / 2,  -ball.height / 2, ball.width / 2, ball.height / 2, -ball.width / 2, -ball.height / 2, ball.width / 2, ball.height / 2, -ball.width / 2, ball.height / 2 };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
			glEnableVertexAttribArray(program.positionAttribute);

			float ballTexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, ballTexCoords);
			glEnableVertexAttribArray(program.texCoordAttribute);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);

			program.SetModelviewMatrix(p1matrix);
			glBindTexture(GL_TEXTURE_2D, p1Texture);

			float p1Vertices[] = { -3.55, -Player1.height / 2, -3.25, -Player1.height / 2, -3.25, Player1.height / 2, -3.55, -Player1.height / 2, -3.25, Player1.height / 2, -3.55, Player1.height / 2 };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p1Vertices);
			glEnableVertexAttribArray(program.positionAttribute);

			float p1TexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, p1TexCoords);
			glEnableVertexAttribArray(program.texCoordAttribute);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);

			program.SetModelviewMatrix(p2matrix);
			glBindTexture(GL_TEXTURE_2D, p2Texture);

			float p2Vertices[] = { 3.25, -Player2.height / 2, 3.55, -Player2.height / 2, 3.55, Player2.height / 2, 3.25, -Player2.height / 2, 3.55, Player2.height / 2, 3.25, Player2.height / 2 };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p2Vertices);
			glEnableVertexAttribArray(program.positionAttribute);

			float p2TexCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, p2TexCoords);
			glEnableVertexAttribArray(program.texCoordAttribute);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);

			SDL_GL_SwapWindow(displayWindow);
		}
	}
	
	SDL_Quit();
	return 0;
}
