#include "Lock.h"

namespace K
{
	REGISTER(Lock);

	Lock::Lock()
	{

	}

	Lock::~Lock()
	{
		this->col = nullptr;
	}

	void Lock::Init()
	{
		this->col = (K::Collider*)this->parent->GetComponentOfType(GetTypeName<K::Collider>());
	}

	void Lock::SetKey(bool found) 
	{
		this->foundKey = found;
	}

	void Lock::Update()
	{
		if (this->col != nullptr) 
		{
			if (this->col->IsHittingWall() || this->col->IsColliding())
			{
				if (this->foundKey) 
				{
					K::Editor::Delete(this->parent);
				}
			}
		}
	}

	void Lock::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Lock Settings"))
		{
			ImGui::Checkbox("Found Key", &this->foundKey);
		}
	}

	void Lock::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			
		}
	}

	const char* Lock::GetPropertyValues()
	{
		return this->properties.c_str();
	}
}