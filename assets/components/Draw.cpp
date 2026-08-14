#include "Draw.h"

namespace K 
{
	REGISTER(Draw);

	K::LineArtLine currentLine;

	K::LinePoint::LinePoint()
	{

	}

	bool K::LineArt::IsCollidingWithTriangle(K::Vector3 P, K::Vector3 A, K::Vector3 B, K::Vector3 C)
	{
		double s1 = C.z - A.z;
		double s2 = C.x - A.x;
		double s3 = B.z - A.z;
		double s4 = P.z - A.z;

		double w1 = (A.x * s1 + s4 * s2 - P.x * s1) / (s3 * s2 - (B.x - A.x) * s1);
		double w2 = (s4 - w1 * s3) / s1;
		return w1 > 0 && w2 > 0 && (w1 + w2) < 1;
	}

	bool K::LineArt::IsEar(int currentIndex, int nextIndex, int previousIndex)
	{
		K::Vector3 A = currentLine.points[previousIndex].position;
		K::Vector3 B = currentLine.points[currentIndex].position;
		K::Vector3 C = currentLine.points[nextIndex].position;

		K::Vector3 AB = A - B;
		K::Vector3 CB = C - B;

		double dot = AB.x * CB.x + AB.z * CB.z;
		double det = AB.x * CB.z - AB.z * CB.x;
		double angle = std::atan2(det, dot);

		if(angle < 0.0)
		{
			angle = (2.0 * std::numbers::pi) + angle;
		}

		if (angle < std::numbers::pi)
		{
			for (int i = 0; i < currentLine.points.size(); i++)
			{
				if (IsCollidingWithTriangle(currentLine.points[i].position, A, B, C))
				{
					return false;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	K::Vector3 center;

	bool less(int firstIndex, int secondIndex)
	{
		K::Vector3 a = currentLine.points[firstIndex].position;
		K::Vector3 b = currentLine.points[secondIndex].position;
		if (a.x - center.x >= 0 && b.x - center.x < 0)
			return true;
		if (a.x - center.x < 0 && b.x - center.x >= 0)
			return false;
		if (a.x - center.x == 0 && b.x - center.x == 0)
		{
			if (a.z - center.z >= 0 || b.z - center.z >= 0)
				return a.z > b.z;
			return b.z > a.z;
		}

		// compute the cross product of vectors (center -> a) x (center -> b)
		int det = (a.x - center.x) * (b.z - center.z) - (b.x - center.x) * (a.z - center.z);
		if (det < 0)
			return true;
		if (det > 0)
			return false;

		// points a and b are on the same line from the center
		// check which point is closer to the center
		int d1 = (a.x - center.x) * (a.x - center.x) + (a.z - center.z) * (a.z - center.z);
		int d2 = (b.x - center.x) * (b.x - center.x) + (b.z - center.z) * (b.z - center.z);
		return d1 > d2;
	}

	bool more(int firstIndex, int secondIndex)
	{
		return !less(firstIndex, secondIndex);
	}

	std::vector<int> K::LineArt::ProcessNodes(std::vector<int> nodes, K::Mesh* mesh)
	{
		int i = 0;

		/* for(int node : nodes)
		{
			center += currentLine.points[node].position;
		}
		center = center / nodes.size();

		std::sort(nodes.begin(), nodes.end(), more); */
		
		while (nodes.size() > 2)
		{
			int previousIndex = nodes[i - 1];
			int currentIndex = nodes[i];
			int nextIndex = nodes[i + 1];

			if(i - 1 < 0)
				previousIndex = nodes[nodes.size() - 1];

			if(i + 1 >= nodes.size())
				nextIndex = nodes[0];

			//std::cout << "Currently processing:" << currentIndex << std::endl;

			if (IsEar(currentIndex, nextIndex, previousIndex))
			{
				int offset = 0;

				for (auto line : this->lineArtLines)
				{
					offset += line.points.size();
				}

				mesh->indices.push_back(currentIndex + offset);
				mesh->indices.push_back(previousIndex + offset);
				mesh->indices.push_back(nextIndex + offset);
				nodes.erase(nodes.begin() + i);
			}
			if (i < nodes.size())
				i++;
			else
				i = 0;
		}
		return nodes;
	}

	void K::LineArt::Render(K::Mesh* mesh)
	{
		int initIndices = mesh->indices.size();
		int initVertices = mesh->vertices.size();

		mesh->vertices.clear();
		mesh->indices.clear();

		std::vector<int> nodes;

		for(int i = 0; i < currentLine.points.size(); i++)
		{
			K::Vertex vert = K::Vertex(currentLine.points[i].position, K::Vector2(0.0f, 0.0f));
			mesh->vertices.push_back(vert);
			nodes.push_back(i);
		}

		nodes = ProcessNodes(nodes, mesh);

		if(initIndices != mesh->indices.size() || initVertices != mesh->vertices.size())
		{
			for (int index : mesh->indices)
			{
				std::cout << index << std::endl;
				std::cout << mesh->vertices[index].position.x << "," << mesh->vertices[index].position.z << "," << mesh->vertices[index].position.z << std::endl;
			}
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

	void K::LineArt::Clear()
	{
		lineArtLines.clear();
	}

	K::Draw::Draw() 
	{

	}

	K::Draw::~Draw() 
	{
		lineart.Clear();
		currentLine.points.clear();
	}

	void K::Draw::DrawPolygon(int sides, float radius)
	{
		for (int i = 0; i < sides; i++)
		{
			float angle = -2.0f * std::numbers::pi / sides;
			K::LinePoint point = K::LinePoint(K::Vector3(radius * std::cos(i * angle), 0.0f, radius * std::sin(i * angle)), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
			currentLine.points.push_back(point);
		}

		this->lineart.Render(this->mesh);
		this->lineart.AddLineArtLine(currentLine);
	}

	void K::Draw::PolygonTest01()
	{
		K::LinePoint point = K::LinePoint(K::Vector3(1.0f, 0.0f, 1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.5f, 0.0f, 0.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(1.0f, 0.0f, -1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.0f, 0.0f, -1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(-0.5f, 0.0f, 0.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.0f, 0.0f, 1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		this->lineart.Render(this->mesh);
		this->lineart.AddLineArtLine(currentLine);
	}

	void K::Draw::PolygonTest02()
	{
		K::LinePoint point = K::LinePoint(K::Vector3(0.5f, 0.0f, 0.6f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.1f, 0.0f, 0.7f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(-0.1f, 0.0f, 0.3f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.3f, 0.0f, 0.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.8f, 0.0f, 0.4f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.5f, 0.0f, -0.5f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(-0.6f, 0.0f, -0.3f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(-0.8f, 0.0f, 0.5f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(0.0f, 0.0f, 1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		this->lineart.Render(this->mesh);
		this->lineart.AddLineArtLine(currentLine);
	}

	void K::Draw::PolygonTest03()
	{
		K::LinePoint point = K::LinePoint(K::Vector3(1.0f, 0.0f, 1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(-1.0f, 0.0f, -1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(-1.0f, 0.0f, 1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		point = K::LinePoint(K::Vector3(1.0f, 0.0f, -1.0f), K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
		currentLine.points.push_back(point);

		this->lineart.Render(this->mesh);
		this->lineart.AddLineArtLine(currentLine);
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
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		//PolygonTest03();
		//DrawPolygon(32, 1.0f);
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
				this->drawingLine = true;
			}
			else if(K::InputManager::IsMouseKeyReleased(0) && this->drawingLine)
			{
				this->drawingLine = false;

				K::Vector3 worldSpaceDirection = K::InputManager::GetWorldMouseDirection();
				K::Vector3 camPosition = *K::Editor::cameraPosition;
				K::Vector3 forward = K::Vector3(0.0f, 1.0f, 0.0f);
				float t = -K::Vector3::DotProduct(forward, camPosition) / K::Vector3::DotProduct(forward, worldSpaceDirection);

				K::Vector3 P = (camPosition + (worldSpaceDirection * t));

				K::LinePoint point = K::LinePoint(P, K::Colour(0.0f, 0.0f, 0.0f), 1.0f);
				currentLine.points.push_back(point);

				this->lineart.Render(this->mesh);
			}
		}
	}

	void K::Draw::UpdateEditor() 
	{
		ImGui::Begin("K-Engine Content Browser");
		ImGui::BeginChild("Draw Settings");
		ImGui::Checkbox("Drawing Overlay", &this->drawingOverlay);
		ImGui::Text("%i", currentLine.points.size());
		ImGui::Text("%i", this->lineart.NumberOfLines());
		ImGui::EndChild();
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