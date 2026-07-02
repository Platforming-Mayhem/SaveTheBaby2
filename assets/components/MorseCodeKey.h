#pragma once
#include <K_Engine.h>
#include "PhysicsEngine.h"
#include "Lock.h"

namespace K 
{
	class KC_API MorseCodeKey : public K::Component
	{
	private:
		bool pressed = false;
		float durationDeltaTime = 0.0f;
		float noInputDuration = 0.0f;
		float playDuration = 0.0f;
		float waitDuration = 0.0f;
		std::string morseCodeLetter;
		std::string word;
		std::string key;

		K::Vector3 bounds[2];
		K::Vector3 boundsModelMatrix[2];

		K::Audio* beepAudio;
		K::Audio* clueAudio;

		std::vector<int> gIndices;
		std::map<K::Lock*, char> affectedLocks;
		std::map<K::Lock*, K::GameObject*> locks;

		std::string properties;
	public:
		MorseCodeKey();

		~MorseCodeKey();

		char ConvertMorseCodeCharacterToASCII(std::string morseCodeChar);

		std::string TextToMorseCode(std::string text);

		bool PlayMorseDuration(float duration);

		bool PlayMorseWaitDuration(float duration);

		void TextToMorseAudio(std::string text);

		void VisualizeTriggerZone();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}