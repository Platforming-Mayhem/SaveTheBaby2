#include "Quit.h"

namespace K 
{
	REGISTER(Quit);

	Quit::Quit()
	{
	}

	Quit::~Quit()
	{
	}

	void Quit::Init()
	{
		for (auto i : K::Editor::GetCurrentScene()->GetGameObjects())
		{
			if (i.second->GetComponentOfType(typeid(K::Camera).name()) != nullptr)
			{
				this->mainCamera = (K::Camera*)i.second->GetComponentOfType(typeid(K::Camera).name());
				break;
			}
		}
		this->mesh = (K::Mesh*)this->parent->GetComponentOfType(typeid(K::Mesh).name());
	}

	void Quit::Update() 
	{
		if (InputManager::IsMouseKeyPressed(GLFW_MOUSE_BUTTON_1))
		{
			K::Vector3 direction = K::InputManager::GetWorldMouseDirection();
			K::Vector3 camPosition = this->mainCamera->GetPosition();
			for (int j = 0; j < this->mesh->indices.size() / 3; j++)
			{
				K::Vector3 vertices[3];
				K::Vector3 normal;
				int index = 0;
				for (int i = 0 + (j * 3); i < 3 + (j * 3); i++)
				{
					K::MultiplyMatrixVector(this->mesh->vertices[this->mesh->indices[i]].position, vertices[index], mesh->parent->GetTransform()->modelMatrix);
					K::Quaternion quat = K::Quaternion::Euler(this->mesh->parent->GetTransform()->rotation);
					K::Matrix4x4 rotationMatrix = quat.QuaternionToMatrix();
					K::MultiplyMatrixVector(this->mesh->vertices[this->mesh->indices[i]].normal, normal, rotationMatrix);
					index++;
				}
				//Triangle vertices[3]
				//Test Against Ray
				K::Vector3 A = vertices[1] - vertices[0];
				K::Vector3 B = vertices[2] - vertices[1];
				K::Vector3 C = vertices[0] - vertices[2];

				float D = -K::Vector3::DotProduct(normal, vertices[0]);
				float t = -(K::Vector3::DotProduct(normal, camPosition) + D) / K::Vector3::DotProduct(normal, direction);

				if (t > 0.0f)
				{
					K::Vector3 P = (camPosition + (direction * t));
					K::Vector3 C0 = P - vertices[0];
					K::Vector3 C1 = P - vertices[1];
					K::Vector3 C2 = P - vertices[2];

					K::Vector3 N0 = K::Vector3::CrossProduct(A, C0);
					K::Vector3 N1 = K::Vector3::CrossProduct(B, C1);
					K::Vector3 N2 = K::Vector3::CrossProduct(C, C2);
					float dot0 = K::Vector3::DotProduct(normal, N0);
					float dot1 = K::Vector3::DotProduct(normal, N1);
					float dot2 = K::Vector3::DotProduct(normal, N2);
					if (dot0 > 0.0f && dot1 > 0.0f && dot2 > 0.0f)
					{
						this->selected = true;
					}
				}
			}
		}
		if (this->selected) 
		{
			K::SceneManager::Quit();
		}
	}

	void Quit::UpdateEditor()
	{
		if (ImGui::CollapsingHeader(this->GetName()))
		{

		}
	}

	void Quit::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				break;
			}
		}
	}

	const char* Quit::GetPropertyValues()
	{
		return this->properties.c_str();
	}
}