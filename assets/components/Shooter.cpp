#include "Shooter.h"

namespace K 
{
	REGISTER(Shooter);

	Shooter::Shooter() 
	{

	}

	Shooter::~Shooter()
	{
		for (K::Bullet* bullet : this->bullets)
		{
			delete bullet;
		}
		bullets.clear();
		delete this->bulletTexture;
	}

	void Shooter::RenderInit() 
	{
		
	}

	void Shooter::Init()
	{
		this->parent->layer = (int)K::Layer::LayerType::Enemy;
		if (this->parent->GetComponentOfType(typeid(K::Collider).name()) != nullptr)
		{
			this->col = (K::Collider*)this->parent->GetComponentOfType(typeid(K::Collider).name());
		}
		if (this->parent->GetComponentOfType(typeid(K::Animator).name()) != nullptr)
		{
			this->anim = (K::Animator*)this->parent->GetComponentOfType(typeid(K::Animator).name());
		}
		if (this->parent->GetComponentOfType(typeid(K::Sprite).name()) != nullptr)
		{
			this->sprite = (K::Sprite*)this->parent->GetComponentOfType(typeid(K::Sprite).name());
		}
		if (this->bulletTexture == nullptr)
			this->bulletTexture = new K::Texture(WATERMARK_TEX);
	}

	void Shooter::Update()
	{
		K::Collider* hit = nullptr;
		if (Physics::HitSector(this->col->GetPosition(), this->radius, this->minAngle, this->maxAngle, { K::Layer::LayerType::Enemy, K::Layer::LayerType::Ground }, &hit) && this->reloadTime <= 0.0f)
		{
			K::Vector3 direction = (hit->GetPosition() - this->col->GetPosition());
			this->bullets.push_back(new K::Bullet(this->col->GetPosition(), direction, 10.0f, 1.0f, this->projectileSpeed));
			this->reloadTime = this->maxReloadTime;
			anim->PlayAnimation(0, this->sprite, false);
			this->sprite->ResetFrame();
		}
		if (this->reloadTime > 0.0f) 
		{
			this->reloadTime -= K::Time::deltaTime();
		}
		if (!this->bullets.empty())
		{
			for (int i = 0; i < this->bullets.size(); i++)
			{
				if (this->bullets[i]->Update())
				{
					delete this->bullets[i];
					this->bullets.erase(this->bullets.begin() + i);
				}
			}
		}

		float fps = 1.0f / this->bulletTexture->GetFrameRate();
		if (this->internalClock >= fps)
		{
			if (this->bulletTexture->GetNumberOfFrames() > 1) 
			{
				if (this->frame < this->bulletTexture->GetNumberOfFrames() - 1)
				{
					this->frame++;
				}
				else
				{
					this->frame = 0;
				}
			}
			this->internalClock = 0.0f;
		}
		this->internalClock += K::Time::deltaTime();

	}

	void Shooter::Render() 
	{
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), this->canChromaKey);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "chromaKey"), this->chromaKeyColour[0], this->chromaKeyColour[1], this->chromaKeyColour[2]);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), true);

		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "texture0"), 0);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "texture1"), 1);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "frame"), this->frame);

		this->bulletTexture->Bind(0);

		if (!bullets.empty())
		{
			for (K::Bullet* bullet : bullets)
			{
				K::Transform temp = K::Transform(new K::Vector3(bullet->GetLocation()), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
				temp.PassModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);
				bullet->Render();
			}
		}

		this->bulletTexture->Unbind();

		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "frame"), 0);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
	}

	void Shooter::RangeVisualisation() 
	{
		glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 0.0f, 1.0f, 0.0f);
		K::Transform temp = K::Transform(new K::Vector3(), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
		*temp.position = this->col->GetPosition();
		temp.PassModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);

		float theta = 360.0f / 16.0f;
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < 16; i++)
		{
			glVertex3f(this->radius * cosf((i * theta) / 57.2958f), 0.0f, this->radius * sinf((i * theta) / 57.2958f));
		}
		glEnd();

		K::Vector3 up = K::Vector3(0.0f, 0.0f, this->radius);
		K::Vector3 rotatedUp;
		K::Vector3 rotateMinAngle = K::Vector3(0.0f, minAngle, 0.0f);
		K::Matrix4x4 rotate = K::Quaternion::Euler(&rotateMinAngle).QuaternionToMatrix();
		K::MultiplyMatrixVector(up, rotatedUp, rotate);

		glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(rotatedUp.x, rotatedUp.y, rotatedUp.z);
		glEnd();

		K::Vector3 rotatedMax;
		K::Vector3 rotateMaxAngle = K::Vector3(0.0f, maxAngle, 0.0f);
		rotate = K::Quaternion::Euler(&rotateMaxAngle).QuaternionToMatrix();
		K::MultiplyMatrixVector(rotatedUp, rotatedMax, rotate);

		glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(rotatedMax.x, rotatedMax.y, rotatedMax.z);
		glEnd();

		rotateMaxAngle = K::Vector3(0.0f, -maxAngle, 0.0f);
		rotate = K::Quaternion::Euler(&rotateMaxAngle).QuaternionToMatrix();
		K::MultiplyMatrixVector(rotatedUp, rotatedMax, rotate);

		glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(rotatedMax.x, rotatedMax.y, rotatedMax.z);
		glEnd();

		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 1.0f, 1.0f, 1.0f);
	}

	void Shooter::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Shooter Settings"))
		{
			ImGui::DragFloat("Projectile Speed", &this->projectileSpeed);
			ImGui::DragFloat("Reload Time", &this->maxReloadTime);
			ImGui::DragFloat("Radius", &this->radius);
			ImGui::DragFloat("Minimum Angle", &this->minAngle);
			ImGui::DragFloat("Maximum Angle", &this->maxAngle);

			ImGui::Text("FPS: %i", this->bulletTexture->GetFrameRate());
			ImGui::Checkbox("Can Chroma Key", &this->canChromaKey);
			if (this->canChromaKey)
			{
				ImGui::ColorPicker3("Chroma Key Colour", this->chromaKeyColour);
			}
			ImGui::Text("ID: %p,%i,%i", this->bulletTexture->GetID(), this->bulletTexture->GetWidth(), this->bulletTexture->GetHeight());
			ImGui::Text("ViewID: %p,%i,%i", this->bulletTexture->GetViewID(), this->bulletTexture->GetWidth(), this->bulletTexture->GetHeight());
			ImGui::ImageButton((void*)(intptr_t)(this->bulletTexture->GetViewID()), ImVec2(128.0f, 128.0f), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					const char* file = (const char*)payload->Data;
					if (this->bulletTexture != nullptr)
						delete this->bulletTexture;
					this->bulletTexture = new K::Texture(file);
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::Button("Delete Color Texture"))
			{
				if (this->bulletTexture != nullptr)
					delete this->bulletTexture;
				this->bulletTexture = new K::Texture(WATERMARK_TEX);
			}

			this->RangeVisualisation();
		}
	}

	void Shooter::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->projectileSpeed = std::stof(temp);
				break;
			case 1:
				this->radius = std::stof(temp);
				break;
			case 2:
				this->minAngle = std::stof(temp);
				break;
			case 3:
				this->maxAngle = std::stof(temp);
				break;
			case 4:
				this->maxReloadTime = std::stof(temp);
				break;
			case 5:
				if (this->bulletTexture != nullptr)
					delete this->bulletTexture;
				this->bulletTexture = new K::Texture(temp.c_str());
				break;
			case 6:
				if (temp == "true")
				{
					this->canChromaKey = true;
				}
				else if (temp == "false")
				{
					this->canChromaKey = false;
				}
				break;
			case 7:
				if (this->canChromaKey)
					this->chromaKeyColour[0] = std::stof(temp);
				break;
			case 8:
				if (this->canChromaKey)
					this->chromaKeyColour[1] = std::stof(temp);
				break;
			case 9:
				if (this->canChromaKey)
					this->chromaKeyColour[2] = std::stof(temp);
				break;
			}
		}
	}

	const char* Shooter::GetPropertyValues()
	{
		this->properties = std::to_string(this->projectileSpeed);
		this->properties += "," + std::to_string(this->radius);
		this->properties += "," + std::to_string(this->minAngle);
		this->properties += "," + std::to_string(this->maxAngle);
		this->properties += "," + std::to_string(this->maxReloadTime);
		this->properties += "," + this->bulletTexture->GetFilePath();
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
		return this->properties.c_str();
	}
}