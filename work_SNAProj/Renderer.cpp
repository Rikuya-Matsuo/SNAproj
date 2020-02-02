#include "Renderer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "MeshComponent.h"
#include "Skeleton.h"
#include "SkeletalMeshComponent.h"
#include "Animation.h"
#include "Camera.h"
#include "Common.h"
#include "System.h"

Renderer::Renderer()
	:mWindow(nullptr)
	,mSDLRenderer(nullptr)
	,mContext(0)
	,mMeshShader(nullptr)
	,mSkinnedShader(nullptr)
	,mSpriteShader(nullptr)
	,mFieldOfView(Common::DegToRad(155.0f))
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(int screenWidth, int screenHeight, bool fullScreen)
{
	mScreenWidth  = screenWidth;
	mScreenHeight = screenHeight;

	// OpenGL アトリビュートのセット
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// GL version 3.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// 8Bit RGBA チャンネル
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE  , 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE , 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// 透過色の有効化
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// ダブルバッファリング
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// ハードウェアアクセラレーションを強制
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Windowの作成
	mWindow = SDL_CreateWindow("SDL & GL Window",
		                       0, 0,
		                       mScreenWidth, mScreenHeight, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	if (!mWindow)
	{
		printf("Windowの作成に失敗: %s", SDL_GetError());
		return false;
	}
	if (fullScreen)
	{
		if (SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP))
		{
			printf("(%d, %d) サイズのフルスクリーン化に失敗\n", screenWidth, screenHeight);
			return false;
		}
		glViewport(0, 0, mScreenWidth, mScreenHeight);
	}

	//SDLRendererの作成
	mSDLRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mSDLRenderer)
	{
		printf("SDLRendererの作成に失敗 : %s", SDL_GetError());
		return false;
	}
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		printf("SDLImageInitPNGの初期化に失敗 : %s", SDL_GetError());
		return false;
	}

	// OpenGLContextの作成
	mContext = SDL_GL_CreateContext(mWindow);

	// Glewの初期化
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("GLEWの初期化に失敗");
		return false;
	}
	// 幾つかのプラットホームでは、GLEWが無害なエラーコードを吐くのでクリアしておく
	glGetError();

	// シェーダープログラムの初期化
	if (!LoadShaders())
	{
		printf("シェーダーの初期化に失敗");
		return false;
	}

	// カリング
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	return true;
}

void Renderer::Shutdown()
{
	//テクスチャ破棄
	for (auto i : mTextures)
	{
		printf("Texture Release : %s\n", i.first.c_str());
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// メッシュ破棄
	for (auto i : mMeshs)
	{
		printf("Mesh Release : %s\n", i.first.c_str());
		i.second->Unload();
		delete i.second;
	}
	mMeshs.clear();

	//シェーダー破棄
	mMeshShader->Unload();

	// スケルトンの破棄
	for (auto s : mSkeletons)
	{
		delete s.second;
	}

	// アニメーションの破棄
	for (auto a : mAnims)
	{
		delete a.second;
	}

	// SDL系の破棄
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::Draw()
{
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);

	// 有効視野角のなかにあるかを調べるラムダ式
	auto isInFieldOfView = [this](MeshComponent * mc)
	{
		// 視野角計算
		Vector3D acCamDir = mc->GetOwner()->GetPosition() - mCameraPointer->GetPosition();
		Vector3D camVec = mCameraPointer->GetViewVector();
		acCamDir.z = camVec.z = 0.0f;
		float dot = Vector3D::Dot(acCamDir, camVec);
		// 二乗値を最初にとって、平方根の計算を一回に節約する
		float cos = (dot * dot) / (acCamDir.LengthSq() * camVec.LengthSq());
		cos = sqrtf(cos);
		float sita = static_cast<float>(acos(cos));

		bool inFieldOfView = (sita <= mFieldOfView / 2);

		return inFieldOfView;
	};

	//メッシュシェーダーで描画する対象の変数をセット
	mMeshShader->SetActive();
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
    // ライティング変数をセット
	SetLightUniforms(mMeshShader);
	// 全てのメッシュコンポーネントを描画
	for (auto mc : mMeshComponents)
	{
		bool inFOV = isInFieldOfView(mc);
		if ((inFOV && mc->GetVisible()) || mc->GetRangeOutDrawFlag())
		{
			mc->Draw(mMeshShader);
		}

		mc->GetOwner()->SetInCameraFlag(inFOV);
	}

	// Draw any skinned meshes now
	mSkinnedShader->SetActive();
	// Update view-projection matrix
	mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);
	// Update lighting uniforms
	SetLightUniforms(mSkinnedShader);
	for (auto sk : mSkeletalMeshes)
	{
		bool inFOV = isInFieldOfView(sk);
		if (inFOV && sk->GetVisible())
		{
			sk->Draw(mSkinnedShader);
		}

		sk->GetOwner()->SetInCameraFlag(inFOV);
	}

	mSpriteShader->SetActive();
	mSpriteShader->SetMatrixUniform("uViewProj", mView * mProjection);
	MeshComponent::SetViewMatrix(mCameraPointer->GetPosition());
	SetLightUniforms(mSpriteShader);
	for (auto ui : mUIs)
	{
		ui->Draw(mSpriteShader);
	}

}

Texture* Renderer::GetTexture(const std::string & fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}

	if (tex)
	{
		printf("Load Texture Success : %s \n", fileName.c_str());
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string & fileName, const Actor * actor)
{
	Mesh* m = nullptr;
	auto iter = mMeshs.find(fileName);

	if (iter != mMeshs.end())
	{
		m = iter->second;

		m->DuplicateDefaultTexture(actor);
	}
	else
	{
		m = new Mesh;
		if (m->Load(fileName, this, actor))
		{
			mMeshs.emplace(fileName, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	printf("Load Mesh Success : %s\n", fileName.c_str());
	return m;
}

void Renderer::AddMeshComponent(MeshComponent* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);

		const int drawOrder = sk->GetDrawOrder();
		auto itr = mSkeletalMeshes.begin();
		for (; itr != mSkeletalMeshes.end(); ++itr)
		{
			if (drawOrder < (*itr)->GetDrawOrder())
			{
				mSkeletalMeshes.insert(itr, sk);
				return;
			}
		}

		if (itr == mSkeletalMeshes.end())
		{
			mSkeletalMeshes.emplace_back(sk);
		}
	}
	else
	{
		const int drawOrder = mesh->GetDrawOrder();
		auto itr = mMeshComponents.begin();
		for (; itr != mMeshComponents.end(); ++itr)
		{
			if (drawOrder < (*itr)->GetDrawOrder())
			{
				mMeshComponents.insert(itr, mesh);
				return;
			}
		}

		if (itr == mMeshComponents.end())
		{
			mMeshComponents.emplace_back(mesh);
		}
	}
}

void Renderer::RemoveMeshComponent(MeshComponent* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
		auto iter = std::find(mSkeletalMeshes.begin(), mSkeletalMeshes.end(), sk);
		mSkeletalMeshes.erase(iter);
	}
	else
	{
		auto iter = std::find(mMeshComponents.begin(), mMeshComponents.end(), mesh);
		mMeshComponents.erase(iter);
	}
}

void Renderer::ShowResource()
{
	printf("\n\n<------------------ textures ------------------>\n");
	for (auto i : mTextures)
	{
		printf("texfile %s\n", i.first.c_str());
	}

	printf("\n<------------------  meshes  ------------------->\n");
	for (auto i : mMeshs)
	{
		printf("meshfile %s\n", i.first.c_str());
	}
}

void Renderer::SetWindowTitle(const std::string & title)
{
	SDL_SetWindowTitle(mWindow, title.c_str());
}

void Renderer::AddUI(MeshComponent * mesh)
{
	mUIs.emplace_back(mesh);
}

void Renderer::RemoveUI(MeshComponent * mesh)
{
	auto itr = std::find(mUIs.begin(), mUIs.end(), mesh);
	if (itr != mUIs.end())
	{
		mUIs.erase(itr);
	}
}

bool Renderer::LoadShaders()
{
	// メッシュシェーダー
	mMeshShader = new Shader();
	if (!mMeshShader->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag"))
	{
		return false;
	}

	mMeshShader->SetActive();
	mView       = Matrix4::CreateLookAt(Vector3D::zero, Vector3D(1.0f), Vector3D(0.0f, 0.0f, 1.0f));
	mProjection = Matrix4::CreatePerspectiveFOV(MathExpantion::ToRadians(70.0f),
		                                        static_cast<float>(mScreenWidth),
		                                        static_cast<float>(mScreenHeight),
		                                        1.0f, 10000.0f);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

	mSkinnedShader = new Shader();
	if (!mSkinnedShader->Load("Shaders/Skinned.vert", "Shaders/Phong.frag"))
	{
		return false;
	}
	mSkinnedShader->SetActive();
	mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);

	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}
	mSpriteShader->SetActive();
	mSpriteShader->SetMatrixUniform("uViewProj", mView * mProjection);

	return true;
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// ビュー行列からカメラ位置を逆算出する
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());

	//アンビエントライト
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);

	//ディレクショナルライト
	shader->SetVectorUniform("uDirLight.mDirection", mDirectionalLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirectionalLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor", mDirectionalLight.mSpecColor);
}

//////////////////////////////////////////////////////////////
// スケルタル情報の取得
// in  : スケルタル情報 .gpskelファイル名
// out : Skeleton情報へのポインタ
// Desc: gpskelファイル名より、スケルタル情報を返す。ない場合はそのファイル名で
//       読み込みを行う。読み込みを行ってもファイルが存在しない場合 nullptrを返す
//       内部でgpskelファイル名をキーとするスケルタル情報のmapが作成される
//////////////////////////////////////////////////////////////
const Skeleton* Renderer::GetSkeleton(const char* fileName)
{
	std::string file(fileName);
	auto iter = mSkeletons.find(file);
	if (iter != mSkeletons.end())
	{
		return iter->second;
	}
	else
	{
		Skeleton* sk = new Skeleton();
		if (sk->Load(file))
		{
			mSkeletons.emplace(file, sk);
		}
		else
		{
			delete sk;
			sk = nullptr;
		}
		return sk;
	}
}

//////////////////////////////////////////////////////////////
// アニメーション情報の取得
// in  : アニメーションデータが格納されている .gpanimファイル名
// out : アニメーション情報へのポインタ
// Desc: gpanimファイル名よりアニメーションデータを返す。ない場合はそのファイル名で
//       読み込みを行う。読み込みを行ってもファイルが存在しない場合 nullptrを返す
//       内部でgpanimファイル名をキーとするアニメーション情報のmapが作成される
//////////////////////////////////////////////////////////////
const Animation* Renderer::GetAnimation(const char* fileName, bool loop)
{
	auto iter = mAnims.find(fileName);
	if (iter != mAnims.end())
	{
		return iter->second;
	}
	else
	{
		Animation* anim = new Animation();
		if (anim->Load(fileName, loop))
		{
			mAnims.emplace(fileName, anim);
		}
		else
		{
			delete anim;
			anim = nullptr;
		}
		return anim;
	}
}


bool GLErrorHandle(const char* location)
{
	GLenum error_code = glGetError();
	if (error_code == GL_NO_ERROR)
	{
		return true;
	}
	do
	{
		const char* msg;
		switch (error_code)
		{
		case GL_INVALID_ENUM:                  msg = "INVALID_ENUM"     ; break;
		case GL_INVALID_VALUE:                 msg = "INVALID_VALUE"    ; break;
		case GL_INVALID_OPERATION:             msg = "INVALID_OPERATION"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: msg = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default: msg = "unknown Error";
		}
		printf("GLErrorLayer: ERROR%04x'%s' location: %s\n", error_code, msg, location);
		error_code = glGetError();
	} while (error_code != GL_NO_ERROR);

	return false;
}
