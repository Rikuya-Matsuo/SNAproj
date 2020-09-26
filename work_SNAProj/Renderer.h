#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glfw3.h>
#include "Matrix.h"

typedef struct _DirectionalLight
{
	Vector3D mDirection;     // ライト方向
	Vector3D mDiffuseColor;	// ライトカラー
	Vector3D mSpecColor;	    // スペキュラーカラー
}DirectionalLight;

class Camera;

class Renderer
{
public:
	Renderer();
	~Renderer();
	bool                   Initialize(int screenWidth, int screenHeight, bool fullScreen);     // SDL & GL初期化
	void                   Shutdown();                                                         // 終了
	void                   Draw();                                                             // 描画

	// セッター系
	void                   SetViewMatrix(const Matrix4& view) { mView = view; }                // ビュー行列のセット
	void                   SetAmbientLight(const Vector3D& ambientColor)                        // アンビエントライトのセット
	{
		mAmbientLight = ambientColor;
	}
	// ゲッター系
	SDL_Renderer*          GetSDLRenderer() { return mSDLRenderer; }                           // SDL系の描画に必要なSDLrendererを得る
	class Texture*         GetTexture(const std::string& fileName);                            // テクスチャをファイル名から返す
	class Mesh*            GetMesh(const std::string& fileName, const class Actor * actor);                               // メッシュをファイル名から返す
	const class Skeleton*  GetSkeleton(const char* fileName);                                  // スケルタルモデルの取得
	const class Animation* GetAnimation(const char* fileName, bool loop);                      // スケルタルアニメーションの取得

	float                  GetScreenWidth() { return static_cast<float>(mScreenWidth); }       // スクリーン幅
	float                  GetScreenHeight() { return static_cast<float>(mScreenHeight); }     // スクリーン高さ
	DirectionalLight&      GetDirectionalLight() { return mDirectionalLight; }                 // ディレクショナルライト
	const Matrix4&         GetViewMatrix() { return mView; }
	const Matrix4&         GetProjectionMatrix() { return mProjection; }

	void SetCamera(Camera * cam) { mCameraPointer = cam; }

	void                   AddMeshComponent(class MeshComponent* mesh);                        // メッシュコンポーネントの追加
	void                   RemoveMeshComponent(class MeshComponent* mesh);                     // メッシュコンポーネントの削除
	void                   ShowResource();                                                     // 登録されている テクスチャ・メッシュリソースの表示（デバッグ用）
	void                   WindowClear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); } // ウィンドウ描画クリア
	void                   WindowFlip() { SDL_GL_SwapWindow(mWindow); }                        // ウィンドウフリップ
	void                   SetWindowTitle(const std::string& title);                           // ウィンドウタイトルのセット
	void AddUI(const class UIScreen * ui);
	void RemoveUI(const class UIScreen * ui);

	std::mutex & GetTextureMutex() { return mTextureMutex; }

private:

	bool                                              LoadShaders();                          // シェーダーの初期化
	void                                              SetLightUniforms(class Shader* shader); // ライト値をシェーダーにセット
	void CreateSpriteVerts();

	int                                               mScreenWidth;      // スクリーン幅                                                           
	int                                               mScreenHeight;     // スクリーン高さ
	std::unordered_map<std::string, class Texture*>   mTextures;         // テクスチャ登録配列
	std::unordered_map<std::string, class Mesh*>      mMeshs;            // メッシュ登録配列
	std::vector<class MeshComponent*>                 mMeshComponents;   // メッシュコンポーネント登録配列
	std::vector<class SkeletalMeshComponent*>         mSkeletalMeshes;   // スケルタルメッシュの描画に使われる
	std::vector<const class UIScreen*>				  mUIs;
	std::unordered_map<std::string, class Skeleton*>  mSkeletons; // スケルタルデータ
	std::unordered_map<std::string, class Animation*> mAnims;    // アニメーションデータ

	const float mFieldOfView;

	//シェーダー関連
	class Shader*										mMeshShader;			// メッシュシェーダー
	class Shader*										mSkinnedShader;			// スキンメッシュシェーダー
	class Shader* mSpriteShader;

	// UI描画用頂点データ
	class VertexArray* mSpriteVerts;

	// 基本行列関連
	Matrix4                                           mView;             // ビュー行列
	Matrix4                                           mProjection;       // プロジェクション行列

	// ライティング関連
	Vector3D                                          mAmbientLight;     // アンビエントライト
	DirectionalLight                                  mDirectionalLight; // ディレクショナルライト

	// レンダリングベース情報関連
	SDL_Window*                                       mWindow;           // SDLウィンドウハンドル 
	SDL_GLContext                                     mContext;          // OpenGLコンテキスト
	SDL_Renderer*                                     mSDLRenderer;      // SDLレンダリングハンドル

	Camera*												mCameraPointer;

	// テクスチャ関連のデータへのアクセスを管理するミューテクス
	std::mutex mTextureMutex;
};

bool GLErrorHandle(const char* location);                              // OpenGLのエラーハンドル取得