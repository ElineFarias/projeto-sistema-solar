//Inclusão de Bibliotecas
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H   
#include "Shader.h"
#include "Esfera.h"
#include "Camera.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <wtypes.h>
#include <ctime>
#include <filesystem>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#define TAU (M_PI * 2.0)


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(std::vector<std::string> faces);
void ShowInfo(Shader &s);
void GetDesktopResolution(float& horizontal, float& vertical) {

		RECT desktop;
		// Pega um handle para a janela da área de trabalho
		const HWND hDesktop = GetDesktopWindow();
	// Obtém o tamanho da tela para a área de trabalho variável
		GetWindowRect(hDesktop, &desktop);
// O canto superior esquerdo terá coordenadas (0,0)
// E o canto inferior direito terá coordenadas
		// (horizontal, vertical)
		horizontal = desktop.right;
		vertical = desktop.bottom;	
}

GLfloat deltaTime = 0.0f;	
GLfloat lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool onRotate = false;
bool onFreeCam = true;
bool SkyBoxExtra = false;
float SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

glm::vec3 point = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 PlanetPos = glm::vec3(0.0f, 0.0f, 0.0f);
GLfloat lastX = (GLfloat)(SCREEN_WIDTH / 2.0);
GLfloat lastY = (GLfloat)(SCREEN_HEIGHT / 2.0);
float PlanetSpeed = .1f;
int PlanetView = 0;

bool keys[1024];
GLfloat SceneRotateY = 0.0f;
GLfloat SceneRotateX = 0.0f;
bool onPlanet = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GL_TRUE);

			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
					//camera.Position = PlanetPos;
					onPlanet = true;
			}
			if (key >= 0 && key < 1024) {
					if (action == GLFW_PRESS)
						keys[key] = true;
					else if (action == GLFW_RELEASE)
						keys[key] = false;
			}
}

bool firstMouse = true;
GLfloat xoff = 0.0f, yoff = 0.0f;

struct Character {
		GLuint TextureID;   // identificador de ID da textura de glyph
		glm::ivec2 Size;    // Tamanho do glyph
		glm::ivec2 Bearing;  // Deslocamento da linha de base para a esquerda/topo de glyph
		GLuint Advance;    // Deslocamento horizontal para avançar para o próximo glyph
};
std::map<GLchar, Character> Characters;
GLuint textVAO, textVBO;

struct PlanetInfo {
		std::string Nome;
		std::string  VelocidadeMediaOrbital;
		std::string TemperaturaMedia; 
		std::string Massa;
		std::string Gravidade;
		std::string QuantidadeLuas;
};
PlanetInfo Info;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
			if (firstMouse) {
					lastX = (GLfloat)xpos;
					lastY = (GLfloat)ypos;
					firstMouse = false;
			}

		GLfloat xoffset = (GLfloat)(xpos - lastX);
		GLfloat yoffset = (GLfloat)(lastY - ypos);
		xoff = xoffset;
		yoff = yoff;

		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		if (onRotate) {
				SceneRotateY += yoffset * 0.1f;
				SceneRotateX += xoffset * 0.1f;
		}
		camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		if (yoffset == 1)
				camera.ProcessKeyboard(SCROLL_FORWARD, deltaTime);
		else {
				camera.ProcessKeyboard(SCROLL_BACKWARD, deltaTime);
		}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		if (onFreeCam) {
			if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
				onRotate = true;
			else onRotate = false;
		}
}

int main() {
	GetDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT); // obtém resolução para criar janela
	camera.LookAtPos = point;

	/* GLFW INICIO */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	/* GLFW INICIO */

	/* GLFW CRIAÇÃO DA JANELA */
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	/* GLFW CRIAÇÃO DA JANELA */


	/* INICIO GLAD */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
	}
	/* INICIO GLAD */


	/* CONFIGURAÇÃO PARA RENDERIZAR TEXTO */
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "fonts/starwarsdemolitionlevel.otf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Define o tamanho para carregar os glyphs
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Desabilita restrição de alinhamento de bytes
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

// Carrega os primeiros 128 caracteres do conjunto ASCII
	for (GLubyte c = 0; c < 128; c++) {
		//Carrega o caractere glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
		}
		// Gerando a textura
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Definição de opções de textura
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Agora armazena o caractere para uso posterior
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Encerrar o FreeType assim que terminar
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
/* CONFIGURAÇÃO PARA RENDERIZAÇÃO DO TEXTO */


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* SHADERS */
	Shader SimpleShader("simpleVS.vs", "simpleFS.fs");
	Shader SkyboxShader("skybox.vs", "skybox.fs");
	Shader texShader("simpleVS.vs", "texFS.fs");
	Shader TextShader("TextShader.vs", "TextShader.fs");
	/* SHADERS */

// PROJEÇÃO PARA RENDERIZAÇÃO DO TEXTO
		glm::mat4 Text_projection = glm::ortho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT);
		TextShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(TextShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(Text_projection));

	float cube[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	/* GERAÇÃO DO SKYBOX */
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	/* GERAÇÃO DO SKYBOX */

/* GERAÇÃO DE VÉRTICES PARA ÓRBITAS */
	std::vector<float> orbVert;
	GLfloat xx;
	GLfloat zz;
	float angl;
	for (int i = 0; i < 2000; i++) {
			angl = (float)(M_PI / 2 - i * (M_PI / 1000));
			xx = sin(angl) * 100.0f;
			zz = cos(angl) * 100.0f;
			orbVert.push_back(xx);
			orbVert.push_back(0.0f);
			orbVert.push_back(zz);
	}
/* GERAÇÃO DE VÉRTICES PARA ÓRBITAS */

	/* VAO-VBO PARA ÓRBITAS*/
	GLuint VBO_t, VAO_t;
	glGenVertexArrays(1, &VAO_t);
	glGenBuffers(1, &VBO_t);
	glBindVertexArray(VAO_t);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_t);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*orbVert.size(), orbVert.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	/* VAO-VBO PARA ÓRBITAS*/

	/* RENDERIZAÇÃO DO TEXTO VAO-VBO*/
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	/* RENDERIZAÇÃO DO TEXTO VAO-VBO*/

	/* CARREGANDO TEXTURAS */
	unsigned int texturaTerra = loadTexture("resources/texturaPlanetas/terra.jpg");
	unsigned int texturaSol = loadTexture("resources/texturaPlanetas/sol.jpg");
	unsigned int texturaLua = loadTexture("resources/texturaPlanetas/lua.jpg");
	unsigned int texturaMercurio = loadTexture("resources/texturaPlanetas/mercurio.jpg");
	unsigned int texturaVenus = loadTexture("resources/texturaPlanetas/venus.jpg");
	unsigned int texturaMarte = loadTexture("resources/texturaPlanetas/marte.jpg");
	unsigned int texturaJupiter = loadTexture("resources/texturaPlanetas/jupiter.jpg");
	unsigned int texturaSaturno = loadTexture("resources/texturaPlanetas/saturno.jpg");
	unsigned int texturaUrano = loadTexture("resources/texturaPlanetas/urano.jpg");
	unsigned int texturaNetuno = loadTexture("resources/texturaPlanetas/netuno.jpg");
	unsigned int texturaSaturnoAnel = loadTexture("resources/texturaPlanetas/anelSaturno.jpg");
//	unsigned int texturaTerra_clouds = loadTexture("resources/texturaPlanetas/nuvensTerra.jpg");
	/* CARREGANDO TEXTURAS */

	/* GERAÇÃO DAS ESFERAS(ASTROS) */
	Esfera Sol(100.0f, 36 * 5, 18 * 5);
	Esfera Mercurio(10.0f, 36, 18);
	Esfera Venus(12.0f, 36, 18);
	Esfera Terra(11.8f, 36, 18);
	Esfera Marte(8.0f, 36, 18);
	Esfera Jupiter(40.0f, 36, 18);
	Esfera Saturno(37.0f, 36, 18);
	Esfera Urano(30.0f, 36, 18);
	Esfera Netuno(30.0f, 36, 19);
	Esfera Lua(5.5f, 36, 18);
	/* GERAÇÃO DAS ESFERAS(ASTROS) */

	std::vector<std::string> faces_extra {
		"resources/skybox/starfield/starfield_rt.tga",
		"resources/skybox/starfield/starfield_lf.tga",
		"resources/skybox/starfield/starfield_up.tga",
		"resources/skybox/starfield/starfield_dn.tga",
		"resources/skybox/starfield/starfield_ft.tga",
		"resources/skybox/starfield/starfield_bk.tga",

	};
	std::vector<std::string> faces {
		"resources/skybox/blue/bkg1_right.png",
		"resources/skybox/blue/bkg1_left.png",
		"resources/skybox/blue/bkg1_top.png",
		"resources/skybox/blue/bkg1_bot.png",
		"resources/skybox/blue/bkg1_front.png",
		"resources/skybox/blue/bkg1_back.png",
	};

	unsigned int cubemapTexture = loadCubemap(faces);
	unsigned int cubemapTextureExtra = loadCubemap(faces_extra);
	GLfloat camX = 10.0f;
	GLfloat camZ = 10.0f;
	
	camera.Position = glm::vec3(0.0f, 250.0f, -450.0f);
	camera.Yaw = 90.0f;
	camera.Pitch = -40.0f;
	camera.ProcessMouseMovement(xoff, yoff);
	onFreeCam = true;
	glm::mat4 view;
	glm::vec3 PlanetsPositions[9];
	while (!glfwWindowShouldClose(window)) {
		
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/* ZOOM CONTROLES */
		if ( !onFreeCam)
		{
			if (camera.Position.y < 200 && camera.Position.y > 200.0f)
				camera.MovementSpeed = 300.0f;
			if (camera.Position.y < 125.f && camera.Position.y > 70.0f)
				camera.MovementSpeed = 200.0f;
			if (camera.Position.y < 70.f && camera.Position.y > 50.0f)
				camera.MovementSpeed = 100.0f;

			if (camera.Position.y > 200 && camera.Position.y < 400.0f)
				camera.MovementSpeed = 400.0f;
			if (camera.Position.y > 125.f && camera.Position.y < 200.0f)
				camera.MovementSpeed = 300.0f;
			if (camera.Position.y > 70.f && camera.Position.y < 125.0f)
				camera.MovementSpeed = 200.0f;
		}
		/* ZOOM CONTROLES */

		processInput(window); // input

		if (!onFreeCam)
		{
			SceneRotateY = 0.0f;
			SceneRotateX = 0.0f;
		}
		if (onFreeCam || PlanetView > 0)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// renderização
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SimpleShader.Use();

		glm::mat4 model = glm::mat4(1.0f);
	
		double viewX;
		double viewZ;
		glm::vec3 viewPos;
		
		SimpleShader.Use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 10000.0f);
		SimpleShader.setMat4("model", model);
		SimpleShader.setMat4("view", view);
		SimpleShader.setMat4("projection", projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaSol);


		/* SOL*/
		glm::mat4 modelo_sol;
		modelo_sol = glm::rotate(modelo_sol, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		modelo_sol = glm::rotate(modelo_sol, (GLfloat)glfwGetTime() * glm::radians(23.5f) * 0.25f, glm::vec3(0.0f, 0.0f, 1.f));
		modelo_sol = glm::translate(modelo_sol, point);
		SimpleShader.setMat4("model", modelo_sol);
		Sol.Draw();
		/* SOL */

		/* MERCURIO */
		glm::mat4 model_mercury;
		double xx = sin(glfwGetTime() * PlanetSpeed) * 100.0f *2.0f *1.3f;
		double zz = cos(glfwGetTime() * PlanetSpeed) * 100.0f *2.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaMercurio);
		model_mercury = glm::translate(model_mercury, point);
		model_mercury = glm::rotate(model_mercury, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_mercury = glm::rotate(model_mercury, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_mercury = glm::translate(model_mercury, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[0] = glm::vec3(xx, 0.0f, zz);
		model_mercury = glm::rotate(model_mercury, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_mercury = glm::rotate(model_mercury, (GLfloat)glfwGetTime() * glm::radians(-90.0f) * 0.05f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_mercury);
		Mercurio.Draw();
		/* MERCURIO */

		/* VENUS */
		glm::mat4 model_venus;
		xx = sin(glfwGetTime() * PlanetSpeed *0.75f) * 100.0f * 3.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.75f) * 100.0f * 3.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaVenus);
		model_venus = glm::translate(model_venus, point);
		model_venus = glm::rotate(model_venus, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_venus = glm::rotate(model_venus, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_venus = glm::translate(model_venus, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[1] = glm::vec3(xx, 0.0f, zz);
		model_venus = glm::rotate(model_venus, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_venus = glm::rotate(model_venus, glm::radians(-132.5f), glm::vec3(0.0f, 1.0f, 0.f));
		model_venus = glm::rotate(model_venus, (GLfloat)glfwGetTime() * glm::radians(-132.5f) * 0.012f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_venus);
		Venus.Draw();
		/* VENUS */

		/* TERRA */
		glm::mat4 model_earth;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaTerra);
		model_earth = glm::translate(model_earth, point);
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_earth = glm::translate(model_earth, glm::vec3(xx, 0.0f, zz));
		glm::vec3 EarthPoint = glm::vec3(xx, 0.0f, zz);
		PlanetsPositions[2] = glm::vec3(xx, 0.0f, zz);
		model_earth = glm::rotate(model_earth, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_earth = glm::rotate(model_earth, glm::radians(-33.25f), glm::vec3(0.0f, 1.0f, 0.f));
		model_earth = glm::rotate(model_earth, (GLfloat)glfwGetTime() * glm::radians(-33.25f) * 2.0f, glm::vec3(0.0f, 0.0f, 1.f));
		camera.LookAtPos = glm::vec3(model_earth[3][0], model_earth[3][1], model_earth[3][2]);
		SimpleShader.setMat4("model", model_earth);
		Terra.Draw();  
		
		/* TERRA */
		
		/* LUA */
		glm::mat4 model_lua;
		xx = sin(glfwGetTime() * PlanetSpeed * 67.55f) * 100.0f * 0.5f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 67.55f) * 100.0f * 0.5f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaLua);
		model_lua = glm::rotate(model_lua, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_lua = glm::rotate(model_lua, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_lua = glm::translate(model_lua, EarthPoint);
		model_lua = glm::translate(model_lua, glm::vec3(xx, 0.0f, zz));
		model_lua = glm::rotate(model_lua, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_lua = glm::rotate(model_lua, glm::radians(-32.4f), glm::vec3(0.0f, 1.0f, 0.f));
		model_lua = glm::rotate(model_lua, (GLfloat)glfwGetTime() * glm::radians(-32.4f) * 3.1f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_lua);
		Lua.Draw();
		/* LUA */


		/* MARTE */
		glm::mat4 model_mars;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaMarte);
		model_mars = glm::translate(model_mars, point);
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_mars = glm::translate(model_mars, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[3] = glm::vec3(xx, 0.0f, zz);
		model_mars = glm::rotate(model_mars, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_mars = glm::rotate(model_mars, glm::radians(-32.4f), glm::vec3(0.0f, 1.0f, 0.f));
		model_mars = glm::rotate(model_mars, (GLfloat)glfwGetTime() * glm::radians(-32.4f) * 2.1f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_mars);
		Marte.Draw();
		/* MARTE */

		/* JUPITER */
		glm::mat4 model_jupiter;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f * 6.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f * 6.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaJupiter);
		model_jupiter = glm::translate(model_jupiter, point);
		model_jupiter = glm::rotate(model_jupiter, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_jupiter = glm::rotate(model_jupiter, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_jupiter = glm::translate(model_jupiter, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[4] = glm::vec3(xx, 0.0f, zz);
		model_jupiter = glm::rotate(model_jupiter, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_jupiter = glm::rotate(model_jupiter, glm::radians(-23.5f), glm::vec3(0.0f, 1.0f, 0.f));
		model_jupiter = glm::rotate(model_jupiter, (GLfloat)glfwGetTime() * glm::radians(-23.5f) * 4.5f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_jupiter);
		Jupiter.Draw();
		/* JUPITER */

		/* SATURNO */
		glm::mat4 model_saturn;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f * 7.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f * 7.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaSaturno);
		model_saturn = glm::translate(model_saturn, point);
		model_saturn = glm::rotate(model_saturn, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_saturn = glm::rotate(model_saturn, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_saturn = glm::translate(model_saturn, glm::vec3(xx, 0.0f, zz));
		glm::vec3 SatrunPoint = glm::vec3(xx, 0.0f, zz);
		PlanetsPositions[5] = glm::vec3(xx, 0.0f, zz);
		model_saturn = glm::rotate(model_saturn, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_saturn = glm::rotate(model_saturn, glm::radians(-34.7f), glm::vec3(0.0f, 1.0f, 0.f));
		model_saturn = glm::rotate(model_saturn, (GLfloat)glfwGetTime() * glm::radians(-34.7f) * 4.48f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_saturn);
		Saturno.Draw();
		/* SATURNO */

		/* URANO */
		glm::mat4 model_uranus;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f * 8.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f * 8.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaUrano);
		model_uranus = glm::translate(model_uranus, point);
		model_uranus = glm::rotate(model_uranus, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_uranus = glm::rotate(model_uranus, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_uranus = glm::translate(model_uranus, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[6] = glm::vec3(xx, 0.0f, zz);
		model_uranus = glm::rotate(model_uranus, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_uranus = glm::rotate(model_uranus, glm::radians(-99.0f), glm::vec3(0.0f, 1.0f, 0.f));
		model_uranus = glm::rotate(model_uranus, (GLfloat)glfwGetTime() * glm::radians(-99.0f) * 4.5f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_uranus);
		Urano.Draw();
		/* URANO */

		/* NETUNO*/
		glm::mat4 model_neptune;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f * 9.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f * 9.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaNetuno);

		model_neptune = glm::translate(model_neptune, point);
		model_neptune = glm::rotate(model_neptune, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_neptune = glm::rotate(model_neptune, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_neptune = glm::translate(model_neptune, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[7] = glm::vec3(xx, 0.0f, zz);
		model_neptune = glm::rotate(model_neptune, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_neptune = glm::rotate(model_neptune, glm::radians(-30.2f), glm::vec3(0.0f, 1.0f, 0.f));
		model_neptune = glm::rotate(model_neptune, (GLfloat)glfwGetTime() * glm::radians(-30.2f) * 4.0f, glm::vec3(0.0f, 0.0f, 1.f));
		
		SimpleShader.setMat4("model", model_neptune);
		Netuno.Draw();
		/* NETUNO */

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaVenus);

		/* ORBITA */
		glBindVertexArray(VAO_t);
		glLineWidth(0.7f);
		glm::mat4 modelorb;
		for (float i = 2; i < 10; i++) {
			modelorb = glm::mat4(1);
			modelorb = glm::translate(modelorb, point);
			modelorb = glm::rotate(modelorb, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
			modelorb = glm::rotate(modelorb, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
			modelorb = glm::scale(modelorb, glm::vec3(i *1.3f, i*1.3f, i*1.3f));
			SimpleShader.setMat4("model", modelorb);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)orbVert.size() / 3);

		}
		modelorb = glm::mat4(1);
		modelorb = glm::rotate(modelorb, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		modelorb = glm::rotate(modelorb, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		modelorb = glm::translate(modelorb, EarthPoint);
		modelorb = glm::scale(modelorb, glm::vec3(0.5f *1.3f , 0.5f *1.3f, 0.5f *1.3f));
		SimpleShader.setMat4("model", modelorb);
		glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)orbVert.size() / 3);
		/* ORBITA */

		/* ANEIS DE SATURNO */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaSaturnoAnel);
		glLineWidth(2.0f);
		GLfloat rr = 0.55f;
		for (int i = 0; i < 25; i++) {
			modelorb = glm::mat4(1);
			
			modelorb = glm::rotate(modelorb, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
			modelorb = glm::rotate(modelorb, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
			modelorb = glm::translate(modelorb, SatrunPoint);
			modelorb = glm::rotate(modelorb, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			modelorb = glm::scale(modelorb, glm::vec3(rr, rr, rr));
			SimpleShader.setMat4("model", modelorb);
			glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)orbVert.size() / 3);
			if (i == 15)
				rr += 0.030f;
			else
				rr += 0.01f;
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaVenus);
		glBindVertexArray(0);
		/* ANEIS DE SATURNO */


		/* DESENHO SKYBOX */
		glDepthFunc(GL_LEQUAL);  
		SkyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		SkyboxShader.setMat4("view", view);
		SkyboxShader.setMat4("projection", projection);
		// skybox cubo
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		if (SkyBoxExtra)
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureExtra);
		else
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);  
		/* DESENHO SKYBOX */

		/*RASTREAMENTO DO PLANETA + MOSTRAR INFORMAÇÕES DO PLANETA*/
		switch (PlanetView) {
		case 1:
			viewX = sin(glfwGetTime() * PlanetSpeed) * 100.0f *3.5f * 1.3f;
			viewZ = cos(glfwGetTime() * PlanetSpeed) * 100.0f *3.5f * 1.3f;
			viewPos = glm::vec3(viewX, 50.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[0], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 2:
			viewX = sin(glfwGetTime() * PlanetSpeed * 0.75f) * 100.0f *4.5f * 1.2f;
			viewZ = cos(glfwGetTime() * PlanetSpeed * 0.75f) * 100.0f *4.5f * 1.2f;
			viewPos = glm::vec3(viewX, 50.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[1], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 3:
			viewX = sin(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f *5.5f * 1.2f;
			viewZ = cos(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f *5.5f * 1.2f;
			viewPos = glm::vec3(viewX, 50.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[2], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 4:
			viewX = sin(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f *6.0f * 1.2f;
			viewZ = cos(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f *6.0f * 1.2f;
			viewPos = glm::vec3(viewX, 20.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[3], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 5:
			viewX = sin(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f *7.5f * 1.3f;
			viewZ = cos(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f *7.5f * 1.3f;
			viewPos = glm::vec3(viewX, 50.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[4], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 6:
			viewX = sin(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f *8.5f * 1.3f;
			viewZ = cos(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f *8.5f * 1.3f;
			viewPos = glm::vec3(viewX, 50.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[5], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 7:
			viewX = sin(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f *9.5f * 1.3f;
			viewZ = cos(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f *9.5f * 1.3f;
			viewPos = glm::vec3(viewX, 50.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[6], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 8:
			viewX = sin(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f *10.5f * 1.3f;
			viewZ = cos(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f *10.5f * 1.3f;
			viewPos = glm::vec3(viewX, 50.0f, viewZ);
			view = glm::lookAt(viewPos, PlanetsPositions[7], glm::vec3(0.0f, 1.0f, 0.0f));
			ShowInfo(TextShader);
			break;

		case 0:
			view = camera.GetViewMatrix();

			RenderText(TextShader, "SISTEMA SOLAR", 25.0f, SCREEN_HEIGHT - 30.0f, 0.50f, glm::vec3(0.7f, 0.7f, 0.11f));
			RenderText(TextShader, "ESTRELA: 1 (SOL) ", 25.0f, SCREEN_HEIGHT - 55.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
			RenderText(TextShader, "PLANETAS: 8 ", 25.0f, SCREEN_HEIGHT - 80.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
			RenderText(TextShader, "SATELITES NATURAIS: 428 ", 25.0f, SCREEN_HEIGHT - 105.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
			RenderText(TextShader, "COMETAS: 4584 (Em 2021) ", 25.0f, SCREEN_HEIGHT - 130.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
		
		if (onFreeCam)
				RenderText(TextShader, "CAMERA  ESTATICA ", SCREEN_WIDTH - 200.0f, SCREEN_HEIGHT - 30.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
			break;
		}
		if (PlanetView > 0)
			RenderText(TextShader, "CAMERA  PLANETAS ", SCREEN_WIDTH - 200.0f, SCREEN_HEIGHT - 30.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
		/* RASTREAMENTO DO PLANETA + MOSTRAR INFORMAÇÕES DO PLANETA*/


		// glfw: troca de buffers e pesquisa de eventos de E/S (teclas pressionadas/liberadas, mouse movido etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO_t);
	glDeleteBuffers(1, &VBO_t);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		SkyBoxExtra = true;

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
			PlanetView = 0;
			onFreeCam = true;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {		//Esta função retorna o último estado relatado para a chave especificada para a janela especificada.
			PlanetView = 0;
			onFreeCam = true;
			camera.Position = glm::vec3(0.0f, 250.0f, -450.0f);
			camera.Yaw = 90.0f;
			camera.Pitch = -40.0f;
			camera.GetViewMatrix();
			camera.ProcessMouseMovement(xoff,yoff);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {			
			PlanetView = 1;
			Info.Nome = "MERCURIO";
			Info.VelocidadeMediaOrbital = "47,4";
			Info.TemperaturaMedia = "167";
			Info.Massa = "0.32868";
			Info.Gravidade = "0.38";
			Info.QuantidadeLuas = "0";
			onFreeCam = false;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			PlanetView = 2;
			Info.Nome = "VENUS";
			Info.VelocidadeMediaOrbital = "35";
			Info.TemperaturaMedia = "464";
			Info.Massa = "0.32868";
			Info.Gravidade = "0.90";
			Info.QuantidadeLuas = "0";
			onFreeCam = false;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			PlanetView = 3;
			Info.Nome = "TERRA";
			Info.VelocidadeMediaOrbital = "29,8";
			Info.TemperaturaMedia = "15";
			Info.Massa = "5.97600";
			Info.Gravidade = "1";
			Info.QuantidadeLuas = "1";
			onFreeCam = false;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
			PlanetView = 4;
			Info.Nome = "MARTE";
			Info.VelocidadeMediaOrbital = "24,1";
				Info.TemperaturaMedia = "-65";
			Info.Massa = "0.63345";
			Info.Gravidade = "0.38";
			Info.QuantidadeLuas = "2";
			onFreeCam = false;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
			PlanetView = 5;
			Info.Nome = "JUPITER";
			Info.VelocidadeMediaOrbital = "13,1";
			Info.TemperaturaMedia = "-110";
			Info.Massa = "1876.64328";
			Info.Gravidade = "2.55";
			Info.QuantidadeLuas = "79(53 confirmados, 26 provisórios)";
			onFreeCam = false;
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
			PlanetView = 6;
			Info.Nome = "SATURNO";
			Info.VelocidadeMediaOrbital= "9,7";
			Info.TemperaturaMedia = "-140";
			Info.Massa = "561.80376";
			Info.Gravidade = "1.12";
			Info.QuantidadeLuas = "82(53 confirmmados, 29 provisórios";
			onFreeCam = false;
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
			PlanetView = 7;
			Info.Nome = "URANO";
			Info.VelocidadeMediaOrbital = "6,8";
			Info.TemperaturaMedia = "-195";
			Info.Massa = "86.05440";
			Info.Gravidade = "0.97";
			Info.QuantidadeLuas = "27";
			onFreeCam = false;
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) { 		
			PlanetView = 8;
			Info.Nome = "NETUNO";
			Info.VelocidadeMediaOrbital = "5,4";
				Info.TemperaturaMedia = "-200";
			Info.Massa = "101.59200";
			Info.Gravidade = "1.17";
			Info.QuantidadeLuas = "14";
			onFreeCam = false;
	}

} 

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;

	for (unsigned int i = 0; i < faces.size(); i++) {
				unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
				if (data) {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
					);
					stbi_image_free(data);
			}
			else {
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
					stbi_image_free(data);
			}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int loadTexture(char const * path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
// Ativa o estado de renderização correspondente
	s.Use();
	glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

// Iterar por todos os caracteres
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO para cada objeto
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Renderização glyph textura sobre  quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
	// Atualiza o conteúdo de VBO memória
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Renderização quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Agora avance os cursores para o próximo glifo (observe que o avanço é o número de 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift por 6 para obter o valor em pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ShowInfo(Shader &s) {
	RenderText(s, "Planeta: " + Info.Nome, 25.0f, SCREEN_HEIGHT - 30.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
	RenderText(s, "Velocidade  media orbital (km/s): " + Info.VelocidadeMediaOrbital, 25.0f, SCREEN_HEIGHT - 50.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
	RenderText(s, "Temperatura media (ºC): " + Info.TemperaturaMedia, 25.0f, SCREEN_HEIGHT - 70.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
	RenderText(s, "Massa (kg * 10^24): " + Info.Massa, 25.0f, SCREEN_HEIGHT - 90.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
	RenderText(s, "Gravidade (g): " + Info.Gravidade, 25.0f, SCREEN_HEIGHT - 110.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
	RenderText(s, "Quantidade de Luas: " + Info.QuantidadeLuas, 25.0f, SCREEN_HEIGHT - 130.0f, 0.35f, glm::vec3(0.7f, 0.7f, 0.11f));
}