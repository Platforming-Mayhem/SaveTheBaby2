#pragma once
#include <K_Engine.h>

namespace K 
{
	KC_API class Move : public K::Component
	{
	private:
		float moveSpeed = 1.0f;
		float time = 0.0f;
		K::Vector3 destination;
		std::string properties;
	public:
		Move();

		~Move();

		float EaseInQuart(float x);

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void Bind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}