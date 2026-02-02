#include "NightKey.h"

namespace K 
{
	REGISTER(NightKey);

	NightKey::NightKey()
	{
		GEOID myGEO = GetUserGeoID(GEOCLASS_NATION);
		int sizeOfBuffer = GetGeoInfo(myGEO, GEO_ISO2, NULL, 0, 0);
		char* buffer = new char[sizeOfBuffer];
		int result = GetGeoInfo(myGEO, GEO_ISO2, buffer, sizeOfBuffer, 0);
		std::cout << buffer << std::endl;
	}

	NightKey::~NightKey()
	{
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