#pragma once

#include <unordered_map>
#include <Core/RendererManager.hpp>
#include <Physics/PhysicsManager.hpp>

#include <Engine/GameObject.hpp>

#include <IK/irrKlang.h>

#include <LowRenderer/Sprite.hpp>

namespace Resources
{

	struct subModelToGen
	{
		Transform3* parent;
		std::string shad;
		std::string path;
		bool finished;
		std::vector<std::string> meshLoaded;
	};

	class Scene
	{
	public:

		//	Constructor
		//	-----------

		Scene();
		~Scene();

		//	Public Internal Variables
		//	-------------------------

		std::atomic_flag subModelToGenListFlag = ATOMIC_FLAG_INIT;

		Core::RendererManager m_rendererManager;
		Physics::PhysicsManager m_physicsManager;

		std::unordered_map<int, GameObject*> m_objectList;
		
		Sprite m_loader_sprite = Sprite("Assets/Loading.png", "Loading");
		Transform3 m_loader_transform;

		bool m_pause = false;

		GameObject* m_selectedGameObject = nullptr;

		//	Sound

		bool ambientStarted = false;
		irrklang::ISound* ambient_music = nullptr;

		//	Public Internal Functions
		//	-------------------------
		GameObject* findGameObjectInScene(std::string name);
		
		std::map<std::string, subModelToGen> m_subModelToGen;

		//	Update function
		//	Parameters : none
		//	-----------------
		void gameLoop();
		void editLoop();

		void awake();
		void start();
		void update();
		void fixedUpdate();
		void lateUpdate();

		//	Draw function
		//	Parameters : none
		//	-----------------
		void draw();
		void drawLoading();
		//	Save current scene in the typed path
		//	Parameters : const char* path
		//	-----------------------------
		void saveScene(const char* path);


		//	Load Scene with the selected path
		//	Parameters : const char* path
		//	-----------------------------
		bool loadScene(const char* path);

		void generateSubGameObjects();

		void pause();

		//	Display ImGUI window of the scene class
		//	Parameters : none
		//	-----------------
		void showImGUIScene();
		void showImGUIGameObject();
	};
}