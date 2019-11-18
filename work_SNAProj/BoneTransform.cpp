/*
#include "BoneTransform.h"

Matrix4 BoneTransform::ToMatrix() const
{
	// 回転と平行移動から、行列を返す
	Matrix4 rot = Matrix4::CreateFromQuaternion(mRotation);
	Matrix4 trans = Matrix4::CreateTranslation(mTranslation);

	return rot * trans;
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	BoneTransform retVal;
	retVal.mRotation = Quaternion::Slerp(a.mRotation, b.mRotation, f);        // aからb に数値fで球面線形補間(slerp)する
	retVal.mTranslation = Vector3::Lerp(a.mTranslation, b.mTranslation, f);   // 平行移動成分はf で線形補間(lerp)する
	return retVal;
}
*/