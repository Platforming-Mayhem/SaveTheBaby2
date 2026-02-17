#pragma once
#include <K_Engine.h>
#include "Lock.h"

namespace K 
{
	class KC_API NightKey : public K::Component
	{
	private:
		std::vector<int> gIndices;
		std::map<K::Lock*, char> affectedLocks;
		std::map<K::Lock*, K::GameObject*> locks;

		float jRise, jSet;

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

		double CalculateSunrise(double latitude, double longitude);

		tm* CalculateSunsetToGMT(double latitude, double longitude);
		
		tm* CalculateSunriseToGMT(double latitude, double longitude);

		tm* GetCurrentTime();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}