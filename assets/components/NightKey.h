#pragma once
#include <K_Engine.h>

namespace K 
{
	class KC_API NightKey : public K::Component
	{
	private:
		std::string properties;
	public:
		NightKey();

		~NightKey();

		float CurrentDateToJulianDate();

		float DaysSince2000();

		float ReturnSign(float value);

		float CalculateMeanSolarTime(float longitude);

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}