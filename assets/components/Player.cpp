#include "Player.h"

namespace K 
{
	REGISTER(Player);

	Player::Player()
	{
		
	}

	Player::~Player() 
	{
		this->animator = nullptr;
		this->col = nullptr;
		this->sprite = nullptr;
		//std::cout << "Player Destructor" << std::endl;
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

	float easeInPow(float time, float power, float duration = 1.0f)
	{
		return powf((time / duration), power);
	}

	float easeOutPow(float time, float power, float duration = 1.0f)
	{
		return 1 - powf(1 - (time * 1 / duration), power);
	}

	float decelerateEaseOutPow(float time, float power, float duration = 1.0f)
	{
		return 1 - powf(1 - (time * 1 / duration) - 1, power);
	}

	void Player::Init() 
	{
		if (this->parent->GetComponentOfType(typeid(K::Animator).name()) != nullptr)
		{
			this->animator = (K::Animator*)this->parent->GetComponentOfType(typeid(K::Animator).name());
		}
		if (this->parent->GetComponentOfType(typeid(K::Collider).name()) != nullptr)
		{
			this->col = (K::Collider*)this->parent->GetComponentOfType(typeid(K::Collider).name());
		}
		if (this->parent->GetComponentOfType(typeid(K::Sprite).name()) != nullptr)
		{
			this->sprite = (K::Sprite*)this->parent->GetComponentOfType(typeid(K::Sprite).name());
		}

		this->direction = K::Vector3();
		this->originalScale = *this->parent->GetTransform()->scale;

		this->parent->layer = (int)K::Layer::LayerType::Player;

		this->col->ResetVelocity();
	}

	void Player::UpdateEditor() 
	{
		if (ImGui::CollapsingHeader("Player Settings")) 
		{
			ImGui::DragFloat("Movement Speed", &this->movementSpeed);
			ImGui::Checkbox("is Slowing Down", &this->isSlowingDown);
		}
	}

	void Player::RenderBind() 
	{
		//Animation States

		switch (this->animationState)
		{
		case 0:
			//Idle
			this->animator->PlayAnimation(0, this->sprite, false);
			this->parent->GetTransform()->rotation->y = 0.0f;
			break;
		case 1:
			//Moving
			this->animator->PlayAnimation(1, this->sprite, false);
			this->parent->GetTransform()->rotation->y = 0.0f;
			break;
		case 2:
			//Jumping
			this->animator->PlayAnimation(2, this->sprite, false);
			this->parent->GetTransform()->rotation->y = 0.0f;
			break;
		case 3:
			//Dashing
			if (!this->sprite->IsPlaying())
			{
				if (this->isJumping)
				{
					this->animationState = 2;
				}
				else if (this->direction.x != 0.0f)
				{
					this->animationState = 1;
				}
				else
				{
					this->animationState = 0;
				}
			}
			else
			{
				this->animator->PlayAnimation(3, this->sprite, false);
			}
			break;
		case 4:
			if (!this->sprite->IsPlaying())
			{
				if (this->isJumping)
				{
					this->animationState = 2;
				}
				else
				{
					this->animationState = 0;
				}
			}
			else
			{
				this->animator->PlayAnimation(4, this->sprite, false);
				this->parent->GetTransform()->rotation->y = 0.0f;
			}
			break;
		}
	}

	void Player::Update() 
	{
		this->direction.z = 0.0f;
		if (InputManager::IsKeyPressed(GLFW_KEY_RIGHT))
		{
			this->direction.x = this->movementSpeed * K::Time::deltaTime() * easeOutPow(this->moveTime, 2.0f);
			this->isSlowingDown = true;
			if (this->flip)
			{
				this->moveTime = 0.0f;
			}
			this->flip = false;
			if (this->moveTime < 1.0f) 
			{
				this->moveTime += K::Time::deltaTime();
			}
			else 
			{
				this->moveTime = 1.0f;
			}
			if (this->animationState == 0)
			{
				this->animationState = 1;
			}
		}
		else if (InputManager::IsKeyPressed(GLFW_KEY_LEFT))
		{
			this->direction.x = -this->movementSpeed * K::Time::deltaTime() * easeOutPow(this->moveTime, 2.0f);
			this->isSlowingDown = true;
			if (!this->flip)
			{
				this->moveTime = 0.0f;
			}
			this->flip = true;
			if (this->moveTime < 1.0f)
			{
				this->moveTime += K::Time::deltaTime();
			}
			else
			{
				this->moveTime = 1.0f;
			}
			if (this->animationState == 0)
			{
				this->animationState = 1;
			}
		}
		else
		{
			if (this->isSlowingDown)
			{
				if (this->moveTime > 0.0f)
				{
					this->animationState = 4;
					this->moveTime -= K::Time::deltaTime() * 2.0f;
					if (this->direction.x > 0.0f)
					{
						this->direction.x = this->movementSpeed * K::Time::deltaTime() * decelerateEaseOutPow(1.0f - this->moveTime, 6.0f);
					}
					else if (this->direction.x < 0.0f)
					{
						this->direction.x = -this->movementSpeed * K::Time::deltaTime() * decelerateEaseOutPow(1.0f - this->moveTime, 6.0f);
					}
				}
				else
				{
					this->moveTime = 0.0f;
					this->direction.x = 0.0f;
					this->isSlowingDown = false;
				}
			}
		}

		if (InputManager::IsKeyPressedDown(GLFW_KEY_Z)) 
		{
			K::Vector3 dashDirection = K::Vector3();
			if (InputManager::IsKeyPressed(GLFW_KEY_RIGHT))
			{
				dashDirection.x = 1.0f;
			}
			else if (InputManager::IsKeyPressed(GLFW_KEY_LEFT))
			{
				dashDirection.x = -1.0f;
			}
			if (InputManager::IsKeyPressed(GLFW_KEY_UP))
			{
				dashDirection.z += 1.0f;
			}
			else if (InputManager::IsKeyPressed(GLFW_KEY_DOWN))
			{
				dashDirection.z += -1.0f;
			}
			if (dashDirection.magnitude() > 0.0f)
			{
				if (K::Physics::Raycast(this->col->GetPosition(), K::Vector3(dashDirection.x, dashDirection.z, 0.0f) * 5.0f, { K::Layer(K::Layer::LayerType::Enemy), K::Layer(K::Layer::LayerType::Player) }))
				{
					this->col->ResetVelocity();
					K::Vector3 closestFuturePoint = K::Physics::GetClosestPoint(*this->parent->GetTransform()->position + (dashDirection.normalise() * 5.0f));
					*this->parent->GetTransform()->position = closestFuturePoint;
					*this->parent->GetTransform()->position -= K::Vector3(dashDirection.x * this->col->GetRadius(), 0.0f, dashDirection.z * (this->col->GetRadius() + (this->col->GetHeight() * 0.5f)));
				}
				else
				{
					this->col->ResetVelocity();
					*this->parent->GetTransform()->position += dashDirection * 5.0f;
				}
				float angle = std::atan2f(-dashDirection.z, dashDirection.x) * 57.2958f;
				this->parent->GetTransform()->rotation->y = angle + 90.0f;
				this->animationState = 3;
				this->sprite->ResetFrame();
			}
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_Z))
		{

		}

		if (InputManager::IsKeyPressedDown(GLFW_KEY_UP) && this->jumpTime == 0.0f)
		{
			this->isJumping = true;
			this->animationState = 2;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_UP) && this->jumpTime < 1.0f)
		{
			this->jumpTime = 1.0f;
		}
		else if (this->jumpTime >= 1.0f)
		{
			this->isJumping = false;
			if (K::Physics::IsColliding(this->parent))
			{
				this->jumpTime = 0.0f;
				this->animationState = 0;
			}
		}
		if (this->isJumping)
		{
			this->col->ResetVelocity();
			this->direction.z = (-(this->jumpTime - 0.5f) + 0.5f) * 0.5f * K::Time::deltaTime() * 60.0f;
			this->jumpTime += K::Time::deltaTime() * 2.5f;
		}
		else
		{
			if (!K::Physics::Raycast(this->col->GetPosition() + K::Vector3(this->col->GetRadius(), 0.0f, -this->col->GetHeight() / 2.0f), K::Vector3(0.0f, -(this->col->GetRadius() + 1.0f), 0.0f), { K::Layer(K::Layer::LayerType::Enemy), K::Layer(K::Layer::LayerType::Player) }) && !K::Physics::Raycast(this->col->GetPosition() - K::Vector3(this->col->GetRadius(), 0.0f, this->col->GetHeight() / 2.0f), K::Vector3(0.0f, -(this->col->GetRadius() + 1.0f), 0.0f), { K::Layer(K::Layer::LayerType::Enemy), K::Layer(K::Layer::LayerType::Player) }))
			{
				this->jumpTime = 1.0f;
			}
		}

		*(this->parent->GetTransform()->position) += this->direction;
	}

	void Player::Unbind() 
	{
		//Flipping Sprite
		if (this->flip)
		{
			this->parent->GetTransform()->scale->x = -this->originalScale.x;
		}
		else
		{
			this->parent->GetTransform()->scale->x = this->originalScale.x;
		}
	}

	void Player::Bind() 
	{
		
	}
}