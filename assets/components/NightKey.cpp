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
		float conversion = 180.0f / std::numbers::pi;
		float sunDeclination = -23.45 * cos((360.0f/365.0f) * (d*10.0f) * (1.0f / conversion));
		latitude = 50.722f;
		sunDeclination = -19.0f;
		float h = std::acos((std::cos(90.833 * (1.0f / conversion))-std::sin(latitude * (1.0f / conversion))*std::sin(sunDeclination * (1.0f / conversion))) / (std::cos(latitude * (1.0f / conversion))*std::cos(sunDeclination * (1.0f / conversion))));
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