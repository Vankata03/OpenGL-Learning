#include "Texture.h"

#include "provided/stb_image/stb_image.h"

Texture::Texture(const std::string& path)

	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	 m_Width(0), m_Height(0), m_BPP(0)
{
	// Load image data
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	// Create OpenGL texture
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	// Set basic texture parameters
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// Upload texture data to GPU
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// Free image data
	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID))
}

void Texture::Bind(unsigned int slot) const
{
	if (slot > 31)
		slot = 0;

	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
