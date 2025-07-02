#pragma once
#include <K_Engine.h>
#include "PhysicsEngine.h"

namespace K 
{
	KC_API class TriggerNextScene : public K::Component
	{
	private:
		K::Vector3 bounds[2];
		K::Vector3 boundsModelMatrix[2];
		bool byIndex = false;
		int index = -1;

		std::string properties;
	public:
		TriggerNextScene();

		~TriggerNextScene();

		void VisualizeTriggerZone();

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}