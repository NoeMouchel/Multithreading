#pragma once

#include <Utils/Singleton.h>

class Timer;

namespace Core
{
	class TimeManager : public Singleton<TimeManager>
	{
	private:
		//	Private Variables
		//	-----------------

		double oldTime = 0.0f;
		double newTime = 0.0f;

		float timeStock = 0.f;
		float deltaTimeUnscaled = 0.f;

	public:

		//	Public Variables
		//	----------------

		float deltaTime = 0.f;
		float fixedDeltaTime = 0.02f;
		float timeScale = 1.f;

		//	Constructor
		//  -----------
		
		TimeManager() = default;

		//	Public Functions
		//	----------------

		void setDeltaTime();
		void setFixedDeltaTime();

		bool fixedDeltaTimeLoop();

		float getTime();
	};
}