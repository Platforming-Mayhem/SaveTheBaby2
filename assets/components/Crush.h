#pragma once
#include <K_Engine.h>
#include "PhysicsEngine.h"

namespace K 
{
	KC_API class Crush : public K::Component
	{
	private:
		K::Vector3 origin;

		K::Vector3* offset;

		float crushTime = 0.0f;

		bool crush = false;

		std::string properties;
	public:
		Crush();

		~Crush();

		float EaseInPow(float value, int pow);

		float EaseInOutQuad(float value);

		void VisualDebug();

		void Init() override;

		void UpdateEditor() override;

		void Update() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}