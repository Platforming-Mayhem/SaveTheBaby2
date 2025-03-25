#include "Animator.h"

namespace K 
{
	REGISTER(Animator);

	K::Animator::Animator() 
	{

	}

	K::Animator::~Animator() 
	{
		if (!this->animations.empty())
		{
			for (int i = 0; i < this->animations.size(); i++) 
			{
				if(this->currentTexture != this->animations[i])
					delete this->animations[i];
			}
			this->animations.clear();
		}
	}

	void K::Animator::Init() 
	{
		
	}

	void K::Animator::Bind() 
	{
		if (!this->animations.empty())
		{
			for (auto i : this->animations)
			{
				i->Bind(0);
				i->Unbind();
			}
		}
	}

	void K::Animator::PlayAnimation(int index, K::Sprite* currentSprite, bool reScale) 
	{
		if (index < this->animations.size()) 
		{
			currentSprite->SetTexture(this->animations[index], reScale);
			this->currentTexture = this->animations[index];
		}
	}

	void Animator::AssignTexture(const char* value)
	{
		this->animations.push_back(new K::Texture(value));
	}

	void K::Animator::Update() 
	{
		
	}

	void K::Animator::UpdateEditor() 
	{
		if (ImGui::CollapsingHeader("Animator Settings")) 
		{
			if (ImGui::BeginListBox("Animations"))
			{
				for (int i = 0; i < this->animations.size(); i++)
				{
					if (ImGui::Selectable(this->animations[i]->GetFilePath().c_str()))
					{
						this->selectedTexture = i;
					}
				}
				ImGui::Dummy(ImGui::GetWindowSize());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
					{
						const char* file = (const char*)payload->Data;
						this->AssignTexture(file);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::EndListBox();
			}
			if (this->selectedTexture < this->animations.size()) 
			{
				ImGui::Text(this->animations[this->selectedTexture]->GetFilePath().c_str());
				ImGui::Checkbox("Is Looping", &this->animations[this->selectedTexture]->isLooping);
			}
			if (ImGui::Button("Delete Animation")) 
			{
				delete this->animations[this->selectedTexture];
				this->animations.erase(this->animations.begin() + this->selectedTexture);
				this->selectedTexture = this->animations.size() - 1;
			}
		}
	}

	void K::Animator::Unbind() 
	{

	}

	void K::Animator::SetPropertyValues(const char* value, int valueIndex) 
	{
		if (value[0] != '\0' && value != nullptr) 
		{
			std::string temp = value;
			if (temp == "true")
			{
				this->animations.back()->isLooping = true;
			}
			else if (temp == "false")
			{
				this->animations.back()->isLooping = false;
			}
			else 
			{
				this->AssignTexture(value);
			}
		}
	}

	const char* K::Animator::GetPropertyValues() 
	{
		this->properties = "";
		if (!this->animations.empty()) 
		{
			for (K::Texture* temp : this->animations)
			{
				this->properties += temp->GetFilePath();
				this->properties += ",";
				if (temp->isLooping)
				{
					this->properties += "true,";
				}
				else
				{
					this->properties += "false,";
				}
			}
			this->properties.pop_back();
		}
		return this->properties.c_str();
	}
}