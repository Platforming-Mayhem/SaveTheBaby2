#pragma once
#include <K_Engine.h>

namespace K 
{
	class KC_API FollowPlayer : public K::Component
	{
	private:
		K::Vector3 offset = K::Vector3();
		K::Vector3* playerPosition;
		std::string properties;
	public:
		FollowPlayer();

		~FollowPlayer();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}