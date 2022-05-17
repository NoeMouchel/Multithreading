#include <LowRenderer/Model.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Core/Log.hpp>

#include <Resources/ResourcesManager.hpp>
#include <Resources/Scene.hpp>

#include <Engine/GameObject.hpp>
#include <Engine/Transform3.hpp>


Model::Model(GameObject* in_gameObject) : Component(in_gameObject)
{
	init(ComponentType::Model);

	m_mesh = nullptr;
	m_material = nullptr;

	m_gameObject->m_sceneReference->m_rendererManager.m_modelList[(int)m_gameObject->m_sceneReference->m_rendererManager.m_modelList.size()] = this;
}

Model::~Model()
{
	m_mesh = nullptr;
	m_material = nullptr;
	m_shader = nullptr;
}


void Model::draw()
{
	if (m_shader && m_mesh)
	{
		m_shader->use();
		m_shader->setMat4("Model", m_transform->getTransformMatrix());

		if (!m_material)
		{
			Resources::ResourcesManager* resources = Resources::ResourcesManager::instance();
			m_material = resources->m_materialList["None"].get();
		}
		

		if (m_material != nullptr)
		{
			//	If no material instance, should take the default parameters of current material
			// Set up material parameters from our local instance

			Resources::Material mat = *m_material;
			mat.setMaterialValue(m_materialInstance);

			m_materialInstance = mat;

			m_shader->setMaterial(m_materialInstance);
		}

		m_mesh->draw();
	}
}


void Model::loadModel(const std::string& modelName, const std::string& shaderName, const std::string& fullPath)
{
	Resources::ResourcesManager* resources = Resources::ResourcesManager::instance();

	loadModel( modelName, resources->m_bindedMeshAndMaterial[modelName],  shaderName,  fullPath);
}


void Model::loadModel(const std::string& modelName, const std::string& matName, const std::string& shaderName, const std::string& fullPath)
{
	m_path = fullPath;

	m_meshName = modelName;
	m_materialName = matName;
	m_shaderName = shaderName;
	m_name = modelName;

	Core::Log* _log = Core::Log::instance();
	Resources::ResourcesManager* _r = Resources::ResourcesManager::instance();

	//	Get model datas

	//	MATERIALS
	m_material = _r->attachMaterial(m_materialName);

	//	MESH
	m_mesh = _r->attachMesh(modelName);

	//	SHADER
	m_shader = _r->attachShader(shaderName);
}


void Model::showImGUI()
{
	Resources::ResourcesManager* resources = Resources::ResourcesManager::instance();

	//	DEBUG
	if (ImGui::Checkbox("Show Normals", &debugNormal))
	{
		m_shader->use();
		m_shader->setBool("showNormal", debugNormal);
	}

	//	END DEBUG
	const char* meshPreview = m_mesh ? m_meshName.c_str() : "none";

	if (ImGui::BeginCombo("Mesh", meshPreview))
	{
		for (auto& mesh : resources->m_meshList)
		{
			if (ImGui::Selectable(mesh.first.c_str(), mesh.first == meshPreview))
			{
				m_mesh = mesh.second.get();
				m_meshName = mesh.first;
			}
		}
		ImGui::EndCombo();
	}

	const char* materialPreview = m_materialName.c_str();

	if (ImGui::BeginCombo("Material", materialPreview))
	{
		for (auto& material : resources->m_materialList)
		{
			if (ImGui::Selectable(material.first.c_str(), material.first == materialPreview))
			{
				m_material = material.second.get();
				//m_material = &material.second;
				m_materialName = material.first;
			}
		}
		ImGui::EndCombo();
	}

	m_materialInstance.showImGui();

	const char* shaderPreview = m_shader ? m_shaderName.c_str() : "none";

	if (ImGui::BeginCombo("Shader", shaderPreview))
	{
		for (auto& shader : resources->m_shaderList)
		{
			if (ImGui::Selectable(shader.first.c_str(), shader.first == shaderPreview))
			{
				m_shader = shader.second.get();
				m_shaderName = shader.first;
			}
		}
		ImGui::EndCombo();
	}
}

void Model::destroy()
{
	//  get Renderer Manager of the scene
	Core::RendererManager* _renderer = &m_gameObject->m_sceneReference->m_rendererManager;

	//  Get last index (trash index)
	int lastIndex = (int)_renderer->m_modelList.size() - 1;

	//  Parsing list of the current renderer component type
	for (auto comp : _renderer->m_modelList)
	{
		//  Once we find "this", let's procees to the retirement of the component from the list
		if (comp.second == this)
		{
			//  Swap current with the last one if it's not already the last one
			if (comp.first != lastIndex)
			{
				_renderer->m_modelList[comp.first] = _renderer->m_modelList[lastIndex];
			}

			//  Set the pointer to null
			_renderer->m_modelList[lastIndex] = nullptr;

			//  Then erase it from the list
			_renderer->m_modelList.erase(lastIndex);

			//  And leave the loop
			//  To avoid errors and optimize
			break;
		}
	}

	//  Then call the common component destoy function
	Component::destroy();
}


void Model::saveComponentInSCNFile(std::ofstream& file)
{
	file << "SINGLEMODEL\t" << m_path << " " << m_name << " " << m_shaderName << " " << m_materialInstance.getPath() << " " << m_materialName << " ";
	m_materialInstance.saveInSCNFile(file);
	file << "\n";
}

void Model::loadComponentFromSCNFile(std::istringstream& lineStream)
{
	m_materialInstance.loadFromSCNFile(lineStream);
}