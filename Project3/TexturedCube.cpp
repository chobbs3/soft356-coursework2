//////////////////////////////////////////////////////////////////////////////
//
//  Triangles.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "LoadShaders.h"
#include <glm/glm.hpp> //includes GLM
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/ext/matrix_clip_space.hpp> // GLM: perspective and ortho 
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include<vector>
#include <string>

#include "TexturedCube.h"
#include <filesystem>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

enum VAO_IDs { Triangles, Indices, Colours, Tex, NumVAOs = 1 };
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
enum Attrib_IDs { vPosition = 0, tPosition = 1, nPosition = 2 };
GLuint program; 
GLuint shaderModuleMatrix;

/*
GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];*/

class model {

	GLuint texture1;
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
	std::vector< glm::vec3 > obj_vertices;
	std::vector< glm::vec2 > obj_uvs;
	std::vector< glm::vec3 > obj_normals;
	#define BUFFER_OFFSET(a) ((void*)(a))
	GLuint vertexVAO;

public:
	void clearData() {
		obj_vertices.clear();
		obj_uvs.clear();
		obj_normals.clear();

		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &uvBuffer);
		glDeleteBuffers(1, &normalBuffer);
		glDeleteTextures(1, &texture1);
		glDeleteVertexArrays(1, &vertexVAO);
	}

	bool loadmodel(std::string objfilename, std::string textfilename )
	{
		glGenVertexArrays(1, &vertexVAO);
		glBindVertexArray(vertexVAO);


		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< glm::vec3 > temp_vertices;
		std::vector< glm::vec2 > temp_uvs;
		std::vector< glm::vec3 > temp_normals;



		FILE* file;
		fopen_s(&file, objfilename.c_str(), "rb");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			return false;
		}

		try {
			unsigned int counter = 0;
			while (1) {

				char line[255];
				if (fgets(line, 255, file) == NULL)
					break; // EOF = End Of File. Quit the loop.


				glm::vec3 vertex;
				glm::vec2 uv;
				glm::vec3 normal;

				if (sscanf_s(line, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z) > 0) {


					temp_vertices.push_back(vertex);
				}
				if (sscanf_s(line, "vt %f %f\n", &uv.x, &uv.y) > 0) {


					temp_uvs.push_back(uv);
				}
				if (sscanf_s(line, "vn %f %f %f\n", &normal.x, &normal.y, &normal.z) > 0) {


					temp_normals.push_back(normal);
				}
				if (line[0] == 'f' && line[1] == ' ') {
					string bob = "";
					string fullLineCont(line);
					vector<unsigned int> allIndexes;
					unsigned int loopSize = (sizeof(line) / sizeof(line[0]));
					for (unsigned int i = 2; i < (sizeof(line) / sizeof(line[0])); i++) {
						if (line[i] == NULL) {
							allIndexes.push_back(stoi(bob));
							break;
						}
						else if (line[i] == '/' || line[i] == ' ') {
							allIndexes.push_back(stoi(bob));
							bob = "";
						}
						else {
							bob = bob + line[i];
						}
					}



					if (allIndexes.size() != 9 && allIndexes.size() != 12) {
						printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						return false;
					}

					unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
					for (unsigned int i = 0; i < (allIndexes.size() / 3); i++) {
						vertexIndex[i] = allIndexes[i * 3];
						uvIndex[i] = allIndexes[(i * 3) + 1];
						normalIndex[i] = allIndexes[(i * 3) + 2];
					}


					vertexIndices.push_back(vertexIndex[0]); //1st triangle
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);

					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);

					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					if (allIndexes.size() == 12) {
						normalIndices.push_back(normalIndex[2]);
						normalIndices.push_back(normalIndex[3]);
						normalIndices.push_back(normalIndex[0]);

						uvIndices.push_back(uvIndex[2]);
						uvIndices.push_back(uvIndex[3]);
						uvIndices.push_back(uvIndex[0]);


						vertexIndices.push_back(vertexIndex[2]); //2nd triangle
						vertexIndices.push_back(vertexIndex[3]);
						vertexIndices.push_back(vertexIndex[0]);
					}
				}
			}

			for (unsigned int i = 0; i < vertexIndices.size(); i++) {
				unsigned int vertexIndex = vertexIndices[i];
				glm::vec3 vertex = temp_vertices[vertexIndex - 1];
				obj_vertices.push_back(vertex);
			}
			for (unsigned int i = 0; i < uvIndices.size(); i++) {
				unsigned int uvIndex = uvIndices[i];
				glm::vec2 uv = temp_uvs[uvIndex - 1];
				obj_uvs.push_back(uv);
			}


			for (unsigned int i = 0; i < normalIndices.size(); i++) {
				unsigned int normalIndex = normalIndices[i];
				glm::vec3 normal = temp_normals[normalIndex - 1];
				obj_normals.push_back(normal);
			}
		}
		catch (...) { std::cout << "Failed to load model"; return false; } //if modles doesnt load, give error message

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, obj_vertices.size() * sizeof(glm::vec3), &obj_vertices[0], GL_STATIC_DRAW);
		glGenBuffers(1, &uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, obj_uvs.size() * sizeof(glm::vec2), &obj_uvs[0], GL_STATIC_DRAW);
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, obj_normals.size() * sizeof(glm::vec3), &obj_normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT,
			GL_FALSE, 0, BUFFER_OFFSET(0));

		loadTexture(texture1, textfilename);
		return true;
	}

	void displayObject(GLuint program, glm::mat4 projection, glm::mat4 view, glm::mat4 modelMatrix) {
		glUseProgram(program);
		glBindVertexArray(vertexVAO);
		// Adding all matrices up to create combined matrix
		glm::mat4 mvp = projection * view * modelMatrix;

		//adding the Uniform to the shader
		int mvpLoc = glGetUniformLocation(program, "mvp");
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(shaderModuleMatrix, 1, GL_FALSE, &modelMatrix[0][0]); //set the shader modles matrix for this object



		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);
		glEnableVertexAttribArray(vPosition);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, obj_vertices.size() * sizeof(glm::vec3), &obj_vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT,
			GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(tPosition);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, obj_uvs.size() * sizeof(glm::vec2), &obj_uvs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(tPosition, 2, GL_FLOAT,
			GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(nPosition);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, obj_normals.size() * sizeof(glm::vec3), &obj_normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(nPosition, 3, GL_FLOAT,
			GL_FALSE, 0, BUFFER_OFFSET(0));
		glDrawArrays(GL_TRIANGLES, 0, obj_vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(NULL);
	}

};


	void loadTexture(GLuint& texture, std::string texturepath)
					{
						// load and create a texture 
					// -------------------------

// texture 1
// ---------
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);
// set the texture wrapping parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// set texture filtering parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// load image, create texture and generate mipmaps
GLint width, height, nrChannels;
stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
unsigned char* data = stbi_load(texturepath.c_str(), &width, &height, &nrChannels, 0);
if (data)
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
	std::cout << "Failed to load texture" << std::endl;
}
stbi_image_free(data);
					}

	model one;
	model two;
	GLFWwindow* window;
	bool oneLoaded;
	bool twoLoaded;
	glm::vec3 camraPosition = glm::vec3(-1.5f, 1.0f, -5);
    float xAngle = 0.55f;//horizontal
	float yAngle = -0.25f; //vertical
	double previosState;
	glm::vec3 modifideLightOrigin = glm::vec3(2, 0.5f, 0);
	glm::vec3 modifideLightPosition = glm::vec3(-1,0.5f,0);
	glm::vec3 modifideLightColor = glm::vec3(1,0,0);
	bool reverseBool = false;
	float rotSpeed = 0.3f;
	float radius = 3;
	float time = 0;
	bool manualOn = false;

	GLuint shaderLightPosition;
	GLuint shaderLightColor;
	GLuint shaderLightBrighness;
	
	GLuint shaderViewMatrix;
	
	


					//----------------------------------------------------------------------------
					//
					// display
					//

					void
						display(bool oneLoaded, bool twoLoaded)
					{
						
						double currentState = glfwGetTime(); //gets time that has passed since the program started
						float deltaTime = float(currentState - previosState); //gets the amouth of time between loops (means if computer is running slower, they will get less loops persecond)
						double xcursor;
						double ycursor;

						time += 0.01f;
						glm::vec2 circleLightPosition = glm::vec2(sin(rotSpeed * time), cos(rotSpeed * time)) * radius; //works out x and z positions imagining a 2D circle
						modifideLightPosition = glm::vec3(modifideLightOrigin.x + circleLightPosition.x, modifideLightOrigin.y, modifideLightOrigin.z + circleLightPosition.y); //places light relitive to the boat (the light sourse is the creeper cude from courswork one)

						if (manualOn == false) { //if its on automatic, the light will change colour depending on where it is in the circle
							modifideLightColor = glm::normalize(modifideLightPosition);
						}
						

						//makes cursor stay in the center of the window
						glfwGetCursorPos(window, &xcursor, &ycursor);
						int windowwidth;
						int windowhight;
						glfwGetWindowSize(window, &windowwidth, &windowhight);
						glfwSetCursorPos(window, windowwidth / 2, windowhight / 2);


						//gets how much the cursor has moved from the middle, since the last frame to work out what way the camra should be pointing
						xAngle +=0.0025* float(windowwidth / 2.0f - xcursor);
						yAngle +=0.0025*float( windowhight / 2.0f - ycursor);
						cout << "\n" << deltaTime << "\n" ;

						

						//clears of data from the screen and gives it a yellow background reday or more data to be drawn
						glUseProgram(program);

						glClearColor(1, 0.975f, 0.8f, 0.0f);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						glEnable(GL_DEPTH_TEST);
						glDepthFunc(GL_LESS);
						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);



						//using calculations to work out direction vectors from camra position
						glm::vec3 forward(cos(yAngle) * sin(xAngle), sin(yAngle), cos(xAngle) * cos(yAngle));
						glm::vec3 right(sin(xAngle - (3.14 / 2)), 0, cos(xAngle - (3.14 / 2)));
						glm::vec3 up = glm::cross(right, forward);

						//lets the user change the postion of the camra using keyboard keys.
						if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
							camraPosition += forward * (deltaTime * 1);
						}

						if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
							camraPosition += right * (deltaTime * 1);
						}

						if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
							camraPosition -= right * (deltaTime * 1);
						}

						if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
							camraPosition -= forward * (deltaTime * 1);
						}


						// creating the view matrix position of the camra, direction you are looking and what the camra considers up
						glm::mat4 view = glm::lookAt(camraPosition, camraPosition + forward, up);

						// creating the projection matrix
						glm::mat4 projection = glm::perspective(45.0f, float(windowwidth) / float(windowhight), 0.1f, 200.0f);

						//sets the shader variables
						glUniformMatrix4fv(shaderViewMatrix, 1, GL_FALSE, &view[0][0]);
						glUniform3f(shaderLightPosition, modifideLightPosition.x, modifideLightPosition.y, modifideLightPosition.z);
						glUniform3f(shaderLightColor, modifideLightColor.x, modifideLightColor.y, modifideLightColor.z);
						glUniform1f(shaderLightBrighness, 50);



						if (oneLoaded) {
							one.displayObject(program, projection, view, glm::translate(glm::translate(glm::mat4(1.0f), modifideLightOrigin), modifideLightPosition));
						}

						if (twoLoaded) {
							two.displayObject(program, projection, view, glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f)), glm::vec3(500, -100, 0)));
						}

						if (!oneLoaded && !twoLoaded) {
							std::cout << "neither model has loaded. please close window and try again.";
						}
						previosState = currentState;
					}

			//----------------------------------------------------------------------------
			//
			// main
			//

					void loadObjs() {
						oneLoaded = one.loadmodel("media/creeper.obj", "media/creeper.png");

						twoLoaded = two.loadmodel("media/low_poly_boat.obj", "media/creeper.png"); //Textures need to be png for mipmapping
					}

					void clearObjs() { //clears object for dada mangment
						one.clearData();
						two.clearData();
						oneLoaded = false;
						twoLoaded = false;
					}

					void manual() { //toggles between manual and automatic
						manualOn = !manualOn;


					}


					void key_callback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
						if (key == GLFW_KEY_C && action == GLFW_PRESS) { //clears objects
							clearObjs();
						}
						else if (key == GLFW_KEY_L && action == GLFW_PRESS) { //loads objects
							loadObjs();
						}

						else if (key == GLFW_KEY_M && action == GLFW_PRESS) { //changes from automatic to manual mode
							manual();
						}

						else if (key == GLFW_KEY_R && action == GLFW_PRESS && manualOn == true) { // increases the value red, insuring it stays between 0 and 1
							modifideLightColor.x = glm::clamp(modifideLightColor.x + 0.1f, 0.0f, 1.0f);
						}

						else if (key == GLFW_KEY_T && action == GLFW_PRESS && manualOn == true) { // decreases the value red, insuring it stays between 0 and 1
							
							modifideLightColor.x = glm::clamp(modifideLightColor.x - 0.1f, 0.0f, 1.0f); 
						}
						else if (key == GLFW_KEY_G && action == GLFW_PRESS && manualOn == true) { // increases the value green, insuring it stays between 0 and 1
							
							modifideLightColor.y = glm::clamp(modifideLightColor.y + 0.1f, 0.0f, 1.0f);
						}

						else if (key == GLFW_KEY_H && action == GLFW_PRESS && manualOn == true) {// decreses the value green, insuring it stays between 0 and 1
							
							modifideLightColor.y = glm::clamp(modifideLightColor.y - 0.1f, 0.0f, 1.0f);
						}

						else if (key == GLFW_KEY_B && action == GLFW_PRESS && manualOn == true) { // increases the value blue, insuring it stays between 0 and 1
							
							modifideLightColor.z = glm::clamp(modifideLightColor.z + 0.1f, 0.0f, 1.0f);
						}

						else if (key == GLFW_KEY_N && action == GLFW_PRESS && manualOn == true) { // decreases the value blue, insuring it stays between 0 and 1
							
							modifideLightColor.z = glm::clamp(modifideLightColor.z - 0.1f, 0.0f, 1.0f);
						}


					}



					
//ive used object oriantated programing which promots incapsilation which measn all relasted code whcih do stuff, is kepted together in its own class/methods
					//eg, the data loader. as ive changed things which are part of the main display function, eg shaders and ligting, i have not needed to incapilate it.
					//as c sharp is very focused onmemory managemt, using objects for everything can be inafficent and slow the progtam down and add unessasay complexity to the code.
			int main(int argc, char** argv) {


				glfwInit();

				window = glfwCreateWindow(800, 600, "Textured Cube", NULL, NULL);

				glfwMakeContextCurrent(window);
				glewInit();
				glfwSetKeyCallback(window, key_callback);

				ShaderInfo  shaders[] =
				{
					{ GL_VERTEX_SHADER, "media/triangles.vert" },
					{ GL_FRAGMENT_SHADER, "media/triangles.frag" },
					{ GL_NONE, NULL }
				};


                previosState = glfwGetTime();
				//saving memory location to varables
				program = LoadShaders(shaders); 
				shaderLightPosition = glGetUniformLocation(program, "lightPositionIn");
				 shaderLightColor = glGetUniformLocation(program, "lightColor");
				 shaderLightBrighness = glGetUniformLocation(program, "brightness");
				 shaderModuleMatrix = glGetUniformLocation(program, "moduleMatrix");
				 shaderViewMatrix = glGetUniformLocation(program, "viewMatrix");

				 


				loadObjs(); //loads objects

				while (!glfwWindowShouldClose(window))
				{
					display(oneLoaded, twoLoaded);
					glfwSwapBuffers(window);
					glfwPollEvents();

				}
				glfwDestroyWindow(window);
				glDeleteProgram(program);
				glfwTerminate();


				
			}
				