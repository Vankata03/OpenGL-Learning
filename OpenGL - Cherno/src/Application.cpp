// Mine
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

// CPP libraries
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Graphics libraries
#include "GL/glew.h"
#include "GLFW/glfw3.h"

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
        char* message = (char*)_alloca(length * sizeof(char));
        
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

    glDetachShader(program, vs);
	glDetachShader(program, fs);

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

	// Set version and core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Learning OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Enable vsync
    if (glewInit() != GLEW_OK)
        std::cout << "Error initialising GLEW!" << std::endl;
    
    // OpenGL version print
	std::cout << glGetString(GL_VERSION) << std::endl;

	// We scope so that destructors are called before glfwTerminate and we don't get the "invalid context" errors
    {
        // Define positions for a triangle (vertex postions)
        float positions[8] = {
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

        // Create the vertex array object (VAO)
        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

		VertexArray va;

		// Create a vertex buffer and layout
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

        // Create an index buffer
        IndexBuffer ib(indices, 6);

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        GLCall(unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource));
        GLCall(glUseProgram(shader));

        float r = 0.0f;
        float increment = 0.05f;

        int location = glGetUniformLocation(shader, "u_Color");
        ASSERT(location != -1);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

			va.Bind();
			ib.Bind();

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            glfwPollEvents();
        }

        GLCall(glDeleteProgram(shader));
    }

    glfwTerminate();
    return 0;
}