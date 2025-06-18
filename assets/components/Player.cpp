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
		this->properties += "," + std::to_string(this->jumpHeight);
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
		case 1:
			this->jumpHeight = std::stof(value);
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
			ImGui::DragFloat("Jump Height", &this->jumpHeight);
		}
	}

	void Player::RenderBind() 
	{
		
	}

	float SineAccelerateByTime(float x, float duration, float snipAmount = 0.0f) 
	{
		if (x >= duration)
			return 1.0f;
		else
			return -((x + snipAmount) * (x - 2 * duration - snipAmount))/((duration + snipAmount)*(duration + snipAmount));
	}

	float SineDecelerateByTime(float x, float duration)
	{
		if (x >= duration)
			return 0.0f;
		else
			return -(((x + duration) * (x - duration)) / (duration * duration));
	}

	float JumpByTime(float x, float duration) 
	{
		return (- (8.0f / (duration * duration)) * x) + ((4 * duration) / (duration * duration));
	}

	void Player::HorizontalMovement() 
	{
		if (InputManager::IsKeyPressedDown(GLFW_KEY_RIGHT))
		{
			this->moveDirection += 1.0f;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_RIGHT))
		{
			this->moveDirection -= 1.0f;
			this->accelerateTime = 0.0f;
			this->decelerateTime += K::Time::deltaTime();
		}
		if (InputManager::IsKeyPressedDown(GLFW_KEY_LEFT))
		{
			this->moveDirection += -1.0f;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_LEFT))
		{
			this->moveDirection -= -1.0f;
			this->accelerateTime = 0.0f;
			this->decelerateTime += K::Time::deltaTime();
		}

		if (this->col->IsHittingWall()) 
		{
			if (this->col->collisionResolution.x > 0.0f)
			{
				this->col->angleUp = std::clamp(this->col->angleUp, 0.0f, 1.0f);
				this->accelerateTime = 0.0f;
				this->decelerateTime = this->decelerationSpeed;
			}
			else if (this->col->collisionResolution.x < 0.0f)
			{
				this->col->angleUp = std::clamp(this->col->angleUp, -1.0f, 0.0f);
				this->accelerateTime = 0.0f;
				this->decelerateTime = this->decelerationSpeed;
			}
		}

		if (this->moveDirection != 0.0f)
		{
			if (this->decelerateTime > 0.0f && this->decelerateTime <= this->decelerationSpeed)
			{
				this->decelerateTime += K::Time::deltaTime();
				this->accelerateTime = 0.0f;
			}
			else
			{
				this->accelerateTime += K::Time::deltaTime();
				this->decelerateTime = 0.0f;
			}
		}
		else
		{
			this->accelerateTime = 0.0f;
			this->decelerateTime += K::Time::deltaTime();
		}
		//Player Accelerates
		if (this->decelerateTime == 0.0f)
		{
			this->previousSpeed = SineAccelerateByTime(this->accelerateTime, 1.0f, 0.3f);
			this->parent->GetTransform()->position->x += this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * this->moveDirection * this->col->angleUp;
			this->parent->GetTransform()->position->z += this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * this->moveDirection * -this->col->angleRight;
		}
		//Player Decelerates
		else if (this->accelerateTime == 0.0f)
		{
			this->decelerationSpeed = this->previousSpeed * 0.3f;
			this->parent->GetTransform()->position->x += SineDecelerateByTime(this->decelerateTime, this->decelerationSpeed) * this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * this->previousDirection * this->col->angleUp;
			this->parent->GetTransform()->position->z += SineDecelerateByTime(this->decelerateTime, this->decelerationSpeed) * this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * this->previousDirection * -this->col->angleRight;
		}
	}

	void Player::VerticalMovement() 
	{
		if (InputManager::IsKeyPressedDown(GLFW_KEY_SPACE) && this->col->IsColliding()) 
		{
			this->isJumping = true;
			this->jumpTime = 0.0f;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_SPACE))
		{

		}
		if (this->col->IsColliding() && this->jumpTime >= 0.5f)
		{
			this->isJumping = false;
		}
		if (this->isJumping) 
		{
			this->jumpTime += K::Time::deltaTime();
			this->parent->GetTransform()->position->z += JumpByTime(this->jumpTime, 1.0f) * K::Time::deltaTime() * this->jumpHeight;
			this->col->ResetVelocity();
		}
	}

	void Player::Update() 
	{
		HorizontalMovement();
		VerticalMovement();
	}

	void Player::Unbind() 
	{
		if (this->moveDirection != 0.0f && this->decelerateTime == 0.0f) 
		{
			this->previousDirection = this->moveDirection;
		}
	}

	void Player::Bind() 
	{
		
	}
}