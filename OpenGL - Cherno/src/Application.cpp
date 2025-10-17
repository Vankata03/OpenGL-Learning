// General libraries
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Graphics libraries
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#ifdef _DEBUG
    #define ASSERT(x) if(!(x)) __debugbreak();
    #define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__));
#else
    #define GLCall(x) x
#endif

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
	return true;
}

// Shader parsing structure
struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

// Function to parse shaders from a file
static ShaderProgramSource ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

    std::string line;
    std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {

            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;

            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;

            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

	return { ss[0].str(), ss[1].str() };
}

// Function to compile a shader of a given type from source code
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
		// Find the error message length
		int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

		// Allocate on the stack
        char* message = (char*)alloca(length * sizeof(char));
        
		// Get the error message and log it
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile" << (type==GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
		std::cout << message << std::endl;

		// Delete the failed shader
        GLCall(glDeleteShader(id));
		return 0;
    }

    return id;
}

// Function to create a shader program from vertex and fragment shader source code
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
    GLCall(unsigned int program = glCreateProgram());
    GLCall(unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader));
    GLCall(unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

    GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));

	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Detaching might not be necessary
    //glDetachShader(program, vs);
	//glDetachShader(program, fs);

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));
	return program;
}
 
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Learning OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window); 
    if (glewInit() != GLEW_OK)
        std::cout << "Error initialising GLEW!" << std::endl;
    
    // OpenGL version print
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Define positions for a triangle (vertex postions)
    float postions[8] = {
         -0.5f, -0.5f,
          0.5f, -0.5f,
          0.5f,  0.5f,
         -0.5f,  0.5f
    };

    // Define an index array
    unsigned int indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    // Create a buffer
    unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), postions, GL_STATIC_DRAW));

	// Tell OpenGL how to interpret the buffer (layout)
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

	// Create an index buffer
	unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	GLCall(unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource));
	GLCall(glUseProgram(shader));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));

        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}