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

		float ReturnSign(float value);

		double CurrentDateToJulianDate();

		double DaysSince2000();

		double CalculateMeanSolarTime(double longitude);

		double CalculateSolarMeanAnomaly(double longitude);

		double CalculateEquationOfCenter(double longitude);

		double CalculateEclipticLongitude(double longitude);

		double CalculateSolarTransit(double longitude);
		
		double CalculateSunDeclination(double latitude, double longitude);

		double CalculateHourAngle(double latitude, double longitude);

		double CalculateSunset(double latitude, double longitude);

		double CalculateJTimeToUTC(double latitude, double longitude);

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}