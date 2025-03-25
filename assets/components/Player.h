#pragma once
#include <K_Engine.h>
#include "Animator.h"
#include "PhysicsEngine.h"

namespace K 
{
	KC_API class Player : public K::Component
	{
	private:
		bool hasGravity = false;
		bool flip = false;

		int animationState = 0;

		bool isJumping = false;
		bool isSlowingDown = false;

		float movementSpeed = 1.0f;

		float moveTime = 0.0f;

		float jumpTime = 0.0f;

		K::Vector3 originalScale;
		K::Vector3 direction;
		K::Animator* animator;
		K::Collider* col;
		K::Sprite* sprite;
		std::string properties;
	public:
		Player();

		~Player();

		float GetMovementSpeed();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void RenderBind() override;

		void Bind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}
