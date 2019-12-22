#include "TestScene01.h"
#include "System.h"
#include "Renderer.h"
#include "Player.h"
#include "SpriteTest.h"
#include "Camera.h"
#include "Input.h"

TestScene01::TestScene01()
{
	SpriteTest * st = new SpriteTest;
	st->SetPosition(Vector3D(0.0f, 0.0f, 0.0f));
	st->SetAffectGravityFlag(false);
	st->SetScale(30.0f);

	Player * pl = new Player;
	pl->SetPosition(Vector3D(0.0f, 0.0f, 50.0f));
	pl->SetAffectGravityFlag(false);
	pl->SetScale(30.0f);

	Camera * cam = new Camera(pl);
	cam->Init(Vector3D(0.0f, 100.0f, 0.0f), pl->GetPosition(), Vector3D(0.0f, 0.0f, 1.0f));
	cam->SetDistanceVector(Vector3D(0.0f, 100.0f, 0.0f));
	cam->SetChaseTargetFlag(false);

	DirectionalLight& dir = System::GetInstance().GetRenderer()->GetDirectionalLight();
	dir.mDirection = Vector3D(0.7f, -0.7f, -0.7f);
	dir.mDiffuseColor = Vector3D(0.7f, 0.7f, 0.7f);
	dir.mSpecColor = Vector3D(0.8f, 0.8f, 0.8f);
}


TestScene01::~TestScene01()
{
}

void TestScene01::Update()
{
	DirectionalLight& dir = System::GetInstance().GetRenderer()->GetDirectionalLight();

	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_F2))
	{
		dir.mDirection = Vector3D(0.7f, -0.7f, -0.7f);
	}

	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_F1))
	{
		dir.mDirection = Vector3D(-0.7f, 0.7f, 0.7f);
	}
}
