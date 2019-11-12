#include "SDL/SDL.h"
#include "System.h"

int main(int argc, char ** argv)
{
	bool successInit = System::GetInstance().Init();

	if (successInit)
	{
		System::GetInstance().Run();
	}

	System::GetInstance().Finish();

	return 0;
}
