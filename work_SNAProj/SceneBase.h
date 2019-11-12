#pragma once
#include <vector>

// �O���錾
class Actor;

class SceneBase
{
public:
	SceneBase();
	virtual ~SceneBase();

	virtual void Update() = 0;

	void Draw();

protected:
	std::vector<Actor *> mActors;
};
