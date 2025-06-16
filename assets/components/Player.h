#pragma once
#include <K_Engine.h>
#include "Animator.h"
#include "PhysicsEngine.h"

namespace K 
{
	KC_API class Player : public K::Component
	{
	private:
		float movementSpeed = 1.0f;
		float accelerateTime = 0.0f;
		float moveDirection = 0.0f;
		float previousDirection = 0.0f;

		K::Collider* col = nullptr;
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
