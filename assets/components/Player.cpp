#include "Player.h"

namespace K 
{
	REGISTER(Player);

	Player::Player()
	{
		
	}

	Player::~Player() 
	{
		this->col = nullptr;
	}

	const char* Player::GetPropertyValues()
	{
		this->properties = std::to_string(this->movementSpeed);
		return this->properties.c_str();
	}

	float Player::GetMovementSpeed() 
	{
		return this->movementSpeed;
	}

	void Player::SetPropertyValues(const char* value, int valueIndex)
	{
		switch (valueIndex)
		{
		case 0:
			this->movementSpeed = std::stof(value);
			break;
		}
	}

	void Player::Init() 
	{
		if (this->parent->GetComponentOfType(typeid(K::Collider).name()) != nullptr)
		{
			this->col = (K::Collider*)this->parent->GetComponentOfType(typeid(K::Collider).name());
		}

		this->parent->layer = (int)K::Layer::LayerType::Player;

		this->col->ResetVelocity();
	}

	void Player::UpdateEditor() 
	{
		if (ImGui::CollapsingHeader("Player Settings")) 
		{
			ImGui::DragFloat("Movement Speed", &this->movementSpeed);
		}
	}

	void Player::RenderBind() 
	{
		
	}

	float SineByTime(float x, float duration, float snipAmount = 0.0f) 
	{
		if (x >= duration)
			return 1.0f;
		else
			return -((x + snipAmount) * (x - 2 * duration - snipAmount))/((duration + snipAmount)*(duration + snipAmount));
	}

	void Player::Update() 
	{
		if (InputManager::IsKeyPressedDown(GLFW_KEY_RIGHT))
		{
			this->moveDirection += 1.0f;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_RIGHT)) 
		{
			this->moveDirection -= 1.0f;
			this->accelerateTime = 0.0f;
		}
		if (InputManager::IsKeyPressedDown(GLFW_KEY_LEFT))
		{
			this->moveDirection += -1.0f;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_LEFT))
		{
			this->moveDirection -= -1.0f;
			this->accelerateTime = 0.0f;
		}
		if (this->moveDirection != 0.0f) 
		{
			this->accelerateTime += K::Time::deltaTime();
		}
		else 
		{
			this->accelerateTime = 0.0f;
		}
		//Player Accelerate
		this->parent->GetTransform()->position->x += SineByTime(this->accelerateTime, 1.6f, 0.3f) * K::Time::deltaTime() * this->movementSpeed * this->moveDirection;
	}

	void Player::Unbind() 
	{
		if (this->moveDirection != 0.0f) 
		{
			this->previousDirection = this->moveDirection;
		}
	}

	void Player::Bind() 
	{
		
	}
}