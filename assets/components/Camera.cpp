#include "Camera.h"

namespace K 
{
	REGISTER(Camera);

	Camera::Camera()
	{
		this->editorCam = new K::Transform(new K::Vector3(), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
		ImGui::SetCurrentContext(K::Editor::GetImGuiContext());
	}

	Camera::~Camera() 
	{
		//std::cout << "Camera Destructor..." << std::endl;
		delete this->editorCam;
	}

	float Camera::GetFOV() 
	{
		return this->FOV;
	}

	float Camera::GetNearPlane()
	{
		return this->nearPlane;
	}

	float Camera::GetFarPlane()
	{
		return this->farPlane;
	}

	void Camera::SetFOV(float newFOV) 
	{
		this->FOV = newFOV;
	}
	
	void Camera::SetActiveState(bool state) 
	{
		this->isActive = state;
	}

	void Camera::SetProjectionMatrix() 
	{
		//Projection Matrix
		float aspect = this->window->width / this->window->height;
		float deltaZ = this->farPlane - this->nearPlane;

		if (this->cameraType == CameraType::Perspective)
		{
			float radians = ((3.14159f * 2.0f) / 360.0f) * this->FOV / 2.0f;
			float fFovRad = tanf(this->FOV * 0.5f / 180.0f * 3.14159f);
			this->projectionMatrix = K::Matrix4x4::IdentityMatrix();
			this->projectionMatrix.m[0][0] = 1.0f / (aspect * fFovRad);
			this->projectionMatrix.m[1][1] = 1.0f / fFovRad;
			this->projectionMatrix.m[2][2] = -((this->farPlane + this->nearPlane) / deltaZ);
			this->projectionMatrix.m[3][2] = -((2.0f * this->farPlane * this->nearPlane) / deltaZ);
			this->projectionMatrix.m[2][3] = -1.0f;
			this->projectionMatrix.m[3][3] = 0.0f;
		}
		else if (this->cameraType == CameraType::Orthographic)
		{
			float t = this->orthoSize, b = -this->orthoSize, l = -this->orthoSize * aspect, r = this->orthoSize * aspect;
			this->projectionMatrix = K::Matrix4x4::IdentityMatrix();
			this->projectionMatrix.m[0][0] = 2.0f / (r - l);
			this->projectionMatrix.m[1][1] = 2.0f / (t - b);
			this->projectionMatrix.m[2][2] = -2.0f / deltaZ;
			this->projectionMatrix.m[3][0] = -(r + l) / (r - l);
			this->projectionMatrix.m[3][1] = -(t + b) / (t - b);
			this->projectionMatrix.m[3][2] = -(this->farPlane + this->nearPlane) / deltaZ;
		}
	}

	void Camera::CameraMatrix() 
	{
		if (this->isActive)
		{
			this->SetProjectionMatrix();
			//View Matrix
			if (this->isEditorCamActive)
			{
				this->editorCam->PassModelMatrix();
				this->viewMatrix = K::QuickInverse(this->editorCam->modelMatrix);
			}
			else 
			{
				this->viewMatrix = K::QuickInverse(this->parent->GetTransform()->modelMatrix);
			}
			if (this->parent->parent != nullptr)
			{
				K::Matrix4x4 scalingMatrix = this->parent->GetTransform()->LocalScaleMatrix(this->parent->parent->GetTransform());
				K::Matrix4x4 invertedScalingMatrix = K::QuickInverse(scalingMatrix);
				this->viewMatrix = K::Matrix4x4::Matrix_MultiplyMatrix(this->viewMatrix, invertedScalingMatrix);
			}
			else
			{
				K::Matrix4x4 scalingMatrix = this->parent->GetTransform()->ScaleMatrix();
				K::Matrix4x4 invertedScalingMatrix = K::QuickInverse(scalingMatrix);
				this->viewMatrix = K::Matrix4x4::Matrix_MultiplyMatrix(this->viewMatrix, invertedScalingMatrix);
			}

			glViewport(0, 0, this->window->width, this->window->height);

			K::Editor::cameraPosition = this->GetPositionPointer();

			K::Editor::projectionMatrix = &this->projectionMatrix;

			K::Editor::viewMatrix = &this->viewMatrix;

			for (auto& mat : K::materialManager.materials) 
			{
				glUseProgram(mat.second.id);

				glUniformMatrix4fv(glGetUniformLocation(mat.second.id, "viewMatrix"), 1, GL_FALSE, &this->viewMatrix.m[0][0]);

				glUniformMatrix4fv(glGetUniformLocation(mat.second.id, "projectionMatrix"), 1, GL_FALSE, &this->projectionMatrix.m[0][0]);

				glUniform3f(glGetUniformLocation(mat.second.id, "fogColour"), this->backgroundColour[0], this->backgroundColour[1], this->backgroundColour[2]);
			}

			glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
		}
	}

	void Camera::Bind()
	{
		if (InputManager::IsKeyPressedDown(GLFW_KEY_V)) 
		{
			if (this->isEditorCamActive) 
			{
				this->isEditorCamActive = false;
			}
			else 
			{
				this->isEditorCamActive = true;
				*this->editorCam->position = *this->parent->GetTransform()->position;
				*this->editorCam->rotation = *this->parent->GetTransform()->rotation;
			}
		}
		else if (InputManager::IsKeyReleased(GLFW_KEY_V)) 
		{

		}
		if (this->isEditorCamActive) 
		{
			K::Vector3 rotatedForward, rotatedRight, rotatedUp;
			K::Vector3 forward = K::Vector3(0.0f, 0.0f, 1.0f);
			K::Vector3 up = K::Vector3(0.0f, 1.0f, 0.0f);
			K::Vector3 right = K::Vector3(1.0f, 0.0f, 0.0f);
			K::Quaternion camQuat = K::Quaternion::Euler(this->editorCam->rotation);
			K::Matrix4x4 camRotation = camQuat.QuaternionToMatrix();
			K::MultiplyMatrixVector(forward, rotatedForward, camRotation);
			K::MultiplyMatrixVector(up, rotatedUp, camRotation);
			K::MultiplyMatrixVector(right, rotatedRight, camRotation);
			if (InputManager::IsKeyPressed(GLFW_KEY_A))
			{
				*this->editorCam->position -= rotatedRight * (K::Time::deltaTime() * 20.0f);
			}
			else if (InputManager::IsKeyPressed(GLFW_KEY_D))
			{
				*this->editorCam->position += rotatedRight * (K::Time::deltaTime() * 20.0f);
			}
			if (InputManager::IsKeyPressed(GLFW_KEY_W))
			{
				*this->editorCam->position += rotatedUp * (K::Time::deltaTime() * 20.0f);
			}
			else if (InputManager::IsKeyPressed(GLFW_KEY_S))
			{
				*this->editorCam->position -= rotatedUp * (K::Time::deltaTime() * 20.0f);
			}
		}
	}

	void Camera::RenderBind() 
	{
		glClearColor(this->backgroundColour[0], this->backgroundColour[1], this->backgroundColour[2], this->backgroundColour[3]);

		this->CameraMatrix();
	}

	K::Matrix4x4 Camera::GetProjectionMatrix() 
	{
		return this->projectionMatrix;
	}

	K::Matrix4x4 Camera::GetViewMatrix() 
	{
		return this->viewMatrix;
	}

	const char* Camera::GetPropertyValues()
	{
		this->properties = std::to_string(this->nearPlane) + ",";
		this->properties += std::to_string(this->farPlane) + ",";
		this->properties += std::to_string(this->FOV) + ",";
		this->properties += std::to_string(this->orthoSize) + ",";
		this->properties += std::to_string(this->movementSpeed) + ",";
		this->properties += std::to_string(this->rotationSpeed) + ",";
		std::string temp;
		if (this->isActive) 
		{
			temp = "true";
		}
		else 
		{
			temp = "false";
		}
		this->properties += temp + ",";
		temp = std::to_string((int)this->cameraType);
		this->properties += temp + ",";
		this->properties += std::to_string(this->backgroundColour[0]) + ",";
		this->properties += std::to_string(this->backgroundColour[1]) + ",";
		this->properties += std::to_string(this->backgroundColour[2]) + ",";
		this->properties += std::to_string(this->backgroundColour[3]);
		return this->properties.c_str();
	}

	void Camera::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr) 
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->nearPlane = std::stof(temp);
				break;
			case 1:
				this->farPlane = std::stof(temp);
				break;
			case 2:
				this->FOV = std::stof(temp);
				break;
			case 3:
				this->orthoSize = std::stof(temp);
				break;
			case 4:
				this->movementSpeed = std::stof(temp);
				break;
			case 5:
				this->rotationSpeed = std::stof(temp);
				break;
			case 6:
				if (temp == "true")
				{
					this->SetActiveState(true);
				}
				else if (temp == "false")
				{
					this->SetActiveState(false);
				}
				break;
			case 7:
				if (temp == std::to_string((int)CameraType::Perspective)) 
				{
					this->cameraType = CameraType::Perspective;
				}
				else if (temp == std::to_string((int)CameraType::Orthographic))
				{
					this->cameraType = CameraType::Orthographic;
				}
				break;
			case 8:
				this->backgroundColour[0] = std::stof(temp);
				break;
			case 9:
				this->backgroundColour[1] = std::stof(temp);
				break;
			case 10:
				this->backgroundColour[2] = std::stof(temp);
				break;
			case 11:
				this->backgroundColour[3] = std::stof(temp);
				break;
			}
		}
	}

	void Camera::Init() 
	{
		this->SetWindow(K::window);
	}

	void Camera::RenderInit() 
	{
		
	}

	void Camera::Unbind()
	{
		
	}

	void Camera::Render()
	{
		
	}

	void Camera::Update() 
	{
		
	}

	void Camera::UpdateEditor()
	{
		//IMGUI
		if (ImGui::CollapsingHeader("Camera Settings")) 
		{
			ImGui::Text("These are the Camera settings");
			if (this->cameraType == CameraType::Perspective)
			{
				if (ImGui::BeginCombo("Camera Type", "Perspective"))
				{
					if (ImGui::Selectable("Perspective"))
					{
						this->cameraType = CameraType::Perspective;
					}
					else if (ImGui::Selectable("Orthographic"))
					{
						this->cameraType = CameraType::Orthographic;
					}
					ImGui::EndCombo();
				}
				ImGui::DragFloat("Camera Near Plane", &this->nearPlane);
				ImGui::DragFloat("Camera Far Plane", &this->farPlane);
				ImGui::SliderFloat("Camera FOV", &this->FOV, 30.0f, 180.0f);
			}
			else if (this->cameraType == CameraType::Orthographic)
			{
				if (ImGui::BeginCombo("Camera Type", "Orthographic"))
				{
					if (ImGui::Selectable("Perspective"))
					{
						this->cameraType = CameraType::Perspective;
					}
					else if (ImGui::Selectable("Orthographic"))
					{
						this->cameraType = CameraType::Orthographic;
					}
					ImGui::EndCombo();
				}
				ImGui::SliderFloat("Camera Size", &this->orthoSize, 1.0f, 100.0f);
			}
			ImGui::DragFloat("Movement Speed", &this->movementSpeed);
			ImGui::DragFloat("Rotation Speed", &this->rotationSpeed);
			ImGui::DragFloat3("Editor Cam Rotation", &this->editorCam->rotation->x);
			ImGui::Checkbox("is Active", &this->isActive);
			ImGui::ColorPicker4("Background Colour", this->backgroundColour);
		}
	}
}