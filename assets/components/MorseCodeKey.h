#pragma once
#include <K_Engine.h>
#include "PhysicsEngine.h"

namespace K 
{
	class KC_API MorseCodeKey : public K::Component
	{
	private:
		bool pressed = false;
		float durationDeltaTime = 0.0f;
		float noInputDuration = 0.0f;
		std::string morseCodeLetter;
		std::string word;

		K::Vector3 bounds[2];
		K::Vector3 boundsModelMatrix[2];

		K::Audio* beepAudio;

		std::string properties;
	public:
		MorseCodeKey();

		~MorseCodeKey();

		char ConvertMorseCodeCharacterToASCII(std::string morseCodeChar);

		std::string TextToMorseCode(std::string text);

		void MorseCodeToAudio(std::string text);

		void VisualizeTriggerZone();

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}