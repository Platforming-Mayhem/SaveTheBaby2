#pragma once
#include <K_Engine.h>
#include "PhysicsEngine.h"
#include "Mesh.h"
#include "Animator.h"

namespace K 
{
	KC_API class Bullet
	{
	private:
		K::Mesh mesh;
		K::Vector3 origin;
		K::Vector3 direction;
		float timeElapsed = 0.0f;
		float lifeTime = 1.0f;
		float radius = 1.0f;
		float bulletSpeed = 1.0f;
	public:

		Bullet(K::Vector3 origin, K::Vector3 direction, float lifeTime = 1.0f, float radius = 1.0f, float bulletSpeed = 1.0f) 
		{
			this->origin = origin;
			this->timeElapsed = 0.0f;
			this->lifeTime = lifeTime;
			this->direction = direction;
			this->radius = radius;
			this->bulletSpeed = bulletSpeed;
			this->direction = this->direction.normalise();
			this->mesh = K::Mesh();
			this->mesh.SetShader("shaders/2D.shader");
			this->mesh.Init();
			this->mesh.RenderInit();
		}

		~Bullet() 
		{
			
		}

		void Render() 
		{
			this->mesh.Render();
		}

		bool Update() 
		{
			if (this->timeElapsed >= this->lifeTime) 
			{
				return true;
			}
			else 
			{
				K::Collider* other = nullptr;
				K::Vector3 closest = K::Physics::GetClosestPoint(this->GetLocation(), { K::Layer::LayerType::Enemy, K::Layer::LayerType::Ground }, &other);
				K::Vector3 displacement = (this->GetLocation() - closest) * K::Vector3(1.0f, 0.0f, 1.0f);
				if (displacement.magnitude() <= this->radius) 
				{
					if (other != nullptr) 
					{
						K::Editor::Delete(other->parent);
					}
				}
				this->timeElapsed += K::Time::deltaTime();
				return false;
			}
		}

		K::Vector3 GetLocation() 
		{
			return this->origin + (this->direction * (this->timeElapsed * this->bulletSpeed));
		}
	};

	KC_API class Shooter : public K::Component
	{
	private:
		std::vector<K::Bullet*> bullets;
		K::Collider* col;
		K::Sprite* sprite;
		K::Animator* anim;
		K::Texture* bulletTexture = nullptr;

		int frame = 0;

		float radius = 1.0f;
		float minAngle = 90.0f;
		float maxAngle = 270.0f;
		float projectileSpeed = 1.0f;
		float reloadTime = 0.0f;
		float maxReloadTime = 1.0f;
		float internalClock = 0;
		float chromaKeyColour[3];

		bool canChromaKey = false;

		std::string properties;
	public:

		Shooter();

		~Shooter();

		void RangeVisualisation();

		void RenderInit() override;

		void Init() override;

		void Render() override;

		void Update() override;

		void UpdateEditor() override;

		void SetPropertyValues(const char* value, int valueIndex) override;

		const char* GetPropertyValues() override;
	};
}