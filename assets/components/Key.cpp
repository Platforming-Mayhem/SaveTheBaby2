#include "Key.h"

namespace K
{
	REGISTER(Key);

	Key::Key()
	{

	}

	Key::~Key()
	{
		this->col = nullptr;
	}

	void Key::Init()
	{
		this->col = (K::Collider*)this->parent->GetComponentOfType(GetTypeName<K::Collider>());
		for (auto gIndex : this->gIndices) 
		{
			K::GameObject* temp = K::Editor::GetCurrentScene()->GetGameObjects().at(gIndex);
			K::Lock* lock = (K::Lock*)temp->GetComponentOfType(GetTypeName<K::Lock>());
			this->locks.insert({ lock, temp });
			this->affectedLocks.insert({lock, true});
		}
	}

	void Key::Update()
	{
		if (this->col != nullptr && !this->locks.empty())
		{
			if (this->col->IsHittingWall() || this->col->IsColliding())
			{
				for (auto lock : this->locks) 
				{
					lock.first->SetKey(true);
					K::Editor::Delete(this->parent);
				}
			}
		}
	}

	void Key::UpdateEditor()
	{
		if (ImGui::CollapsingHeader("Key Settings"))
		{
			if (ImGui::Button("Get all active locks")) 
			{
				for (auto gameObject : K::Editor::GetCurrentScene()->GetGameObjects()) 
				{
					K::Lock* lock = (K::Lock*)gameObject.second->GetComponentOfType(GetTypeName<K::Lock>());
					if (lock != nullptr)
					{
						locks.insert({ lock, gameObject.second });
						affectedLocks.insert({ lock, false });
					}
				}
			}
			if (!locks.empty()) 
			{
				if (ImGui::BeginListBox("Locks"))
				{
					for (auto lock : locks)
					{
						ImGui::Checkbox(lock.second->GetName(), (bool*)(& affectedLocks.at(lock.first)));
					}
					ImGui::EndListBox();
				}
			}
		}
	}

	void Key::SetPropertyValues(const char* value, int valueIndex)
	{
		if (value[0] != '\0' && value != nullptr)
		{
			std::string temp = value;
			this->gIndices.push_back(std::stoi(temp));
		}
	}

	const char* Key::GetPropertyValues()
	{
		this->properties.clear();
		for (auto affectedLock : this->affectedLocks) 
		{
			if ((bool)affectedLock.second) 
			{
				this->properties += std::to_string(locks.at(affectedLock.first)->GetIndex()) + ",";
			}
		}
		this->properties.erase(this->properties.size() - 1);
		return this->properties.c_str();
	}
}