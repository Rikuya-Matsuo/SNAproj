#include "TitleScene.h"
#include "TitleBoard.h"
#include "Camera.h"

TitleScene::TitleScene()
{
	TitleBoard * tb = new TitleBoard;
	tb->SetScale(80.0f);
	tb->SetPosition(Vector3D::zero);

	Camera * cam = new Camera(tb);
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), tb->GetPosition(), Vector3D(0.0f, 0.0f, 1.0f));
}


TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
}
