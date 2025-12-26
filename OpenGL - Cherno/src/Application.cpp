// Mine
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

// CPP libraries
#include <iostream>

// Graphics libraries
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
 
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
    window = glfwCreateWindow(960, 540, "Learning OpenGL", NULL, NULL);
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
        float positions[] = {
              100.0f, 100.0f, 0.0f, 0.0f,
              200.0f, 100.0f, 1.0f, 0.0f,
			  200.0f, 200.0f, 1.0f, 1.0f,
			  100.0f, 200.0f, 0.0f, 1.0f
        };

        // Define an index array
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

		// Create a vertex array object
		VertexArray va;

		// Create a vertex buffer and layout
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

        // Create an index buffer
        IndexBuffer ib(indices, 6);

		// TODO: Move to perspective - Create a projection matrix
        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));

        glm::mat4 mvp = proj * view * model;

		// Create and bind a shader
        Shader shader("res/shaders/Basic.shader");
		shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

		// TODO: Remove - Unbind everything
        va.Unbind();
		shader.Unbind();
		vb.Unbind();
        ib.Unbind();

		Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

			Texture texture("res/textures/wood.jpg");
            texture.Bind();
            shader.SetUniform1i("u_Texture", 0);

            renderer.Draw(va, ib, shader);

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}