#pragma once
#include "ComponentBase.h"
#include "BitFlagFunc.h"
#include <cstddef>

class Shader;
class Mesh;

class MeshComponent : public ComponentBase
{
public:
	MeshComponent(class Actor* owner, int drawOrder, bool isSkeletal = false);       // メッシュコンポーネントコンストラクタ。（要：親アクター）
	~MeshComponent();

	void Update() override;

	virtual void Draw(Shader* shader);                          // メッシュコンポーネントの描画
	virtual void SetMesh(Mesh* mesh);          // メッシュコンポーネントで使用するメッシュのセット
	const Mesh * GetMesh() { return mMesh; }
	void SetTextureIndex(size_t index) { mTextureIndex = index; }     // テクスチャのセット

	void SetVisible(bool visible) { BitFlagFunc::SetFlagByBool(visible, mMeshCompFlags, mVisibleFlagMask); }             // 表示フラグのセット
	bool GetVisible() const { return mMeshCompFlags & mVisibleFlagMask; }                      // 表示フラグのゲット

	bool GetIsSkeletal() const { return mMeshCompFlags & mIsSkeletalFlagMask; }                // スケルタルモデルかどうかの取得

	int GetDrawOrder() const { return mDrawOrder; }

	void SetRangeOutDrawFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mMeshCompFlags, mRangeOutDrawFlagMask); }
	bool GetRangeOutDrawFlag() const { return mMeshCompFlags & mRangeOutDrawFlagMask; }

	static void SetViewMatrix(const Vector3D & camPos) { mCamOffset = camPos; }

protected:
	typedef Uint8 FlagType;
	static const FlagType mVisibleFlagMask;
	static const FlagType mIsSkeletalFlagMask;
	static const FlagType mAnimationModeFlagMask;
	static const FlagType mRangeOutDrawFlagMask;			//視野角の外にあっても描画を行う。やたらと真にすると重くなる可能性あり。


	FlagType mMeshCompFlags;

	Mesh* mMesh;                                                // メッシュオブジェクトの取得

	size_t mTextureIndex;                                             // テクスチャーのインデックス

	static Vector3D mCamOffset;

	const int mDrawOrder;
};
