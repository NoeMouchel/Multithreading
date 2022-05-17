#include <iostream>
#include <iomanip>

#include <Core/Log.hpp>

#include <Resources/ResourcesManager.hpp>
#include <Utils/File.h>
#include <Utils/StringExtractor.h>

#include <imgui.h>

#include <ft2build.h>
#include FT_FREETYPE_H  


/*=================================== Constructor/ init ===================================*/

Resources::ResourcesManager::ResourcesManager()
{
	Core::Log* _log = Core::Log::instance();

	_log->write("Creating ResourcesManager");

	//	Load used fonts
	loadFont("Resource/Fonts/abnes.ttf");
	loadFont("Resource/Fonts/arial.ttf");
}

Resources::ResourcesManager::~ResourcesManager()
{
	m_materialList.clear();
	for (auto t : m_textureList)
	{
		t.second->destroyTexture();
	}
	m_textureList.clear();

	m_meshList.clear();
	m_shaderList.clear();
}

void Resources::ResourcesManager::initialize()
{
	//	Create a none texture
	Resources::Texture("Assets/none.png", "None");

	Resources::Material none_mat;

	none_mat.m_diffuse = { 1.000f,.350f, 1.000f };
	none_mat.m_emissive = { 1.000f,.350f, 1.000f };

	none_mat.m_text_diffuse = TextureMap(m_textureList["None"].get());

	Core::lockFlag(m_matListFlag);

	if (m_materialList.find("None") == m_materialList.end())
	{
		m_materialList["None"] = std::make_shared<Resources::Material>(none_mat);
	}
	else
	{
		*m_materialList["None"] = Resources::Material(none_mat);
	}

	Core::unlockFlag(m_matListFlag);
}


/*=================================== Verifications ===================================*/



bool Resources::RawVerticesList::empty()
{
	if (this->Pos.empty()) return true;

	return false;
}


std::atomic_flag m_loadedFileFlag = ATOMIC_FLAG_INIT;
bool Resources::ResourcesManager::fileLoaded(const std::string& path)
{
	Core::lockFlag(m_loadedFileFlag);
	for (std::string curr_file : m_loadedFiles)
	{
		if (path == curr_file)
		{
			Core::unlockFlag(m_loadedFileFlag);
			return true;
		}
	}
	m_loadedFiles.push_back(path);

	Core::unlockFlag(m_loadedFileFlag);

	return false;
}

std::atomic_flag m_loadedOBJFlag = ATOMIC_FLAG_INIT;
bool Resources::ResourcesManager::OBJLoaded(const std::string& path)
{
	Core::lockFlag(m_loadedOBJFlag);
	for (std::string curr_file : m_loadedOBJFile)
	{
		if (path == curr_file)
		{
			Core::unlockFlag(m_loadedOBJFlag);
			return true;
		}
	}
	m_loadedOBJFile.push_back(path);

	Core::unlockFlag(m_loadedOBJFlag);

	return false;
}



std::atomic_flag m_loadedTextureFlag = ATOMIC_FLAG_INIT;
bool Resources::ResourcesManager::textureLoaded(const std::string& name)
{
	Core::lockFlag(m_loadedTextureFlag);
	for (std::string curr_file : m_loadedTextFile)
	{
		if (name == curr_file)
		{
			Core::unlockFlag(m_loadedTextureFlag);
			return true;
		}
	}
	m_loadedTextFile.push_back(name);

	Core::unlockFlag(m_loadedTextureFlag);

	return false;
}



void Resources::ResourcesManager::generateGLTextures()
{
	//	Lock the flag
	Core::lockFlag(m_textToGenFlag);
	if (m_textToGen.empty() == false)
	{
		for (std::string& t : m_textToGen)
		{
			//	Verify (just in case)
			if (m_textureList[t]->m_generated == false)
			{
				//	Try to cast to TextureCubeMap
				Resources::TextureCubeMap* cubemap = dynamic_cast<Resources::TextureCubeMap*>(m_textureList[t].get());

				//	If it's a cubeMap call it's own override generateGL() function
				if (cubemap)
				{
					cubemap->generateGL();

					//	And pass to next texture
					continue;
				}

				//	Else generate a default texture
				m_textureList[t].get()->generateGL();
			}
		}

		//	Clear the list now that they are loaded
		m_textToGen.clear();
	}
	//	Free the flag
	Core::unlockFlag(m_textToGenFlag);
}




void Resources::ResourcesManager::generateGLMeshes()
{
	//	Lock the flag
	Core::lockFlag(m_meshToGenFlag);
	if (m_meshToGen.empty() == false)
	{
		for (std::string& m : m_meshToGen)
		{
			//	Verify (just in case)
			if (m_meshList[m]->m_generated == false)
			{
				//	Generate current mesh
				m_meshList[m]->generateGL();
			}
		}
		//	Clear the list now that they are loaded
		m_meshToGen.clear();
	}
	//	Free the flag
	Core::unlockFlag(m_meshToGenFlag);
}



/*=================================== Sets / gets ===================================*/


//	Set Indices in order
void setIndices(std::istringstream& line, std::vector<unsigned int>& indices)
{
	for (int i = 0; i < 3; i++)
	{
		unsigned int indice[3];

		for (int j = 0; j < 3; j++)
		{
			line >> indice[j];

			//	Ignore '/'
			line.ignore();		
		}

		//	Coordinates	indices
		indices.push_back(indice[0] - 1);

		//	Texture UV indices
		indices.push_back(indice[1] - 1);

		//	Normal indices
		indices.push_back(indice[2] - 1);
	}
}



//	Sort Vertices and place them in a new mesh
Resources::Mesh setMesh(const Resources::RawVerticesList& rawList, const std::vector<unsigned int>& indices)
{
	std::vector<Resources::Vertex> vertices;

	//	Sort the vertices
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Resources::Vertex	vrt;
		size_t posIndex = indices[i];
		size_t texCoordIndex = indices[i + 1];
		size_t normalsIndex = indices[i + 2];

		vrt.Position = rawList.Pos[posIndex];
		vrt.TexCoords = rawList.Tex[texCoordIndex];
		vrt.Normals = rawList.Nor[normalsIndex];

		vertices.push_back(vrt);
	}

	//	Clear the RawList Indices, because, they do not reset 
	//	(Same indices for every meshes) Unlike vertices informations

	Resources::Mesh out(vertices);

	vertices.clear();

	return out;
}



void Resources::ResourcesManager::createNewTexture(const std::string& path, const std::string& textName)
{
	Texture newTexture(path, textName);
	m_atomicTextureList[textName] = A_UNLOCKED;
}



void Resources::ResourcesManager::createNewTextureCubeMap(const std::string& textName, const std::vector<std::string>& faces)
{
	TextureCubeMap newTexture(textName, faces);
	m_atomicTextureList[textName] = A_UNLOCKED;
}



Resources::Texture* Resources::ResourcesManager::loadTexture(const std::string& path, const std::string& textName)
{
	if (!textureLoaded(textName))
	{
		m_textureList[textName] = std::make_shared<Resources::Texture>();
		m_atomicTextureList[textName] = A_LOCKED;

#ifdef MULTITHREAD_ENABLED
		Core::ThreadPool* _pool = Core::ThreadPool::instance();
		_pool->addInQueue(std::bind(&Resources::ResourcesManager::createNewTexture, this, path, textName));
#else
		createNewTexture(path, textName);
#endif
	}
	return m_textureList[textName].get();
}



Resources::Texture* Resources::ResourcesManager::loadTextureCubeMap(const std::string& path, const std::string& textName)
{
	if (!textureLoaded(textName))
	{

		std::ifstream file;

		if (!FileParser::openFile(path, file)) return m_textureList["None"].get();

		int i = 0;

		std::vector<std::string> faces;

		std::string currentLine = "";
		std::string dir = Extractor::ExtractDirectory(path);

		while (std::getline(file, currentLine))
		{

			if (i > 6)
			{
				//  Put log warning
				break;
			}

			faces.push_back(dir + currentLine);

			i++;
		}
		file.close();


		m_textureList[textName] = std::make_shared<Resources::TextureCubeMap>();
		m_atomicTextureList[textName] = A_LOCKED;

#ifdef MULTITHREAD_ENABLED
		Core::ThreadPool* _pool = Core::ThreadPool::instance();
		_pool->addInQueue(std::bind(&Resources::ResourcesManager::createNewTextureCubeMap, this, textName, faces));
#else
		createNewTextureCubeMap(textName, faces);
#endif

	}

	return m_textureList[textName].get();
}



std::vector<const char*> Resources::ResourcesManager::getAllTextureName()
{
	std::vector<const char*> names;
	for (const auto& tex : m_textureList)
	{
		names.push_back(tex.first.c_str());
	}
	return names;
}



void  Resources::ResourcesManager::setTexture(const std::string& textName, Texture& text)
{
	Core::lockFlag(m_textListFlag);
	if (m_textureList.find(textName) == m_textureList.end())
	{
		m_textureList[textName] = std::make_shared<Resources::Texture>(text);
	}
	else
	{
		*m_textureList[textName] = Resources::Texture(text);
	}
	Core::unlockFlag(m_textListFlag);

	//	Add the loaded texture in the text to gen list

	Core::lockFlag(m_textToGenFlag);
	m_textToGen.push_back(textName);
	Core::unlockFlag(m_textToGenFlag);
}



void  Resources::ResourcesManager::setTexture(const std::string& textName, TextureCubeMap& text)
{
	Core::lockFlag(m_textListFlag);

	if (m_textureList.find(textName) == m_textureList.end())
	{
		m_textureList[textName] = std::make_shared<Resources::TextureCubeMap>(text);
	}
	else
	{
		*m_textureList[textName] = Resources::TextureCubeMap(text);
	}
	Core::unlockFlag(m_textListFlag);

	//	Add the loaded texture in the text to gen list

	Core::lockFlag(m_textToGenFlag);
	
	m_textToGen.push_back(textName);
			
	Core::unlockFlag(m_textToGenFlag);
}



void Resources::ResourcesManager::loadShader(const std::string& shader_name, const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path)
{
	if (!fileLoaded(shader_name))
	{
		if (m_shaderList.find(shader_name) == m_shaderList.end())
		{
			m_shaderList[shader_name] = std::make_shared<Resources::Shader>(vertex_path, fragment_path, geometry_path);
		}
		else
		{
			*m_shaderList[shader_name].get() = Resources::Shader(vertex_path, fragment_path, geometry_path);
		}
	}
}



void Resources::ResourcesManager::loadFont(const std::string& path)
{
	Core::Log* _log = Core::Log::instance();

	if (fileLoaded(path)) return;


	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		_log->writeError("FreeType -> Could not init FreeType Library");
	}

	FT_Face face;

	if (FT_New_Face(ft, path.c_str(), 0, &face))
	{
		_log->writeFailure("FreeType -> Failed to load font : \"" + path + "\"");
		return;
	}
	else
	{
		_log->writeSuccess("FreeType -> loaded font : \"" + path + "\"");
	}

	FT_Set_Pixel_Sizes(face, 0, 96);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction


	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 

		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// set texture options

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use

		Character character = {
			texture,
			{(float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows},
			{(float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top},
			(unsigned int)face->glyph->advance.x
		};

		m_characterListPerFonts[Extractor::ExtractFilename(path)][c] = character;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}



/*=================================== Parser ===================================*/



void Resources::ResourcesManager::parseMTL(const std::string& path, const std::string& fileName, stringList& material_name_list, bool& success)
{
	std::ifstream file;

	std::string fullpath = path + fileName;

	if (fileLoaded(fullpath))
	{
		success = true;
		return;
	}
	
	//	If opening is unsuccessful, remove it from the list of loaded files
	if (FileParser::openFile(fullpath, file) == false)
	{
		int i = 0;
		for (std::string& s : m_loadedFiles)
		{
			if (s == fullpath)
			{
				m_loadedFiles.erase(m_loadedFiles.begin() + i);
				break;
			}
			i++;
		}

		success = false;
		return;
	}
	

	std::string currentLine;
	std::string fileNameWithoutExtension = Extractor::ExtractNameWithoutExtension(fileName);


	bool is_empty = true;
	while (std::getline(file, currentLine))
	{
		if (currentLine[0] == '#' || currentLine[0] == '\n') continue;

		std::istringstream lineStream(currentLine);

		//	Get the type of the next values
		std::string type = FileParser::getString(lineStream);

		//	Material - New One
		if (type == "newmtl")
		{
			Resources::Material new_mat;

			new_mat.setPath() = path + fileName;

			std::string name = fileNameWithoutExtension + "_" + FileParser::getString(lineStream);

			material_name_list.push_back(name);

			new_mat.loadMaterial(file);

			Core::lockFlag(m_matListFlag);
			
			if (m_materialList.find(name) == m_materialList.end())
			{
				m_materialList[name] = std::make_shared<Resources::Material>(new_mat);
			}
			else
			{
				*m_materialList[name] = new_mat;
			}

			Core::unlockFlag(m_matListFlag);
			
			continue;
		}
	}

	file.close();

	Core::Log::instance()->writeSuccess("> Loaded MTL file : \"" + fullpath + "\"");

	success = true;
}



bool putInRawList(std::istringstream& lineStream, std::string& type, Resources::RawVerticesList& outRawList, std::vector<unsigned int>& out_indice)
{
	if (type == "v") outRawList.Pos.push_back(FileParser::getVector3(lineStream));

	else if (type == "vt") outRawList.Tex.push_back(FileParser::getVector2(lineStream));

	else if (type == "vn") outRawList.Nor.push_back(FileParser::getVector3(lineStream));

	else if (type == "f")  setIndices(lineStream, out_indice);

	else return false;

	return true;
}



//	Parse Object file
//	-----------------
bool Resources::ResourcesManager::parseOBJ(const std::string& path, const std::string& fileName)
{
	if (FileParser::CheckExtension(std::string(fileName), "obj") == false) return false;

	std::ifstream file;

	std::string fullpath = path + fileName;

	if (FileParser::openFile(fullpath, file) == false) return false;
	
	//	Variables
	//	---------

	RawVerticesList rawMeshList;

	std::vector<unsigned int> indices;

	stringList	material_name_list;

	std::string	currentLine;
	std::string currentMesh;
	std::string	MTLFile;

	bool hasMatLib = false;


	#ifdef MULTITHREAD_ENABLED
	std::atomic<bool>* threadStop = &Core::ThreadPool::instance()->m_stop;
	#endif

	//	FIle Parser Loop
	//	----------------

	std::string currMesh = "";

	bool done = false;
	while (!done)
	{
		done = true;
		if (std::getline(file, currentLine)) done = false;


		#ifdef MULTITHREAD_ENABLED
		if(*threadStop) return false;
		#endif

		if (currentLine[0] == '#') continue;

		std::istringstream lineStream(currentLine);

		//	Get the type of the next values
		std::string	type = FileParser::getString(lineStream);



		if ((type == "o" || done))
		{

			if (currMesh != "")
			{
				//	Mesh creation

				Core::lockFlag(m_meshListFlag);

				//	Create and save a shared ptr if the mesh with the name currMesh do not exist
				if (m_meshList.find(currMesh) == m_meshList.end())
				{
					m_meshList[currMesh] = std::make_shared<Resources::Mesh>(setMesh(rawMeshList, indices));
				}

				//	Else change the pointed value of the already existant sharedPtr
				else
				{
					*m_meshList[currMesh] = setMesh(rawMeshList, indices);
				}

				//	Set the path of the Mesh (used for saving the mesh in a scene)
				m_meshList[currMesh]->setPath() = path;
				
				Core::unlockFlag(m_meshListFlag);

				//	Add in the 'to generate' list so the program can generate(GL functions) the meshes later in the main thread

				Core::lockFlag(m_meshToGenFlag);
				m_meshToGen.push_back(currMesh);
				Core::unlockFlag(m_meshToGenFlag);

				Core::lockFlag(m_objListFlag);
				m_obj[std::string(fileName)].push_back(currMesh);
				Core::unlockFlag(m_objListFlag);

				//	Clear the indice list
				indices.clear();
			}


			if (type == "o")
			{
				//	Add the new name to the mesh name list
				currMesh = FileParser::getString(lineStream);
			}

			continue;
		}


		//	Get Unsorted Vertice data
		if (currMesh != "")
		{
			if (putInRawList(lineStream, type, rawMeshList, indices)) continue;

			//	Save the material used by the mesh in the "used material" list
			if (type == "usemtl")
			{
				if (hasMatLib)
				{
					//	Get material name
					std::string mtlName = Extractor::ExtractNameWithoutExtension(MTLFile) + "_" + FileParser::getString(lineStream);

					Core::lockFlag(m_bindedMeshMatListFlag);
					m_bindedMeshAndMaterial[currMesh] = mtlName;
					Core::unlockFlag(m_bindedMeshMatListFlag);


					continue;
				}

				//	Else if there isn't material lib OR the material doesn't exist,
				//	the used material is None for the current mesh

				Core::lockFlag(m_bindedMeshMatListFlag);
				m_bindedMeshAndMaterial[currMesh] = "None";
				Core::unlockFlag(m_bindedMeshMatListFlag);

				continue;
			}
		}

		//	Load material file that contain differents materials used by the OBJ
		if (type == "mtllib")
		{
			//	Parse MTL file
			lineStream >> MTLFile;

			bool success = false;

			parseMTL(path, MTLFile, material_name_list, success);

			if (success)
			{
				hasMatLib = true;
			}
			continue;
		}
	}

	file.close();

	Core::Log::instance()->writeSuccess("> Loaded OBJ file : \"" + fullpath + "\"");

	return true;
}



bool Resources::ResourcesManager::loadOBJ(const std::string& path, const std::string& fileName)
{
	//	If file loaded return true
	//	Else parse the file

	Core::Log* _log = Core::Log::instance();
	std::string fullpath = path + fileName;

	if (OBJLoaded(fullpath) == false)
	{
		_log->write("+ Loading new OBJ file : \"" + fullpath + "\"");
	}
	else
	{
		return true;
	}
	
	return parseOBJ(path, fileName);
}


/*=================================== ATTACHERS ===================================*/


Resources::Mesh* Resources::ResourcesManager::attachMesh(const std::string& meshName)
{
	Resources::Mesh* out;

	Core::lockFlag(m_meshListFlag);

	//	If non existant, create it
	if (m_meshList.find(meshName) == m_meshList.end())
	{
		m_meshList[meshName] = std::make_shared<Resources::Mesh>();
		//_log->writeFailure("Couldn't assign mesh \"" + modelName + "\"");
	}

	//	Then get the pointer in the out pointer
	out = m_meshList[meshName].get();

	Core::unlockFlag(m_meshListFlag);

	return out;
}


Resources::Shader* Resources::ResourcesManager::attachShader(const std::string& shaderName)
{
	Resources::Shader* out;

	Core::lockFlag(m_shadListFlag);

	//	If non existant, create it
	if (m_shaderList.find(shaderName) == m_shaderList.end())
	{
		m_shaderList[shaderName] = std::make_shared<Resources::Shader>();
	}

	//	Then get the pointer in the out pointer
	out = m_shaderList[shaderName].get();

	Core::unlockFlag(m_shadListFlag);

	return out;
}


Resources::Material* Resources::ResourcesManager::attachMaterial(const std::string& materialName)
{

	Resources::Material* out;

	Core::lockFlag(m_matListFlag);

	//	If non existant, create it
	if (m_materialList.find(materialName) == m_materialList.end())
	{
		m_materialList[materialName] = std::make_shared<Resources::Material>();
	}

	//	Then get the pointer in the out pointer
	out = m_materialList[materialName].get();

	Core::unlockFlag(m_matListFlag);

	return out;
}




/*=================================== IMGUI ===================================*/


void Resources::ResourcesManager::showImGuiResources()
{
	if (ImGui::CollapsingHeader("Resources", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& mat : m_materialList)
		{
			if (ImGui::TreeNode(mat.first.c_str()))
			{
				mat.second->showImGui();
				ImGui::TreePop();
			}
		}
	}
}



void Resources::ResourcesManager::showImGuiWindows()
{
	if (ImGui::Begin("ResourcesManager"))
	{
		//	Object Loader
		ImGui::PushID("ObjectLoader");
		ImGui::Text("Object Loader");

		static char OBJPath[256] = "Assets/";
		ImGui::InputText("###OBJPATH", OBJPath, IM_ARRAYSIZE(OBJPath));
		ImGui::SameLine();

		if (FileParser::CheckExtension(OBJPath, "obj"))
		{
			if (ImGui::Button("Load", { 50,20 }))
			{
				loadOBJ(
					Extractor::ExtractDirectory(std::string(OBJPath)),
					Extractor::ExtractFilename(std::string(OBJPath))
				);
			}
		}
		else ImGui::NewLine();
		ImGui::PopID();

		//	Texture Loader
		ImGui::PushID("TextureLoader");
		ImGui::Text("Texture Loader");

		static char TexturePath[256] = "Assets/";
		ImGui::InputText("###TEXPATH", TexturePath, IM_ARRAYSIZE(TexturePath));
		ImGui::SameLine();

		if (FileParser::CheckExtension(TexturePath, acceptedImageExtension, IM_ARRAYSIZE(acceptedImageExtension)))
		{
			if (ImGui::Button("Load", { 50,20 }))
			{
				loadTexture(
					TexturePath,
					Extractor::ExtractBeforeLast(Extractor::ExtractFilename(std::string(TexturePath)), '.', false)
				);
			}
		}
		else ImGui::NewLine();
		ImGui::PopID();

		//	Shader Loader
		ImGui::PushID("ShaderLoader");
		ImGui::Text("Shader Loader");

		static char ShaderName[256];
		ImGui::Text("Resource/Shader/");
		ImGui::InputText("###SHADPATH", ShaderName, IM_ARRAYSIZE(ShaderName));
		ImGui::SameLine();

		if (FileParser::CheckExtension(ShaderName, "shad"))
		{
			if (ImGui::Button("Load", { 50,20 }))
			{
				Resources::loadShadFile(Extractor::ExtractBeforeLast(ShaderName, '.', false));
			}
		}
		else ImGui::NewLine();
		ImGui::PopID();

		//	MTL Loader
		ImGui::PushID("MTLLoader");
		ImGui::Text("MTL Loader");

		static char MTLPath[256] = "Assets/";
		ImGui::InputText("###MTLPATH", MTLPath, IM_ARRAYSIZE(MTLPath));
		ImGui::SameLine();

		if (FileParser::CheckExtension(MTLPath, "mtl"))
		{
			if (ImGui::Button("Load", { 50,20 }))
			{
				bool success = false;
				Resources::stringList nonUsed;
				parseMTL(
					Extractor::ExtractDirectory(std::string(MTLPath)),
					Extractor::ExtractFilename(std::string(MTLPath)),
					nonUsed, success
				);
			}
		}
		else ImGui::NewLine();
		ImGui::PopID();
	}
	ImGui::End();
}
