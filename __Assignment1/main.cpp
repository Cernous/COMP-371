//
// COMP 371 Labs Assignment 1
//

#include <iostream>
#include <list>
#include <algorithm>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "OBJloader.h"  //For loading .obj files
#include "OBJloaderV2.h"  //For loading .obj files using a polygon list format
#include "shaderloader.h" // for loading glsl shader programs
using namespace glm;
using namespace std;

// Macros
#define CAMERASENSITIVITY 1.0f                  // @NOTE: ON WAYLAND - Do AngularSpeed to 500.0f, 1.0f is for WSL

GLuint loadTexture(const char *filename);

const char* getVertexShaderSource();

const char* getFragmentShaderSource();

const char* getTexturedVertexShaderSource();

const char* getTexturedFragmentShaderSource();

int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource);

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
    : position(_position), color(_color), uv(_uv) {}
    
    vec3 position;
    vec3 color;
    vec2 uv;
};

// Textured Cube model
const TexturedColoredVertex texturedCubeVertexArray[] = {  // position,                            color
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), //left - red
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),
    
    TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
    
    TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    
    TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f)),
    
    TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    
    TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    
    TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)), // right - purple
    TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),
    
    TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
    
    TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top - yellow
    TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    
    TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
};

int createTexturedCubeVertexArrayObject();

int createTexturedCubeVertexArrayObject()
{
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    
    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
                          );
    glEnableVertexAttribArray(0);
    
    
    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
                          );
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TexturedColoredVertex),
                          (void*)(2*sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
                          );
    glEnableVertexAttribArray(2);
    
    return vertexArrayObject;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
	glUseProgram(shaderProgram);
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

GLuint loadTexture(const char *filename)
{
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data){
        std::cerr << "Error::Texture could not load texture file: " << filename << std::endl;
        return 0;
    }

    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format=0;
    switch(nrChannels){
        case 1:
            format=GL_RED;
            break;
        case 3:
            format=GL_RGB;
            break;
        case 4:
            format=GL_RGBA;
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureId; 
}

GLuint setupModelVBO(string path, int& vertexCount) {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	
	//read the vertex data from the model's OBJ file
	loadOBJ(path.c_str(), vertices, normals, UVs);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); //Becomes active VAO
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

	//Vertex VBO setup
	GLuint vertices_VBO;
	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Normals VBO setup
	GLuint normals_VBO;
	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	//UVs VBO setup
	GLuint uvs_VBO;
	glGenBuffers(1, &uvs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
	vertexCount = vertices.size();
	return VAO;
}

//Sets up a model using an Element Buffer Object to refer to vertex data
GLuint setupModelEBO(string path, int& vertexCount)
{
	vector<int> vertexIndices; //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> UVs;

	//read the vertices from the cube.obj file
	//We won't be needing the normals or UVs for this program
	loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO); //Becomes active VAO
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

	//Vertex VBO setup
	GLuint vertices_VBO;
	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Normals VBO setup
	GLuint normals_VBO;
	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	//UVs VBO setup
	GLuint uvs_VBO;
	glGenBuffers(1, &uvs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	//EBO setup
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	vertexCount = vertexIndices.size();
	return VAO;
}

int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // make it full screen but its too laggy
    // int mWidth, mHeight = 0;
    // GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    // glfwGetMonitorPhysicalSize(monitor, &mWidth, &mHeight);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(800, 600, "Comp371 - Assignment1", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Very important for different scaling and screen size ratio - Readjust the viewport for the wayland user
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //Imported shader compile and linking from tut06
    std::string shaderPathPrefix = "Shaders/";

    GLuint shaderScene = loadSHADER(shaderPathPrefix + "scene_vertex.glsl",
                                  shaderPathPrefix + "scene_frag.glsl");
    
    GLuint shaderShadow = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl",
                                   shaderPathPrefix + "shadow_fragment.glsl");

    GLuint textureScene = loadSHADER(shaderPathPrefix + "texture_vertex.glsl",
                                   shaderPathPrefix + "texture_frag.glsl");

    // // Compile and link shaders here ...
    // int shaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    // int texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());

    string suzannePath = "Models/suzanne.obj";
    string tablePath = "Models/table.obj";

    GLuint suzanneTexID = loadTexture("Textures/Suzanne_Tex.png");
    GLuint tableTexID = loadTexture("Textures/Table_Tex.png");

    // Why not use EBO? I was too lazy to see if it worked with textures

    int suzanneVertices;
    GLuint suzanneVAO = setupModelVBO(suzannePath, suzanneVertices);

    int tableVertices;
    GLuint tableVAO = setupModelVBO(tablePath, tableVertices);
    
    // Camera parameters for view transform
    vec3 cameraPosition(0.6f,2.0f,10.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);
    
    // Other camera parameters, &mH
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 15 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable
    
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
                                             800.0f / 600.0f,  // aspect ratio
                                             0.01f, 100.0f);   // near and far (near > 0)

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp ); // up
    
    // Set View and Projection matrices on both shaders
    setViewMatrix(shaderScene, viewMatrix);
    setViewMatrix(textureScene, viewMatrix);

    setProjectionMatrix(shaderScene, projectionMatrix);
    setProjectionMatrix(textureScene, projectionMatrix);

    
    
    // Define and upload geometry to the GPU here ...
    // Change HERE ==== MUCH EASIER WAY TO PERFORM THIS
    // int vao = createVertexBufferObject();
    int vao = createTexturedCubeVertexArrayObject();
    
    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    
    // Other OpenGL states to set once
    // Enable Backface culling
    // glEnable(GL_CULL_FACE);
    
    //Enable Depth Test
    glEnable(GL_DEPTH_TEST);

    float spinningObjAngle = 0.0f;
    
    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // Each frame, reset color of each pixel to glClearColor
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shaderScene to draw the cubes
        glUseProgram(shaderScene);
        
        // Draw geometry
        glBindVertexArray(vao);

        GLuint worldMatrixLocation = glGetUniformLocation(shaderScene, "worldMatrix");

        // Time-based animation value
        float time = glfwGetTime();
        float offset = sin(time) * 5.5f;
        float offset2 = cos(time) * 5.0f;

        // Uniform locations
        GLuint useOverrideLoc = glGetUniformLocation(shaderScene, "useOverride");
        GLuint overrideColorLoc = glGetUniformLocation(shaderScene, "overrideColor");

        // First cube (static center)
        glUniform1i(useOverrideLoc, GL_FALSE);
        mat4 cubeCenter = glm::mat4(1.0f);
        setWorldMatrix(shaderScene,cubeCenter);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Second cube (light)
        glUniform1i(glGetUniformLocation(shaderScene, "isEmissive"), true);
        glUniform1i(glGetUniformLocation(shaderScene, "useOverride"), GL_TRUE);
        glUniform3f(glGetUniformLocation(shaderScene, "overrideColor"), 1.0f, 1.0f, 1.0f);
        glUniform1i(useOverrideLoc, GL_TRUE);
        glUniform3f(overrideColorLoc, 1.0f, 1.0f, 1.0f);

        mat4 cubeTop = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f + offset, 0.0f, 0.0f + offset2));
        setWorldMatrix(shaderScene,cubeTop);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Third cube (light)
        mat4 cubeRight = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f - offset, 0.0f - offset2, 0.0f - offset));
        setWorldMatrix(shaderScene,cubeRight);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Set light emitter positions **after** cube matrices are created
        glm::vec3 emitterPositions[2] = {
            glm::vec3(cubeTop[3]),
            glm::vec3(cubeRight[3])
        };
        float emitterStrengths[2] = { 10.0f, 10.0f };

        // Draw geometry
        glUniform1i(useOverrideLoc, GL_FALSE);
        mat4 cubeWorldMatrix = 
            translate(mat4(1.0f), vec3(1.0f, 2.3f, 1.2f)) * 
            rotate(mat4(1.0f), radians(65.0f), vec3(0.0f, 1.0f, 0.0f)) *
            scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        setWorldMatrix(shaderScene, cubeWorldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0  
        
        // Send emitter data to shader
        glUniform1i(glGetUniformLocation(shaderScene, "numEmitters"), 2);
        glUniform3fv(glGetUniformLocation(shaderScene, "emitterPos"), 2, &emitterPositions[0].x);
        glUniform1fv(glGetUniformLocation(shaderScene, "ambientStrength"), 2, emitterStrengths);

        // Switch to the textured shader program to draw the more sophisticated meshes that have textures
        glUseProgram(textureScene);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(textureScene, "textureSampler"), 0);
        
        // Draw Table
        glBindTexture(GL_TEXTURE_2D, tableTexID);
        mat4 tableWorldMatrix = 
            translate(mat4(1.0f), vec3(0.0f, -0.5f, 0.0f)) * 
            scale(mat4(1.0f), vec3(2.5f, 2.5f, 2.5f));
        setWorldMatrix(textureScene, tableWorldMatrix);
        glBindVertexArray(tableVAO);
        glDrawArrays(GL_TRIANGLES, 0, tableVertices);
        
        // Spinning Suzanne
        spinningObjAngle += 45.0f * dt;

        // Draw Suzanne
        glBindTexture(GL_TEXTURE_2D, suzanneTexID);
        mat4 suzanneWorldMatrix = 
            translate(mat4(1.0f), vec3(0.0f, 1.3f, 0.0f)) *
            rotate(mat4(1.0f), radians(spinningObjAngle), vec3(0.0f, 1.0f, 0.0f)) *
            scale(mat4(1.0f), vec3(0.7f, 0.7f, 0.7f));
        setWorldMatrix(textureScene, suzanneWorldMatrix);
        glBindVertexArray(suzanneVAO);
        glDrawArrays(GL_TRIANGLES, 0, suzanneVertices);

        glUniform1i(glGetUniformLocation(textureScene, "numEmitters"), 2);
        glUniform3fv(glGetUniformLocation(textureScene, "emitterPos"), 2, &emitterPositions[0].x);
        glUniform1fv(glGetUniformLocation(textureScene, "ambientStrength"), 2, emitterStrengths);
        glBindVertexArray(0);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Handle inputs
        // Does it really matter? Probably not
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = true;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = false;
        }

        
        // This was solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;
        
        
        //         - Update camera horizontal and vertical angle
        //...
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        const float cameraAngularSpeed = CAMERASENSITIVITY; 
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));

        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);

        cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(cameraSideVector);
        if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS)
        {
            cameraPosition += cameraLookAt * dt * currentCameraSpeed;
        }
        
        if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS)
        {
            cameraPosition -= cameraLookAt * dt * currentCameraSpeed;
        }
        
        if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS)
        {
            cameraPosition += cameraSideVector * dt * currentCameraSpeed;
        }
        
        if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS)
        {
            cameraPosition -= cameraSideVector * dt * currentCameraSpeed;
        }

        // Adding Spacebar for up
         if (glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS)
        {
            cameraPosition += vec3(0.0f,1.0f,0.0f)*dt*currentCameraSpeed;
        }
        // adding anchor for down
         if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL ) == GLFW_PRESS)
        {
            cameraPosition -= vec3(0.0f,1.0f,0.0f)*dt*currentCameraSpeed;
        }
      
        // Set the view matrix for first and third person cameras
        // - In first person, camera lookat is set like below
        // - In third person, camera position is on a sphere looking towards center
// <<<<<<< gordito
//         mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
//         GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
//         glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
// =======
        mat4 viewMatrix = mat4(1.0);
        
        if(cameraFirstPerson){
            viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp );
        } else {
            float radius = 5.0f;
            glm::vec3 position = cameraPosition - glm::vec3(radius * cosf(phi)*cosf(theta),
                                                  radius * sinf(phi),
                                                  -radius * cosf(phi)*sinf(theta));
;
            viewMatrix = lookAt(position, position + cameraLookAt, cameraUp);
        }

        setViewMatrix(shaderScene, viewMatrix);
        setViewMatrix(textureScene, viewMatrix);


    }

    
    // Shutdown GLFW
    glfwTerminate();
    
	return 0;
}
