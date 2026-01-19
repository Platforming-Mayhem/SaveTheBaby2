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
		clueAudio = new K::Audio(600.0f, 0.2f);
	}

	MorseCodeKey::~MorseCodeKey()
	{
		delete beepAudio;
		delete clueAudio;
	}

	void MorseCodeKey::Init() 
	{
		for (auto gIndex : this->gIndices)
		{
			K::GameObject* temp = K::Editor::GetCurrentScene()->GetGameObjects().at(gIndex);
			K::Lock* lock = (K::Lock*)temp->GetComponentOfType(GetTypeName<K::Lock>());
			this->locks.insert({ lock, temp });
			this->affectedLocks.insert({ lock, true });
		}
	}

	char MorseCodeKey::ConvertMorseCodeCharacterToASCII(std::string morseCodeChar)
	{
		if (morseCodeDecoder.find(morseCodeChar) != morseCodeDecoder.end())
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
			val += morseCodeEncoder.at(std::toupper(letter));
			val += ' ';
		}
		return val;
	}

	bool MorseCodeKey::PlayMorseDuration(float duration)
	{
		if (this->playDuration < duration - K::Time::deltaTime())
		{
			this->playDuration += K::Time::deltaTime();
			this->clueAudio->Play(false);
			return true;
		}
		else 
		{
			this->clueAudio->Stop();
			this->playDuration = 0.0f;
			return false;
		}
	}

	bool MorseCodeKey::PlayMorseWaitDuration(float duration)
	{
		if (this->waitDuration < duration - K::Time::deltaTime())
		{
			this->waitDuration += K::Time::deltaTime();
			this->clueAudio->Stop();
			return true;
		}
		else
		{
			this->waitDuration = 0.0f;
			return false;
		}
	}

	int iterator = 0;
	bool wait = false;

	void MorseCodeKey::TextToMorseAudio(std::string text)
	{
		std::string morseCode = TextToMorseCode(text);
		if (iterator < morseCode.size()) 
		{
			if (!wait) 
			{
				switch (morseCode[iterator])
				{
				case '.':
					if (!PlayMorseDuration(0.1f))
					{
						wait = true;
					}
					break;
				case '-':
					if (!PlayMorseDuration(0.3f))
					{
						wait = true;
					}
					break;
				case ' ':
					if (!PlayMorseWaitDuration(0.3f))
					{
						wait = true;
					}
					break;
				case '/':
					if (!PlayMorseWaitDuration(0.7f))
					{
						wait = true;
					}
					break;
				}
			}
			if (wait) 
			{
				if (!PlayMorseWaitDuration(0.1f))
				{
					iterator++;
					wait = false;
				}
			}
		}
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
			TextToMorseAudio("Life And Death");
			this->pressed = InputManager::IsKeyPressed(GLFW_KEY_UP);
		}
		else 
		{
			iterator = 0;
			wait = false;
			clueAudio->Stop();
			this->waitDuration = 0.0f;
			this->playDuration = 0.0f;
		}
		if (this->pressed) 
		{
			this->durationDeltaTime += K::Time::deltaTime();
			this->noInputDuration = 0.0f;
			beepAudio->Play(false);
		}
		else 
		{
			if (this->durationDeltaTime > 0.0f) 
			{
				if (this->durationDeltaTime <= 0.1f)
				{
					this->morseCodeLetter += ".";
					this->durationDeltaTime = 0.0f;
				}
				else
				{
					this->morseCodeLetter += "-";
					this->durationDeltaTime = 0.0f;
				}
			}
			if (this->noInputDuration >= 0.3f && this->morseCodeLetter != "")
			{
				this->word += this->ConvertMorseCodeCharacterToASCII(this->morseCodeLetter);
				this->morseCodeLetter = "";
				this->noInputDuration = 0.0f;
			}
			if (this->noInputDuration >= 0.7f && this->word != "")
			{
				std::transform(this->key.begin(), this->key.end(), this->key.begin(), ::toupper);
				std::cout << this->word << std::endl;
				if (this->word == this->key) 
				{
					for (auto lock : this->locks)
					{
						lock.first->SetKey(true);
					}
				}
				this->word = "";
				this->noInputDuration = 0.0f;
			}
			this->noInputDuration += K::Time::deltaTime();
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
			ImGui::InputText("Key", &this->key);
			this->VisualizeTriggerZone();
			if (ImGui::Button("Get all active locks"))
			{
				for (auto gameObject : K::Editor::GetCurrentScene()->GetGameObjects())
				{
					K::Lock* lock = (K::Lock*)gameObject.second->GetComponentOfType(GetTypeName<K::Lock>());
					if (lock != nullptr)
					{
						locks.insert({ lock, gameObject.second });
						affectedLocks.insert({ lock, false });
					}
				}
			}
			if (!locks.empty())
			{
				if (ImGui::BeginListBox("Locks"))
				{
					for (auto lock : locks)
					{
						ImGui::Checkbox(lock.second->GetName(), (bool*)(&affectedLocks.at(lock.first)));
					}
					ImGui::EndListBox();
				}
			}
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
			case 6:
				this->key = temp;
				break;
			}
			if (valueIndex > 6) 
			{
				std::string temp = value;
				this->gIndices.push_back(std::stoi(temp));
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
		this->properties += std::to_string(this->bounds[1].z) + ",";
		this->properties += this->key + ",";
		for (auto affectedLock : this->affectedLocks)
		{
			if ((bool)affectedLock.second)
			{
				this->properties += std::to_string(locks.at(affectedLock.first)->GetIndex()) + ",";
			}
		}
		this->properties.erase(this->properties.size() - 1);
		return this->properties.c_str();
	}
}