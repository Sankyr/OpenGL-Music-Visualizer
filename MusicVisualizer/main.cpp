#define _CRT_SECURE_NO_WARNINGS

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "fmod_errors.h"

//#include "tiny_obj_loader.h"

#include "ShaderLoader.h"
#include "Mesh.h"
//#include "Model.h"

extern "C" {
	_declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

const double FRAMERATE = 1 / 60.0;
float scale = .5;
int windowWidth = 1920 * scale, windowHeight = 1080 * scale;

GLuint shaderProagram;
Mesh spectrumLines;

//Audio variables
FMOD::System *audioSystem = NULL;
FMOD::Channel *channel = NULL;
FMOD_DSP_PARAMETER_FFT *fft = NULL;
int sampleRate;

void FMOD_CheckResult(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		throw;
	}
}

void printFFT() {
	printf("numChannels: %d, length: %d\n", fft->numchannels, fft->length);
	for (int ch = 0; ch < fft->numchannels; ch++) {
		for (int bin = 0; bin < fft->length / 2; bin++)
			printf("%f (%f), ", fft->spectrum[ch][bin], sampleRate / ((float)fft->length)*(bin + 1));
		printf("\n");
	}
}

void initAudio() {
	// Create the main system object.
	FMOD_CheckResult(FMOD::System_Create(&audioSystem));

	// Initialize FMOD.
	FMOD_CheckResult(audioSystem->init(512, FMOD_INIT_NORMAL, 0));

	FMOD::Sound *sound;
	FMOD_CheckResult(audioSystem->createSound("Audio/Dubstep.mp3", FMOD_CREATESTREAM, NULL, &sound));

	FMOD::DSP* dsp;
	FMOD_CheckResult(audioSystem->createDSPByType(FMOD_DSP_TYPE::FMOD_DSP_TYPE_FFT, &dsp));


	FMOD_CheckResult(audioSystem->playSound(sound, NULL, true, &channel));

	FMOD_CheckResult(channel->addDSP(0, dsp));

	FMOD_CheckResult(dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void **)&fft, 0, 0, 0));

	FMOD_CheckResult(audioSystem->getSoftwareFormat(&sampleRate, NULL, NULL));
	printf("sample rate: %d\n", sampleRate);
}

void draw(GLFWwindow *window) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProagram);
	glBindVertexArray(spectrumLines.vao);
	//glUniformMatrix4fv(Uniform::mMatrix, 1, GL_FALSE, glm::value_ptr(female.transform.getTransformMatrix()));
	glDrawElements(GL_LINES, spectrumLines.indices.size(), GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
}

void update(GLFWwindow *window, double timeElapsed) {
	//numBin is fft->length / 2, twice as many verts per bin so use fft->length
	for (int i = 0; i < fft->length; i+=2) {
		//1st channel only
		spectrumLines.vertexData[i+1].position.y = 25*fft->spectrum[0][i/2];
	}
	spectrumLines.reuploadVertexDataToGPU();
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && (action != GLFW_RELEASE)) {
		bool paused;
		FMOD_CheckResult(channel->getPaused(&paused));
		FMOD_CheckResult(channel->setPaused(!paused));
	}
}

void resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, (GLuint)width, (GLuint)height);
	glfwGetFramebufferSize(window, &width, &height);
}

void init() {
	int numBins = 1024;//fft->length / 2
	spectrumLines.vertexData.reserve(2*numBins);
	spectrumLines.indices.reserve(2 * numBins);
	for (int i = 0; i < numBins; i++) {
		spectrumLines.indices.push_back(2*i);
		spectrumLines.indices.push_back(2*i+1);
		spectrumLines.vertexData.push_back(VertexData(glm::vec3(((float)i)/numBins*2-1, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)));
		spectrumLines.vertexData.push_back(VertexData(glm::vec3(((float)i) / numBins*2-1, 1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0)));
	}

	spectrumLines.setupMesh();

	shaderProagram = loadShaders("Shaders/BarShader.vert", "Shaders/BarShader.frag");

	//glUseProgram(shaderProagram);
	//glUniformMatrix4fv(Uniform::pMatrix, 1, GL_FALSE, glm::value_ptr(glm::perspective(fov, aspect, near, far)));

	glEnable(GL_DEPTH_TEST);
}

int main() {
	srand((unsigned int)time(NULL));

	GLFWwindow *window;

	//Initialize the glfw library (MUST COME FIRST)
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	window = glfwCreateWindow(windowWidth, windowHeight, "Music Visualizer", NULL, NULL);
	//fullscreen
	//window = glfwCreateWindow((int)(WIDTH * WINDOW_SCALE), (int)(HEIGHT * WINDOW_SCALE), "CVDI Project", glfwGetPrimaryMonitor(), NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//set callbacks
	glfwSetKeyCallback(window, keyboard);
	glfwSetFramebufferSizeCallback(window, resize);
	/*glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, cursorPosition);
	glfwSetScrollCallback(window, scrollWheel);
	
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetWindowPosCallback(window, windowMoved);*/

	//Must be here because glew
	glewExperimental = GL_TRUE;

	if (glewInit())
		exit(EXIT_FAILURE);

	//init opengl parameters
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//make some objects
	//glfwGetFramebufferSize(window, &width, &height);

	initAudio();
	init();

	//render loop
	double timeNow = 0, timePrev = 0, timeElapsed;
	while (!glfwWindowShouldClose(window)) {
		timeNow = glfwGetTime();
		timeElapsed = timeNow - timePrev;

		if (timeElapsed > FRAMERATE) {
			timePrev = timeNow;
			FMOD_CheckResult(audioSystem->update());
			//printFFT();
			update(window, timeElapsed);
			draw(window);
			glfwPollEvents();
			//printf("FPS: %d\t\r", (int)(1 / timeElapsed));
		}
	}

	//cleanup
	FMOD_CheckResult(audioSystem->release());
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

	
}