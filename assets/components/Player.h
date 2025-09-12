#pragma once
#include <K_Engine.h>
#include "Animator.h"
#include "Mesh.h"
#include "PhysicsEngine.h"

namespace K 
{
	KC_API class Player : public K::Component
	{
	private:
		float movementSpeed = 1.0f;
		float jumpHeight = 12.0f;

		float currentSpeed = 0.0f;
		float decelerationSpeed = 0.3f;

		float accelerateTime = 0.0f;
		float decelerateTime = 0.0f;
		float jumpTime = 0.5f;
		float jumpBufferTime = 0.0f;
		float fallBufferTime = 0.0f;
		float grabTime = 0.0f;
		float climbTime = 0.0f;

		static float moveDirection;
		float previousDirection = 0.0f;
		float previousSpeed = 0.0f;

		float cornerX = 0.0f;
		float cornerZ = 0.0f;

		int jumps = 1;
		int maxJumps = 1;

		bool isClimbingUp = false;
		bool isGrabbing = false;
		bool isJumping = false;
		bool canMoveHorizontally = true;
		bool jumpButtonPressed = false;

		K::Mesh* mesh = nullptr;
		K::Collider* col = nullptr;
		std::string properties;
	public:

		static int lastInteractedIndex;

		static K::Vector3 spawnPoint;

		Player();

		~Player();

		float GetMovementSpeed();

		void LedgeGrab();

		void HorizontalMovement();

		void VerticalMovement();

		void Init() override;

		void RenderInit() override;

		void Update() override;

		void UpdateEditor() override;

		void RenderBind() override;

		void Bind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}
