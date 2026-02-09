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

	void NightKey::Init()
	{
		char mystring[100];
		FILE* file = popen("curl http://ip-api.com/json\?fields\=lat --silent | jq '.lat'", "r");
		std::fgets(mystring , 100 , file);
		pclose(file);
		float latitude = std::stof(mystring);
		auto currentDate = std::chrono::system_clock::now();
		time_t tt = std::chrono::system_clock::to_time_t(currentDate);
		tm local_tm = *localtime(&tt);
		mktime( &local_tm );
		float d = local_tm.tm_yday;
		float conversion = std::numbers::pi / 180.0f;
		float sunDeclination = 23.45 * std::sin((360.0f/365.0f) * (d+284.0f) * conversion);
		float fractionalYear = ((2 * std::numbers::pi) / 365.0f) * (d - 1);
		float EoT = 229.18f*(0.000075+0.001868*std::cos(fractionalYear))
		std::cout << latitude << ":" << sunDeclination << std::endl;
		float h = std::acos(-std::tan(latitude * conversion)*std::tan(sunDeclination * conversion)) * (1.0f / conversion) / 15.0f;
		std::cout << h << std::endl;
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