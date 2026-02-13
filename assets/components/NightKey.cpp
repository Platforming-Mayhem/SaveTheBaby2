#include "NightKey.h"

namespace K 
{
	REGISTER(NightKey);

	NightKey::NightKey()
	{
		
	}

	NightKey::~NightKey()
	{
		
	}

	float NightKey::ReturnSign(float value)
	{
		if(value > 0.0f)
		{
			return 1.0f;
		}
		else if(value < 0.0f)
		{
			return -1.0f;
		}
		else
		{
			return 0.0f;
		}
	}

	float NightKey::CurrentDateToJulianDate()
	{
		auto currentDate = std::chrono::system_clock::now();
		//std::gmtime(currentDate);
		float julianDate = std::chrono::duration_cast<std::chrono::seconds>(currentDate.time_since_epoch()).count() / 86400.0f + 2440587.5f;
		return julianDate;
	}

	float NightKey::DaysSince2000()
	{
		float Jdate = CurrentDateToJulianDate();
		float n = Jdate - (2451545.0f + 0.0009f) + 69.184f / 86400.0f;
		return ceil(n);
	}

	float NightKey::CalculateMeanSolarTime(float longitude)
	{
		float n = DaysSince2000();
		float meanSolarTime = n + 0.0009 - longitude / 360.0f;
		return meanSolarTime;
	}

	float NightKey::CalculateSolarMeanAnomaly(float longitude) 
	{
		float jStar = CalculateMeanSolarTime(longitude);
		float m = fmod(357.5291f + 0.98560028f * jStar, 360.0f);
		return m;
	}

	float NightKey::CalculateEquationOfCenter(float longitude) 
	{
		float m = CalculateSolarMeanAnomaly(longitude);
		float conversion = std::numbers::pi / 180.0f;
		float mRad = m * conversion;
		float c = 1.9148f * sin(mRad) + 0.02f * sin(2.0f * mRad) + 0.0003f * sin(3.0f * mRad);
		return c;
	}

	float NightKey::CalculateEclipticLongitude(float longitude) 
	{
		float m = CalculateSolarMeanAnomaly(longitude);
		float c = CalculateEquationOfCenter(longitude);
		float lamda = fmod(m + c + 180.0f + 102.9372f, 360.0f);
		return lamda;
	}

	float NightKey::CalculateSolarTransit(float longitude) 
	{
		float jStar = CalculateMeanSolarTime(longitude);
		float m = CalculateSolarMeanAnomaly(longitude);
		float conversion = std::numbers::pi / 180.0f;
		float mRad = m * conversion;
		float lamda = CalculateEclipticLongitude(longitude);
		float lamdaRad = lamda * conversion;
		float jTransit = 2451545.0f + jStar + 0.0053f * sin(mRad) - 0.0069f * sin(2.0f * lamdaRad);
		return jTransit;
	}

	float NightKey::CalculateSunDeclination(float latitude, float longitude) 
	{
		float conversion = std::numbers::pi / 180.0f;
		float lamda = CalculateEclipticLongitude(longitude);
		float lamdaRad = lamda * conversion;
		float sin_d = sin(lamdaRad) * sin(23.4397 * conversion);
		float cos_d = cos(asin(sin_d));
		float sigma = asin((sin((-0.833 - 2.076 * sqrt(0.0f) / 60.0) * conversion) - sin(latitude * conversion) * sin_d) / (cos(latitude * conversion) * cos_d)) / conversion;
		return sigma;
	}

	float NightKey::CalculateHourAngle(float latitude, float longitude) 
	{
		float conversion = std::numbers::pi / 180.0f;
		float sigma = CalculateSunDeclination(latitude, longitude);
		float sigmaRad = sigma * conversion;
		float thetaRad = latitude * conversion;
		float w0 = acos((sin(-0.833f * conversion) - sin(thetaRad) * sin(sigmaRad)) / (cos(thetaRad) * cos(sigmaRad))) / conversion;
		return w0;
	}

	float NightKey::CalculateSunset(float latitude, float longitude)
	{
		float jTransit = CalculateSolarTransit(longitude);
		float w0 = CalculateHourAngle(latitude, longitude);
		float jSet = jTransit + w0 / 360.0f;
		float m = CalculateSolarMeanAnomaly(longitude);
		float jStar = CalculateMeanSolarTime(longitude);
		float lamda = CalculateEclipticLongitude(longitude);
		std::cout << std::setprecision(20) << "M:" << m << std::endl;
		std::cout << "J*:" << jStar << std::endl;
		std::cout << "L:" << lamda << std::endl;
		std::cout << "JTransit:" << jTransit << std::endl;
		return jSet;
	}

	float NightKey::CalculateJTimeToUTC(float latitude, float longitude) 
	{
		float jSet = CalculateSunset(latitude, longitude);
		std::cout << "J_Set:" << (jSet - 2440587.5f) * 86400.0f << std::endl;
		return jSet;
	}

	void NightKey::Init()
	{
		CalculateJTimeToUTC(50.72f, -1.8667f);
	}

	void NightKey::Update() 
	{
		
	}

	void NightKey::UpdateEditor()
	{
		if (ImGui::CollapsingHeader(this->GetName()))
		{

		}
	}

	void NightKey::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				break;
			}
		}
	}

	const char* NightKey::GetPropertyValues()
	{
		return this->properties.c_str();
	}
}