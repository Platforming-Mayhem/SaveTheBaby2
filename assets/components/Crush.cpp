#include "Crush.h"

namespace K 
{
	REGISTER(Crush);

	Crush::Crush()
	{
		this->offset = new K::Vector3(0.0f, 0.0f, 0.0f);
	}

	Crush::~Crush()
	{
		delete this->offset;
	}

	void Crush::Init()
	{
		this->origin = *this->parent->GetTransform()->position;
	}

	float Crush::EaseInPow(float value, int pow)
	{
		return std::clamp(std::powf(value, pow), 0.0f, 1.0f);
	}

	float Crush::EaseInOutQuad(float value) 
	{
		return value < 0.5f ? 2 * value * value : 1 - std::powf(-2.0f * value + 2.0f, 2.0f) / 2.0f;
	}

	void Crush::Update() 
	{
		#if _DEBUG

#else
		if (this->crushTime >= 1.0f) 
		{
			if (this->crush) 
			{
				this->crush = false;
			}
			else 
			{
				this->crush = true;
			}
			this->crushTime = 0.0f;
		}
		else 
		{
			this->crushTime += K::Time::deltaTime();
		}
		if (this->crush) 
		{
			K::Vector3 originOffsetted = this->origin + *this->offset;
			*this->parent->GetTransform()->position = K::Vector3::Lerp(this->origin, originOffsetted, this->EaseInPow(this->crushTime, 3.0f));
		}
		else 
		{
			K::Vector3 originOffsetted = this->origin + *this->offset;
			*this->parent->GetTransform()->position = K::Vector3::Lerp(originOffsetted, this->origin, this->EaseInOutQuad(this->crushTime));
		}
#endif
	}

	void Crush::VisualDebug() 
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
		K::Transform transform = K::Transform(new K::Vector3(), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
		transform.PassModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &transform.modelMatrix.m[0][0]);

		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(this->parent->GetTransform()->position->x, this->parent->GetTransform()->position->y, this->parent->GetTransform()->position->z);
		glVertex3f(this->parent->GetTransform()->position->x + this->offset->x, this->parent->GetTransform()->position->y + this->offset->y, this->parent->GetTransform()->position->z + this->offset->z);
		glEnd();
		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 1.0f, 1.0f, 1.0f);
	}

	void Crush::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Crush Settings"))
		{
			ImGui::DragFloat("Crush Time Offset", &this->crushTime);
			ImGui::DragFloat3("Offset", (float*)this->offset);
			this->VisualDebug();
		}
	}

	void Crush::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->offset->x = std::stof(temp);
				break;
			case 1:
				this->offset->y = std::stof(temp);
				break;
			case 2:
				this->offset->z = std::stof(temp);
				break;
			case 3:
				this->crushTime = std::stof(temp);
			}
		}
	}

	const char* Crush::GetPropertyValues()
	{
		this->properties = std::to_string(this->offset->x);
		this->properties += "," + std::to_string(this->offset->y);
		this->properties += "," + std::to_string(this->offset->z);
		this->properties += "," + std::to_string(this->crushTime);
		return this->properties.c_str();
	}
}