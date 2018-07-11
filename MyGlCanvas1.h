#pragma once
#include <NANOGUI/nanogui.h>
#include "gl_utils.h"
#include "Primitives\ShapeGenerator.h"
#include <GLM/mat4x4.hpp>
#include <GLM/gtx/transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_access.hpp>
#include <GLM/gtx/string_cast.hpp>

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <nanogui/glcanvas.h>
#include <iostream>
#include <string>

#include <iomanip>
#include <sstream>

#define GL_LOG_FILE "gl.log"
#define MY_PI 3.14159265358979323846
#define ONE_DEG_IN_RAD ( 2.0 * MY_PI ) / 360.0 // 0.017444444

class MyGLCanvas : public nanogui::GLCanvas {

public:

	glm::mat4 identityMatrix;
	const unsigned int VERTEX_BYTE_SIZE = 6 * sizeof(GLfloat);
	int fullTransformMatrixUniformLocation;
	double previous_seconds;
	double current_seconds;
	double elapsed_seconds;
	int g_gl_width = 100;
	int g_gl_height = 100;
	ShapeData shape;
	GLuint shader_programme;

	//Ship
	GLuint shipVao;
	GLuint vertexBufferId;
	GLuint indexArrayBufferId;
	float shipXPosition;
	float shipSpeed;

	//shots
	int maxShots;
	GLuint *shotVaos;
	glm::mat4 *shots;
	bool *activeShots;
	float *shotYposition;
	float *shotXposition;
	GLuint *vertexShotBufferId;
	GLuint *indexShotBufferId;


	//aliens
	int maxAliens;
	glm::mat4 *aliens;
	GLuint *alienVaos;
	GLuint *vertexAlienBufferId;
	GLuint *indexAlienBufferId;
	bool *activeAliens;
	float *alienXStartPosition;
	float *alienXPosition;
	float direction;
	float lastPosition;
	float alienXDelta;
	float alienYPosition;
	float alienSpeed;

	//Nanogui interface pointers
	nanogui::TextBox *pointBox;
	nanogui::Label *infoBox;
	int points;
	nanogui::Slider *speedSlider;
	nanogui::TextBox *speedBox;
	nanogui::ComboBox *alienQtyBomboBox;
	nanogui::Slider *shipSpeedSlider;
	nanogui::TextBox *shipSpeedBox;

	MyGLCanvas(Widget *parent) : nanogui::GLCanvas(parent) {
		using namespace nanogui;
		identityMatrix = glm::mat4(1.0f);
		shipVao = 0;
		shipXPosition = 0.0f;
		shipSpeed = 1.0f;
		previous_seconds = glfwGetTime();

		//Shots
		maxShots = 10;
		shotVaos = new GLuint[maxShots];
		vertexShotBufferId = new GLuint[maxShots];
		indexShotBufferId = new GLuint[maxShots];
		activeShots = new bool[maxShots]{ 0,0,0,0,0,0,0,0,0,0 };
		shotYposition = new float[maxShots]{ -0.9f, -0.9f, -0.9f, -0.9f, -0.9f, -0.9f, -0.9f, -0.9f, -0.9f, -0.9f };
		shotXposition = new float[maxShots]{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, };
		shots = new glm::mat4[maxShots];

		//Aliens
		maxAliens = 20;
		aliens = new glm::mat4[maxAliens];
		alienVaos = new GLuint[maxAliens];
		vertexAlienBufferId = new GLuint[maxAliens];
		indexAlienBufferId = new GLuint[maxAliens];
		activeAliens = new bool[maxAliens]{ 0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0};
		alienXStartPosition = new float[maxAliens]{ -0.95f, -0.85f, -0.75f, -0.65f, -0.55f, -0.45f, -0.35f, -0.25f, -0.15f, -0.05f, 0.05f, 0.15f, 0.25f, 0.35f, 0.45f, 0.55f, 0.65f, 0.75f, 0.85f, 0.95f };
		alienXPosition = new float[maxAliens] { -0.95f, -0.85f, -0.75f, -0.65f, -0.55f, -0.45f, -0.35f, -0.25f, -0.15f, -0.05f, 0.05f, 0.15f, 0.25f, 0.35f, 0.45f, 0.55f, 0.65f, 0.75f, 0.85f, 0.95f };
		direction = 1.0f;
		lastPosition = 0.0f;
		alienXDelta = 0.2f;
		alienYPosition = 0.9f;
		alienSpeed = 1.0f;

		//Points
		points = 0;
		
		startGl();
		sendDataToGl();
		installGlShaders();
		glUseProgram(shader_programme);
		fullTransformMatrixUniformLocation = glGetUniformLocation(shader_programme, "fullTransformMatrix");
		alienStartPoint();
	}

	virtual void drawGL() override {
		using namespace nanogui;

		//Time
		current_seconds = glfwGetTime();
		elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		//Clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use Shader Program
		glUseProgram(shader_programme);

		drawnShip(fullTransformMatrixUniformLocation);
		drawnShots(elapsed_seconds, fullTransformMatrixUniformLocation);

		lastPosition += (elapsed_seconds * alienSpeed * direction);
		if (fabs(lastPosition) > 1.0f) {
			direction = -direction;
		}
		else {
		}

		//lastPosition vai de -1.0 a 1.0 corretamente
		//alienXDelta -e 0.02
		drawnAliens((alienXDelta * alienSpeed) * lastPosition, fullTransformMatrixUniformLocation);
		colisionTest();
		endGame();
	}

	void startGl() {
		glewExperimental = GL_TRUE;
		glewInit();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	void sendDataToGl() {
		startGl();

		//Ship
		shape = ShapeGenerator::triangle(1);
		glGenVertexArrays(1, &shipVao);
		glBindVertexArray(shipVao);
		glGenBuffers(1, &vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(GLfloat) * 3));
		glGenBuffers(1, &indexArrayBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrayBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_STATIC_DRAW);
		shape.cleanUp();

		//Shots
		ShapeData* shots = new ShapeData[maxShots];
		for (int i = 0; i < maxShots; i++) {
			glGenVertexArrays(1, &shotVaos[i]);
			glBindVertexArray(shotVaos[i]);
			shots[i] = ShapeGenerator::triangle(1);
			glGenBuffers(1, &vertexShotBufferId[i]);
			glBindBuffer(GL_ARRAY_BUFFER, vertexShotBufferId[i]);
			glBufferData(GL_ARRAY_BUFFER, shots[i].vertexBufferSize(), shots[i].vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
			glGenBuffers(1, &indexShotBufferId[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexShotBufferId[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, shots[i].indexBufferSize(), shots[i].indices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(GLfloat) * 3));
			shots[i].cleanUp();
		}

		//Aliens
		ShapeData* aliens = new ShapeData[maxAliens];
		for (int i = 0; i < maxShots; i++) {
			glGenVertexArrays(1, &alienVaos[i]);
			glBindVertexArray(alienVaos[i]);
			aliens[i] = ShapeGenerator::triangle(1);
			glGenBuffers(1, &vertexAlienBufferId[i]);
			glBindBuffer(GL_ARRAY_BUFFER, vertexAlienBufferId[i]);
			glBufferData(GL_ARRAY_BUFFER, aliens[i].vertexBufferSize(), aliens[i].vertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
			glGenBuffers(1, &indexAlienBufferId[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexAlienBufferId[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, aliens[i].indexBufferSize(), aliens[i].indices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(GLfloat) * 3));
			aliens[i].cleanUp();
		}
		
	}

	bool installGlShaders() {
		char vertex_shader[1024 * 256];
		char fragment_shader[1024 * 256];
		parse_file_into_str("test_vs.glsl", vertex_shader, 1024 * 256);
		parse_file_into_str("test_fs.glsl", fragment_shader, 1024 * 256);

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		const GLchar *p = (const GLchar *)vertex_shader;
		glShaderSource(vs, 1, &p, NULL);
		glCompileShader(vs);

		// check for compile errors
		int params = -1;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
		if (GL_TRUE != params) {
			fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
			print_shader_info_log(vs);
			return 1; // or exit or something
		}

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		p = (const GLchar *)fragment_shader;
		glShaderSource(fs, 1, &p, NULL);
		glCompileShader(fs);

		// check for compile errors
		glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
		if (GL_TRUE != params) {
			fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
			print_shader_info_log(fs);
			return 1; // or exit or something
		}

		shader_programme = glCreateProgram();
		glAttachShader(shader_programme, fs);
		glAttachShader(shader_programme, vs);
		glLinkProgram(shader_programme);

		glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
		if (GL_TRUE != params) {
			fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
				shader_programme);
			print_programme_info_log(shader_programme);
			return false;
		}
		return true;
	}

	void moveShipRight() {
		if (shipXPosition <= 1.0f) {
			float step = shipSpeed * elapsed_seconds;
			shipXPosition += step;
		}
	}

	void moveShipLeft() {
		if (shipXPosition >= -1.0f) {
			float step = shipSpeed * elapsed_seconds;
			shipXPosition -= step;
		}
	}

	void drawnShip(int fullTransformMatrixUniformLocation) {
		glBindVertexArray(shipVao);
		glm::mat4 transform;
		transform = glm::translate(identityMatrix, glm::vec3(shipXPosition, -0.8f, 0.0f));
		glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort)));
	}

	void shot() {
		int freeShotSlot;
		bool hasFreeShotSlot = 0;
		for (int i = 0; i < maxShots; i++) {
			if (!activeShots[i]) {
				freeShotSlot = i;
				hasFreeShotSlot = 1;
				break;
			}
		}
		if (hasFreeShotSlot) {
			activeShots[freeShotSlot] = 1;
			shotXposition[freeShotSlot] = shipXPosition;
			shotYposition[freeShotSlot] = -0.9f;
			glm::mat4 transform = glm::translate(identityMatrix, glm::vec3(shipXPosition, shotYposition[freeShotSlot], 0.0f));
			shots[freeShotSlot] = transform;
		}
	}

	void restartShot(int i) {
		if (shotYposition[i] >= 1.0f) {
			activeShots[i] = 0;
			shotYposition[i] = -0.9f;
			shotXposition[i] = 0.0f;
		}
	}

	void drawnShots(float step, int fullTransformMatrixUniformLocation) {
		glm::mat4 transform;
		for (int i = 0; i < maxShots; i++) {
			if (activeShots[i]) {
				restartShot(i);
				glBindVertexArray(shotVaos[i]);
				transform = glm::translate(shots[i], glm::vec3(0.0f, step, 0.0f));
				glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
				shots[i] = transform;
				shotYposition[i] += step;
			}
		}
	}

	void drawnAliens(float step, int fullTransformMatrixUniformLocation) {
		glm::mat4 transform;
		for (int i = 0; i < maxAliens; i++) {
			if (activeAliens[i]) {
				glBindVertexArray(alienVaos[i]);
				alienXPosition[i] = alienXStartPosition[i] + step;
				transform = glm::translate(aliens[i], glm::vec3(step, 0.0f, 0.0f));
				glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
			}
		}
	}

	void alienStartPoint() {
		float alienSize = 0.1f;
		for (unsigned short i = 0; i < maxAliens; i++) {
			glm::mat4 translate = glm::translate(identityMatrix, glm::vec3(alienXStartPosition[i], alienYPosition, 0.0f));
			aliens[i] = translate;
		}
	}

	void colisionTest() {
		for (int i = 0; i < maxShots; i++) {
			if (activeShots[i]) {
				if (shotYposition[i] >= 0.85f && shotYposition[i] <= 0.95f) {
					for (int j = 0; j < maxAliens; j++) {
						if (shotXposition[i] + 0.05f >= alienXPosition[j] && shotXposition[i] - 0.05f <= alienXPosition[j]) {
							//std::cout << "BOOM " << " ALIEN " << j << " HAS FUCKED UP" << std::endl;
							infoBox->setCaption("BOOM,  alien " + std::to_string(j) + " foi abatido.");
							points += 10;
							pointBox->setValue(std::to_string(points));
							activeShots[i] = 0;
							activeAliens[j] = 0;
						}
					}
				}
			}
		}
	}

	void endGame() {
		using namespace nanogui;
		bool endGame = true;
		for (int i = 0; i < maxAliens; i++) {
			if (activeAliens[i] == 1) {
				endGame = false;
				break;
			}
		}
		if (endGame) {
			infoBox->setCaption("Fim de jogo.");
			//MessageDialog(this, MessageDialog::Type::Information, "Fim de jogo", "Você Ganho", "OK", "Cancel", false);
		}
	}

	void resetShots() {
		for (int i = 0; i < maxShots; i++) {
			activeShots[i] = 0;
			shotXposition[i] = 0.0f;
			shots[i] = glm::mat4(1.0f);
		}
	}

	void resetAliens() {
		std::string str = "Jogo reiniciado.";
		float aux = -1.05f;
		for (int i = 0; i < maxAliens; i++) {
			aux += 0.10f;
			aliens[i] = glm::mat4(1.0f);
			alienXStartPosition[i] = aux;
			alienXPosition[i] = aux;
			direction = 1.0f;
			lastPosition = 0.0f;
			alienXDelta = 0.2f;
			alienYPosition = 0.9f;
			alienSpeed = 1.0f;

			if (i < 4 || i > 15) {
				activeAliens[i] = 0;
			}
			else {
				activeAliens[i] = 1;
			}
		}
		infoBox->setCaption(str);
		alienStartPoint();
	}

	void restartGame() {
		//Ship
		shipXPosition = 0.0f;
		shipSpeed = 1.0f;
		//Shots
		resetShots();
		//Aliens
		resetAliens();
		//Interface
		resetInterface();
	}

	void setAlienSpeed(float speed) {
		alienSpeed = speed;
	}

	void setShipSpeed(float speed) {
		shipSpeed = speed;
	}

	void setAlienQty(int qty) {
		if (qty == 12) {
			activeAliens[0] = 0;
			activeAliens[1] = 0;
			activeAliens[2] = 0;
			activeAliens[3] = 0;
			activeAliens[16] = 0;
			activeAliens[17] = 0;
			activeAliens[18] = 0;
			activeAliens[19] = 0;
		}
		if (qty == 14) {
			activeAliens[0] = 0;
			activeAliens[1] = 0;
			activeAliens[2] = 0;
			activeAliens[3] = 1;
			activeAliens[16] = 1;
			activeAliens[17] = 0;
			activeAliens[18] = 0;
			activeAliens[19] = 0;
		}
		if (qty == 16) {
			activeAliens[0] = 0;
			activeAliens[1] = 0;
			activeAliens[2] = 1;
			activeAliens[3] = 1;
			activeAliens[16] = 1;
			activeAliens[17] = 1;
			activeAliens[18] = 0;
			activeAliens[19] = 0;
		}
		if (qty == 18) {
			activeAliens[0] = 0;
			activeAliens[1] = 1;
			activeAliens[2] = 1;
			activeAliens[3] = 1;
			activeAliens[16] = 1;
			activeAliens[17] = 1;
			activeAliens[18] = 1;
			activeAliens[19] = 0;
		}
		if (qty == 20) {
			activeAliens[0] = 1;
			activeAliens[1] = 1;
			activeAliens[2] = 1;
			activeAliens[3] = 1;
			activeAliens[17] = 1;
			activeAliens[18] = 1;
			activeAliens[19] = 1;
			activeAliens[20] = 1;
		}

		std::string str = "";
		for (int i = 0; i < maxAliens; i++) {
			str += std::to_string(alienXPosition[i]) + '\n';
		}

		infoBox->setCaption(str);
	}

	void resetInterface() {
		speedSlider->setValue(alienSpeed);
		std::ostringstream buff;
		buff << std::setprecision(2) << alienSpeed;
		speedBox->setValue(buff.str());
		pointBox->setValue("0");
		alienQtyBomboBox->setSelectedIndex(0);
		shipSpeedSlider->setValue(shipSpeed);
		std::ostringstream buff1;
		buff1 << std::setprecision(2) << shipSpeed;
		shipSpeedBox->setValue(buff1.str());
	}
};