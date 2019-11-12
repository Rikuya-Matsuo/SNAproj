#pragma once
#include <vector>

// ‘O•ûéŒ¾
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
