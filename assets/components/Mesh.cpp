#include "Mesh.h"

namespace K 
{
	REGISTER(Mesh);

	Mesh::Mesh() 
	{
		this->vertices = K::Quad;

		this->indices = { 0, 3, 1, 0, 2, 3 };
	}

	Mesh::~Mesh() 
	{
		std::cout << "Begin Mesh Destruction..." << std::endl;
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteBuffers(1, &this->VBO);
		glDeleteBuffers(1, &this->EBO);
		this->vertices.clear();
		this->indices.clear();
		//std::cout << "End Mesh Destruction..." << std::endl;
	}

	void Mesh::SetShader(std::string newShader) 
	{
		this->shader = newShader;
	}

	bool Mesh::LoadOBJModel(std::string file)
	{
		std::ifstream newMesh(ASSET_DIR + file);
		int index = 0;
		std::vector<K::Vector3> positions;
		std::vector<K::Vector3> normals;
		std::vector<K::Vector2> uvs;
		if (!newMesh.is_open())
			return false;
		else
			vertices.clear();
			indices.clear();
		
		for (std::string currentLine; std::getline(newMesh, currentLine); )
		{
			if (currentLine.find("v ") != std::string::npos)
			{
				currentLine.erase(currentLine.begin(), currentLine.begin() + 2);

				std::size_t xPos = currentLine.find(" ");
				std::string x = currentLine.substr(0, xPos);
				currentLine.erase(currentLine.begin(), currentLine.begin() + x.length() + 1);

				std::size_t yPos = currentLine.find(" ");
				std::string y = currentLine.substr(0, yPos);
				currentLine.erase(currentLine.begin(), currentLine.begin() + y.length() + 1);

				std::size_t zPos = currentLine.find(" ");
				std::string z = currentLine.substr(0, zPos);

				K::Vector3 vertex;

				vertex.x = std::stof(x);
				vertex.y = std::stof(y);
				vertex.z = std::stof(z);

				positions.push_back(vertex);
			}
			else if (currentLine.find("vn ") != std::string::npos)
			{
				currentLine.erase(currentLine.begin(), currentLine.begin() + 3);

				std::size_t xPos = currentLine.find(" ");
				std::string x = currentLine.substr(0, xPos);
				currentLine.erase(currentLine.begin(), currentLine.begin() + x.length() + 1);

				std::size_t yPos = currentLine.find(" ");
				std::string y = currentLine.substr(0, yPos);
				currentLine.erase(currentLine.begin(), currentLine.begin() + y.length() + 1);

				std::size_t zPos = currentLine.find(" ");
				std::string z = currentLine.substr(0, zPos);

				K::Vector3 normal;

				normal.x = std::stof(x);
				normal.y = std::stof(y);
				normal.z = std::stof(z);

				normals.push_back(normal);
			}
			else if (currentLine.find("vt ") != std::string::npos)
			{
				currentLine.erase(currentLine.begin(), currentLine.begin() + 3);

				std::size_t xPos = currentLine.find(" ");
				std::string x = currentLine.substr(0, xPos);
				currentLine.erase(currentLine.begin(), currentLine.begin() + x.length() + 1);

				std::size_t yPos = currentLine.find(" ");
				std::string y = currentLine.substr(0, yPos);

				K::Vector2 uv;

				uv.x = std::stof(x);
				uv.y = std::stof(y);

				uvs.push_back(uv);
			}
			else if (currentLine.find("f ") != std::string::npos)
			{
				currentLine.erase(currentLine.begin(), currentLine.begin() + 2);

				for(int i = 0; i < 3; i++)
				{
					std::size_t vertex = currentLine.find("/");
					std::string vertexVal = currentLine.substr(0, vertex);
					int vertexIndex = std::stoi(vertexVal);
					currentLine.erase(currentLine.begin(), currentLine.begin() + vertexVal.length() + 1);

					std::size_t uvSize = currentLine.find("/");
					std::string uvVal = currentLine.substr(0, uvSize);
					int uvIndex = std::stoi(uvVal);
					currentLine.erase(currentLine.begin(), currentLine.begin() + uvVal.length() + 1);

					std::size_t normalSize;
					int normalIndex;

					if(currentLine.find(" ") != std::string::npos)
					{
						normalSize = currentLine.find(" ");
						std::string normalVal = currentLine.substr(0, normalSize);
						normalIndex = std::stoi(normalVal);
						currentLine.erase(currentLine.begin(), currentLine.begin() + normalVal.length() + 1);
					}
					else
					{
						normalSize = currentLine.length();
						std::string normalVal = currentLine.substr(0, normalSize);
						normalIndex = std::stoi(normalVal);
						currentLine.erase(currentLine.begin(), currentLine.begin() + normalVal.length());
					}

					K::Vector3 position = positions[vertexIndex - 1];
					K::Vector3 normal = normals[normalIndex - 1];
					K::Vector2 uv = uvs[uvIndex - 1];

					K::Vertex vert(position, uv, normal);
					vertices.push_back(vert);
					indices.push_back(index);
					index++;
				}
			}
		}
		return true;
	}

	void Mesh::RenderInit() 
	{
		if (this->shader == "") 
		{
			this->shader = "shaders/2D.shader";

			this->parent->SetMaterial(new K::Material(this->shader));
		}

		glCreateVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(K::Vertex), &this->vertices[0], GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		this->RenderUnbind();
	}

	void Mesh::Init() 
	{
		glEnable(GL_CULL_FACE); 
	}

	void Mesh::Bind() 
	{
		
	}

	void Mesh::Update() 
	{
		
	}

	void Mesh::RenderBind() 
	{
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

		glUniform3f(this->parent->GetMaterial()->GetShader()->GetUniform("colorTint"), this->colourTint.rgb[0], this->colourTint.rgb[1], this->colourTint.rgb[2]);
		glUniform1i(this->parent->GetMaterial()->GetShader()->GetUniform("canDepth"), this->canDepth);
	}

	void Mesh::Render() 
	{
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr);
	}

	void Mesh::RenderUnbind() 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Mesh::ReloadGeometry()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(K::Vertex), &this->vertices[0], GL_DYNAMIC_DRAW);
	}

	void Mesh::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Mesh Renderer Settings")) 
		{
			ImGui::Text("Vertices: %i", this->vertices.size());
			ImGui::Text("Indices: %i", this->indices.size());
			ImGui::Checkbox("Can Depth", &this->canDepth);
			ImGui::ColorPicker3("Colour Tint", &this->colourTint.rgb[0]);

			if (this->mesh.empty()) 
			{
				ImGui::Text("No Mesh Data -- Using Default Quad");
			}
			else 
			{
				ImGui::Text(this->mesh.c_str());
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MODEL"))
				{
					const char* file = (const char*)payload->Data;
					this->mesh = file;
					this->LoadOBJModel(this->mesh);
					this->ReloadGeometry();
				}
				ImGui::EndDragDropTarget();
			}
		}
		if (ImGui::CollapsingHeader("Material Settings")) 
		{
			ImGui::InputText("Shader Location: ", &this->shader);
			if (ImGui::Button("Recompile Shader")) 
			{
				this->parent->SetMaterial(new K::Material(this->shader));
			}
		}
	}

	K::Colour Mesh::GetColourTint()
	{
		return this->colourTint;
	}

	void Mesh::SetColourTint(float r, float g, float b) 
	{
		this->colourTint.rgb[0] = r;
		this->colourTint.rgb[1] = g;
		this->colourTint.rgb[2] = b;
	}

	void Mesh::Unbind() 
	{
		
	}

	void Mesh::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			switch (valueIndex)
			{
			case 0:
				this->colourTint.rgb[0] = std::stof(temp);
				break;
			case 1:
				this->colourTint.rgb[1] = std::stof(temp);
				break;
			case 2:
				this->colourTint.rgb[2] = std::stof(temp);
				break;
			case 3:
				if (temp == "true") 
				{
					this->canDepth = true;
				}
				else if(temp == "false")
				{
					this->canDepth = false;
				}
				break;
			case 4:
				if (temp != "") 
				{
					this->shader = temp;
					this->parent->SetMaterial(new K::Material(this->shader));
				}
				break;
			case 5:
				if (temp != "")
				{
					this->mesh = temp;
					this->LoadOBJModel(this->mesh);
				}
				break;
			}
		}
	}

	const char* Mesh::GetPropertyValues()
	{
		this->properties = std::to_string(this->colourTint.rgb[0]) + ",";
		this->properties += std::to_string(this->colourTint.rgb[1]) + ",";
		this->properties += std::to_string(this->colourTint.rgb[2]) + ",";
		if (this->canDepth) 
		{
			this->properties += "true,";
		}
		else 
		{
			this->properties += "false,";
		}
		this->properties += this->shader + ",";
		this->properties += this->mesh;
		return this->properties.c_str();
	}
}