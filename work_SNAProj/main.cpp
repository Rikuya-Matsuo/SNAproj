#include "SDL/SDL.h"
#include "System.h"
#include "GameScene.h"
#include "TitleScene.h"

int main(int argc, char ** argv)
{
	bool successInit = System::GetInstance().Init();

	System::GetInstance().SetScene(new TitleScene);

	if (successInit)
	{
		System::GetInstance().Run();
	}

	System::GetInstance().Finish();

	return 0;
}
