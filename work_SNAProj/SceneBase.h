#pragma once
#include <vector>

// 前方宣言
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
