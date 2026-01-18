#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <cmath>


gps::Window myWindow;

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

glm::vec3 lightDir;
glm::vec3 lightColor;

GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint spotLightPosLoc;
GLint spotLightDirLoc;
GLint spotLightColorLoc;
GLint spotLightCutoffLoc;
GLint spotLightOuterCutoffLoc;

GLuint shadowMapFBO;
GLuint depthMap;

float moonRotation = 0.0f;

gps::SkyBox mySkybox;
gps::Shader mySkyboxShader;

gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

gps::Camera spaceShipCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));



GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

double lastX = 512, lastY = 384;
bool firstMouse = true;
float sensitivity = 0.1f;

float fogDensity = 0.01f;
GLint fogDensityLoc;
GLint skyboxFogDensityLoc;

gps::Model3D teapot;
gps::Model3D spaceship;
gps::Model3D moon;
gps::Model3D martian;
gps::Model3D starship;
GLfloat angle;

gps::Shader myBasicShader;

//Taken from lab10
void initSkybox() {
	std::vector<const GLchar*> skyBoxFaces;
	skyBoxFaces.push_back("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/cubemap/nx.png");
	skyBoxFaces.push_back("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/cubemap/px.png");
	skyBoxFaces.push_back("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/cubemap/py.png");
	skyBoxFaces.push_back("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/cubemap/ny.png");
	skyBoxFaces.push_back("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/cubemap/nz.png");
	skyBoxFaces.push_back("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/cubemap/pz.png");

	mySkybox.Load(skyBoxFaces);
}


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    myCamera.rotate(yoffset, -xoffset);

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }
	if (pressedKeys[GLFW_KEY_UP]) {
		myCamera.rotate(1.0f, 0.0f);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}
	if (pressedKeys[GLFW_KEY_DOWN]) {
		myCamera.rotate(-1.0f, 0.0f);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}
	if (pressedKeys[GLFW_KEY_LEFT]) {
		myCamera.rotate(0.0f, 1.0f);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}
	if (pressedKeys[GLFW_KEY_RIGHT]) {
		myCamera.rotate(0.0f, -1.0f);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_M]) {
        fogDensity += 0.001f;
        if (fogDensity > 1.0f) fogDensity = 1.0f;
        std::cout << "Fog density: " << fogDensity << std::endl;
        myBasicShader.useShaderProgram();
        glUniform1f(fogDensityLoc, fogDensity);
        mySkyboxShader.useShaderProgram();
        glUniform1f(skyboxFogDensityLoc, fogDensity);
    }
    if (pressedKeys[GLFW_KEY_N]) {
        fogDensity -= 0.001f;
        if (fogDensity < 0.0f) fogDensity = 0.0f;
        std::cout << "Fog density: " << fogDensity << std::endl;
        myBasicShader.useShaderProgram();
        glUniform1f(fogDensityLoc, fogDensity);
        mySkyboxShader.useShaderProgram();
        glUniform1f(skyboxFogDensityLoc, fogDensity);
    }
	if (pressedKeys[GLFW_KEY_O]) {
		myCamera.move(gps::MOVE_UP, 0.1f);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}
	if (pressedKeys[GLFW_KEY_L]) {
		myCamera.move(gps::MOVE_DOWN, 0.1f);
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}
	if (pressedKeys[GLFW_KEY_G]) {
		view = spaceShipCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view*model));

	}

	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}
void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void initShadowMap() {
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(GL_TEXTURE_2D, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048,2048,  0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    //teapot.LoadModel("models/teapot/teapot20segUT.obj");
	spaceship.LoadModel("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/spaceship/p_96_spacestation_v3.obj");
	moon.LoadModel("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/moon/Moon_2K.obj", "/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/moon/");
	martian.LoadModel("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/martian/Marvin.obj", "/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/martian/");
	starship.LoadModel("/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/starship/StarShip2.obj", "/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/starship/");
}

void initShaders() {
	myBasicShader.loadShader(
        "/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/shaders/basic.vert",
        "/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/shaders/basic.frag");
    mySkyboxShader.loadShader(
        "/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/shaders/skyboxShader.vert",
        "/Users/andreivarga/Documents/an3/GPLAB/ProiectGP/shaders/skyboxShader.frag");
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    //teapot.Draw(shader);
	auto spaceshipModel = model;
	float time = (float)glfwGetTime();
	spaceshipModel = glm::rotate(model, glm::radians(5.0f*sin(time)), glm::vec3(0.0f, 1.0f, 0.0f));

	spaceshipModel = glm::rotate(spaceshipModel, glm::radians(10.0f*cos(time)), glm::vec3(1.0f, 0.0f, 0.0f));

	spaceship.DrawSpaceship(shader, spaceshipModel);

}

void initUniforms() {
	myBasicShader.useShaderProgram();

	// create model matrix for teapot
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// compute normal matrix for teapot
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
	                              (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
	                              0.1f, 30.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	spotLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightPos");
	spotLightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightDir");
	spotLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightColor");
	glUniform3f(spotLightColorLoc, 1.0f, 1.0f, 0.6f);
	spotLightCutoffLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightCutoff");
	glUniform1f(spotLightCutoffLoc, glm::cos(glm::radians(12.5f)));
	spotLightOuterCutoffLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightOuterCutoff");
	glUniform1f(spotLightOuterCutoffLoc, glm::cos(glm::radians(17.5f)));

    fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
    glUniform1f(fogDensityLoc, fogDensity);

    skyboxFogDensityLoc = glGetUniformLocation(mySkyboxShader.shaderProgram, "fogDensity");
    mySkyboxShader.useShaderProgram();
    glUniform1f(skyboxFogDensityLoc, fogDensity);
}

void renderSkybox(gps::Shader shader) {
	mySkybox.Draw(shader, view, projection);
}

void renderMoon(gps::Shader shader) {
    shader.useShaderProgram();
    glm::mat4 moonModel = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 5.0f, -10.0f));
    moonModel = glm::scale(moonModel, glm::vec3(2.0f));
	moonModel = glm::rotate(moonModel, moonRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	moonRotation += 0.001f;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(moonModel));

    glm::mat3 moonNormalMatrix = glm::mat3(glm::inverseTranspose(view * moonModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(moonNormalMatrix));

    moon.Draw(shader);
}
	void renderMartian(gps::Shader shader) {
		shader.useShaderProgram();

		glm::mat4 martianModel = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -6.0f));
		martianModel = glm::scale(martianModel, glm::vec3(0.10f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(martianModel));

		glm::mat3 martianNormalMatrix = glm::mat3(glm::inverseTranspose(view * martianModel));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(martianNormalMatrix));

		martian.DrawMartian(shader, martianModel);
	}
void renderStarship(gps::Shader shader) {
    shader.useShaderProgram();

    float time = (float)glfwGetTime();


    float movementSpeed = 0.1f;
    float radius = 5.0f;
    float posX = radius * cos(time * movementSpeed+1000);
    float posZ = radius * sin(time * movementSpeed*2.0f + 1000) -10.0f;
    float posY = 2+5.0f*sin(-time * movementSpeed * 1.6f + 1000);

    glm::mat4 starshipModel = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
    starshipModel = glm::scale(starshipModel, glm::vec3(0.05f));

    // Orientation logic: align the model's forward axis with the direction of movement
    glm::vec3 tangent = glm::normalize(glm::vec3(-radius * movementSpeed * sin(time * movementSpeed),
                                                movementSpeed * 1.6f * cos(time * movementSpeed * 1.6f),
                                                radius * movementSpeed * 2.0f * cos(time * movementSpeed * 2.0f)));


    float yaw = atan2(tangent.x, tangent.z);
    starshipModel = glm::rotate(starshipModel, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	spaceShipCamera.rotate(0.0f, yaw);

    float pitch = -asin(tangent.y);
    starshipModel = glm::rotate(starshipModel, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	spaceShipCamera.rotate(pitch, 0.0f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(starshipModel));

    // Update spotlight uniforms
    glm::vec3 nosePos = glm::vec3(starshipModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    glm::vec3 forwardDir = glm::normalize(glm::vec3(starshipModel * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
	spaceShipCamera.setLookAt( nosePos + forwardDir);
	spaceShipCamera.moveCameraPosition(nosePos+ 0.6f*glm::normalize(forwardDir));
    glUniform3fv(spotLightPosLoc, 1, glm::value_ptr(nosePos));
    glUniform3fv(spotLightDirLoc, 1, glm::value_ptr(forwardDir));

    glm::mat3 starshipNormalMatrix = glm::mat3(glm::inverseTranspose(view * starshipModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(starshipNormalMatrix));

    starship.Draw(shader);
}


void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the scene

	// render the teapot
	renderTeapot(myBasicShader);
    renderMoon(myBasicShader);
	renderMartian(myBasicShader);
    renderStarship(myBasicShader);
	renderSkybox(mySkyboxShader);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }



    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
	initSkybox();
    setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {


        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
