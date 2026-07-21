#include "Draw.h"

namespace K 
{
	REGISTER(Draw);

	K::LineArtLine currentLine;

	K::LinePoint::LinePoint()
	{

	}

	void K::LineArt::Render(K::Mesh* mesh)
	{
		int offset = 0;
		int initIndices = mesh->indices.size();
		int initVertices = mesh->vertices.size();

		//Current Line

		for(auto line : this->lineArtLines)
		{
			offset += line.points.size();
		}

		for(int i = 0; i < currentLine.points.size(); i++)
		{
			if(!currentLine.points[i].rendered)
			{
				K::Vertex vert = K::Vertex(currentLine.points[i].position, K::Vector2(0.0f, 0.0f));
				mesh->vertices.push_back(vert);

				if(i - 1 > 0)
				{
					mesh->indices.push_back(i + offset);

					mesh->indices.push_back(i - 1 + offset);

					mesh->indices.push_back(0 + offset);
				}
				currentLine.points[i].rendered = true;
			}
		}

		if(initIndices != mesh->indices.size() || initVertices != mesh->vertices.size())
		{
			mesh->ReloadGeometry();
		}
	}

	K::LineArt::LineArt()
	{

	}

	K::LineArt::~LineArt()
	{
		
	}

	int K::LineArt::NumberOfLines()
	{
		return this->lineArtLines.size();
	}

	void K::LineArt::AddLineArtLine(LineArtLine lineArtLine)
	{
		lineArtLines.push_back(lineArtLine);
	}

	K::Draw::Draw() 
	{

	}

	K::Draw::~Draw() 
	{
		
	}

	void K::Draw::Init() 
	{
		this->mesh = (K::Mesh*)this->parent->GetComponentOfType(GetTypeName<K::Mesh>());
		if(this->mesh == nullptr)
			std::cout << "Cannot find Mesh" << std::endl;
		else
		{
			this->mesh->vertices.clear();
			this->mesh->indices.clear();
		}
	}

	void K::Draw::Bind() 
	{
		
	}

	void K::Draw::Update() 
	{
		if(this->drawingOverlay)
		{
			if(K::InputManager::IsMouseKeyPressed(0))
			{
				if(!this->drawingLine)
					currentLine.points.clear();
				this->drawingLine = true;

				K::Vector3 worldSpaceDirection = K::InputManager::GetWorldMouseDirection();
				K::Vector3 camPosition = *K::Editor::cameraPosition;

				K::Vector3 P = (camPosition + (worldSpaceDirection * 10.0f));

				K::LinePoint point = K::LinePoint(P, K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
				currentLine.points.push_back(point);
			}
			else if(K::InputManager::IsMouseKeyReleased(0) && this->drawingLine)
			{
				this->drawingLine = false;
				this->lineart.AddLineArtLine(currentLine);
			}
		}
		this->lineart.Render(this->mesh);
	}

	void K::Draw::UpdateEditor() 
	{
		ImGui::Begin("K-Engine Content Browser");
		if (ImGui::BeginChild("Draw Settings")) 
		{
			ImGui::Checkbox("Drawing Overlay", &this->drawingOverlay);
			ImGui::Text("%i", currentLine.points.size());
			ImGui::Text("%i", this->lineart.NumberOfLines());
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void K::Draw::Unbind() 
	{

	}

	void K::Draw::SetPropertyValues(const char* value, int valueIndex) 
	{
		if (value[0] != '\0' && value != nullptr) 
		{
			
		}
	}

	const char* K::Draw::GetPropertyValues() 
	{
		return this->properties.c_str();
	}
}