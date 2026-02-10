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
		time_t tt = std::chrono::system_clock::to_time_t(currentDate);
		tm utc_tm = *gmtime(&tt);
		int year = utc_tm.tm_year + 1900;
		int month = utc_tm.tm_mon + 1;
		int day = utc_tm.tm_mday;
		float ut = utc_tm.tm_hour + (utc_tm.tm_min / 60.0f);
		float sign = ReturnSign((100 * year)+month-190002.5);
		std::cout << std::format("{0}.{1}.{2}", day, month, year) << std::endl;
		float julianDate = (367.0f * year) - ((7.0f/4.0f) * (year + ((month + 9)/12.0f))) + ((275.0f * month)/9.0f) + day + 1721013.5f - (0.5f * sign) + (ut / 24.0f) + 0.5f + 2.0f;
		return julianDate;
	}

	float NightKey::DaysSince2000()
	{
		float Jdate = CurrentDateToJulianDate();
		float n = Jdate - 2451545.0f + 0.0008;
		return n;
	}

	float NightKey::CalculateMeanSolarTime(float longitude)
	{
		float n = DaysSince2000();
		float meanSolarTime = n - (longitude / 360.0f);
		return meanSolarTime;
	}

	void NightKey::Init()
	{
		std::cout << DaysSince2000() << std::endl;
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