//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#define lightNum 6

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Model3DRGBA.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include "Keyframe.hpp"

#include <iostream>

int glWindowWidth = 1600;
int glWindowHeight = 900;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 16384;
const unsigned int SHADOW_HEIGHT = 16384;

glm::mat4 model;
GLuint modelLoc;
GLuint modelLocReflection;
glm::mat4 view;
GLuint viewLoc;
GLuint viewLocReflection;
glm::mat4 projection;
GLuint projectionLoc;
GLuint projectionLocReflection;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
GLuint normalMatrixLocReflection;
glm::mat4 sunlightRotation;
glm::vec3 sunlightDir = glm::vec3(-12.5f, 50.0f, -50.0f);
GLuint sunlightDirLoc;
glm::vec3 sunlightColor;
GLuint sunlightColorLoc;

GLfloat alpha;
GLuint alphaLoc;
GLuint alphaLocReflection;
GLuint fogLoc;
GLuint fogLocReflection;

GLuint lightEnabledLoc;
GLuint lightPositionLoc;
GLuint lightDirectionLoc;
GLuint lightColorLoc;
GLuint lightFOVLoc;
GLuint sunlightStrengthLoc;
GLuint lightStrengthLoc;

GLuint skyboxBoostLoc;

GLuint ambientStrengthLoc;
int flat;
GLuint flatLoc;

GLfloat sunlightAngle;

gps::Camera myCamera(
				glm::vec3(167.057281f, 25.313057f, 63.185169f),
				glm::vec3(166.357254f, 25.059582f, 63.852783f),
				glm::vec3(-0.183431f, 0.967342f, 0.174937f),
				true);
float cameraSpeed = 0.0f;
float cameraSpeedMax = 1.0f;
float cameraAcceleration = 0.002f;

bool pressedKeys[1024];
float angleY = 0.0f;

gps::Model3D track;
gps::Model3DRGBA transparent;
gps::Model3D car_body;
gps::Model3D car_hatch;
gps::Model3D car_backlight_left;
gps::Model3D car_backlight_right;
gps::Model3D car_stoplight_left;
gps::Model3D car_stoplight_right;
gps::Model3D car_headlight_left;
gps::Model3D car_headlight_right;
gps::Model3D car_hub_left;
gps::Model3D car_hub_right;
gps::Model3D car_wheel_front_left;
gps::Model3D car_wheel_front_right;
gps::Model3D car_wheel_back_left;
gps::Model3D car_wheel_back_right;
gps::Model3D alphaObj;
gps::Model3D reflectiveObj;
gps::Model3D lightCube;
gps::Model3D screenQuad;

gps::Model3D csys;

gps::SkyBox skybox;

gps::Shader myCustomShader;
gps::Shader screenQuadShader;
gps::Shader depthShader;
gps::Shader skyboxShader;
gps::Shader reflectionShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;
GLfloat near_plane = 0.1f, far_plane = 255.0f;

enum CameraMode {
	FREE,
	PRESENTATION,
	FOLLOW_CAR,
	INSIDE_CAR
};
CameraMode cameraStatus;
float last_xpos, last_ypos;
const float PITCH_MULT = 0.0025, YAW_MULT = 0.0025;
bool firstRun = true;
bool replace_start_frame = false;
milliseconds animationLengthFromPosition = 1000ms;
milliseconds animation_initial_time;
int keyframe_index;
enum CarSteering {
	STRAIGHT = 0,
	RIGHT = -1,
	LEFT = 1
};
float car_heading = -90.0f; //degrees
float car_rotation_speed_current = 0.0f;
milliseconds car_rotation_max_out_time = 100ms;
float car_rotation_speed_max = 0.1f; //degrees


CarSteering carSteering = STRAIGHT;
CarSteering carLastSteering = STRAIGHT;
CarSteering carPreviousSteering = STRAIGHT;
milliseconds carSteeringChangedTimestamp = 0ms;
float last_car_steering_factor = 0.0f;
float car_steering_factor = 0.0f;

glm::vec3 car_position = glm::vec3(11.0f, 1.4f, 115.0f);
float car_speed = 0.0f;
float last_car_speed = 0.0f;
float car_speed_increment = 0.001f;
float car_speed_decrement = 0.003f;
float car_speed_rolling_friction_coefficient = 0.00001f;
float car_speed_air_friction_coefficient = 0.0001f;

float steering_animation_multiplier = 15.0f;
float odometry = 0.0f;
float rotationMultiplierFront = 1.3f; //obtained from wheel dimensions and adjusted
float rotationMultiplierBack = 1.15f; //obtained from wheel dimensions and adjusted

milliseconds last_time;
milliseconds current_time;

GLfloat sunlightStrength;
GLfloat sunlightStrengthMoon = 0.02f;
GLfloat sunlightStrengthSun = 0.5f;

GLfloat skyboxBoost;
GLfloat skyboxBoostMoon = 0.1f;
GLfloat skyboxBoostSun = 2.0f;

GLfloat ambientStrengthDefault = 0.2f;
GLfloat ambientStrengthSunlight = 1.0f;
GLfloat ambientStrengthLights = 50.0f;

bool l_pressed_last = false;
bool headlights_on = false;
bool stoplights_on = false;
bool backlights_on = false;
bool car_should_stop = false;
bool f_pressed_last = false;
bool e_pressed_last = false;
bool q_pressed_last = false;

bool debug_mode = false;

float fov = 60.0f;

int fog;

std::vector<gps::Keyframe> keyframe_vector{
	gps::Keyframe(glm::vec3(256.199f, 114.94f, -94.55f), glm::vec3(255.68f, 114.26f, -94.02f), glm::vec3(-0.473771f, 0.732525f, 0.488823f), 0ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(256.199f, 114.94f, -94.55f), glm::vec3(255.68f, 114.26f, -94.02f), glm::vec3(-0.473771f, 0.732525f, 0.488823f), 3000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(-2.60f, 13.49f, 176.25f), glm::vec3(-1.67f, 13.11f, 176.25f), glm::vec3(0.381362f, 0.924425f, 0.001177f), 5000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(-2.60f, 13.49f, 176.25f), glm::vec3(-1.67f, 13.11f, 176.25f), glm::vec3(0.381362f, 0.924425f, 0.001177f), 8000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(142.63f, 13.49f, 176.25f), glm::vec3(143.56f, 13.11f, 176.25f), glm::vec3(0.381362f, 0.924425f, 0.001177f), 12000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(121.67f, 8.45f, 197.66f), glm::vec3(121.55f, 8.00f, 198.54f), glm::vec3(-0.060186f, 0.889855f, 0.452257f), 13000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(121.67f, 8.45f, 197.66f), glm::vec3(121.55f, 8.00f, 198.54f), glm::vec3(-0.060186f, 0.889855f, 0.452257f), 16000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(15.46f, 2.94f, 111.25f), glm::vec3(14.68f, 2.62f, 111.79f), glm::vec3(-0.263895f, 0.947244f, 0.181901f), 18000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(15.46f, 2.94f, 111.25f), glm::vec3(14.68f, 2.62f, 111.79f), glm::vec3(-0.263895f, 0.947244f, 0.181901f), 21000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(43.90f, 13.84f, 89.93f), glm::vec3(81.49f, 10.57f, 74.42f), glm::vec3(0.216423f, 0.971035f, -0.101254f), 23000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(43.90f, 13.84f, 89.93f), glm::vec3(81.49f, 10.57f, 74.42f), glm::vec3(0.216423f, 0.971035f, -0.101254f), 25000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(117.43f, 13.84f, 89.93f), glm::vec3(81.49f, 10.57f, 74.42f), glm::vec3(-0.216423f, 0.971035f, -0.101254f), 27000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(117.43f, 13.84f, 89.93f), glm::vec3(81.49f, 10.57f, 74.42f), glm::vec3(-0.216423f, 0.971035f, -0.105182f), 29000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(619.07f, 692.78f, -490.38f), glm::vec3(618.54f, 692.11f, -489.86f), glm::vec3(-0.475133f, 0.744350f, 0.469246f), 31000ms, gps::Keyframe::ANGULAR)
	,gps::Keyframe(glm::vec3(619.07f, 692.78f, -490.38f), glm::vec3(618.54f, 692.11f, -489.86f), glm::vec3(-0.475133f, 0.744350f, 0.469246f), 35000ms, gps::Keyframe::ANGULAR)
};

std::vector<const GLchar*> faces{
	"textures/skybox/px.png"
	,"textures/skybox/nx.png"
	,"textures/skybox/py.png"
	,"textures/skybox/ny.png"
	,"textures/skybox/pz.png"
	,"textures/skybox/nz.png"
};

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 10000.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (!firstRun && cameraStatus == FREE) {
		myCamera.rotate(-PITCH_MULT * (ypos - last_ypos), -YAW_MULT * (xpos - last_xpos));
	}
	else {
		firstRun = false;
	}

	myCustomShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	last_xpos = xpos;
	last_ypos = ypos;
}

void processMovement()
{
	//camera movement
	if (cameraStatus == FREE && (pressedKeys[GLFW_KEY_W] || pressedKeys[GLFW_KEY_S] || pressedKeys[GLFW_KEY_A] || pressedKeys[GLFW_KEY_D]) || pressedKeys[GLFW_KEY_LEFT_SHIFT] || pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
		cameraSpeed = cameraSpeed > cameraSpeedMax ? cameraSpeedMax : cameraSpeed + cameraAcceleration * (current_time.count() - last_time.count());
		if (pressedKeys[GLFW_KEY_W] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_S] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_A] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_D] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_LEFT_SHIFT] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_UP, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_LEFT_CONTROL] && cameraStatus == FREE) {
			myCamera.move(gps::MOVE_DOWN, cameraSpeed);
		}
	}
	else {
		cameraSpeed = 0.0f;
	}

	//camera locks
	if (pressedKeys[GLFW_KEY_0]) {
		if (cameraStatus != PRESENTATION) {
			animation_initial_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			keyframe_index = 0;
			if (replace_start_frame) {
				keyframe_vector.at(0) = gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), 0ms);
				keyframe_vector.at(keyframe_vector.size() - 1) = gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), keyframe_vector.at(keyframe_vector.size() - 1).getTimestamp(), gps::Keyframe::ANGULAR);
				keyframe_vector.at(keyframe_vector.size() - 1).incrementTimestamp(animationLengthFromPosition);
			}
			else {
				keyframe_vector.push_back(gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), keyframe_vector.at(keyframe_vector.size()-1).getTimestamp(), gps::Keyframe::ANGULAR));
				keyframe_vector.at(keyframe_vector.size() - 1).incrementTimestamp(animationLengthFromPosition);
				keyframe_vector.insert(keyframe_vector.begin(), gps::Keyframe(myCamera.getPosition(), myCamera.getTarget(), myCamera.getUp(), 0ms, gps::Keyframe::ANGULAR));
				for (int i = 1; i < keyframe_vector.size(); i++) {
					keyframe_vector.at(i).incrementTimestamp(animationLengthFromPosition);
				}
				replace_start_frame = true;
			}
			cameraStatus = PRESENTATION;
		}
	}
	if (pressedKeys[GLFW_KEY_1]) {
		if (cameraStatus != FOLLOW_CAR) {
			cameraStatus = FOLLOW_CAR;
		}
	}
	if (pressedKeys[GLFW_KEY_2]) {
		if (cameraStatus != INSIDE_CAR) {
			cameraStatus = INSIDE_CAR;
		}
	}
	if (pressedKeys[GLFW_KEY_9]) {
		if (cameraStatus != FREE) {
			cameraStatus = FREE;
		}
	}

	//car acceleration
	backlights_on = false;
	stoplights_on = false;
	if (pressedKeys[GLFW_KEY_UP] ^ pressedKeys[GLFW_KEY_DOWN]) {
		if (car_speed != 0.0 || !car_should_stop) {
			last_car_speed = car_speed;
			if (pressedKeys[GLFW_KEY_UP]) {
				if (car_speed < 0) {
					car_speed += car_speed_decrement * (current_time.count() - last_time.count());
					car_should_stop = true;
				}
				else {
					car_speed += car_speed_increment * (current_time.count() - last_time.count());
				}
			}
			if (pressedKeys[GLFW_KEY_DOWN]) {
				if (car_speed > 0) {
					car_speed -= car_speed_decrement * (current_time.count() - last_time.count());
					car_should_stop = true;
				}
				else {
					car_speed -= car_speed_increment * (current_time.count() - last_time.count());
				}
			}
			if (car_speed * last_car_speed < 0) {
				car_speed = 0.0f;
			}
		}
	}
	else {
		car_should_stop = false;
	}

	if (car_speed > (car_speed_rolling_friction_coefficient + car_speed_air_friction_coefficient * car_speed * car_speed) && car_speed > 0.0001f) {
		car_speed -= (car_speed_rolling_friction_coefficient + car_speed_air_friction_coefficient * car_speed * car_speed) * (current_time.count() - last_time.count());
	}
	else if (car_speed < -(car_speed_rolling_friction_coefficient + car_speed_air_friction_coefficient * car_speed * car_speed) && car_speed < -0.0001f) {
		car_speed += (car_speed_rolling_friction_coefficient + car_speed_air_friction_coefficient * car_speed * car_speed) * (current_time.count() - last_time.count());
	}
	else {
		car_speed = 0.0f;
	}

	//steering
	carLastSteering = carSteering;
	carSteering = STRAIGHT;
	if (car_speed > 0.0f) {
		car_heading += car_rotation_speed_current * (current_time.count() - last_time.count());
	}
	else if (car_speed < 0.0f) {
		car_heading -= car_rotation_speed_current * (current_time.count() - last_time.count());
	}
	if (pressedKeys[GLFW_KEY_LEFT] ^ pressedKeys[GLFW_KEY_RIGHT]) {
		if (pressedKeys[GLFW_KEY_LEFT]) {
			carSteering = LEFT;
		}
		else {
			carSteering = RIGHT;
		}
	}
	if (carSteering != carLastSteering) {
		carSteeringChangedTimestamp = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		carPreviousSteering = carLastSteering;
	}

	//headlights
	if (pressedKeys[GLFW_KEY_L] != l_pressed_last) {
		if (pressedKeys[GLFW_KEY_L]) {
			headlights_on = !headlights_on;
		}
		l_pressed_last = pressedKeys[GLFW_KEY_L];
	}

	//backlights
	if (car_speed < 0) {
		backlights_on = true;
	}

	//stoplights
	else if (pressedKeys[GLFW_KEY_DOWN]) {
		stoplights_on = true;
	}

	if (pressedKeys[GLFW_KEY_F1]) { //solid
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (pressedKeys[GLFW_KEY_F2]) { //wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_F3]) { //point
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (pressedKeys[GLFW_KEY_F4]) { //flat
		flat = 2;
	}
	if (pressedKeys[GLFW_KEY_F5]) { //normals flat
		flat = 1;
	}
	if (pressedKeys[GLFW_KEY_F6]) { //smooth
		flat = 0;
	}

	//sunlight rotation
	if (pressedKeys[GLFW_KEY_R]) {
		sunlightAngle += 0.1f;
	}

	//fog
	if (pressedKeys[GLFW_KEY_F] != f_pressed_last) {
		if (pressedKeys[GLFW_KEY_F]) {
			if (fog == 0) {	
				fog = 1;
			}
			else {
				fog = 0;
			}
		}
		f_pressed_last = pressedKeys[GLFW_KEY_F];
	}

	//sunlight or moonlight
	if (pressedKeys[GLFW_KEY_E] != e_pressed_last) {
		if (pressedKeys[GLFW_KEY_E]) {
			if (sunlightStrength == sunlightStrengthMoon) {
				sunlightStrength = sunlightStrengthSun;
				skyboxBoost = skyboxBoostSun;
			}
			else {
				sunlightStrength = sunlightStrengthMoon;
				skyboxBoost = skyboxBoostMoon;
			}
		}
		e_pressed_last = pressedKeys[GLFW_KEY_E];
	}

	//display current camera position
	if (pressedKeys[GLFW_KEY_Q] != q_pressed_last) {
		if (pressedKeys[GLFW_KEY_Q]) {
			fprintf(stdout, "Position: %ff, %ff, %ff\nTarget %ff, %ff, %ff\nUp %ff, %ff, %ff\n", 
				myCamera.getPosition().x, myCamera.getPosition().y, myCamera.getPosition().z,
				myCamera.getTarget().x, myCamera.getTarget().y, myCamera.getTarget().z,
				myCamera.getUp().x, myCamera.getUp().y, myCamera.getUp().z);
		}
		q_pressed_last = pressedKeys[GLFW_KEY_Q];
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initObjects() {
	//csys.LoadModel("objects/csys/csys.obj"); //reprezentare a sistemului de coordonate, doar pentru debugging
	track.LoadModel("objects/track3/track3v2.obj");
	transparent.LoadModel("objects/track3/transparent.obj");

	car_body.LoadModel("objects/future_car_2/body.obj");
	car_hatch.LoadModel("objects/future_car_2/hatch.obj");
	car_backlight_left.LoadModel("objects/future_car_2/backlight_left.obj");
	car_backlight_right.LoadModel("objects/future_car_2/backlight_right.obj");
	car_stoplight_left.LoadModel("objects/future_car_2/stoplight_left.obj");
	car_stoplight_right.LoadModel("objects/future_car_2/stoplight_right.obj");
	car_headlight_left.LoadModel("objects/future_car_2/headlight_left.obj");
	car_headlight_right.LoadModel("objects/future_car_2/headlight_right.obj");
	car_hub_left.LoadModel("objects/future_car_2/hub_left.obj");
	car_hub_right.LoadModel("objects/future_car_2/hub_right.obj");
	car_wheel_front_left.LoadModel("objects/future_car_2/wheel_front_left.obj");
	car_wheel_front_right.LoadModel("objects/future_car_2/wheel_front_right.obj");
	car_wheel_back_left.LoadModel("objects/future_car_2/wheel_back_left.obj");
	car_wheel_back_right.LoadModel("objects/future_car_2/wheel_back_right.obj");

	alphaObj.LoadModel("objects/track3/alpha.obj");
	reflectiveObj.LoadModel("objects/track3/reflect.obj");

	//lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");

	skybox.Load(faces);
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthShader.loadShader("shaders/depthShader.vert", "shaders/depthShaders.frag");
	depthShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	reflectionShader.loadShader("shaders/reflectionShader.vert", "shaders/reflectionShader.frag");
	reflectionShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 10000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	sunlightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(sunlightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	sunlightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sunlightDir");	
	glUniform3fv(sunlightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * sunlightRotation)) * sunlightDir));

	//set light color
	sunlightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	sunlightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sunlightColor");
	glUniform3fv(sunlightColorLoc, 1, glm::value_ptr(sunlightColor));

	//set default ambient strength
	ambientStrengthLoc = glGetUniformLocation(myCustomShader.shaderProgram, "ambientStrengthTexture");
	glUniform1f(ambientStrengthLoc, ambientStrengthDefault);

	//set sunlight strength
	sunlightStrength = sunlightStrengthMoon;
	sunlightStrengthLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sunlightStrength");
	glUniform1f(sunlightStrengthLoc, sunlightStrength);

	//flat
	flat = 0;
	flatLoc = glGetUniformLocation(myCustomShader.shaderProgram, "flatness");
	glUniform1i(flatLoc, flat);

	//alpha
	alpha = 1.0f;
	alphaLoc = glGetUniformLocation(myCustomShader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, alpha);

	//fog
	fog = 0;
	fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "enableFog");
	glUniform1i(fogLoc, fog);

	//reflection
	reflectionShader.useShaderProgram();
	modelLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLocReflection, 1, GL_FALSE, glm::value_ptr(model));
	viewLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLocReflection, 1, GL_FALSE, glm::value_ptr(view));
	projectionLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLocReflection, 1, GL_FALSE, glm::value_ptr(projection));
	normalMatrixLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLocReflection, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	alphaLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "alpha");
	glUniform1f(alphaLocReflection, alpha);
	fogLocReflection = glGetUniformLocation(reflectionShader.shaderProgram, "enableFog");
	glUniform1i(fogLocReflection, fog);

	//skybox
	skyboxShader.useShaderProgram();
	skyboxBoost = skyboxBoostMoon;
	skyboxBoostLoc = glGetUniformLocation(skyboxShader.shaderProgram, "boost");
	glUniform1f(skyboxBoostLoc, skyboxBoost);
}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::vec3(sunlightRotation * glm::vec4(sunlightDir, 1.0f)), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-700.0f, 700.0f, -700.0f, 700.0f, near_plane, far_plane);
	return lightProjection * lightView;
}

glm::mat4 rotateAroundPoint(glm::mat4 initialMatrix, glm::vec3 point, float angle, glm::vec3 rotationAxis) {
	glm::mat4 matrix = glm::translate(initialMatrix, point);
	matrix = glm::rotate(matrix, angle, rotationAxis);
	matrix = glm::translate(matrix, -point);
	return matrix;
}

glm::mat4 rotateAroundPoint(glm::vec3 point, float angle, glm::vec3 rotationAxis) {
	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), point);
	matrix = glm::rotate(matrix, angle, rotationAxis);
	matrix = glm::translate(matrix, -point);
	return matrix;
}

void calculateAnimations() {
	glm::mat4 car_vector_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 rotation_vector = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	car_position += -car_speed * glm::vec3(car_vector_rotation_matrix * rotation_vector);
	odometry += car_speed;
	if (debug_mode) {
		fprintf(stdout, "odometry: %f\n", odometry);
	}

	car_steering_factor = ((float)(current_time - carSteeringChangedTimestamp).count()) / car_rotation_max_out_time.count() ;
	if (carSteering == STRAIGHT) {
		if (car_steering_factor > 1.0f) {
			car_rotation_speed_current = 0;
		}
		else {
			if (glm::sign(last_car_steering_factor - car_steering_factor) != last_car_steering_factor) {
				car_rotation_speed_current = 0;
			}
			else {
				car_rotation_speed_current = (last_car_steering_factor - car_steering_factor) * carPreviousSteering * car_rotation_speed_max;
			}
		}
	}
	else {
		last_car_steering_factor = car_steering_factor > 1.0f ? 1.0f : car_steering_factor;
		if (car_steering_factor > 1.0f) {
			car_rotation_speed_current = carSteering * car_rotation_speed_max;
		}
		else {
			car_rotation_speed_current = car_steering_factor * carSteering * car_rotation_speed_max;
		}
	}
	if (debug_mode) {
		fprintf(stdout, "car_rotation_speed_current = %f\n", car_rotation_speed_current);
	}
}

glm::mat4 generateCarModel() {
	return glm::translate(rotateAroundPoint(car_position, glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f)), car_position);
}

void drawObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	if (!depthPass) {
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		alpha = 1.0f;
		glUniform1f(alphaLoc, alpha);
		glUniform1i(flatLoc, flat);
		glUniform1f(sunlightStrengthLoc, sunlightStrength);
		//glUniform1i(fogLoc, fog);
	}
	
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	track.Draw(shader);
	transparent.Draw(shader);

	model = generateCarModel();
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car_body.Draw(shader);

	model = rotateAroundPoint(car_position, glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(-2.45493f, -0.257394f, 0.95001f), glm::radians(car_rotation_speed_current * steering_animation_multiplier * (current_time.count() - last_time.count())), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, car_position + glm::vec3(-2.45493f, -0.257394f, 0.95001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car_hub_left.Draw(shader);

	model = rotateAroundPoint(car_position, glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(-2.45493f, -0.257394f, 0.95001f), glm::radians(car_rotation_speed_current * steering_animation_multiplier * (current_time.count() - last_time.count())), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(-2.45493f, -0.257394f, 0.95001f), odometry * rotationMultiplierFront, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, car_position + glm::vec3(-2.45493f, -0.257394f, 0.95001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car_wheel_front_left.Draw(shader);

	model = rotateAroundPoint(car_position, glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(-2.45493f, -0.257394f, -0.95001f), glm::radians(car_rotation_speed_current * steering_animation_multiplier * (current_time.count() - last_time.count())), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, car_position + glm::vec3(-2.45493f, -0.257394f, -0.95001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car_hub_right.Draw(shader);

	model = rotateAroundPoint(car_position, glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(-2.45493f, -0.257394f, -0.95001f), glm::radians(car_rotation_speed_current * steering_animation_multiplier * (current_time.count() - last_time.count())), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(-2.45493f, -0.257394f, -0.95001f), odometry * rotationMultiplierFront, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, car_position + glm::vec3(-2.45493f, -0.257394f, -0.95001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car_wheel_front_right.Draw(shader);

	model = rotateAroundPoint(car_position, glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(2.63676f, -0.151724f, 1.6113f), odometry * rotationMultiplierBack, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, car_position + glm::vec3(2.63676f, -0.151724f, 1.6113f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car_wheel_back_left.Draw(shader);

	model = rotateAroundPoint(car_position, glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
	model = rotateAroundPoint(model, car_position + glm::vec3(2.63676f, -0.151724f, -1.6113f), odometry * rotationMultiplierBack, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, car_position + glm::vec3(2.63676f, -0.151724f, -1.6113f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	car_wheel_back_right.Draw(shader);

	model = generateCarModel();
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	if (headlights_on && !depthPass) {
		glUniform1f(ambientStrengthLoc, ambientStrengthLights);
		car_headlight_left.Draw(shader);
		car_headlight_right.Draw(shader);
		glUniform1f(ambientStrengthLoc, ambientStrengthDefault);
	}
	else {
		car_headlight_left.Draw(shader);
		car_headlight_right.Draw(shader);
	}
	if (stoplights_on && !depthPass) {
		glUniform1f(ambientStrengthLoc, ambientStrengthLights);
		car_stoplight_left.Draw(shader);
		car_stoplight_right.Draw(shader);
		glUniform1f(ambientStrengthLoc, ambientStrengthDefault);
	}
	else {
		car_stoplight_left.Draw(shader);
		car_stoplight_right.Draw(shader);
	}
	if (backlights_on && !depthPass) {
		glUniform1f(ambientStrengthLoc, ambientStrengthLights);
		car_backlight_left.Draw(shader);
		car_backlight_right.Draw(shader);
		glUniform1f(ambientStrengthLoc, ambientStrengthDefault);
	}
	else {
		car_backlight_left.Draw(shader);
		car_backlight_right.Draw(shader);
	}
}

void drawLights(gps::Shader shader) {
	shader.useShaderProgram();
	int lightEnabled[6]{ headlights_on, headlights_on, stoplights_on, stoplights_on, backlights_on, backlights_on };
	lightEnabledLoc = glGetUniformLocation(shader.shaderProgram, "lightEnabled");
	glUniform1iv(lightEnabledLoc, lightNum, lightEnabled);

	model = generateCarModel();

	glm::vec3 lightPosition[6]{
		glm::vec3(model * glm::vec4(glm::vec3(-3.0f, 0.035446f, 0.52336f), 1.0f))
		,glm::vec3(model * glm::vec4(glm::vec3(-3.0f, 0.035446f, -0.52336f), 1.0f))
		,glm::vec3(model * glm::vec4(glm::vec3(3.4f, 0.49779f, 0.8808f), 1.0f))
		,glm::vec3(model * glm::vec4(glm::vec3(3.4f, 0.49779f, -0.8808f), 1.0f))
		,glm::vec3(model * glm::vec4(glm::vec3(3.6f, 0.449269f, 0.700796f), 1.0f))
		,glm::vec3(model * glm::vec4(glm::vec3(3.6f, 0.449269f, -0.700796f), 1.0f))
	};
	lightPositionLoc = glGetUniformLocation(shader.shaderProgram, "lightPosition");
	glUniform3fv(lightPositionLoc, lightNum, glm::value_ptr(lightPosition[0]));

	glm::vec3 lightDirection[6]{
		glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(car_heading), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
		,glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(car_heading), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
		,glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(car_heading + 180), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
		,glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(car_heading + 180), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
		,glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(car_heading + 180), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
		,glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(car_heading + 180), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
	};
	lightDirectionLoc = glGetUniformLocation(shader.shaderProgram, "lightDirection");
	glUniform3fv(lightDirectionLoc, lightNum, glm::value_ptr(lightDirection[0]));

	glm::vec3 lightColor[6]{
		glm::vec3(1.0f,1.0f,1.0f)
		,glm::vec3(1.0f,1.0f,1.0f)
		,glm::vec3(1.0f,0.0f,0.0f)
		,glm::vec3(1.0f,0.0f,0.0f)
		,glm::vec3(1.0f,1.0f,1.0f)
		,glm::vec3(1.0f,1.0f,1.0f)
	};
	lightColorLoc = glGetUniformLocation(shader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, lightNum, glm::value_ptr(lightColor[0]));

	float lightFOV[6]{
		120.0f
		,120.0f
		,150.0f
		,150.0f
		,150.0f
		,150.0f
	};
	lightFOVLoc = glGetUniformLocation(shader.shaderProgram, "lightFOV");
	glUniform1fv(lightFOVLoc, lightNum, lightFOV);

	float lightStrength[6]{
		2.0f
		,2.0f
		,0.2f
		,0.2f
		,0.2f
		,0.2f
	};
	lightStrengthLoc = glGetUniformLocation(shader.shaderProgram, "lightStrength");
	glUniform1fv(lightStrengthLoc, lightNum, lightStrength);
}

void drawReflectiveObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLocReflection, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLocReflection, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		alpha = 1.0f;
		glUniform1f(alphaLocReflection, alpha);
		glUniform1i(fogLocReflection, fog);
	}
	reflectiveObj.Draw(shader);
}

void drawTransparentObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	model = generateCarModel();
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		alpha = 0.5f;
		glUniform1f(alphaLoc, alpha);
		glUniform1i(fogLoc, fog);
	}
	car_hatch.Draw(shader);

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	if (!depthPass) {
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		alpha = 0.5f;
		glUniform1f(alphaLoc, alpha);
	}
	alphaObj.Draw(shader);
}

void drawSkybox() {
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
	glUniform1f(skyboxBoostLoc, skyboxBoost);
	skybox.Draw(skyboxShader, view, projection);
}

void renderScene() {
	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	depthShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthShader, true);
	drawReflectiveObjects(depthShader, true);
	drawTransparentObjects(depthShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// render depth map on screen - toggled with the M key
	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT);
		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {
		// final scene rendering pass (with shadows)
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		sunlightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(sunlightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(sunlightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * sunlightRotation)) * sunlightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		calculateAnimations();
		switch (cameraStatus) {
		case PRESENTATION:
			if (keyframe_index == keyframe_vector.size() - 2 && keyframe_vector.at(keyframe_index + 1).getTimestamp() <= current_time - animation_initial_time) {
				cameraStatus = FREE;
			}
			else {
				if (keyframe_vector.at(keyframe_index + 1).getTimestamp() <= current_time - animation_initial_time) {
					keyframe_index++;
				}
				gps::Keyframe interpolatedKeyframe = keyframe_vector.at(keyframe_index).getInterpolatedKeyframe(keyframe_vector.at(keyframe_index + 1), current_time - animation_initial_time);
				myCamera = gps::Camera(interpolatedKeyframe.getPostionVec(), interpolatedKeyframe.getTargetVec(), interpolatedKeyframe.getUpVec(), true);
			}
			break;
		case FOLLOW_CAR:
			glm::mat4 car_vector_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(car_heading), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 camera_vector_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(car_heading + 180), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 cameraPosition = car_position - 8.0f * glm::vec3(camera_vector_rotation_matrix * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)) + glm::vec3(0.0f, 3.0f, 0.0f);
			glm::vec3 cameraTarget = car_position - glm::vec3(car_vector_rotation_matrix * glm::vec4(4.0f, 0.0f, 0.0f, 1.0f));
			myCamera = gps::Camera(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f), true);
			break;
		case INSIDE_CAR:
			glm::mat4 camera_vector_rotation_matrix_inside = glm::rotate(glm::mat4(1.0f), glm::radians(car_heading + 180), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 cameraPositionInside = car_position + 1.1f * glm::vec3(camera_vector_rotation_matrix_inside * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)) + glm::vec3(0.0f, 0.6f, 0.0f);
			glm::vec3 cameraTargetInside = car_position + 2.0f * glm::vec3(camera_vector_rotation_matrix_inside * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)) + glm::vec3(0.0f, 0.4f, 0.0f);
			myCamera = gps::Camera(cameraPositionInside, cameraTargetInside, glm::vec3(0.0f, 1.0f, 0.0f), true);
			break;
		}
		drawLights(myCustomShader);
		drawObjects(myCustomShader, false);
		drawReflectiveObjects(reflectionShader, false);
		drawSkybox();
		drawTransparentObjects(myCustomShader, false);
	}
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	current_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	while (!glfwWindowShouldClose(glWindow)) {
		last_time = current_time;
		current_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		if (debug_mode) {
			fprintf(stdout, "current time %lld\n", current_time - animation_initial_time);
		}
		processMovement();
		renderScene();		
		glCheckError();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
