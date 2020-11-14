#include "SDL/SDL.h"
#include "System.h"
#include "TitleScene.h"

int main(int argc, char ** argv)
{
	// システムの初期化
	bool successInit = System::GetInstance().Init();

	// 最初のシーンをタイトルにセット
	System::GetInstance().SetScene(new TitleScene);

	// 初期化が成功した場合システム駆動
	if (successInit)
	{
		System::GetInstance().Run();
	}

	// 後片付け
	System::GetInstance().Finish();

	return 0;
}
