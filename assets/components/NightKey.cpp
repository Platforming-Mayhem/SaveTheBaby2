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

	double NightKey::CurrentDateToJulianDate()
	{
		auto currentDate = std::chrono::system_clock::now();
		double yesterdayJulianDate = (std::chrono::duration_cast<std::chrono::seconds>(currentDate.time_since_epoch()).count() - 86400) / 86400.0 + 2440587.5;
		return yesterdayJulianDate;
	}

	double NightKey::DaysSince2000()
	{
		double Jdate = CurrentDateToJulianDate();
		double n = Jdate - (2451545.0 + 0.0009) + 69.184 / 86400.0;
		return ceil(n);
	}

	double NightKey::CalculateMeanSolarTime(double longitude)
	{
		double n = DaysSince2000();
		double meanSolarTime = n + 0.0009 - longitude / 360.0;
		return meanSolarTime;
	}

	double NightKey::CalculateSolarMeanAnomaly(double longitude)
	{
		double jStar = CalculateMeanSolarTime(longitude);
		double m = fmod(357.5291 + 0.98560028 * jStar, 360.0);
		return m;
	}

	double NightKey::CalculateEquationOfCenter(double longitude)
	{
		double m = CalculateSolarMeanAnomaly(longitude);
		double conversion = std::numbers::pi / 180.0;
		double mRad = m * conversion;
		double c = 1.9148 * sin(mRad) + 0.02 * sin(2.0 * mRad) + 0.0003 * sin(3.0 * mRad);
		return c;
	}

	double NightKey::CalculateEclipticLongitude(double longitude)
	{
		double m = CalculateSolarMeanAnomaly(longitude);
		double c = CalculateEquationOfCenter(longitude);
		double lamda = fmod(m + c + 180.0 + 102.9372, 360.0);
		return lamda;
	}

	double NightKey::CalculateSolarTransit(double longitude)
	{
		double jStar = CalculateMeanSolarTime(longitude);
		double m = CalculateSolarMeanAnomaly(longitude);
		double conversion = std::numbers::pi / 180.0;
		double mRad = m * conversion;
		double lamda = CalculateEclipticLongitude(longitude);
		double lamdaRad = lamda * conversion;
		double jTransit = 2451545.0 + jStar + 0.0053 * sin(mRad) - 0.0069 * sin(2.0 * lamdaRad);
		return jTransit;
	}

	double NightKey::CalculateSunDeclination(double latitude, double longitude)
	{
		double conversion = std::numbers::pi / 180.0;
		double lamda = CalculateEclipticLongitude(longitude);
		double lamdaRad = lamda * conversion;
		double sigma = asin(sin(lamdaRad) * sin(23.4397f * conversion)) / conversion;
		return sigma;
	}

	double NightKey::CalculateHourAngle(double latitude, double longitude)
	{
		double conversion = std::numbers::pi / 180.0;
		double sigma = CalculateSunDeclination(latitude, longitude);
		double sigmaRad = sigma * conversion;
		double thetaRad = latitude * conversion;
		double altitudeAngleRad = -0.833 * conversion;
		double w0 = (sin(altitudeAngleRad) - sin(thetaRad) * sin(sigmaRad)) / (cos(thetaRad) * cos(sigmaRad));
		return acos(w0) / conversion;
	}

	double NightKey::CalculateSunset(double latitude, double longitude)
	{
		double jTransit = CalculateSolarTransit(longitude);
		double w0 = CalculateHourAngle(latitude, longitude);
		double jSet = jTransit + w0 / 360.0;
		double m = CalculateSolarMeanAnomaly(longitude);
		double c = CalculateEquationOfCenter(longitude);
		double jStar = CalculateMeanSolarTime(longitude);
		double lamda = CalculateEclipticLongitude(longitude);
		double sigma = CalculateSunDeclination(latitude, longitude);
		/*std::cout << std::setprecision(20) << std::fixed << "M:" << m << std::endl;
		std::cout << "S:" << sigma << std::endl;
		std::cout << "C:" << c << std::endl;
		std::cout << "J*:" << jStar << std::endl;
		std::cout << "L:" << lamda << std::endl;
		std::cout << "w0:" << w0 << std::endl;
		std::cout << "JTransit:" << (jTransit - 2440587.5) * 86400 << std::endl;*/
		return jSet;
	}

	double NightKey::CalculateJTimeToUTC(double latitude, double longitude)
	{
		double jSet = (CalculateSunset(latitude, longitude) - 2440587.5) * 86400.0;
		time_t time = jSet;
		std::cout << "J_Set:" << std::ctime(&time) << std::endl;
		return jSet;
	}

	void NightKey::Init()
	{
    	//std::cout << std::chrono::current_zone()->name() << '\n';
		CalculateJTimeToUTC(50.72, -1.8667);
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