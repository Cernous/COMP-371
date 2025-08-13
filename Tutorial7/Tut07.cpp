#include <vector>
#include <random>
#include <iostream>

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Particle system parameters
const int MAX_PARTICLES = 1000;
const float PARTICLE_LIFETIME = 3.0f;
const vec3 EMITTER_POSITION(0.0f, 0.0f, 0.0f);
const vec3 PARTICLE_GRAVITY(0.0f, -0.5f, 0.0f);
const vec2 PARTICLE_SIZE(0.1f, 0.1f);

struct Particle {
    vec3 position;
    vec3 velocity;
    vec4 color;
    float life;
};

// Shader sources
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    
    uniform mat4 projection;
    uniform mat4 view;
    uniform vec3 particlePos;
    uniform vec2 size;
    
    void main() {
        vec3 position = particlePos + vec3(aPos.x * size.x, aPos.y * size.y, 0.0);
        gl_Position = projection * view * vec4(position, 1.0);
    }
)glsl";

const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 particleColor;
    
    void main() {
        FragColor = particleColor;
    }
)glsl";

GLuint shaderProgram;
GLuint VAO, VBO;

void createQuad() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void createShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

class ParticleSystem {
private:
    vector<Particle> particles;
    default_random_engine generator;
    uniform_real_distribution<float> distribution;
    float emissionRate = 100.0f;
    float timeSinceLastEmission = 0.0f;
    
public:
    ParticleSystem() : distribution(-1.0f, 1.0f) {
        particles.resize(MAX_PARTICLES);
    }
    
    void emitParticles(float dt, vec3 cameraPos) {
      //TODO
      timeSinceLastEmission += dt;
      int particlesToEmit = timeSinceLastEmission * emissionRate;
      timeSinceLastEmission -= particlesToEmit /emissionRate;

      for(int i=0; i < particlesToEmit; i++) {
        int idx = findUnusedParticle();
        if(idx == -1) return;

        particles[idx].life = PARTICLE_LIFETIME;
        particles[idx].position = EMITTER_POSITION;
        particles[idx].velocity = vec3(
            distribution(generator) * 0.5f,
            abs(distribution(generator)) * 2.0f,
            distribution(generator) * 0.5f
        );
        particles[idx].color = vec4(
            0.5f + distribution(generator) * 0.5f,
            0.5f + distribution(generator) * 0.5f,
            0.5f + distribution(generator) * 0.5f,
            1.0f
        );
      }
    }
    
    void update(float dt) {
      //TODO
      for(auto& p : particles){
        if(p.life > 0.0f){
            p.life -= dt;
            if(p.life > 0.0f){
                p.velocity += PARTICLE_GRAVITY *dt;
                p.position += p.velocity *dt;
                p.color.a = p.life /PARTICLE_LIFETIME;
            }
        }
      }
    }
    
    void render(mat4 view, mat4 projection) {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniform2fv(glGetUniformLocation(shaderProgram, "size"), 1, &PARTICLE_SIZE[0]);
        
        glBindVertexArray(VAO);
        for(const auto& p : particles) {
            if(p.life > 0.0f) {
                glUniform3fv(glGetUniformLocation(shaderProgram, "particlePos"), 1, &p.position[0]);
                glUniform4fv(glGetUniformLocation(shaderProgram, "particleColor"), 1, &p.color[0]);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
        }
        glBindVertexArray(0);
    }

private:
    int findUnusedParticle() {
        for(size_t i = 0; i < particles.size(); i++) {
            if(particles[i].life <= 0.0f) return i;
        }
        return -1;
    }
};

int main() {
    if(!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Particle System", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) return -1;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Very important for different scaling and screen size ratio - Readjust the viewport for the wayland user
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    createShaders();
    createQuad();
    ParticleSystem ps;

    vec3 cameraPos(0.0f, 0.0f, 3.0f);
    mat4 projection = perspective(radians(45.0f), (float)WIDTH/HEIGHT, 0.1f, 100.0f);
    float lastTime = glfwGetTime();

    while(!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 view = lookAt(cameraPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
        
        ps.emitParticles(deltaTime, cameraPos);
        ps.update(deltaTime);
        ps.render(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
          glfwSetWindowShouldClose(window, true);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}