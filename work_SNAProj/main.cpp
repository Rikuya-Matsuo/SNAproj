﻿#include "SDL/SDL.h"
#include "System.h"
#include "TestScene.h"
#include "TestScene01.h"

int main(int argc, char ** argv)
{
	bool successInit = System::GetInstance().Init();

	System::GetInstance().SetScene(new TestScene01);

	if (successInit)
	{
		System::GetInstance().Run();
	}

	System::GetInstance().Finish();

	return 0;
}
