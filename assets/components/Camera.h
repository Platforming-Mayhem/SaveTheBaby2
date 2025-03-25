#pragma once
#include <K_Engine.h>

namespace K
{
	KC_API class Camera : public K::Component
	{
		bool isEditorCamActive = false;
		bool isActive = false;

		float nearPlane = 0.9f;
		float farPlane = 1000.0f;
		float FOV = 60.0f;
		float orthoSize = 1.0f;
		float movementSpeed = 10.0f;
		float rotationSpeed = 100.0f;
		float backgroundColour[4];

		K::Window* window = nullptr;

		K::Transform* editorCam = nullptr;

		enum class CameraType { Perspective = 0, Orthographic = 1 };
		CameraType cameraType = CameraType::Perspective;

		K::Matrix4x4 projectionMatrix;
		K::Matrix4x4 viewMatrix;

		std::string properties;
	public:

		Camera();

		~Camera();

		float GetFOV();

		float GetNearPlane();

		float GetFarPlane();

		K::Matrix4x4 GetProjectionMatrix();

		K::Matrix4x4 GetViewMatrix();

		K::Vector3 GetPosition() 
		{
			if (this->isEditorCamActive) 
			{
				return *this->editorCam->position;
			}
			else 
			{
				return *this->parent->GetTransform()->position;
			}
		}

		K::Vector3* GetPositionPointer()
		{
			if (this->isEditorCamActive)
			{
				return this->editorCam->position;
			}
			else
			{
				return this->parent->GetTransform()->position;
			}
		}

		void SetWindow(K::Window* window)
		{
			this->window = window;
		}

		void SetActiveState(bool state);

		void SetFOV(float newFOV);

		void CameraMatrix();

		void SetProjectionMatrix();

		void Init() override;

		void RenderInit() override;

		void Render() override;

		void Update() override;

		void UpdateEditor() override;

		void Bind() override;

		void RenderBind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}