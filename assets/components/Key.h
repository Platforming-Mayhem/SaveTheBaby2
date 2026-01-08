#pragma once
#include "Lock.h"

namespace K
{
	KC_API class Key : public K::Component
	{
	private:
		std::vector<int> gIndices;
		std::map<K::Lock*, char> affectedLocks;
		std::map<K::Lock*, K::GameObject*> locks;
		K::Collider* col = nullptr;
		std::string properties;
	public:
		Key();

		~Key();

		void Init() override;

		void UpdateEditor() override;

		void Update() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}