// Mine
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

// CPP libraries
#include <iostream>

// Graphics libraries
#include "GL/glew.h"
#include "GLFW/glfw3.h"

 
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
        std::cout << "Error initializing GLEW!" << std::endl;
    
    // OpenGL version print
	std::cout << glGetString(GL_VERSION) << std::endl;

	// We scope so that destructor are called before glfwTerminate and we don't get the "invalid context" errors
    {
        // Define positions for a triangle (vertex positions)
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

		// Create a vertex array object
		VertexArray va;

		// Create a vertex buffer and layout
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

        // Create an index buffer
        IndexBuffer ib(indices, 6);

        Shader shader("res/shaders/Basic.shader");
		shader.Bind();

        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		// Unbind everything
        va.Unbind();
		shader.Unbind();
		vb.Unbind();
        ib.Unbind();

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

			va.Bind();
			ib.Bind();

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}