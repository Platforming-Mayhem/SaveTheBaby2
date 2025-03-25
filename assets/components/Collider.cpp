#include "Collider.h"
#include "PhysicsEngine.h"

namespace K 
{
	REGISTER(Collider);

	Collider::Collider() 
	{
		
	}

	Collider::~Collider()
	{
		//std::cout << "Begin Collider Destruction..." << std::endl;
		K::Physics::Remove(this);
		this->linePoints.clear();
		this->linePoints.shrink_to_fit();
		this->linePointsModelMatrix.clear();
		this->linePointsModelMatrix.shrink_to_fit();
		//std::cout << "End Collider Destruction..." << std::endl;
	}

	void Collider::CapsuleEditor()
	{
		ImGui::DragFloat("Capsule Radius", &this->radius);
		ImGui::DragFloat("Capsule Height", &this->height);
		ImGui::DragFloat3("Offset", (float*)&this->offset);
	}

	void Collider::CapsuleVisualDebug()
	{
		//Draw Circle
		glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 0.0f, 1.0f, 0.0f);
		K::Vector3 topPosition = this->GetPosition() + K::Vector3(0.0f, 0.0f, this->GetHeight() * 0.5f);
		K::Vector3 bottomPosition = this->GetPosition() - K::Vector3(0.0f, 0.0f, this->GetHeight() * 0.5f);
		K::Transform temp = K::Transform(new K::Vector3(), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
		*temp.position = topPosition;
		temp.PassModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);
		float theta = 360.0f / 16.0f;
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= 8; i++)
		{
			glVertex3f(this->radius * cosf((i * theta) / 57.2958f), 0.0f, this->radius * sinf((i * theta) / 57.2958f));
		}
		glEnd();
		*temp.position = bottomPosition;
		temp.PassModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);
		glBegin(GL_LINE_STRIP);
		for (int i = 8; i <= 16; i++)
		{
			glVertex3f(this->radius * cosf((i * theta) / 57.2958f), 0.0f, this->radius * sinf((i * theta) / 57.2958f));
		}
		glEnd();
		*temp.position = this->GetPosition();
		temp.PassModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);

		glBegin(GL_LINE_STRIP);
		glVertex3f(this->radius, 0.0f, this->GetHeight() * 0.5f);
		glVertex3f(this->radius, 0.0f, this->GetHeight() * -0.5f);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex3f(-this->radius, 0.0f, this->GetHeight() * 0.5f);
		glVertex3f(-this->radius, 0.0f, this->GetHeight() * -0.5f);
		glEnd();

		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 1.0f, 1.0f, 1.0f);
	}

	void Collider::CapsuleColliderStatic()
	{

	}

	void Collider::CapsuleCollider()
	{
		if (K::Physics::CanGetClosestPoint())
		{
			if (K::Physics::IsColliding(this->parent))
			{
				this->time = 0.0f;
			}
			else
			{
				*(this->parent->GetTransform()->position) += K::Vector3(0.0f, 0.0f, -this->time * K::Physics::GetFixedTimeStep() * K::Time::deltaTime());
				this->time += (1 / K::Physics::GetFixedTimeStep()) * K::Time::deltaTime() * 60.0f * 0.6f;
			}
			*this->parent->GetTransform()->position += K::Physics::GetCollisionResolution(this);
		}
	}

	void Collider::Init()
	{
		K::Physics::Attach(this);
	}

	void Collider::RenderInit() 
	{
		this->ResetVelocity();
	}

	void Collider::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Collider Settings")) 
		{
			ImGui::Checkbox("Is Static", &this->isStatic);
			switch (this->colliderType)
			{
			case ColliderType::Circle:
				if (ImGui::BeginCombo("Collider Type", "Circle"))
				{
					if (ImGui::Selectable("Line"))
					{
						this->colliderType = ColliderType::Line;
					}
					else if (ImGui::Selectable("Capsule"))
					{
						this->colliderType = ColliderType::Capsule;
					}
					ImGui::EndCombo();
				}
				this->CircleVisualDebug();
				this->CircleEditor();
				break;
			case ColliderType::Line:
				if (ImGui::BeginCombo("Collider Type", "Line"))
				{
					if (ImGui::Selectable("Circle"))
					{
						this->colliderType = ColliderType::Circle;
					}
					else if (ImGui::Selectable("Capsule"))
					{
						this->colliderType = ColliderType::Capsule;
					}
					ImGui::EndCombo();
				}
				this->LineVisualDebug();
				this->LineEditor();
				break;
			case ColliderType::Capsule:
				if (ImGui::BeginCombo("Collider Type", "Capsule"))
				{
					if (ImGui::Selectable("Circle"))
					{
						this->colliderType = ColliderType::Circle;
					}
					else if (ImGui::Selectable("Line"))
					{
						this->colliderType = ColliderType::Line;
					}
					ImGui::EndCombo();
				}
				this->CapsuleVisualDebug();
				this->CapsuleEditor();
				break;
			}
			ImGui::Checkbox("Is Colliding", &this->isColliding);
		}
	}

	void Collider::LineEditor() 
	{
		if (ImGui::Button("Add Line")) 
		{
			this->linePoints.push_back(K::Line(K::Vector3(0.0f, 0.0f, 0.0f), K::Vector3(1.0f, 0.0f, 0.0f)));
			this->linePointsModelMatrix.push_back(K::Line(K::Vector3(0.0f, 0.0f, 0.0f), K::Vector3(1.0f, 0.0f, 0.0f)));
			this->selectedLine = this->linePoints.size() - 1;
		}
		if (this->linePoints.size() > 0) 
		{
			if (ImGui::BeginListBox("Line Points")) 
			{
				for (int i = 0; i < this->linePoints.size(); i++)
				{
					if (ImGui::Selectable(std::to_string(i).c_str()))
					{
						this->selectedLine = i;
					}
				}
				ImGui::EndListBox();
			}

			ImGui::DragFloat("X1:", &this->linePoints[this->selectedLine].point[0].x);
			ImGui::DragFloat("Y1:", &this->linePoints[this->selectedLine].point[0].y);
			ImGui::DragFloat("X2:", &this->linePoints[this->selectedLine].point[1].x);
			ImGui::DragFloat("Y2:", &this->linePoints[this->selectedLine].point[1].y);
			if (ImGui::Button("Delete Line")) 
			{
				this->linePoints.erase(this->linePoints.begin() + this->selectedLine);
				this->linePointsModelMatrix.erase(this->linePointsModelMatrix.begin() + this->selectedLine);
				if (this->linePoints.size() > 0)
				{
					this->selectedLine = this->linePoints.size() - 1;
				}
				else 
				{
					this->selectedLine = 0;
				}
			}
		}
	}

	void Collider::CircleEditor()
	{
		ImGui::DragFloat("Circle Radius", &this->radius);
		ImGui::DragFloat3("Offset", (float*)&this->offset);
	}

	void Collider::CircleCollider()
	{
		if (K::Physics::CanGetClosestPoint())
		{
			if (K::Physics::IsColliding(this->parent))
			{
				this->time = 0.0f;
			}
			else
			{
				*this->parent->GetTransform()->position += K::Vector3(0.0f, 0.0f, -this->time * K::Time::deltaTime() * K::Physics::GetFixedTimeStep());
				this->time += 1.0f / K::Physics::GetFixedTimeStep();
			}
			*this->parent->GetTransform()->position += K::Physics::GetCollisionResolution(this);
		}
	}

	void Collider::ResetVelocity() 
	{
		this->time = 0.0f;
	}

	void Collider::CircleColliderStatic() 
	{
		if (K::Physics::CanGetClosestPoint())
		{
			*this->parent->GetTransform()->position += K::Physics::GetCollisionResolution(this, { K::Layer::LayerType::Player, K::Layer::LayerType::Enemy });
		}
	}

	void Collider::CircleVisualDebug() 
	{
		//Draw Circle
		glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
		glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 0.0f, 1.0f, 0.0f);
		K::Transform temp = K::Transform(new K::Vector3(), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
		*temp.position = this->GetPosition();
		temp.PassModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);
		float theta = 360.0f / 16.0f;
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < 16; i++)
		{
			glVertex3f(this->radius * cosf((i * theta) / 57.2958f), 0.0f, this->radius * sinf((i * theta) / 57.2958f));
		}
		glEnd();
		//Draw contact points
		if (K::Physics::CanGetClosestPoint())
		{
			//DEBUGGING
			glClear(GL_DEPTH_BUFFER_BIT);
			glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
			glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
			glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
			for (K::ContactPoint pointOnLine : K::Physics::GetClosestPoints(this->GetPosition()))
			{
				if ((pointOnLine.position - this->GetPosition()).magnitude() < this->GetRadius())
				{
					glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 1.0f, 0.0f, 0.0f);
				}
				else 
				{
					glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 0.0f, 0.0f, 1.0f);
				}
				*temp.position = pointOnLine.position;
				*temp.scale = K::Vector3(0.1f, 0.1f, 0.1f);
				temp.PassModelMatrix();
				glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &temp.modelMatrix.m[0][0]);
				glBegin(GL_QUADS);
				glVertex3f(-1.0f, 0.0f, 1.0f);
				glVertex3f(-1.0f, 0.0f, -1.0f);
				glVertex3f(1.0f, 0.0f, -1.0f);
				glVertex3f(1.0f, 0.0f, 1.0f);
				glEnd();
			}
		}
		glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 1.0f, 1.0f, 1.0f);
	}

	float Collider::GetRadius() 
	{
		return this->radius;
	}

	float Collider::GetHeight() 
	{
		return this->height;
	}

	void Collider::LineVisualDebug()
	{
		glUseProgram(this->parent->GetMaterial()->GetShader()->shader);
		if (this->linePoints.size() > 0)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "canChromaKey"), false);
			glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasNormal"), false);
			glUniform1i(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "hasTexture"), false);
			K::Transform transform = K::Transform(new K::Vector3(), new K::Vector3(), new K::Vector3(1.0f, 1.0f, 1.0f));
			transform.PassModelMatrix();
			glUniformMatrix4fv(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "modelMatrix"), 1, GL_FALSE, &transform.modelMatrix.m[0][0]);
			for (int i = 0; i < this->linePoints.size(); i++)
			{
				if (i == this->selectedLine) 
				{
					glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 0.0f, 0.0f, 1.0f);
				}
				else 
				{
					glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 0.0f, 1.0f, 0.0f);
				}
				glBegin(GL_LINES);
				glVertex3f(this->linePointsModelMatrix[i].point[0].x, 0.0f, this->linePointsModelMatrix[i].point[0].y);
				glVertex3f(this->linePointsModelMatrix[i].point[1].x, 0.0f, this->linePointsModelMatrix[i].point[1].y);
				glEnd();
			}
			glUniform3f(glGetUniformLocation(this->parent->GetMaterial()->GetShader()->shader, "colorTint"), 1.0f, 1.0f, 1.0f);
		}
	}

	void Collider::LineColliderStatic() 
	{
		for (int i = 0; i < this->linePoints.size(); i++) 
		{
			K::Vector3 points[2];
			K::Vector3 beforeModelMatrix[2];
			beforeModelMatrix[0] = K::Vector3(this->linePoints[i].point[0].x, 0.0f, this->linePoints[i].point[0].y);
			beforeModelMatrix[1] = K::Vector3(this->linePoints[i].point[1].x, 0.0f, this->linePoints[i].point[1].y);
			K::MultiplyMatrixVector(beforeModelMatrix[0], points[0], this->parent->GetTransform()->modelMatrix);
			K::MultiplyMatrixVector(beforeModelMatrix[1], points[1], this->parent->GetTransform()->modelMatrix);
			this->linePointsModelMatrix[i] = K::Line(K::Vector3(points[0].x, points[0].z, 0.0f), K::Vector3(points[1].x, points[1].z, 0.0f));
		}
	}

	K::Vector3* Collider::ClosestPointLineCollider(K::Vector3 P) 
	{
		int closestIndex = 0;
		float distance = INFINITY;
		for (int i = 0; i < this->linePoints.size(); i++) 
		{
			K::Vector3 J = K::Vector3(this->PointOnLine(this->linePointsModelMatrix[i].point[0], this->linePointsModelMatrix[i].point[1], P).x, P.y, this->PointOnLine(this->linePointsModelMatrix[i].point[0], this->linePointsModelMatrix[i].point[1], P).y);
			K::Vector3 PJ = J - P;
			if (PJ.magnitude() < distance && PJ.magnitude() > 0.0f) 
			{
				distance = PJ.magnitude();
				closestIndex = i;
			}
			else if (PJ.magnitude() <= 0.0f) 
			{
				distance = 0.0f;
				closestIndex = i;
				break;
			}
		}
		K::Vector3 J = K::Vector3(this->PointOnLine(this->linePoints[closestIndex].point[0], this->linePoints[closestIndex].point[1], P).x, 0.0f, this->PointOnLine(this->linePoints[closestIndex].point[0], this->linePoints[closestIndex].point[1], P).y);
		return &J;
	}

	K::Vector3 Collider::PointOnLine(K::Vector3 A, K::Vector3 B, K::Vector3 P)
	{
		K::Vector3 N = K::Vector3(-(B.y - A.y), B.x - A.x, 0.0f);
		float C = (N.x*P.z) - (N.y*P.x);
		float p = (-((A.x * B.y - B.x * A.y) * N.x) - ((B.x - A.x) * C))/(((B.x - A.x) * N.y) + ((A.y - B.y) * N.x));
		float q = (C * (A.y - B.y) - N.y * (A.x * B.y - B.x * A.y)) / (N.y * (B.x - A.x) + (A.y - B.y) * (N.x));
		K::Vector3 J = K::Vector3(p, q, 0.0f);
		K::Vector3 AJ = (J - A).normalise();
		K::Vector3 BJ = (J - B).normalise();
		K::Vector3 AB = (A - B).normalise();
		float U = K::Vector3::DotProduct(BJ, AB);
		float V = K::Vector3::DotProduct(AJ, AB);
		if (V >= 0.0f) 
		{
			return A;
		}
		else if (U <= 0.0f) 
		{
			return B;
		}
		else if (U > 0.0f && V < 0.0f)
		{
			return J;
		}
	}

	K::Vector3* Collider::GetNormal(K::Vector3 A, K::Vector3 B) 
	{
		K::Vector3 N = K::Vector3(-(B.y - A.y), 0.0f, B.x - A.x);
		N.normalise();
		return &N;
	}

	K::Line* Collider::GetLine(int index) 
	{
		return &this->linePointsModelMatrix[index];
	}

	K::Vector3* Collider::GetOffset() 
	{
		return &this->offset;
	}

	K::Vector3 Collider::GetPosition() 
	{
		K::Vector3 pos = K::Vector3(this->parent->GetTransform()->position->x, 0.0f, this->parent->GetTransform()->position->z);
		if (this->parent->GetTransform()->scale->x > 0.0f) 
		{
			pos += K::Vector3(this->offset.x, 0.0f, this->offset.z);
		}
		else 
		{
			pos += K::Vector3(-this->offset.x, 0.0f, this->offset.z);
		}
		return pos;
	}

	int Collider::GetNumberOfPoints() 
	{
		return this->linePoints.size();
	}

	void Collider::SetIsColliding(bool value) 
	{
		this->isColliding = value;
	}

	bool Collider::IsColliding() 
	{
		return this->isColliding;
	}

	bool Collider::IsStatic() 
	{
		return this->isStatic;
	}

	void Collider::LineCollider() 
	{
		
	}

	void Collider::Update()
	{
		if (this->isStatic)
		{
			switch (this->colliderType)
			{
			case ColliderType::Circle:
				CircleColliderStatic();
				break;
			case ColliderType::Line:
				LineColliderStatic();
				break;
			case ColliderType::Capsule:
				CapsuleColliderStatic();
				break;
			}
		}
		else
		{
			switch (this->colliderType)
			{
			case ColliderType::Circle:
				CircleCollider();
				break;
			case ColliderType::Line:
				LineCollider();
				break;
			case ColliderType::Capsule:
				CapsuleCollider();
				break;
			}
		}
	}

	void Collider::Unbind()
	{
		
	}

	void Collider::Bind()
	{
		
	}

	int pointNumber = 0;

	void Collider::SetPoints(const char* value)
	{
		float temp = std::stof(value);
		pointNumber++;
		if (pointNumber % 2 == 0) 
		{
			//EVEN
			if (pointNumber % 4 == 0) 
			{
				this->linePoints[this->selectedLine].point[1].y = temp;
			}
			else 
			{
				this->linePoints[this->selectedLine].point[0].y = temp;
			}
		}
		else 
		{
			//ODD
			if ((pointNumber - 1) % 4 == 0) 
			{
				this->linePoints.push_back(K::Line(K::Vector3(), K::Vector3()));
				this->linePointsModelMatrix.push_back(K::Line(K::Vector3(), K::Vector3()));
				this->selectedLine = this->linePoints.size() - 1;
				//Set Value
				this->linePoints[this->selectedLine].point[0].x = temp;
			}
			else 
			{
				this->linePoints[this->selectedLine].point[1].x = temp;
			}
		}
	}

	void Collider::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				if (temp == "true")
				{
					this->SetStaticState(true);
				}
				else if (temp == "false")
				{
					this->SetStaticState(false);
				}
				break;
			case 1:
				if (temp == std::to_string((int)ColliderType::Circle))
				{
					this->colliderType = ColliderType::Circle;
				}
				else if (temp == std::to_string((int)ColliderType::Line))
				{
					this->colliderType = ColliderType::Line;
				}
				else if (temp == std::to_string((int)ColliderType::Capsule)) 
				{
					this->colliderType = ColliderType::Capsule;
				}
				break;
			case 2:
				if (this->colliderType == ColliderType::Circle || this->colliderType == ColliderType::Capsule)
				{
					this->radius = std::stof(temp);
				}
				break;
			case 3:
				if (this->colliderType == ColliderType::Circle || this->colliderType == ColliderType::Capsule)
				{
					this->offset.x = std::stof(temp);
				}
				break;
			case 4:
				if (this->colliderType == ColliderType::Circle || this->colliderType == ColliderType::Capsule)
				{
					this->offset.y = std::stof(temp);
				}
				break;
			case 5:
				if (this->colliderType == ColliderType::Circle || this->colliderType == ColliderType::Capsule)
				{
					this->offset.z = std::stof(temp);
				}
				break;
			case 6:
				if (this->colliderType == ColliderType::Capsule) 
				{
					this->height = std::stof(temp);
				}
			}

			if (valueIndex > 1 && this->colliderType == ColliderType::Line)
			{
				this->SetPoints(temp.c_str());
			}
		}
	}

	void Collider::SetStaticState(bool state) 
	{
		this->isStatic = state;
	}

	const char* Collider::GetPropertyValues() 
	{
		std::string temp;
		if (this->isStatic)
		{
			temp = "true";
		}
		else
		{
			temp = "false";
		}
		this->properties = temp;
		this->properties += "," + std::to_string((int)this->colliderType);
		if (this->colliderType == ColliderType::Circle)
		{
			this->properties += "," + std::to_string(this->radius);
			this->properties += "," + std::to_string(this->offset.x);
			this->properties += "," + std::to_string(this->offset.y);
			this->properties += "," + std::to_string(this->offset.z);
		}
		else if (this->colliderType == ColliderType::Line) 
		{
			for (K::Line l : this->linePoints)
			{
				this->properties += "," + std::to_string(l.point[0].x);
				this->properties += "," + std::to_string(l.point[0].y);
				this->properties += "," + std::to_string(l.point[1].x);
				this->properties += "," + std::to_string(l.point[1].y);
			}
		}
		else if (this->colliderType == ColliderType::Capsule) 
		{
			this->properties += "," + std::to_string(this->radius);
			this->properties += "," + std::to_string(this->offset.x);
			this->properties += "," + std::to_string(this->offset.y);
			this->properties += "," + std::to_string(this->offset.z);
			this->properties += "," + std::to_string(this->height);
		}
		return this->properties.c_str();
	}
}