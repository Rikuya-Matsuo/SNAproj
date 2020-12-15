#include "System.h"
#include "Input.h"
#include "PhysicManager.h"
#include "Common.h"
#include "Player.h"
#include "SceneBase.h"
#include "Renderer.h"
#include "Camera.h"
#include "UIScreen.h"

#include <cstdlib>
#include <ctime>
#include <algorithm>

#ifdef DEBUG_SNA
Uint64 System::mFrameCount = 0;
#endif // DEBUG_SNA

const float System::mMaxDeltaTime = 1.0f / 30.0f;

System::System():
	mCurrentScene(nullptr),
	mRenderer(nullptr),
	mWindowWidth(1920),
	mWindowHeight(1080),
	mSortActorFlag(false)
{
}

System::~System()
{
}

bool System::Init()
{
	SDL_Log("Start to initialize.\n");

	// SDLの初期化
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		SDL_Log("Failed to initialize SDL.\n--%s--\n", SDL_GetError());
		return false;
	}

	// 画像関係ライブラリの初期化
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Failed to initialize SDL_image.\n--%s--\n", SDL_GetError());
		return false;
	}

	// 前フレームの時間を初期化
	mPrevTicksCount = SDL_GetTicks();

	SDL_Log("Success to initialize SDL\n");

	// レンダラー生成
	mRenderer = new Renderer;

	// リリース版でのみフルスクリーン
	bool fullScreen;
#ifdef DEBUG_SNA
	fullScreen = false;
#else
	fullScreen = true;
#endif

	// レンダラー初期化
	if (!mRenderer->Initialize(mWindowWidth, mWindowHeight, fullScreen))
	{
		SDL_Log("Failed to initialize the renderer.\n");
	}

	// 乱数種設定
	unsigned int seed = static_cast<unsigned int>(time(nullptr));
	srand(seed);

	// 本当ならここで何回か（５回ほど）rand()を空回しするとよいそうだが、
	// 乱数予測が重要なゲームではないことを鑑みて割愛。

	return true;
}

void System::Run()
{
	SDL_Log("System start running...\n");

	// ゲームループ
	bool quitFlag = false;
	while (!quitFlag)
	{
		// デルタタイムの計算
		UpdateDeltaTime();

		// アクターのソートを要請された際は、ソートを行う
		if (mSortActorFlag)
		{
			SortActor();
			mSortActorFlag = false;
		}

		// 入力情報の取得
		Input::GetInstance().Update();

#ifdef DEBUG_SNA
		// デバッグ時のみ、任意のタイミングでブレークポイントに突入するための記述
		if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_TAB))
		{
			// ブレークポイント設置
			SDL_Delay(0);
		}
#endif // DEBUG_SNA

		// シーンの更新
		UpdateScene();

		// アクターの更新
		UpdateActor();

		// アクターに対する重力の適用
		GravityFall();

		// 当たり判定
		PhysicManager::GetInstance().CheckHit();

		// 各アクターに対し、当たり判定による押し返しの適用
		FixActorPosition();

		// UI更新
		UpdateTopUI();

		// カメラ更新
		if (mActiveCamera != nullptr)
		{
			mActiveCamera->Update();

			mRenderer->SetCamera(mActiveCamera);
			Matrix4 viewMatrix = mActiveCamera->GetViewMatrix();
			mRenderer->SetViewMatrix(viewMatrix);
		}

		// シーン遷移処理
		if (mCurrentScene->GetSceneChangeFlag())
		{
			ChangeScene(quitFlag);
		}

		// ゲーム終了処理
		if (Input::GetInstance().GetQuitEventFlag() || Input::GetInstance().GetKey(SDL_SCANCODE_ESCAPE))
		{
			quitFlag = true;
		}

		// 描画
		Draw();

		// このフレームにおける入力状態を保存
		Input::GetInstance().LastUpdate();

#ifdef DEBUG_SNA
		mFrameCount++;
#endif // DEBUG_SNA
	}

	SDL_Log("System quit running\n");
}

void System::Finish()
{
	// アクターコンテナをクリア
	Common::DeleteContainerOfPointer(mActorList);
	std::list<Actor *>().swap(mActorList);

	// シーンの削除
	if (mCurrentScene != nullptr)
	{
		delete mCurrentScene;
	}

	// レンダラー削除
	if (mRenderer != nullptr)
	{
		mRenderer->Shutdown();
		delete mRenderer;
	}

	// 画像関係ライブラリとSDLの終了
	IMG_Quit();
	SDL_Quit();

	SDL_Log("System finish to shut down");
}

//#define SHOW_DELTA_TIME
//#define SHOW_GAME_TIME

void System::UpdateDeltaTime()
{
	// 30FPSで固定する
	mDeltaTime = 0.0f;
	Uint32 ticksCount = SDL_GetTicks();
	while (mDeltaTime < mMaxDeltaTime)
	{
		ticksCount = SDL_GetTicks();
		mDeltaTime = (ticksCount - mPrevTicksCount) / 1000.0f;
	}
	mPrevTicksCount = ticksCount;
	mDeltaTime = mMaxDeltaTime;

#ifdef SHOW_DELTA_TIME
	// 調整前のデルタタイム表示
	SDL_Log("%lf", mDeltaTime);
#endif

	// 値が大きすぎれば調節する
	if (mDeltaTime > mMaxDeltaTime)
	{
		mDeltaTime = mMaxDeltaTime;
		SDL_Log("Delta time over\n");
	}
#ifdef SHOW_GAME_TIME
	static float timer = 0.0f;
	timer += mDeltaTime;
	SDL_Log("%lf", timer);
#endif
}

void System::SortActor()
{
	// 更新優先順が昇順となるようにソート
	mActorList.sort([](const Actor * lhs, const Actor * rhs) { return lhs->GetPriority() <= rhs->GetPriority(); });
}

void System::FixActorPosition()
{
	// 各アクターの当たり判定押し返しを適用
	for (auto actor : mActorList)
	{
		actor->FixPosition();
	}
}

void System::UpdateScene()
{
	// シーンがnullでない限り、シーン更新
	if (mCurrentScene != nullptr)
	{
		mCurrentScene->Update();
	}
}

void System::UpdateActor()
{
	// アクター更新
	for (auto actor : mActorList)
	{
		actor->Update();
	}
}

void System::UpdateTopUI()
{
	// 最前面のUIのみ更新
	if (!mActiveUIList.empty())
	{
		mActiveUIList.back()->Update();
	}
}

void System::Draw()
{
	// 背景色設定
	glClearColor(0.2f, 0.6f, 0.9f, 1.0f);

	// 裏画面をクリア
	mRenderer->WindowClear();

	// 裏画面に描画
	mRenderer->Draw();

	// 裏画面を表画面に表示
	mRenderer->WindowFlip();
}

void System::ChangeScene(bool & quitFlag)
{
	// シーンが変更された際、接触情報をクリアする
	PhysicManager::GetInstance().ClearHitState();

	// 次のシーンを取得
	SceneBase * nextScene = mCurrentScene->GetNextScene();

	// 次のシーンが設定されている場合
	if (nextScene)
	{
		// 現在のシーンを削除し、次のシーンを現在のシーンに設定する
		delete mCurrentScene;
		mCurrentScene = nextScene;

		// 前のシーンに所属するアクターを削除する
		// ただし、シーンを飛び越えるフラグが真のものは削除しない
		auto itr = mActorList.begin();
		for (; itr != mActorList.end(); ++itr)
		{
			if ((*itr)->GetBelongScene() == mCurrentScene)
			{
				continue;
			}

			if (!(*itr)->GetBeyondSceneFlag())
			{
				delete *itr;

				itr--;
			}
		}

		// シーンを飛び越えてきたアクターは、シーン跳躍時の関数を呼ぶ
		for (itr = mActorList.begin(); itr != mActorList.end(); ++itr)
		{
			if ((*itr)->GetBeyondSceneFlag())
			{
				// シーンを飛び越えるフラグを偽にする
				(*itr)->SetBeyondSceneFlag(false);
				(*itr)->OnBeyondScene();
			}
		}
	}
	// 次のシーンが設定されていない場合
	else
	{
		quitFlag = true;
	}
}

void System::GravityFall()
{
	// 重力適用対象のアクターに、重力を適用
	for (auto actor : mActorList)
	{
		if (actor->GetAffectGravityFlag())
		{
			PhysicManager::GetInstance().GravityAffect(actor);
		}
	}
}

void System::ResisterActor(const Actor * in_act)
{
	// アクターを挿入
	Actor * actor = const_cast<Actor*>(in_act);

	mActorList.emplace_back(const_cast<Actor*>(actor));
}

void System::DeresisterActor(const Actor * in_act)
{
	// アクターを検索
	auto target = std::find(mActorList.begin(), mActorList.end(), const_cast<Actor*>(in_act));
	
	// 検索がヒットすればコンテナから削除
	if (target != mActorList.end())
	{
		mActorList.erase(target);
	}
}

void System::ResisterUIScreen(const UIScreen * in_uiScr)
{
	// UIの登録
	mActiveUIList.emplace_back(const_cast<UIScreen*>(in_uiScr));
}

void System::DeresisterUIScreen(const UIScreen * in_uiScr)
{
	// UIを検索
	auto target = std::find(mActiveUIList.begin(), mActiveUIList.end(), const_cast<UIScreen*>(in_uiScr));

	// 検索がヒットすればコンテナから削除
	if (target != mActiveUIList.end())
	{
		mActiveUIList.erase(target);
	}
}

void System::ReportCameraDelete(const Camera * cam)
{
	// 現在のアクティブカメラをnullに
	if (mActiveCamera == cam)
	{
		mActiveCamera = nullptr;
	}
}
