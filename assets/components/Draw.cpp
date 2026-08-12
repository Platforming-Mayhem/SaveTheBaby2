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
		K::Vector3 v0 = A - C;
		K::Vector3 v1 = B - A;
		K::Vector3 v2 = P - A;

		v0.y = 0.0f;
		v1.y = 0.0f;
		v2.y = 0.0f;

		float dot00 = K::Vector3::DotProduct(v0, v0);
		float dot01 = K::Vector3::DotProduct(v0, v1);
		float dot02 = K::Vector3::DotProduct(v0, v2);
		float dot11 = K::Vector3::DotProduct(v1, v1);
		float dot12 = K::Vector3::DotProduct(v1, v2);

		float denom = dot00 * dot11 - dot01 * dot01;
		if(std::abs(denom) < 1e-20)
			return true;
		float invDenom = 1.0f / denom;
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
		return (u >= 0) && (v >= 0) && (u + v < 1);
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

		if (angle <= std::numbers::pi)
		{
			for (int i = 0; i < currentLine.points.size(); i++)
			{
				if (i != previousIndex && i != currentIndex && i != nextIndex && IsCollidingWithTriangle(currentLine.points[i].position, A, B, C))
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

	bool K::LineArt::ProcessNode(int index, K::Mesh* mesh)
	{
		int currentIndex = index;
		int previousIndex = index - 1;
		int nextIndex = index + 1;

		if (previousIndex < 0)
		{
			previousIndex = currentLine.points.size() - 1;
		}

		if (nextIndex >= currentLine.points.size())
		{
			nextIndex = 0;
		}

		std::cout << "Currently processing:" << currentIndex << std::endl;

		while (currentLine.points[previousIndex].indexAdded || previousIndex == currentIndex || previousIndex == nextIndex)
		{
			if (previousIndex <= 0)
			{
				previousIndex = currentLine.points.size() - 1;
			}
			else
			{
				previousIndex--;
			}
			if (previousIndex == index - 1)
			{
				return false;
			}
		}

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
			currentLine.points[currentIndex].indexAdded = true;
			return true;
		}
		else
		{
			return false;
		}
	}

	std::vector<int> K::LineArt::Triangulate(std::vector<int> points, K::Mesh *mesh)
	{
		std::vector<int> triangles;

		std::vector<int> initialPoints = points;
		if (points.size() < 3) // let's make sure that the user don't feed the function with less than 3 points !
			return triangles;
		else
		{
			bool impossibleToTriangulate = false;
			bool triangleFound = true;

			while (points.size() != 0) // run the algorithm until our polygon is empty
			{
				if (!triangleFound) // if we've looped once without finding any ear, the program is stuck, the polygon is not triangulable for our algorithm (likely to be a 8 shape or such self intersecting polygon)
					return triangles;

				triangleFound = false; // we want to find a new ear at each loop

				for (int i(0); i < points.size() - 2; i++) // for each 3 consecutive points we check if it's an ear : an ear is a triangle that wind in the right direction and that do not contain any other point of the polygon
				{
					if (!triangleFound) // if we still didn't find an ear
					{
						bool result = IsEar(points[i + 1], points[i + 2], points[i]);

						if (result) // now, we have found an ear :
						{
							triangleFound = true;

							triangles.push_back(points[i]); // so we add our 3 vec2f to the triangle array : it's one of our triangles !
							triangles.push_back(points[i + 1]);
							triangles.push_back(points[i + 2]);

							int offset = 0;

							for (auto line : this->lineArtLines)
							{
								offset += line.points.size();
							}

							mesh->indices.push_back(points[i] + offset); // so we add our 3 vec2f to the triangle array : it's one of our triangles !
							mesh->indices.push_back(points[i + 1] + offset);
							mesh->indices.push_back(points[i + 2] + offset);

							std::vector<int> bufferArray;
							for (int j(0); j < points.size(); j++) // then we delete the triangle in the points array : we already know that it's an ear, we don't need it anymore
							{
								if (j != i + 1) // we copiy all the points in a buffer array except the point we don't want
								{
									bufferArray.push_back(points[j]);
								}
							}
							points = bufferArray;
						}
					}
				}
			}
		}
		return triangles; // we return the triangle array
	}

	std::vector<int> K::LineArt::ProcessNodes(std::vector<int> nodes, K::Mesh *mesh)
	{
		for(int index = 0; index < nodes.size(); index++)
		{
			int previousIndex = nodes[index - 1];
			int currentIndex = nodes[index];
			int nextIndex = nodes[index + 1];

			if (index + 1 >= nodes.size() - 1)
				nextIndex = nodes[0];

			if (index - 1 < 0)
				previousIndex = nodes[nodes.size() - 1];

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
				currentLine.points[currentIndex].indexAdded = true;
				nodes.erase(nodes.begin() + index);
			}
		}
		return nodes;
	}

	void K::LineArt::Render(K::Mesh* mesh)
	{
		int initIndices = mesh->indices.size();
		int initVertices = mesh->vertices.size();

		std::vector<int> nonEars;

		for(int i = 0; i < currentLine.points.size(); i++)
		{
			if(!currentLine.points[i].vertexAdded)
			{
				K::Vertex vert = K::Vertex(currentLine.points[i].position, K::Vector2(0.0f, 0.0f));
				mesh->vertices.push_back(vert);
				currentLine.points[i].vertexAdded = true;
			}
			if (!currentLine.points[i].indexAdded)
			{
				nonEars.push_back(i);
				currentLine.points[i].indexAdded = true;
			}
		}

		std::cout << "starting non ears left:" << nonEars.size() << std::endl;

		nonEars = Triangulate(nonEars, mesh);

		std::cout << "final non ears left:" << nonEars.size() << std::endl;

		if(initIndices != mesh->indices.size() || initVertices != mesh->vertices.size())
		{
			for (int index : mesh->indices)
			{
				std::cout << index << std::endl;
				std::cout << mesh->vertices[index].position.x << "," << mesh->vertices[index].position.y << "," << mesh->vertices[index].position.z << std::endl;
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
		PolygonTest01();
		//DrawPolygon(7, 1.0f);
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
				this->lineart.Render(this->mesh);
				this->lineart.AddLineArtLine(currentLine);
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