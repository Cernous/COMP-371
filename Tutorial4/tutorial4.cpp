//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include <iostream>
#include <list>
#include <algorithm>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace glm;
using namespace std;

class Projectile
{
public:
    Projectile(vec3 position, vec3 velocity, int shaderProgram) : mPosition(position), mVelocity(velocity)
    {
        mWorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    }
    
    void Update(float dt)
    {
        mPosition += mVelocity * dt;
    }
    
    void Draw() {
        // this is a bit of a shortcut, since we have a single vbo, it is already bound
        // let's just set the world matrix in the vertex shader
        
        mat4 worldMatrix = translate(mat4(1.0f), mPosition) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(mWorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
private:
    GLuint mWorldMatrixLocation;
    vec3 mPosition;
    vec3 mVelocity;
};

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


int main(int argc, char*argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();
    
#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(800, 600, "Comp371 - Lab 03", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);

    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Very important for different scaling and screen size ratio
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    // Load Textures
    GLuint brickTextureID = loadTexture("Textures/brick.jpg");
    GLuint cementTextureID = loadTexture("Textures/cement.jpg");
    
    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Compile and link shaders here ...
    int colorShaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    int texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());
    
    // Camera parameters for view transform
    vec3 cameraPosition(0.6f,1.0f,10.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);
    
    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

    // Spinning cube at camera position
    float spinningCubeAngle = 0.0f;
    
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
                                             800.0f / 600.0f,  // aspect ratio
                                             0.01f, 100.0f);   // near and far (near > 0)
    
    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
                             cameraPosition + cameraLookAt,  // center
                             cameraUp ); // up
    
    // Set View and Projection matrices on both shaders
    setViewMatrix(colorShaderProgram, viewMatrix);
    setViewMatrix(texturedShaderProgram, viewMatrix);

    setProjectionMatrix(colorShaderProgram, projectionMatrix);
    setProjectionMatrix(texturedShaderProgram, projectionMatrix);

    // Define and upload geometry to the GPU here ...
    int texturedCubeVAO = createTexturedCubeVertexArrayObject();

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
    
    // Other OpenGL states to set once
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    // Container for projectiles to be implemented in tutorial
    list<Projectile> projectileList;

    glBindVertexArray(texturedCubeVAO);

    // Entering Main Loop
    while(!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw textured geometry
        glUseProgram(texturedShaderProgram);

        glActiveTexture(GL_TEXTURE0);
        GLuint textureLocation = glGetUniformLocation(texturedShaderProgram, "textureSampler");
        glBindTexture(GL_TEXTURE_2D, brickTextureID);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0
        
        // Draw ground
        mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.01f, 0.0f)) * scale(mat4(1.0f), vec3(1000.0f, 0.02f, 1000.0f));
        setWorldMatrix(texturedShaderProgram, groundWorldMatrix);
        
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
        
        // Draw pillars
        glBindTexture(GL_TEXTURE_2D, cementTextureID);
        mat4 pillarWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 10.0f, 0.0f)) * scale(mat4(1.0f), vec3(2.0f, 20.0f, 2.0f));
        setWorldMatrix(texturedShaderProgram, pillarWorldMatrix);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        for (int i=0; i<20; ++i)
        {
            for (int j=0; j<20; ++j)
            {
                // FIXME: it would be more efficient to set the cement texture and draw all pillars, then switch to brick texture and draw all pillar bases
                // use cement texture for pillar
                glBindTexture(GL_TEXTURE_2D, cementTextureID);
                pillarWorldMatrix = translate(mat4(1.0f), vec3(- 100.0f + i * 10.0f, 5.0f, -100.0f + j * 10.0f)) * scale(mat4(1.0f), vec3(1.0f, 10.0f, 1.0f));
                setWorldMatrix(texturedShaderProgram, pillarWorldMatrix);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                
                // use brick texture for base
                glBindTexture(GL_TEXTURE_2D, brickTextureID);
                pillarWorldMatrix = translate(mat4(1.0f), vec3(- 100.0f + i * 10.0f, 0.55f, -100.0f + j * 10.0f)) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(1.1f, 1.1f, 1.1f));
                setWorldMatrix(texturedShaderProgram, pillarWorldMatrix);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        
        // Draw colored geometry
        glUseProgram(colorShaderProgram);

        // Update and draw projectiles
        for (list<Projectile>::iterator it = projectileList.begin(); it != projectileList.end(); ++it)
        {
            it->Update(dt);
            it->Draw();
        }
        
        // Spinning cube at camera position
        spinningCubeAngle += 180.0f * dt;
        
        // Draw avatar in view space for first person camera
        // and in world space for third person camera
        if (cameraFirstPerson)
        {
            // Wolrd matrix is identity, but view transform like a world transform relative to camera basis
            // (1 unit in front of camera)
            //
            // This is similar to a weapon moving with camera in a shooter game
            mat4 spinningCubeViewMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.0f)) *
                                          rotate(mat4(1.0f), radians(spinningCubeAngle), vec3(0.0f, 1.0f, 0.0f)) *
                                          scale(mat4(1.0f), vec3(0.01f, 0.01f, 0.01f));
            
            setWorldMatrix(colorShaderProgram, mat4(1.0f));
            setViewMatrix(colorShaderProgram, spinningCubeViewMatrix);
        }
        else
        {
            // In third person view, let's draw the spinning cube in world space, like any other models
            mat4 spinningCubeWorldMatrix = translate(mat4(1.0f), cameraPosition) *
                                           rotate(mat4(1.0f), radians(spinningCubeAngle), vec3(0.0f, 1.0f, 0.0f)) *
                                           scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
            
            setWorldMatrix(colorShaderProgram, spinningCubeWorldMatrix);
        }
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        
        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Handle inputs
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
        
        
        // - Calculate mouse motion dx and dy
        // - Update camera horizontal and vertical angle
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        
        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;
        
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Convert to spherical coordinates
        const float cameraAngularSpeed = 60.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle   -= dy * cameraAngularSpeed * dt;
        
        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        
        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);
        
        cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));
        
        glm::normalize(cameraSideVector);
        
        
        // Use camera lookat and side vectors to update positions with ASDW
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
        
        // Set the view matrix for first and third person cameras
        // - In first person, camera lookat is set like below
        // - In third person, camera position is on a sphere looking towards center
        mat4 viewMatrix(1.0f);
        
        if (cameraFirstPerson)
        {
            viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp );
        }
        else
        {
            // Position of the camera is on the sphere looking at the point of interest (cameraPosition)
            float radius = 5.0f;
            vec3 position = cameraPosition - vec3(radius * cosf(phi)*cosf(theta),
                                                  radius * sinf(phi),
                                                  -radius * cosf(phi)*sinf(theta));
;
            viewMatrix = lookAt(position, cameraPosition, cameraUp);
        }
        
        setViewMatrix(colorShaderProgram, viewMatrix);
        setViewMatrix(texturedShaderProgram, viewMatrix);

        
        // Shoot projectiles on mouse left click
        // To detect onPress events, we need to check the last state and the current state to detect the state change
        // Otherwise, you would shoot many projectiles on each mouse press
        if (lastMouseLeftState == GLFW_RELEASE && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            const float projectileSpeed = 25.0f;
            projectileList.push_back(Projectile(cameraPosition, projectileSpeed * cameraLookAt, colorShaderProgram));
        }
        lastMouseLeftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    }

    glfwTerminate();
    
	return 0;
}

const char* getVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                ""
                "uniform mat4 worldMatrix;"
                "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
                "uniform mat4 projectionMatrix = mat4(1.0);"
                ""
                "out vec3 vertexColor;"
                "void main()"
                "{"
                "   vertexColor = aColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "}";
}

const char* getFragmentShaderSource()
{
    return
                "#version 330 core\n"
                "in vec3 vertexColor;"
                "out vec4 FragColor;"
                "void main()"
                "{"
                "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
                "}";
}

const char* getTexturedVertexShaderSource()
{
    // use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
                "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;"
                "layout (location = 1) in vec3 aColor;"
                "layout (location = 2) in vec2 aUV;"
                ""
                "uniform mat4 worldMatrix;"
                "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
                "uniform mat4 projectionMatrix = mat4(1.0);"
                ""
                "out vec3 vertexColor;"
                "out vec2 vertexUV;"
                "void main()"
                "{"
                "   vertexColor = aColor;"
                "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
                "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
                "   vertexUV = aUV;"
                "}"; //TODO: Replace this with the actual textured vertex shader
}

const char* getTexturedFragmentShaderSource()
{
    return
                "#version 330 core\n"
                "in vec3 vertexColor;"
                "in vec2 vertexUV;"
                "uniform sampler2D textureSampler;"
                ""
                "out vec4 FragColor;"
                "void main()"
                "{"
                "   vec4 textureColor = texture(textureSampler, vertexUV);"
                "   FragColor = textureColor;" //* vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
                "}";
}

int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
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

    return textureId; //TODO: replace with texture loading code
}

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