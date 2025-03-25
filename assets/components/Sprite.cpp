#include "Sprite.h"

namespace K 
{
	REGISTER(Sprite);

	Sprite::Sprite()
	{
		
	}

	Sprite::~Sprite() 
	{
		//std::cout << "Begin Sprite Destruction..." << this->texture->GetFilePath() << std::endl;
		delete this->texture;
		delete this->normalTexture;
		if(this->renderTexture != this->texture)
			delete this->renderTexture;
		//std::cout << "End Sprite Destruction..." << std::endl;
	}

	const char* Sprite::GetPropertyValues()
	{
		if (this->texture->GetFilePath() == "101") 
		{
			this->properties = "";
		}
		else 
		{
			this->properties = this->texture->GetFilePath();
		}

		if (this->canChromaKey) 
		{
			this->properties += ",true";
			this->properties += "," + std::to_string(this->chromaKeyColour[0]);
			this->properties += "," + std::to_string(this->chromaKeyColour[1]);
			this->properties += "," + std::to_string(this->chromaKeyColour[2]);
		}
		else 
		{
			this->properties += ",false";
		}
		if (this->normalTexture->GetFilePath() == "101")
		{
			this->properties += ",";
		}
		else
		{
			this->properties += "," + this->normalTexture->GetFilePath();
		}
		if (this->isLooping) 
		{
			this->properties += ",true";
		}
		else 
		{
			this->properties += ",false";
		}
		return this->properties.c_str();
	}

	void Sprite::ResetFrame() 
	{
		this->frame = 0;
		this->internalClock = 0.0f;
	}

	void Sprite::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
			{
				this->SetColorTexture(temp.c_str());
			}
			break;
			case 1:
			{
				if (temp == "true")
				{
					this->canChromaKey = true;
				}
				else if (temp == "false")
				{
					this->canChromaKey = false;
				}
			}
			break;
			case 2:
			{
				if (this->canChromaKey) 
				{
					this->chromaKeyColour[0] = std::stof(temp);
				}
				else 
				{
					this->SetNormalTexture(temp.c_str());
					this->hasNormal = true;
				}
			}
			break;
			case 3:
			{
				if (this->canChromaKey)
				{
					this->chromaKeyColour[1] = std::stof(temp);
				}
				else
				{

				}
			}
			break;
			case 4:
			{
				if (this->canChromaKey)
				{
					this->chromaKeyColour[2] = std::stof(temp);
				}
				else
				{

				}
			}
			break;
			case 5:
			{
				if (this->canChromaKey)
				{
					this->SetNormalTexture(temp.c_str());
					this->hasNormal = true;
				}
				else 
				{

				}
			}
			break;
			case 6:
			{
				if (temp == "true")
				{
					this->isLooping = true;
				}
				else if (temp == "false")
				{
					this->isLooping = false;
				}
				if (this->texture)
					this->texture->isLooping = this->isLooping;
			}
			break;
			}
		}
	}

	K::Texture* Sprite::GetTexture() 
	{
		return this->texture;
	}

	void Sprite::SetColorTexture(const char* value)
	{
		if (this->texture != nullptr)
			delete this->texture;
		this->texture = new K::Texture(value);
	}

	void Sprite::SetNormalTexture(const char* value)
	{
		if (this->normalTexture != nullptr)
			delete this->normalTexture;
		this->normalTexture = new K::Texture(value);
	}

	void Sprite::SetColorTexture(unsigned int resource)
	{
		if (this->texture != nullptr)
			delete this->texture;
		this->texture = new K::Texture(resource);
	}

	void Sprite::SetNormalTexture(unsigned int resource)
	{
		if (this->normalTexture != nullptr)
			delete this->normalTexture;
		this->normalTexture = new K::Texture(resource);
	}

	void Sprite::SetTexture(K::Texture* newTexture, bool reScale) 
	{
		if (this->renderTexture != newTexture)
		{
			this->renderTexture = newTexture;
			this->ResetFrame();
			if (this->renderTexture->GetFilePath().contains(".gif"))
			{
				this->parent->SetMaterial(new K::Material("shaders/3D.shader"));
				glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
			}
			else
			{
				this->parent->SetMaterial(new K::Material("shaders/2D.shader"));
				glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
			}
			if (reScale) 
			{
				this->parent->GetTransform()->scale->x = this->renderTexture->GetWidth() / 32.0f;
				this->parent->GetTransform()->scale->z = this->renderTexture->GetHeight() / 32.0f;
			}
		}
	}

	bool Sprite::IsPlaying() 
	{
		return this->isPlaying;
	}

	void Sprite::RenderInit() 
	{
		if (this->renderTexture != nullptr)
			delete this->renderTexture;
		this->SetTexture(this->texture, false);
	}

	void Sprite::Init()
	{
		if (this->texture == nullptr)
			this->SetColorTexture(WATERMARK_TEX);
		if (this->normalTexture == nullptr)
			this->SetNormalTexture(WATERMARK_TEX);
	}

	void Sprite::RenderBind() 
	{
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), this->canChromaKey);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), this->hasNormal);
		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "chromaKey"), this->chromaKeyColour[0], this->chromaKeyColour[1], this->chromaKeyColour[2]);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), true);

		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "texture0"), 0);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "texture1"), 1);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "frame"), this->frame);

		this->renderTexture->Bind(0);

		this->normalTexture->Bind(1);
	}

	void Sprite::Render() 
	{
		
	}

	void Sprite::RenderUnbind() 
	{
		this->renderTexture->Unbind();
		this->normalTexture->Unbind();
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "frame"), 0);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
	}

	void Sprite::Bind() 
	{
		
	}

	void Sprite::Unbind()
	{
		
	}

	int Sprite::GetFrameNumber() 
	{
		return this->frame;
	}

	void Sprite::NextFrame() 
	{
		if (this->renderTexture->GetNumberOfFrames() > 1)
		{
			if (this->renderTexture->isLooping)
			{
				if (this->frame < this->renderTexture->GetNumberOfFrames() - 1)
				{
					this->frame++;
				}
				else
				{
					this->frame = 0;
				}
				this->isPlaying = true;
			}
			else 
			{
				if (this->frame < this->renderTexture->GetNumberOfFrames() - 1)
				{
					this->frame++;
					this->isPlaying = true;
				}
				else 
				{
					this->frame = this->renderTexture->GetNumberOfFrames() - 1;
					this->isPlaying = false;
				}
			}
		}
	}

	void Sprite::Update()
	{
		float fps = 1.0f / this->renderTexture->GetFrameRate();
		if (this->internalClock >= fps)
		{
			this->NextFrame();
			this->internalClock = 0.0f;
		}
		this->internalClock += K::Time::deltaTime();
	}

	void Sprite::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Sprite Settings"))
		{
			ImGui::Text("FPS: %i", this->renderTexture->GetFrameRate());
			ImGui::Checkbox("has Normal Texture", &this->hasNormal);
			ImGui::Checkbox("Can Chroma Key", &this->canChromaKey);
			if (this->canChromaKey) 
			{
				ImGui::ColorPicker3("Chroma Key Colour", this->chromaKeyColour);
			}
			ImGui::Text("ID: %p,%i,%i", this->renderTexture->GetID(), this->renderTexture->GetWidth(), this->renderTexture->GetHeight());
			ImGui::Text("ViewID: %p,%i,%i",this->renderTexture->GetViewID(), this->renderTexture->GetWidth(), this->renderTexture->GetHeight());
			ImGui::ImageButton((void*)(intptr_t)(this->renderTexture->GetViewID()), ImVec2(128.0f, 128.0f), ImVec2(0,1), ImVec2(1,0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					const char* file = (const char*)payload->Data;
					this->SetColorTexture(file);
					this->SetTexture(this->texture, false);
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::Button("Delete Color Texture")) 
			{
				this->SetColorTexture(WATERMARK_TEX);
				this->SetTexture(this->texture, false);
			}
			ImGui::Text("ID: %p,%i,%i", this->normalTexture->GetID(), this->normalTexture->GetWidth(), this->normalTexture->GetHeight());
			ImGui::Text("ViewID: %p,%i,%i", this->normalTexture->GetViewID(), this->normalTexture->GetWidth(), this->normalTexture->GetHeight());
			ImGui::ImageButton((void*)(intptr_t)(this->normalTexture->GetViewID()), ImVec2(128.0f, 128.0f), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					const char* file = (const char*)payload->Data;
					this->SetNormalTexture(file);
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::Button("Delete Normal Texture"))
			{
				this->SetNormalTexture(WATERMARK_TEX);
				this->hasNormal = false;
			}
			ImGui::Checkbox("Is Looping", &this->isLooping);
		}
	}
}