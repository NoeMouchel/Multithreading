#include <Engine/GameObject.hpp>
#include <Engine/Transform3.hpp>

#include <Core/Log.hpp>
#include <Config.hpp>

#include <LowRenderer/Model.hpp>
#include <LowRenderer/Light.hpp>
#include <LowRenderer/Camera.hpp>
#include <LowRenderer/Sprite.hpp>
#include <LowRenderer/ParticleSystem.hpp>
#include <LowRenderer/SpriteBillboard.h>

#include <Scripts/Player.hpp>
#include <Scripts/Enemy.hpp>
#include <Scripts/Weapon.hpp>
#include <Scripts/Button.hpp>
#include <Scripts/PauseMenu.hpp>

#include <Physics/RigidBody3.hpp>
#include <Physics/Collider3.hpp>
#include <Physics/Collider2.hpp>
using namespace Physics;

#include <Resources/ResourcesManager.hpp>
#include <Resources/Scene.hpp>

#include <Utils/StringExtractor.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <typeinfo>


GameObject::GameObject()
{
	addComponent<Transform3>();
	m_sceneReference = nullptr;
}

GameObject::GameObject(Resources::Scene* in_sceneReference)
{
	addComponent<Transform3>();
	m_sceneReference = in_sceneReference;

	m_key = (int)m_sceneReference->m_objectList.size();
}

GameObject::~GameObject()
{
	Core::Log* _log = Core::Log::instance();
	_log->write("-\t Destroying Object : " + m_name);

	while (m_componentList.empty() == false)
	{
		m_componentList[0]->destroy();
	}
}

bool GameObject::isActive()
{
	if (m_transform->m_parent && m_isEnabled)
	{
		return m_transform->m_parent->m_gameObject->isActive();
	}
	return m_isEnabled;
}


void GameObject::birth()
{
	for (auto comp : m_componentList)
	{
		comp.second->birth();
	}
}

void GameObject::awake()
{
	for (auto comp : m_componentList)
	{
		if (comp.second->isActive() && !comp.second->m_hasAwaken)
		{
			comp.second->awake();
			comp.second->m_hasAwaken = true;
		}
	}
}

void GameObject::start()
{
	for (auto comp : m_componentList)
	{
		if (comp.second->isActive() && !comp.second->m_hasStarted)
		{
			comp.second->start();
			comp.second->m_hasStarted = true;
		}
	}
}


void GameObject::update()
{
	for (auto comp : m_componentList)
	{
		if (comp.second->isActive())
		{
			comp.second->update();
		}
	}
}


void GameObject::fixedUpdate()
{
	for (auto comp : m_componentList)
	{
		if (comp.second->isActive())
		{
			comp.second->fixedUpdate();
		}
	}
}

void GameObject::lateUpdate()
{
	for (auto comp : m_componentList)
	{
		if (comp.second->isActive())
		{
			comp.second->lateUpdate();
		}
	}
}


void GameObject::destroy()
{

	//	Get last index (trash index)
	int lastComp = (int)m_sceneReference->m_objectList.size() - 1;

	//	Swap positions in the list
	m_sceneReference->m_objectList[m_key] = m_sceneReference->m_objectList[lastComp];
	m_sceneReference->m_objectList[m_key]->m_key = m_key;

	//	Delete last component
	m_sceneReference->m_objectList[lastComp] = nullptr;
	m_sceneReference->m_objectList.erase(lastComp);

	//	Delete self
	delete this;
}


bool GameObject::hasUniqueComponentOfType(const int i)
{
	int k;	//	unused

	switch (i)
	{
	case (int)ComponentType::Transform:			return tryGetComponentIndex<Transform3>(k);
	case (int)ComponentType::Model:				return tryGetComponentIndex<Model>(k);
	case (int)ComponentType::Camera:			return tryGetComponentIndex<Camera>(k);
	case (int)ComponentType::Light:				return tryGetComponentIndex<Light>(k);
	case (int)ComponentType::RigidBody:			return tryGetComponentIndex<Rigidbody3>(k);
	case (int)ComponentType::SphereCollider3D:	return tryGetComponentIndex<SphereCollider3D>(k);
	case (int)ComponentType::BoxCollider3D:		return tryGetComponentIndex<BoxCollider3D>(k);
	case (int)ComponentType::BoxCollider2D:		return tryGetComponentIndex<BoxCollider2D>(k);
	case (int)ComponentType::Sprite:			return tryGetComponentIndex<Sprite>(k);
	case (int)ComponentType::Player:			return tryGetComponentIndex<Player>(k);
	case (int)ComponentType::Enemy:				return tryGetComponentIndex<Player>(k);
	case (int)ComponentType::Weapon:			return tryGetComponentIndex<Weapon>(k);
	case (int)ComponentType::Button:			return tryGetComponentIndex<Button>(k);
	case (int)ComponentType::PauseMenu:			return tryGetComponentIndex<PauseMenu>(k);
	default: return false;
	}
}

void GameObject::addComponentOfType(const int i)
{
	switch (i)
	{
		case (int)ComponentType::Transform:			addComponent<Transform3>();			break;
		case (int)ComponentType::Model:				addComponent<Model>();				break;
		case (int)ComponentType::Camera:			addComponent<Camera>();				break;
		case (int)ComponentType::Light:				addComponent<Light>();				break;
		case (int)ComponentType::ParticleSystem:	addComponent<ParticleSystem>();		break;
		case (int)ComponentType::SpriteBillboard:	addComponent<SpriteBillboard>();	break;
		case (int)ComponentType::RigidBody:			addComponent<Rigidbody3>();			break;
		case (int)ComponentType::SphereCollider3D:	addComponent<SphereCollider3D>();	break;
		case (int)ComponentType::BoxCollider3D:		addComponent<BoxCollider3D>();		break;
		case (int)ComponentType::BoxCollider2D:		addComponent<BoxCollider2D>();		break;
		case (int)ComponentType::Sprite:			addComponent<Sprite>();				break;
		case (int)ComponentType::Player:			addComponent<Player>();				break;
		case (int)ComponentType::Enemy:				addComponent<Enemy>();				break;
		case (int)ComponentType::Weapon:			addComponent<Weapon>();				break;
		case (int)ComponentType::Button:			addComponent<Button>();				break;
		case (int)ComponentType::PauseMenu:			addComponent<PauseMenu>();			break;
		default: break;
	}
}




//	Add Model Component from .scn file
//	--------------------------------------


void ConstructModels(const std::string& directory, const std::string& file, const std::string& shaderName, Resources::Scene* sceneRef, Transform3* parent)
{
	Resources::ResourcesManager* resources = Resources::ResourcesManager::instance();

	Core::lockFlag(sceneRef->subModelToGenListFlag);
	sceneRef->m_subModelToGen[file] = { parent , shaderName,directory + file, false };
	Core::unlockFlag(sceneRef->subModelToGenListFlag);

	if (resources->loadOBJ(directory, file) == false)
	{
		Core::lockFlag(sceneRef->subModelToGenListFlag);

		sceneRef->m_subModelToGen.erase(file);

		Core::unlockFlag(sceneRef->subModelToGenListFlag);

		return;
	}

	Core::lockFlag(sceneRef->subModelToGenListFlag);
	sceneRef->m_subModelToGen[file].finished = true;
	Core::unlockFlag(sceneRef->subModelToGenListFlag);

	#ifdef DETAILED_LOG
	Core::Log* _log = Core::Log::instance();
	_log->write("+ Adding Model to new gameObject");
	#endif

}



void GameObject::loadOBJModelFromStringStream(std::istringstream& lineStream)
{

	std::string directory;
	std::string file;
	FileParser::separatePathAndName(directory, file, lineStream);

	std::string shaderName = FileParser::getString(lineStream);

	Resources::loadShadFile(shaderName);

	#ifdef MULTITHREAD_ENABLED
		Core::ThreadPool* _pool = Core::ThreadPool::instance();
		_pool->addInQueue(std::bind(&ConstructModels,directory, file, shaderName, m_sceneReference, m_transform));
	#else
		ConstructModels(directory, file, shaderName, m_sceneReference, m_transform);
	#endif
}


struct ModelDatas
{
	std::string directory;
	std::string file;
	std::string shaderName;
	std::string modelName;
	std::string matFile;
	std::string matDirectory;
	std::string materialName;
};


void constructSingleModel(Model* in_model, const ModelDatas d)
{
	Resources::ResourcesManager* resources = Resources::ResourcesManager::instance();

	if (resources->loadOBJ(d.directory, d.file) == false) return;

	if (!in_model) return;


	if (d.matFile != "")
	{
		bool success;
		Resources::stringList materialList;
		resources->parseMTL(d.matDirectory, d.matFile, materialList, success);

		in_model->loadModel(d.modelName, d.materialName, d.shaderName, d.directory + d.file);
	}
	else
	{
		//	Get model datas
		in_model->loadModel(d.modelName, d.shaderName, d.directory + d.file);
	}
}


void GameObject::loadSingleModelFromStringStream(std::istringstream& lineStream)
{
	//	Stocking model datas
	//	--------------------

	ModelDatas d;

	FileParser::separatePathAndName(d.directory, d.file, lineStream);

	d.modelName = FileParser::getString(lineStream);

	d.shaderName = FileParser::getString(lineStream);
	FileParser::separatePathAndName(d.matDirectory, d.matFile, lineStream);

	d.materialName = FileParser::getString(lineStream);

	Resources::loadShadFile(d.shaderName);

	Resources::ResourcesManager* resources = Resources::ResourcesManager::instance();

	#ifdef DETAILED_LOG
	Core::Log* _log = Core::Log::instance();
	_log->write("+ Adding Model to new gameObject");
	#endif

	//	Creating New Models from loaded meshes
	//	--------------------------------------

	//	Create model component
	Model* _Model = addComponent<Model>();

	#ifdef MULTITHREAD_ENABLED
		Core::ThreadPool* _pool = Core::ThreadPool::instance();
		_pool->addInQueue(std::bind(&constructSingleModel, _Model, d));
	#else
		constructSingleModel(_Model, d);
	#endif

	_Model->loadComponentFromSCNFile(lineStream);
}





void GameObject::loadFromScnFile(std::ifstream& file, Resources::Scene& sceneReference)
{
	std::string type = "";
	std::string curr_line;

#ifdef MULTITHREAD_ENABLED
	Core::ThreadPool* _pool = Core::ThreadPool::instance();
#endif
	while (type != "END")
	{
		//m_sceneReference->drawLoading();
		
		std::getline(file, curr_line);
		std::istringstream lineStream(curr_line);

		//	Get the type of the next values
		type = FileParser::getString(lineStream);

		if (type[0] == '#') continue;
		if (type == "TRANSFORM")			m_transform->loadComponentFromSCNFile(lineStream);
		else if (type == "FILEMODEL")		loadOBJModelFromStringStream(lineStream);
		else if (type == "SINGLEMODEL")		loadSingleModelFromStringStream(lineStream);
		else if (type == "LIGHT")			addComponent<Light>()->loadComponentFromSCNFile(lineStream);
		else if (type == "CAMERA")			addComponent<Camera>()->loadComponentFromSCNFile(lineStream);
		else if (type == "PARTICLESYSTEM")	addComponent<ParticleSystem>()->loadComponentFromSCNFile(lineStream);
		else if (type == "SPRITEBILLBOARD")	addComponent<SpriteBillboard>()->loadComponentFromSCNFile(lineStream);
		else if (type == "RIGIDBODY")		addComponent<Rigidbody3>()->loadComponentFromSCNFile(lineStream);
		else if (type == "SCOLLIDER3")		addComponent<SphereCollider3D>()->loadComponentFromSCNFile(lineStream);
		else if (type == "BCOLLIDER3")		addComponent<BoxCollider3D>()->loadComponentFromSCNFile(lineStream);
		else if (type == "BCOLLIDER2")		addComponent<BoxCollider2D>()->loadComponentFromSCNFile(lineStream);
		else if (type == "SPRITE")			addComponent<Sprite>()->loadComponentFromSCNFile(lineStream);

		else if (type == "PLAYER")			addComponent<Player>()->loadComponentFromSCNFile(lineStream);
		else if (type == "ENEMY")			addComponent<Enemy>()->loadComponentFromSCNFile(lineStream);
		else if (type == "WEAPON")			addComponent<Weapon>()->loadComponentFromSCNFile(lineStream);
		else if (type == "BUTTON")			addComponent<Button>()->loadComponentFromSCNFile(lineStream);
		else if (type == "PAUSEMENU")		addComponent<PauseMenu>()->loadComponentFromSCNFile(lineStream);
		else if (type != "END")
		{
			Core::Log* _log = Core::Log::instance();
			_log->writeWarning("couldn't find component type : " + type);
		}
	}
}

void loadGameObject(std::ifstream& file, std::istringstream& lineStream, Resources::Scene& sceneReference)
{
	Core::Log* _log = Core::Log::instance();

	//	GameObject out;
	int i = (int)sceneReference.m_objectList.size();
	GameObject* go = sceneReference.m_objectList[i] = new GameObject(&sceneReference);

	//	Set gameObject name
	go->m_name = FileParser::getString(lineStream);
	go->m_isEnabled = FileParser::getBool(lineStream);
	go->m_isStatic = FileParser::getBool(lineStream);
	go->m_layer = LayerDatas::instance()->m_layerList[FileParser::getString(lineStream)];
	_log->write("+ Loading Game Object \"" + go->m_name + "\"");

	go->loadFromScnFile(file, sceneReference);
}


std::string GameObject::getGameObjectID() const
{
	if (m_transform->m_parent)
	{
		return m_transform->m_parent->m_gameObject->getGameObjectID() + "_" + m_name;
	}
	return m_name;
}


//	ImGui of GameObject
//	--------------------

void GameObject::showImGUIGameObject()
{
	if (ImGui::Checkbox("Enable Object", &m_isEnabled))
	{
		manuallyDisabled = !m_isEnabled;
	}
	ImGui::Checkbox("Is Static", &m_isStatic);


	if (ImGui::Button("Delete GameObject"))
	{
		m_sceneReference->m_selectedGameObject = nullptr;
		destroy();
	
		return;
	}


	LayerDatas* _layers = LayerDatas::instance();

	std::string layerPreview = m_layer.name;

	ImGui::NewLine();
	if (ImGui::BeginCombo("Layer", layerPreview.c_str()))
	{
		for (auto& layer : _layers->m_layerList)
		{
			if (m_layer.layer == layer.second.layer) continue;

			if (ImGui::Selectable(layer.second.name.c_str()))
			{
				m_layer = layer.second;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Text("Components : ");

	int id = 0;
	int lastComp = (int)m_componentList.size() - 1;

	for (auto comp : m_componentList)
	{
		ImGui::PushID(("Component" + std::to_string(id)).c_str());
		Component* c = comp.second;
		c->showImGUIComponent();

		ImGui::PopID();

		if(m_componentList.find(lastComp) == m_componentList.end())
		{
			break;
		}

		id++;
	}

	const char* componentPreview = "none";

	ComponentTypeInfo* _componentManager = ComponentTypeInfo::instance();
	ImGui::NewLine();
	if (ImGui::BeginCombo("Add component", componentPreview))
	{
		for (int n = 0; n < (int)ComponentType::COUNT; n++)
		{
			if (_componentManager->m_componentList[n].isUnique && hasUniqueComponentOfType(n)) continue;

			const char* current = _componentManager->m_componentList[n].name.c_str();

			if (ImGui::Selectable(current))
			{
				addComponentOfType(n);
			}
		}
		ImGui::EndCombo();
	}
}
