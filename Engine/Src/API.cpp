#include <iostream>

#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Config.hpp>
#include <API.hpp>

#include <Core/Window.hpp>
#include <Core/TimeManager.h>
#include <Core/InputsManager.hpp>
#include <Core/GameManager.hpp>
#include <LowRenderer/Text.hpp>

#include <Core/Log.hpp>
#include <Resources/Texture.hpp>
#include <Resources/Shader.hpp>
#include <Core/Graph.hpp>
#include <Core/ThreadPool.hpp>

struct mode
{
	int		plf = 2;
	bool	cull_face = false;
	bool	depth_test = false;
};


/*==================================================================================*/
/*=====================================- INIT -=====================================*/
/*==================================================================================*/

int API::init()
{
	Core::Window* _window = Core::Window::instance();

	//  Setup Dear ImGui
	m_editor.init(_window->m_window);
	
	return 0;
}


void API::loading()
{
	Core::Graph* _graph = Core::Graph::instance();
	_graph->initialize();
}


/*==================================================================================*/
/*=====================================- LOOP -=====================================*/
/*==================================================================================*/

static void newFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


static void endFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void API::windowLoop()
{
	Core::TimeManager* _time     = Core::TimeManager::instance();
	Core::Window* _window		 = Core::Window::instance();

	Core::Graph* _graph			 = Core::Graph::instance();
	Core::Log* _log				 = Core::Log::instance();

	Core::InputsManager* _inputs = Core::InputsManager::instance();
	Core::GameManager* _manager  = Core::GameManager::instance();
	TextRender* _textRender      = TextRender::instance();

	m_editor.m_graph = _graph;
	m_editor.setTheme();

	_inputs->init();

	_graph->loadScene("Assets/MainMenu.scn");
	_graph->m_mode = EngineMode::FULLPLAYMODE;
	// render loop
	glEnable(GL_LINE_SMOOTH);

	while (!glfwWindowShouldClose(_window->m_window) && _graph->m_quit == false)
	{
		//	Set Time
		_window->update();
		_time->setDeltaTime();
		_inputs->updateInputs();
		_manager->update();

		newFrame();

		glClearColor(0.330f, 0.315f, 0.305f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_graph->graphLoop();

		if (_graph->m_mode != EngineMode::FULLPLAYMODE)
		{
			m_editor.updateEditorWindows();
		}

		endFrame();
		glfwSwapBuffers(_window->m_window);
		glfwPollEvents();
	}

	end();
}


void API::end()
{
	m_editor.popTheme();

	//	Kill in a designed order
	//	Threadpool mustg be before the resources manager to not abort

	#ifdef MULTITHREAD_ENABLED
	Core::ThreadPool::instance()->kill();
	#endif

	Core::GameManager::instance()->kill();
	Core::Graph::instance()->kill();

	Resources::ResourcesManager::instance()->kill();
	Core::TimeManager::instance()->kill();
	Core::InputsManager::instance()->kill();
	TextRender::instance()->kill();

	Core::Log::instance()->kill();

	Core::Window::instance()->kill();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	ImGui::DestroyContext();
}