#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Core/Log.hpp>
#include <Resources/Texture.hpp>
#include <Resources/ResourcesManager.hpp>

#include <Utils/StringExtractor.h>

#include <Resources/ResourcesManager.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


void Resources::TextureDatas::destroy()
{
	//	If datas has been filled
	if (filled)
	{
		stbi_image_free(data);
	}
}


Resources::Texture::Texture(const std::string& filename, const std::string& textName)
{
	//	Log Instance
	Core::Log* _log = Core::Log::instance();

	//	Local Variables

	int		colorType;
	float*	tempdata;

	//	Flip the texture vertically so it isn't inverted
	stbi_set_flip_vertically_on_load(true);

	m_path = Extractor::ExtractDirectory(filename);
	m_name = textName;


	//	Load the texture data
	//	If datas are successfully loaded
	if (tempdata = stbi_loadf(filename.c_str(), &m_width, &m_height, &colorType, 0))
	{ 
		m_datas.push_back(TextureDatas());
		TextureDatas* td = &m_datas[0];

		//	Get datas from the image and pass the bool value "filled" to true
		td->data = tempdata;
		td->filled = true;

		//	Get the Formats of the image
		/**/ if (colorType == 1) { td->formats[0] = td->formats[1] = GL_RED; }
		else if (colorType == 3) { td->formats[0] = GL_RGB;   td->formats[1] = GL_RGB; }
		else if (colorType == 4) { td->formats[0] = GL_RGBA;   td->formats[1] = GL_RGBA; }

		//	Register the texture in the resources manager
		Resources::ResourcesManager* _resources = Resources::ResourcesManager::instance();
		_resources->setTexture(textName, *this);

		_log->writeSuccess("> Loaded texture \"" + std::string(filename) + "\"");
		return;
	}

	//	Else the texture loading failed
	_log->writeFailure("Failed to load texture \"" + std::string(filename) + "\"");
}

void Resources::Texture::destroyTexture()
{
	if (m_generated == false)
	{
		while (m_datas.empty() == false)
		{
			m_datas.back().destroy();
			m_datas.pop_back();
		}
	}
}

void Resources::Texture::generateGL()
{
	if (m_datas.empty() || m_datas[0].filled == false) return;

	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	Automatically manage if the texture is RGB or RGBA
	//	--------------------------------------------------

	glTexImage2D(GL_TEXTURE_2D, 0, m_datas[0].formats[0], m_width, m_height, 0, m_datas[0].formats[1], GL_FLOAT, m_datas[0].data);

	for(Resources::TextureDatas& d : m_datas) stbi_image_free(d.data);
	m_datas.clear();

	glGenerateMipmap(GL_TEXTURE_2D);

	//	Reset
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_generated = true;
}


void  Resources::Texture::showImGui() const
{
	if ((void*)(intptr_t)m_ID)
	{
		ImGui::Image((void*)(intptr_t)m_ID, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
	}
}



void Resources::Texture::bind(const int bindID) const
{
	glActiveTexture(bindID);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}


//	---------------------------------------------------------

//	CubeMap Texture
//	---------------

Resources::TextureCubeMap::TextureCubeMap(const std::string& textName, const std::vector<std::string>& faces)
{
	//	Log Instance
	Core::Log* _log = Core::Log::instance();

	//	Flip the texture vertically so it isn't inverted
	stbi_set_flip_vertically_on_load(true);

	//	Load the texture data
	//	If datas are successfully loaded

	bool success = true;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		//	Add new m_datas to the texture
		m_datas.push_back(TextureDatas());
		TextureDatas* td = &m_datas.back();

		int colorType;

		//	Parse the image and get datas from it
		td->data = stbi_loadf(faces[i].c_str(), &m_width, &m_height, &colorType, 0);

		if (td->data)
		{
			//	Pass the bool value "filled" to true
			td->filled = true;

			//	Get Formats

			GLenum f1;
			GLenum f2;

			/**/ if (colorType == 1) { f1 = f2 = GL_RED; }
			else if (colorType == 3) { f1 = GL_RGB;  f2 = GL_RGB; }
			else if (colorType == 4) { f1 = GL_RGBA;  f2 = GL_RGBA; }

			td->formats[0] = f1;
			td->formats[1] = f2;

			_log->writeSuccess("> Loaded one face of cubemap texture \"" + std::string(faces[i]) + "\"");
		}
		else
		{
			//	Else the texture loading failed
			_log->writeFailure("! Cubemap texture failed to load at path: \"" + faces[i] + "\"");
			stbi_image_free(td->data);
			success = false;
		}
	}

	if (success)
	{
		Resources::ResourcesManager* _resources = Resources::ResourcesManager::instance();


		_log->writeSuccess("> Loaded Cubemap \"" + std::string(textName) + "\"");
		_resources->setTexture(textName, *this);

		return;
	}
	
	//	Else the texture loading failed
	_log->writeFailure("! Failed to load CubeMap texture \"" + std::string(textName) + "\"");
}

//	Generate cube map Texture
void Resources::TextureCubeMap::generateGL()
{
	if (m_datas.empty()) return;

	for (TextureDatas& data : m_datas)
	{
		if (data.filled == false)
		{
			return;
		}
	}

	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

	int i = 0;
	for (TextureDatas& data : m_datas)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, data.formats[0], m_width, m_height, 0, data.formats[1], GL_FLOAT, data.data);

		stbi_image_free(data.data);

		i++;
	}
	m_datas.clear();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_generated = true;
}


//	---------------------------------------------------------

//	Map Texture
//	-----------


Resources::TextureMap::TextureMap(Texture* in_texture)
{
	m_texture = in_texture;
}

Resources::TextureMap::TextureMap(const std::string& path, const std::string& textName, const Maths::Vector3f& in_offset, const Maths::Vector3f& in_tiling)
{
	
	m_name = textName;
	m_texture = Resources::ResourcesManager::instance()->loadTexture(path, textName);
	m_offset = in_offset;
	m_tiling = in_tiling;
}


void Resources::TextureMap::bind(const int bindID) const
{
	glActiveTexture(bindID);
	glBindTexture(GL_TEXTURE_2D, getTextureID());
}

GLuint Resources::TextureMap::getTextureID() const
{
	if(Resources::ResourcesManager::instance()->m_atomicTextureList[m_name] == A_LOCKED) return 0;

	if (m_texture)
	{
		return m_texture->getID();
	}

	return 0;
}


void Resources::TextureMap::showImGui()
{
	if (m_texture)
	{
		ImGui::DragFloat3("Offset", &m_offset.x);
		ImGui::DragFloat3("Tiling", &m_tiling.x);

		m_texture->showImGui();
	}
}



//	Bump Map Texture

Resources::BumpMap::BumpMap(const std::string& path, const std::string& textName, const Maths::Vector3f& in_offset, const Maths::Vector3f& in_tiling, const float in_multiplier) : TextureMap(path, textName, in_offset, in_tiling)
{
	m_multiplier = in_multiplier;
}


void Resources::BumpMap::showImGui()
{
	if (m_texture)
	{
		ImGui::DragFloat3("Offset", &m_offset.x);
		ImGui::DragFloat3("Tiling", &m_tiling.x);
		ImGui::SliderFloat("Multiplier", &m_multiplier,0.f, 5.f);

		m_texture->showImGui();
	}
}
