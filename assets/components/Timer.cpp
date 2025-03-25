#include "Timer.h"

namespace K 
{
	REGISTER(Timer);

	Timer::Timer() 
	{
		
	}

	Timer::~Timer() 
	{
		
	}

	void Timer::Init() 
	{
		#if _DEBUG
			this->startCountDown = false;
		#else
			this->startCountDown = true;
		#endif
	}

	void Timer::Update() 
	{
		if (this->startCountDown)
		{
			if (this->countDown > 0.0f)
			{
				this->countDown -= K::Time::deltaTime();
			}
			else
			{
				K::SceneManager::LoadNextScene();
				this->startCountDown = false;
			}
		}
	}

	void Timer::UpdateEditor() 
	{
		if (ImGui::CollapsingHeader("Timer Settings")) 
		{
			ImGui::DragFloat("Countdown", &this->countDown);
		}
	}

	void Timer::Bind() 
	{

	}

	void Timer::Unbind() 
	{
		
	}
	
	void Timer::SetPropertyValues(const char* value, int valueIndex) 
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->countDown = std::stof(temp);
				break;
			}
		}
	}

	const char* Timer::GetPropertyValues() 
	{
		this->properties = std::to_string(this->countDown);
		return this->properties.c_str();
	}
}