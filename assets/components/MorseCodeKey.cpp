#include "MorseCodeKey.h"

namespace K 
{
	REGISTER(MorseCodeKey);

	std::unordered_map<std::string, char> morseCodeDecoder = {
				{".-",'A'},
				{"-...",'B'},
				{"-.-.",'C'},
				{"-..",'D'},
				{".",'E'},
				{"..-.",'F'},
				{"--.",'G'},
				{"....",'H'},
				{"..",'I'},
				{".---",'J'},
				{"-.-",'K'},
				{".-..",'L'},
				{"--",'M'},
				{"-.",'N'},
				{"---",'O'},
				{".--.",'P'},
				{"--.-",'Q'},
				{".-.",'R'},
				{"...",'S'},
				{"-",'T'},
				{"..-",'U'},
				{"...-",'V'},
				{".--",'W'},
				{"-..-",'X'},
				{"-.--",'Y'},
				{"--..",'Z'}
	};

	std::unordered_map<char, std::string> morseCodeEncoder = {
				{'A',".-"},
				{'B',"-..."},
				{'C',"-.-."},
				{'D',"-.."},
				{'E',"."},
				{'F',"..-."},
				{'G',"--."},
				{'H',"...."},
				{'I',".."},
				{'J',".---"},
				{'K',"-.-"},
				{'L',".-.."},
				{'M',"--"},
				{'N',"-."},
				{'O',"---"},
				{'P',".--."},
				{'Q',"--.-"},
				{'R',".-."},
				{'S',"..."},
				{'T',"-"},
				{'U',"..-"},
				{'V',"...-"},
				{'W',".--"},
				{'X',"-..-"},
				{'Y',"-.--"},
				{'Z',"--.."},
				{' ',"/"}
	};

	MorseCodeKey::MorseCodeKey()
	{
		beepAudio = new K::Audio(600.0f, 0.2f);
	}

	MorseCodeKey::~MorseCodeKey()
	{
		delete beepAudio;
	}

	char MorseCodeKey::ConvertMorseCodeCharacterToASCII(std::string morseCodeChar)
	{
		if (morseCodeChar.size() <= 4) 
		{
			return morseCodeDecoder.at(morseCodeChar);
		}
		else 
		{
			return '\0';
		}
	}

	std::string MorseCodeKey::TextToMorseCode(std::string text)
	{
		std::string val;
		for (auto letter : text) 
		{
			val += morseCodeEncoder.at(letter);
			val += ' ';
		}
		return val;
	}

	int iterator = 0;

	void MorseCodeKey::MorseCodeToAudio(std::string text)
	{
		std::string morseCode = TextToMorseCode(text);
	}

	void MorseCodeKey::VisualizeTriggerZone()
	{
		K::Material mat = K::Material("shaders/2D.shader");
		glUseProgram(mat.GetShader()->shader);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUniform1i(mat.GetShader()->GetUniform("canChromaKey"), false);
		glUniform1i(mat.GetShader()->GetUniform("hasTexture"), false);
		glUniform3f(mat.GetShader()->GetUniform("colorTint"), 0.0f, 1.0f, 0.0f);
		K::Transform temp = K::Transform(new K::Vector3(), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
		temp.PassModelMatrix();
		glUniformMatrix4fv(mat.GetShader()->GetUniform("modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);

		glBegin(GL_LINE_LOOP);
		glVertex3f(this->boundsModelMatrix[0].x, this->boundsModelMatrix[0].y, this->boundsModelMatrix[0].z);
		glVertex3f(this->boundsModelMatrix[0].x, this->boundsModelMatrix[0].y, this->boundsModelMatrix[1].z);
		glVertex3f(this->boundsModelMatrix[1].x, this->boundsModelMatrix[1].y, this->boundsModelMatrix[1].z);
		glVertex3f(this->boundsModelMatrix[1].x, this->boundsModelMatrix[1].y, this->boundsModelMatrix[0].z);
		glEnd();

		glUniform3f(mat.GetShader()->GetUniform("colorTint"), 1.0f, 1.0f, 1.0f);
	}

	void MorseCodeKey::Update() 
	{
		K::MultiplyMatrixVector(this->bounds[0], this->boundsModelMatrix[0], this->parent->GetTransform()->modelMatrix);
		K::MultiplyMatrixVector(this->bounds[1], this->boundsModelMatrix[1], this->parent->GetTransform()->modelMatrix);
		K::Collider* temp = nullptr;
		if (K::Physics::Hitbox(this->boundsModelMatrix[0], this->boundsModelMatrix[1], { K::Layer::LayerType::Enemy, K::Layer::LayerType::Ground }, &temp))
		{
			this->pressed = InputManager::IsKeyPressed(GLFW_KEY_UP);
		}
		if (this->pressed) 
		{
			this->durationDeltaTime += K::Time::deltaTime();
			this->noInputDuration = 0.0f;
			beepAudio->Play(false);
		}
		else 
		{
			if (this->durationDeltaTime <= 0.1f && this->durationDeltaTime > 0.0f)
			{
				this->morseCodeLetter += ".";
			}
			else if (this->durationDeltaTime >= 0.3f && this->durationDeltaTime > 0.0f)
			{
				this->morseCodeLetter += "-";
			}
			if (this->noInputDuration >= 0.3f && this->morseCodeLetter != "")
			{
				this->word += this->ConvertMorseCodeCharacterToASCII(this->morseCodeLetter);
				this->morseCodeLetter = "";
				this->noInputDuration = 0.0f;
			}
			if (this->noInputDuration >= 0.7f && this->word != "")
			{
				std::cout << this->word << std::endl;
				this->word = "";
				this->noInputDuration = 0.0f;
			}
			this->noInputDuration += K::Time::deltaTime();
			this->durationDeltaTime = 0.0f;
			beepAudio->Stop();
		}
	}

	void MorseCodeKey::UpdateEditor()
	{
		if (ImGui::CollapsingHeader(this->GetName()))
		{
			ImGui::Checkbox("Pressed", &this->pressed);
			ImGui::DragFloat3("Bottom Left", &this->bounds[0].x);
			ImGui::DragFloat3("Top Right", &this->bounds[1].x);
			this->VisualizeTriggerZone();
		}
	}

	void MorseCodeKey::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->bounds[0].x = std::stof(temp);
				break;
			case 1:
				this->bounds[0].y = std::stof(temp);
				break;
			case 2:
				this->bounds[0].z = std::stof(temp);
				break;
			case 3:
				this->bounds[1].x = std::stof(temp);
				break;
			case 4:
				this->bounds[1].y = std::stof(temp);
				break;
			case 5:
				this->bounds[1].z = std::stof(temp);
				break;
			}
		}
	}

	const char* MorseCodeKey::GetPropertyValues()
	{
		this->properties = std::to_string(this->bounds[0].x) + ",";
		this->properties += std::to_string(this->bounds[0].y) + ",";
		this->properties += std::to_string(this->bounds[0].z) + ",";
		this->properties += std::to_string(this->bounds[1].x) + ",";
		this->properties += std::to_string(this->bounds[1].y) + ",";
		this->properties += std::to_string(this->bounds[1].z);
		return this->properties.c_str();
	}
}