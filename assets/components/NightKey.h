#pragma once
#include <K_Engine.h>

namespace K 
{
	class KC_API NightKey : public K::Component
	{
	private:
		std::string properties;
	public:
		NightKey();

		~NightKey();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}