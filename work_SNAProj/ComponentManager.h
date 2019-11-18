#pragma once
#include <list>
class ComponentBase;

class ComponentManager final
{
public:
	~ComponentManager();

	static ComponentManager& GetInstance()
	{
		static ComponentManager instance;
		return instance;
	}

	void UpdateComponents();

	void ResisterComponent(ComponentBase * in_cmp);

	void DeresisterComponent(ComponentBase * in_cmp);

private:
	ComponentManager();

	std::list<ComponentBase *> mComponentList;
};
