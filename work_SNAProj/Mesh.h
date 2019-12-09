// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Collision.h"
#include <vector>
#include <string>
#include <unordered_map>

class Renderer;
class Texture;
class VertexArray;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool Load(const std::string& fileName, Renderer* renderer);   	// ���b�V���̃��[�h�E�A�����[�h
	void Unload();                                                         	

	int LoadTexture(const std::string& fileName, Renderer* renderer);

	VertexArray* GetVertexArray() { return mVertexArray; }            // ���b�V���̒��_�z��̎擾
	Texture* GetTexture(size_t index);                                // �w�肳�ꂽ�C���f�b�N�X����e�N�X�`���̎擾
	const std::string& GetShaderName() const { return mShaderName; }        // �V�F�[�_�[���̎擾

	float GetRadius() const { return mRadius; }                             // �o�E���f�B���O�X�t�B�A�̔��a���擾

	const AABB & GetCollisionBox() const { return mBox; }

	bool IsAssigned(int index);

private:
	// ���b�V���̃e�N�X�`��
	std::unordered_map<int, Texture *> mTextures;

	VertexArray* mVertexArray;                                        // ���b�V���̒��_�z��

	std::string mShaderName;	                                            // ���b�V���w��̃V�F�[�_�[
	float mRadius;	                                                        // �o�E���f�B���O�X�t�B�A�̔��a
	float mSpecPower;	                                                    // �\�ʂ̃X�y�L�����[�l

	AABB mBox;																// �o�E���f�B���O�{�b�N�X
};