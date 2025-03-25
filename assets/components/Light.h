#pragma once
#include <K_Engine.h>

namespace K 
{
	KC_API class Light : public K::Component
	{
	private:
		K::Vector3 lightDirection = K::Vector3(0.0f, 0.0f, 1.0f);
		std::string properties;
	public:
		enum class LightType { Directional = 0, Point = 1};

		LightType lightType = LightType::Directional;

		Light();

		~Light();

		void Init() override;

		void Update() override;

		void UpdateEditor() override;

		void RenderBind() override;

		void Bind() override;

		void Unbind()  override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}