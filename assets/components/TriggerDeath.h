#pragma once
#include <K_Engine.h>
#include "PhysicsEngine.h"

namespace K
{
	KC_API class TriggerDeath : public K::Component
	{
	private:
		bool invert = false;
		K::Vector3 bounds[2];
		K::Vector3 boundsModelMatrix[2];

		std::string properties;
	public:

		TriggerDeath();

		~TriggerDeath();

		void VisualizeTriggerZone();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void Bind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}