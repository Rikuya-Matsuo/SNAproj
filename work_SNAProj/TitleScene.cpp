﻿#include "TitleScene.h"
#include "TitleBoard.h"
#include "Camera.h"
#include "Input.h"
#include "TestScene.h"

TitleScene::TitleScene()
{
	TitleBoard * tb = new TitleBoard;
	tb->SetScale(80.0f);
	tb->SetPosition(Vector3D::zero);

	Camera * cam = new Camera(tb);
	cam->Init(Vector3D(0.0f, -50.0f, 0.0f), tb->GetPosition(), Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetDistanceVector(Vector3D(0.0f, 55.0f, 0.0f));
	cam->SetActive();
}

TitleScene::~TitleScene()
{
	while (!mCameras.empty())
	{
		delete mCameras.back();
		mCameras.pop_back();
	}
}

void TitleScene::Update()
{
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_RETURN))
	{
		mNextScene = new TestScene;

		mFlags |= mSceneChangeFlagMask;
	}
}
