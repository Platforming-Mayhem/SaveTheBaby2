#include "Move.h"
#include "PhysicsEngine.h"

namespace K 
{
	REGISTER(Move);

	Move::Move()
	{

	}

	Move::~Move()
	{

	}

	float Move::EaseInQuart(float x)
	{
		return x * x * x * x;
	}

	void Move::Init()
	{
		this->time = 0.2f;
		destination = *this->parent->GetTransform()->position;
	}

	void Move::Update()
	{

	}

	void Move::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Move Settings"))
		{
			ImGui::DragFloat("Move Speed", &this->moveSpeed);
		}
	}

	void Move::Bind()
	{
#if _DEBUG

#else
		float speed = EaseInQuart(this->time);
		if (speed < 1.0f)
		{
			this->time += K::Time::deltaTime() * 0.25f;
		}
		else if (speed >= 1.0f)
		{
			this->time = 1.0f;
		}
		destination.x += speed * K::Time::deltaTime() * this->moveSpeed;
		destination.z = K::Physics::GetClosestPoint(*this->parent->GetTransform()->position + K::Vector3(30.0f, 0.0f, -10.0f), { K::Layer(K::Layer::LayerType::Enemy), K::Layer(K::Layer::LayerType::Player) }).z + 15.0f;
		*this->parent->GetTransform()->position = K::Vector3::Lerp(*this->parent->GetTransform()->position, this->destination, K::Time::deltaTime());
#endif
	}

	void Move::Unbind()
	{

	}

	void Move::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->moveSpeed = std::stof(temp);
				break;
			}
		}
	}

	const char* Move::GetPropertyValues()
	{
		this->properties = std::to_string(this->moveSpeed);
		return this->properties.c_str();
	}
}