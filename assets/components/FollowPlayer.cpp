#include "FollowPlayer.h"
#include "Player.h"

namespace K 
{
	REGISTER(FollowPlayer);

	FollowPlayer::FollowPlayer()
	{
	}

	FollowPlayer::~FollowPlayer()
	{
	}

	void FollowPlayer::Init() 
	{
		for (auto g : K::Editor::GetCurrentScene()->GetGameObjects()) 
		{
			if (g.second->GetComponentOfType(K::GetTypeName<K::Player>()) != nullptr) 
			{
				this->playerPosition = g.second->GetTransform()->position;
			}
		}
	}

	void FollowPlayer::Update() 
	{
		K::Vector3 target = *this->playerPosition - K::Vector3(0.0f, 10.0f, 0.0f) + this->offset;
		*this->parent->GetTransform()->position = target;
	}

	void FollowPlayer::UpdateEditor()
	{
		if (ImGui::CollapsingHeader(this->GetName()))
		{
			ImGui::DragFloat3("Offset", &this->offset.x);
		}
	}

	void FollowPlayer::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->offset.x = std::stof(temp);
				break;
			case 1:
				this->offset.y = std::stof(temp);
				break;
			case 2:
				this->offset.z = std::stof(temp);
				break;
			}
		}
	}

	const char* FollowPlayer::GetPropertyValues()
	{
		this->properties += std::to_string(this->offset.x) + ",";
		this->properties += std::to_string(this->offset.y) + ",";
		this->properties += std::to_string(this->offset.z);
		return this->properties.c_str();
	}
}