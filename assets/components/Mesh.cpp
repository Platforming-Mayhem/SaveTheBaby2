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
		//std::cout << "Begin Mesh Destruction..." << std::endl;
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

	bool Mesh::LoadModelsAssimp(std::string file)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(ASSET_DIR + file, aiProcess_Triangulate);
		if (scene->HasMeshes())
		{
			vertices.clear();
			indices.clear();

			unsigned int offset_faces = 0;

			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[i];

				for (unsigned int k = 0; k < mesh->mNumFaces; k++)
				{
					aiFace face = mesh->mFaces[k];
					for (unsigned int l = 0; l < face.mNumIndices; l++)
						indices.push_back(face.mIndices[l] + offset_faces);
				}

				for (unsigned int j = 0; j < mesh->mNumVertices; j++)
				{
					Vector3 vertex;
					vertex.x = mesh->mVertices[j].x;
					vertex.y = mesh->mVertices[j].y;
					vertex.z = mesh->mVertices[j].z;
					Vector3 normal;
					if (mesh->HasNormals()) 
					{
						normal.x = mesh->mNormals[j].x;
						normal.y = mesh->mNormals[j].y;
						normal.z = mesh->mNormals[j].z;
					}
					Vector2 texCoord;
					if (mesh->HasTextureCoords(0)) 
					{
						texCoord.x = mesh->mTextureCoords[0][j].x;
						texCoord.y = mesh->mTextureCoords[0][j].y;
					}
					K::Vertex vert(vertex, texCoord, normal);

					vertices.push_back(vert);
				}

				offset_faces += mesh->mNumVertices;
			}
		}
		return scene->HasMeshes();
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

		glUniform3f(this->parent->GetMaterial()->GetShader()->GetUniform("colorTint"), this->colourTint[0], this->colourTint[1], this->colourTint[2]);
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

	void Mesh::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Mesh Renderer Settings")) 
		{
			ImGui::Text("Vertices: %i", this->vertices.size());
			ImGui::Text("Indices: %i", this->indices.size());
			ImGui::Checkbox("Can Depth", &this->canDepth);
			ImGui::ColorPicker3("Colour Tint", this->colourTint);

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
					this->LoadModelsAssimp(this->mesh);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_DYNAMIC_DRAW);

					glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
					glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(K::Vertex), &this->vertices[0], GL_DYNAMIC_DRAW);
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
		return K::Colour(this->colourTint[0], this->colourTint[1], this->colourTint[2]);
	}

	void Mesh::SetColourTint(float r, float g, float b) 
	{
		this->colourTint[0] = r;
		this->colourTint[1] = g;
		this->colourTint[2] = b;
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
				this->colourTint[0] = std::stof(temp);
				break;
			case 1:
				this->colourTint[1] = std::stof(temp);
				break;
			case 2:
				this->colourTint[2] = std::stof(temp);
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
					this->LoadModelsAssimp(this->mesh);
				}
				break;
			}
		}
	}

	const char* Mesh::GetPropertyValues()
	{
		this->properties = std::to_string(this->colourTint[0]) + ",";
		this->properties += std::to_string(this->colourTint[1]) + ",";
		this->properties += std::to_string(this->colourTint[2]) + ",";
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