#include "GuideUI.h"
#include "Common.h"

GuideUI::GuideUI(const char * texPath):
	SpriteActor(texPath, 100)
{
	SetScale(800.0f);
	SetPosition(Vector3D(0, 0, -650.0f));

	mRotationAxis = Vector3D(1, 0, 0);
	mRotationAngle = Common::DegToRad(20.0f);
}

GuideUI::~GuideUI()
{
}
