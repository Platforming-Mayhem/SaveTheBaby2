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

		float CalculateSolarMeanAnomaly(float longitude);

		float CalculateEquationOfCenter(float longitude);

		float CalculateEclipticLongitude(float longitude);

		float CalculateSolarTransit(float longitude);
		
		float CalculateSunDeclination(float latitude, float longitude);

		float CalculateHourAngle(float latitude, float longitude);

		float CalculateSunset(float latitude, float longitude);

		float CalculateJTimeToUTC(float latitude, float longitude);

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}