#pragma once
#include <K_Engine.h>

namespace K 
{
	KC_API class Timer : public K::Component
	{
	private:
		std::string properties;
		bool startCountDown;
		float countDown = 1.0f;
	public:

		Timer();

		~Timer();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void Bind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}