#include "Player.h"

namespace K 
{
	float Player::moveDirection = 0.0f;
	int Player::lastInteractedIndex = -1;
	K::Vector3 Player::spawnPoint = K::Vector3();

	REGISTER(Player);

	Player::Player()
	{
		
	}

	Player::~Player() 
	{
		this->col = nullptr;
		this->mesh = nullptr;
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
		this->parent->layer = (int)K::Layer::LayerType::Player;
		
		this->col = (K::Collider*)this->parent->GetComponentOfType(GetTypeName<K::Collider>());
		this->mesh = (K::Mesh*)this->parent->GetComponentOfType(GetTypeName<K::Mesh>());

		this->col->ResetVelocity();
	}

	void Player::RenderInit() 
	{
		if (K::Player::lastInteractedIndex != -1) 
		{
			*this->parent->GetTransform()->position = K::Player::spawnPoint;
			K::Player::lastInteractedIndex = -1;
			K::Player::spawnPoint = K::Vector3();
		}
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
			moveDirection += 1.0f;
		}
		else if (InputManager::IsKeyPressedDown(GLFW_KEY_LEFT))
		{
			moveDirection += -1.0f;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_RIGHT))
		{
			moveDirection -= 1.0f;
			this->accelerateTime = 0.0f;
			this->decelerateTime += K::Time::deltaTime();
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_LEFT))
		{
			moveDirection -= -1.0f;
			this->accelerateTime = 0.0f;
			this->decelerateTime += K::Time::deltaTime();
		}

		if (moveDirection != 0.0f)
		{
			if (this->decelerateTime > 0.0f && this->decelerateTime < this->decelerationSpeed)
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

		if (this->col->IsHittingWall() && std::fabsf(this->col->wallUp) < 0.2f)
		{
			if (this->col->wallRight > 0.8f && moveDirection > 0.0f)
			{
				this->accelerateTime = -0.5f;
				this->decelerateTime = 0.0f;
				//std::cout << "Stop Move Right: " << this->moveDirection << ":" << -this->col->angleRight << std::endl;
			}
			else if (this->col->wallRight > 0.8f && moveDirection < 0.0f)
			{
				this->accelerateTime = 0.0f;
				this->decelerateTime = 0.0f;
				//std::cout << "Move Left: " << this->moveDirection << ":" << -this->col->angleRight << std::endl;
			}
			if (this->col->wallRight < -0.8f && moveDirection < 0.0f)
			{
				this->accelerateTime = -0.5f;
				this->decelerateTime = 0.0f;
				//std::cout << "Stop Move Left: " << this->moveDirection << ":" << -this->col->angleRight << std::endl;
			}
			else if (this->col->wallRight < -0.8f && moveDirection > 0.0f)
			{
				this->accelerateTime = 0.0f;
				this->decelerateTime = 0.0f;
				//std::cout << "Move Right: " << this->moveDirection << ":" << -this->col->angleRight << std::endl;
			}
		}
		else 
		{
			if (this->accelerateTime < 0.0f) 
			{
				this->accelerateTime = 0.0f;
			}
		}

		if (this->canMoveHorizontally) 
		{
			//Player Accelerates
			if (this->decelerateTime == 0.0f)
			{
				float newTime = this->accelerateTime + K::Time::deltaTime();
				this->previousSpeed = (SineAccelerateByTime(this->accelerateTime, 1.4f, 0.5f) + SineAccelerateByTime(newTime, 1.4f, 0.5f)) / 2.0f;
				this->mesh->SetColourTint(this->previousSpeed, 0.0f, 0.0f);
				this->parent->GetTransform()->position->x += this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * moveDirection * this->col->angleUp;
				this->parent->GetTransform()->position->z += this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * moveDirection * -this->col->angleRight;
			}
			//Player Decelerates
			else if (this->accelerateTime == 0.0f)
			{
				this->decelerationSpeed = this->previousSpeed * 0.14f;
				float newTime = this->decelerateTime + K::Time::deltaTime();
				float decelerationAmount = (SineDecelerateByTime(this->decelerateTime, this->decelerationSpeed) + SineDecelerateByTime(newTime, this->decelerationSpeed)) / 2.0f;
				this->mesh->SetColourTint(decelerationAmount, 0.0f, 0.0f);
				this->parent->GetTransform()->position->x += decelerationAmount * this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * this->previousDirection * this->col->angleUp;
				this->parent->GetTransform()->position->z += decelerationAmount * this->previousSpeed * K::Time::deltaTime() * this->movementSpeed * this->previousDirection * -this->col->angleRight;
			}
		}
	}

	void Player::LedgeGrab() 
	{
		K::ContactPoint contactPoint = K::Physics::GetClosestPoint(this->col->GetPosition() + K::Vector3(0.0f, 0.0f, (this->col->GetHeight() / 2.0f) + this->col->GetRadius()), { K::Layer::LayerType::Player });
		if (contactPoint.normal.magnitude() > 1.0f && this->col->IsHittingWall() && !this->col->IsColliding() && contactPoint.normal.z > 0.0f && !this->isClimbingUp && contactPoint.position.z > this->col->GetPosition().z + (this->col->GetHeight() / 2.0f))
		{
			this->isGrabbing = true;
			this->cornerX = contactPoint.position.x - this->col->GetOffset().x + this->col->GetRadius() * contactPoint.normal.x;
			this->cornerZ = contactPoint.position.z - (this->col->GetHeight() / 2.0f) - this->col->GetRadius() - this->col->GetOffset().z;
		}
		if (this->isGrabbing) 
		{
			if (InputManager::IsKeyPressed(GLFW_KEY_UP))
			{
				this->climbTime = 0.0f;
				this->jumpTime = 0.5f;
				this->isClimbingUp = true;
				this->canMoveHorizontally = true;
				this->isGrabbing = false;
			}
			else if (InputManager::IsKeyPressed(GLFW_KEY_DOWN)) 
			{
				this->jumpTime = 0.5f;
				this->parent->GetTransform()->position->z = this->cornerZ - this->col->GetRadius();
				this->canMoveHorizontally = true;
				this->isGrabbing = false;
			}
			else 
			{
				this->parent->GetTransform()->position->x = this->cornerX;
				this->parent->GetTransform()->position->z = this->cornerZ;
				this->jumps = 0;
				this->canMoveHorizontally = false;
				this->col->ResetVelocity();
			}
		}
		if (this->isClimbingUp) 
		{
			if (this->climbTime < 1.0f) 
			{
				this->climbTime += K::Time::deltaTime() * 2.0f;
				this->col->ResetVelocity();
				this->parent->GetTransform()->position->x = this->cornerX;
				this->parent->GetTransform()->position->z = this->cornerZ + ((this->climbTime * this->climbTime) * (this->col->GetHeight() + (this->col->GetRadius() * 2.0f)));
				this->canMoveHorizontally = false;
			}
			else 
			{
				this->parent->GetTransform()->position->x = this->cornerX - (this->col->GetRadius() * 2.0f * contactPoint.normal.x);
				this->parent->GetTransform()->position->z = this->cornerZ + this->col->GetHeight() + (this->col->GetRadius() * 2.0f);
				this->canMoveHorizontally = true;
				this->isClimbingUp = false;
			}
		}
	}

	void Player::VerticalMovement() 
	{
		if (InputManager::IsKeyPressedDown(GLFW_KEY_SPACE)) 
		{
			this->jumpButtonPressed = true;
			this->jumpBufferTime = 0.0f;
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_SPACE))
		{

		}
		if (!this->col->IsColliding() && this->jumps == this->maxJumps)
		{
			if (this->fallBufferTime >= 0.2f) 
			{
				this->jumps = 0;
			}
			else 
			{
				this->fallBufferTime += K::Time::deltaTime();
			}
		}
		if (this->jumpButtonPressed)
		{
			if (this->jumpBufferTime >= 0.2f && !this->isGrabbing)
			{
				this->jumpButtonPressed = false;
			}
			else
			{
				this->jumpBufferTime += K::Time::deltaTime();
				if (this->jumps > 0)
				{
					this->isJumping = true;
					this->jumpTime = 0.0f;
					this->jumps--;
					this->jumpButtonPressed = false;
					this->jumpBufferTime = 0.2f;
				}
			}
		}
		if (this->col->IsColliding() && this->jumpTime >= 0.5f)
		{
			this->isJumping = false;
			this->jumps = this->maxJumps;
			this->fallBufferTime = 0.0f;
		}
		if (this->isJumping) 
		{
			this->jumpTime += K::Time::deltaTime() * 2.0f;
			this->parent->GetTransform()->position->z += JumpByTime(this->jumpTime, 1.0f) * K::Time::deltaTime() * this->jumpHeight * 2.0f;
			this->col->angleRight = 0.0f;
			this->col->ResetVelocity();
		}
		LedgeGrab();
	}

	void Player::Update() 
	{
		HorizontalMovement();
		VerticalMovement();
		if (moveDirection != 0.0f && this->decelerateTime == 0.0f) 
		{
			this->previousDirection = moveDirection;
		}
	}

	void Player::Unbind() 
	{
		
	}

	void Player::Bind() 
	{
		
	}
}