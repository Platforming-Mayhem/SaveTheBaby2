#pragma once
#include <K_Engine.h>
#include "Collider.h"

namespace K
{
	KC_API class Lock : public K::Component
	{
	private:
		bool foundKey = false;
		K::Collider* col = nullptr;
		std::string properties;
	public:
		Lock();

		~Lock();

		void SetKey(bool found);

		void Init() override;

		void UpdateEditor() override;

		void Update() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}