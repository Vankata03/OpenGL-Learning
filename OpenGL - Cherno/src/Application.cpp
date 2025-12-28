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

#include "provided/imgui/imgui.h"
#include "provided/imgui/imgui_impl_glfw.h"
#include "provided/imgui/imgui_impl_opengl3.h"

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

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();

	// We scope so that destructor are called before glfwTerminate and we don't get the "invalid context" errors
    {
        float positions[] = {
              -50.0f, -50.0f, 0.0f, 0.0f,
               50.0f, -50.0f, 1.0f, 0.0f,
			   50.0f,  50.0f, 1.0f, 1.0f,
			  -50.0f,  50.0f, 0.0f, 1.0f,

			   50.0f,  50.0f,  0.0f, 0.0f,
			   150.0f, 50.0f,  1.0f, 0.0f,
			   150.0f, 150.0f, 1.0f, 1.0f,
			   50.0f,  150.0f, 0.0f, 1.0f,
        };

        // Define an index array
        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
        };

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

		// Create a vertex array object
		VertexArray va;

		// Create a vertex buffer and layout
        VertexBuffer vb(positions, 8 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

        // Create an index buffer
        IndexBuffer ib(indices, 12);

		// TODO: Move to perspective - Create a projection matrix
        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		// Create and bind a shader
        Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		glm::vec4 color(0.8f, 0.3f, 0.8f, 1.0f);
        shader.SetUniform4f("u_Color", color);

		Texture texture("res/textures/wood.jpg");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		// TODO: Remove - Unbind everything
        va.Unbind();
		shader.Unbind();
		vb.Unbind();
        ib.Unbind();

		Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;
		glm::vec3 translationA(200, 200, 0);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

			ImGui_ImplGlfw_NewFrame();

            shader.Bind();
            color.r = r;
			shader.SetUniform4f("u_Color", color);

            { 
			    glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
			    glm::mat4 mvp = proj * view * model;
			    shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }

            if (r > 1.0f)
                increment = -0.05f;
            else if (r < 0.0f)
                increment = 0.05f;

            r += increment;

			// Start the ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// ImGui UI code
			ImGui::SliderFloat3("float", &translationA.x, 0.0f, 500.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            
			// Render the ImGui frame
			ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}