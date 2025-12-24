#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	: m_Filepath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader();
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader()
{
	std::ifstream stream(m_Filepath);

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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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
		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
		std::cout << message << std::endl;

		// Delete the failed shader
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	
	m_UniformLocationCache[name] = location;
	
	return location;
}