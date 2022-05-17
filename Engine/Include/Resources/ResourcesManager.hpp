#pragma once

#include <map>

#include <vector>
#include <deque>
#include <memory>
#include <Utils/Singleton.h>
#include <Resources/Mesh.hpp>
#include <Core/ThreadPool.hpp>

namespace Resources
{
	struct RawVerticesList
	{
		std::deque<Maths::Vector3f>	Pos;
		std::deque<Maths::Vector2f>	Tex;
		std::deque<Maths::Vector3f>	Nor;
		bool empty();
	};


	struct Character {
		unsigned int	TextureID = 0;  // ID handle of the glyph texture
		Maths::Vector2f Size;			// Size of glyph
		Maths::Vector2f Bearing;		// Offset from baseline to left/top of glyph
		unsigned int	Advance = 0;    // Offset to advance to next glyph
	};

	typedef std::map<char, Character> FontCharacter;
	typedef std::map<std::string, FontCharacter> FontList;
	typedef std::vector<std::string> stringList;

	class ResourcesManager : public Singleton<ResourcesManager>
	{
	private:

		//	Private Internal Variables
		//	--------------------------

		std::string acceptedImageExtension[5] =
		{
			"png", "jpeg", "jpg", "bmp", "tga"
		};

		stringList m_loadedFiles;
		stringList m_loadedTextFile;
		stringList m_loadedOBJFile;
		stringList m_textToGen;
		stringList m_meshToGen;

		std::atomic_flag m_textToGenFlag = ATOMIC_FLAG_INIT;
		std::atomic_flag m_meshToGenFlag = ATOMIC_FLAG_INIT;

		//	Private Internal Functions
		//	--------------------------

		//	Check if a texture has been loaded
		//	Parameters : const std::string& path
		//	------------------------------------
		bool textureLoaded(const std::string& path);

		//	Check if a obj has been loaded
		//	Parameters : const std::string& path
		//	------------------------------------
		bool OBJLoaded(const std::string& path);

		//	Parse a OBJ file
		//	Parameters : const std::string& path, const std::string& fileName, Resources::stringList& mesh_name_list
		//	--------------------------------------------------------------------------------------------------------
		bool parseOBJ(const std::string& path, const std::string& fileName);

		//	Create new Texture from a file
		//	Parameters : const std::string& path, const std::string& textName
		//	------------------------------------------------------------------
		void createNewTexture(const std::string& path, const std::string& textName);

		//	Create new CubeMap Texture from a file
		//	Parameters : const std::string& path, const std::string& textName
		//	------------------------------------------------------------------
		void createNewTextureCubeMap(const std::string& textName , const std::vector<std::string>& faces);

	//	--------------------------------------------------------------------------------------------------------------

	public:
		//	Constructor / Destructor
		//	------------------------

		ResourcesManager();
		~ResourcesManager();

		//	Public Internal Variables
		//	-------------------------


		std::atomic_flag m_matListFlag  = ATOMIC_FLAG_INIT;
		std::atomic_flag m_shadListFlag = ATOMIC_FLAG_INIT;
		std::atomic_flag m_meshListFlag = ATOMIC_FLAG_INIT;
		std::atomic_flag m_textListFlag = ATOMIC_FLAG_INIT;
		std::atomic_flag m_objListFlag = ATOMIC_FLAG_INIT;
		std::atomic_flag m_bindedMeshMatListFlag = ATOMIC_FLAG_INIT;

		std::map<std::string, stringList>	m_obj;
		std::map<std::string, std::string>	m_bindedMeshAndMaterial;

		std::map<std::string, std::shared_ptr<Resources::Mesh>>		m_meshList;
		std::map<std::string, std::shared_ptr<Resources::Shader>>	m_shaderList;
		std::map<std::string, std::shared_ptr<Resources::Material>>	m_materialList;
		std::map<std::string, std::shared_ptr<Resources::Texture>>	m_textureList;
		std::map<std::string, std::atomic<bool>>	m_atomicTextureList;

		FontList m_characterListPerFonts;

		//	Public Internal Functions
		//	-------------------------

		//	Initialize the ResourceManager
		//	Parameters : none
		//	-----------------
		void initialize();

		//	Load OBJ file
		//	Parameters : const std::string& path, const std::string& fileName
		//	-----------------------------------------------------------------
		bool loadOBJ(const std::string& path, const std::string& fileName);

		//	Load Shader
		//	Parameters : const std::string& shader_name, const std::string& vertex_path, const std::string& fragment_path
		//	-------------------------------------------------------------------------------------------------------------
		void loadShader(const std::string& shader_name, const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path);

		//	Parse a MTL file
		//	Parameters : const std::string& path, const std::string& fileName, Resources::stringList& material_name_list
		//	------------------------------------------------------------------------------------------------------------
		void parseMTL(const std::string& path, const std::string& fileName, Resources::stringList& material_name_list, bool& success);

		//	Load Texture from a file
		//	Parameters : const std::string& path, const std::string& textName
		//	------------------------------------------------------------------
		Resources::Texture* loadTexture(const std::string& path, const std::string& textName);

		//	Get Cube Map Texture from a file
		//	Parameters : const std::string& path, const std::string& textName
		//	-----------------------------------------------------------
		Resources::Texture* loadTextureCubeMap(const std::string& path, const std::string& textName);

		//	Set Texture
		//	Parameters : const std::string& textName, Resources::Texture& text
		//	------------------------------------------------------------------
		void setTexture(const std::string& textName, Resources::Texture& text);

		//	Set Texture
		//	Parameters : const std::string& textName, Resources::TextureCubeMap& text
		//	-------------------------------------------------------------------------
		void setTexture(const std::string& textName, Resources::TextureCubeMap& text);

		//	Generate ungenerated texture
		//	(call GL functions, so cannot be called in a thread)
		//	-------------------
		void generateGLTextures();


		//	Generate ungenerated meshes
		//	(call GL functions, so cannot be called in a thread)
		//	-------------------
		void generateGLMeshes();


		//	Check if a file is loaded
		//	Parameters : const std::string& path
		//	-----------------------------
		bool fileLoaded(const std::string& path);
		
		//	Return Combo string of all textures
		//	Parameters : const std::string& path
		//	-----------------------------
		std::vector<const char*> getAllTextureName();

		//	Load font
		//	Parameters : const std::string& path
		//	------------------------------------
		void loadFont(const std::string& path);

		//	Attach a Mesh to a pointer, if the mesh is not created yet, it is created in there
		//	Parameters : const std::string& meshName
		//	----------------------------------------
		Resources::Mesh* attachMesh(const std::string& meshName);

		//	Attach a Shader to a pointer, if the shader is not created yet, it is created in there
		//	Parameters : const std::string& shaderName
		//	------------------------------------------
		Resources::Shader* attachShader(const std::string& shaderName);

		//	Attach a Material to a pointer, if the shader is not created yet, it is created in there
		//	Parameters : const std::string& materialName
		//	--------------------------------------------
		Resources::Material* attachMaterial(const std::string& materialName);


		void clear();

		void showImGuiWindows();
		void showImGuiResources();
	};
}
