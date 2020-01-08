// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Mesh.h"
#include "Common.h"
#include "Renderer.h"
#include "Texture.h"
#include "AnimationChips.h"
#include "VertexArray.h"
#include "Vector.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "MathExpantion.h"
#include <cmath>
#include <cstring>

const char* Mesh::mBoardMeshPath = "assets/board.gpmesh";

const Mesh::FlagType Mesh::mAnimModeFlagMask = 1 << 0;
const Mesh::FlagType Mesh::mAnimLoopEndFlagMask = 1 << 1;
const Mesh::FlagType Mesh::mIsBoardFlagMask = 1 << 2;

namespace
{
	union Vertex
	{
		float f;
		uint8_t b[4];
	};
}

Mesh::Mesh()
	: mVertexArray(nullptr)
	, mRadius(0.0f)
	, mSpecPower(100.0f)
	, mFlags(0)
{
	
}

Mesh::~Mesh()
{
	for (auto anim : mAnimations)
	{
		for (auto chips : anim.second)
		{
			delete chips.second;
		}
	}
}

//メッシュのロード
bool Mesh::Load(const std::string & fileName, Renderer* renderer, const Actor * actor)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		printf("File not found: Mesh %s", fileName.c_str());
		return false;
	}
	// JSONの解析を行う
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	// JSONオブジェクトか？
	if (!doc.IsObject())
	{
		printf("Mesh %s is not valid json\n", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	// バージョンチェック
	if (ver != 1)
	{
		printf("Mesh %s not version 1", fileName.c_str());
		return false;
	}

	// ボードメッシュであればフラグを立てる
	// 入力されたパスを全部小文字に合わせる
	std::string fileNameAllLower(fileName);
	const char * ccFileName = fileName.c_str();
	for (int i = 0; ccFileName[i] != '\0'; ++i)
	{
		fileNameAllLower[i] = SDL_tolower(ccFileName[i]);
	}
	fileNameAllLower += '\0';
	if (!strcmp(fileNameAllLower.c_str(), mBoardMeshPath))
	{
		mFlags |= mIsBoardFlagMask;
	}

	mShaderName = doc["shader"].GetString();

	// 頂点レイアウトとサイズをファイルからセット
	VertexArray::Layout layout = VertexArray::PosNormTex;
	size_t vertSize = 8;

	std::string vertexFormat = doc["vertexformat"].GetString();
	if (vertexFormat == "PosNormSkinTex")
	{
		layout = VertexArray::PosNormSkinTex;
		// This is the number of "Vertex" unions, which is 8 + 2 (for skinning)s　1個の頂点の集合の数　８　＋　２（スキニング分）
		// 3 (位置xyz) + 3(法線xyz) + 2(Boneと重み）＋　2(UV)  の計　10個分（40byte) 　
		vertSize = 10;
	}

	// テクスチャのロード
	const rapidjson::Value& textures = doc["textures"];
	if (!textures.IsArray() || textures.Size() < 1)
	{
		printf("Mesh %s has no textures, there should be at least one", fileName.c_str());
		return false;
	}

	// スペキュラー強度
	mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

	// テクスチャ読み込み
	for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		// このテクスチャ既に読みこんでる？
		std::string texName = textures[i].GetString();
		Texture* t = renderer->GetTexture(texName);
		if (t == nullptr)
		{
			// テクスチャ読み込みのトライ
			t = renderer->GetTexture(texName);
			if (t == nullptr)
			{
				// NULLのままであればデフォルトテクスチャをセット
				t = renderer->GetTexture("Assets/Default.png");
			}
		}

		// メッシュ読み込み時にデフォルトで設定されるテクスチャの設定
		if (mDefaultTexture.find(actor) != mDefaultTexture.end() &&
			mDefaultTexture[actor] != t)
		{
			delete mDefaultTexture[actor];
		}

		mDefaultTexture[actor] = t;

		// 現時点のテクスチャをデフォルトのものとして設定する
		mCurrentTexture[actor] = mDefaultTexture[actor];
	}

	// 頂点読み込み
	const rapidjson::Value& vertsJson = doc["vertices"];
	if (!vertsJson.IsArray() || vertsJson.Size() < 1)
	{
		printf("Mesh %s has no vertices", fileName.c_str());
		return false;
	}

	// 頂点データ
	std::vector<Vertex> vertices;
	vertices.reserve(vertsJson.Size() * vertSize);
	mRadius = 0.0f;
	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
	{
		// 現時点で、８つの要素(位置xyz 法線xyz テクスチャuvの要素）が入っている
		const rapidjson::Value& vert = vertsJson[i];
		if (!vert.IsArray())
		{
			printf("Unexpected vertex format for %s", fileName.c_str());
			return false;
		}

		// 頂点位置から　バウンディングスフィアの半径を計算する（原点からの距離が最大のものを半径とする）
		Vector3D pos(static_cast<float>(vert[0].GetDouble()),
			        static_cast<float>(vert[1].GetDouble()),
			        static_cast<float>(vert[2].GetDouble()));
		mRadius = Common::Larger(mRadius, pos.LengthSq());

		// バウンディングボックス算出
		if (i == 0)
		{
			mBox.InitMinMax(pos);
		}
		else
		{
			mBox.RenewalMinMax(pos);
		}

		// 頂点レイアウトが PosNormTexなら（ボーンが無い）
		if (layout == VertexArray::PosNormTex)
		{
			Vertex v;
			// Add the floats　float値を追加
			for (rapidjson::SizeType j = 0; j < vert.Size(); j++)
			{
				v.f = static_cast<float>(vert[j].GetDouble());
				vertices.emplace_back(v);
			}
		}
		else // ボーンデータ入りなら　PosNormSkinTexなら
		{
			Vertex v;
			// Add pos/normal　頂点と法線を追加　6個分
			for (rapidjson::SizeType j = 0; j < 6; j++)
			{
				v.f = static_cast<float>(vert[j].GetDouble());
				vertices.emplace_back(v);
			}

			// Add skin information　スキン情報追加（ボーン番号:unsigned charの1バイト分）
			for (rapidjson::SizeType j = 6; j < 14; j += 4)  //ループとしては2回転する　1回転目はボーン番号、2回転目はボーンウェイトをintとして取ってくる（使用時に浮動小数化するっぽい）
			{
				v.b[0] = vert[j].GetUint();
				v.b[1] = vert[j + 1].GetUint();
				v.b[2] = vert[j + 2].GetUint();
				v.b[3] = vert[j + 3].GetUint();
				vertices.emplace_back(v);
			}

			// Add tex coords　テクスチャ座標
			for (rapidjson::SizeType j = 14; j < vert.Size(); j++)
			{
				v.f = static_cast<float>(vert[j].GetDouble());
				vertices.emplace_back(v);
			}
		}
	}

	// We were computing length squared earlier　バウンディングスフィアの半径を計算
	mRadius = sqrt(mRadius);

	// Load in the indices　頂点インデックスをロード
	const rapidjson::Value& indJson = doc["indices"];
	if (!indJson.IsArray() || indJson.Size() < 1)
	{
		printf("Mesh %s has no indices", fileName.c_str());
		return false;
	}

	//頂点インデックスを引っ張ってくる
	std::vector<unsigned int> indices;
	indices.reserve(indJson.Size() * 3);
	for (rapidjson::SizeType i = 0; i < indJson.Size(); i++)
	{
		const rapidjson::Value& ind = indJson[i];
		if (!ind.IsArray() || ind.Size() != 3)
		{
			printf("Invalid indices for %s", fileName.c_str());
			return false;
		}

		indices.emplace_back(ind[0].GetUint());
		indices.emplace_back(ind[1].GetUint());
		indices.emplace_back(ind[2].GetUint());
	}

	// Now create a vertex array　頂点配列を作成する
	mVertexArray = new VertexArray(vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
		layout, indices.data(), static_cast<unsigned>(indices.size()));
	return true;
}

void Mesh::Unload()
{
	delete mVertexArray;
	mVertexArray = nullptr;

}

void Mesh::Update(const Actor * actor)
{
	if (mAnimations[actor].count(mActiveAnimIndex[actor]))
	{
		// エイリアス生成
		AnimationChips * animChips = mAnimations[actor][mActiveAnimIndex[actor]];

		// アニメーションの更新
		animChips->Update();

		// テクスチャ取得
		mCurrentTexture[actor] = animChips->GetCurrentTexture();

		// アニメーションループ終了フラグの取得
		bool loopEnd = animChips->GetLoopEndFlag();
		BitFlagFunc::SetFlagByBool(loopEnd, mFlags, mAnimLoopEndFlagMask);
	}
}

bool Mesh::LoadTexture(const std::string & fileName, Renderer * renderer, const Actor * actor)
{
	Texture * tex = renderer->GetTexture(fileName);
	if (tex == nullptr)
	{
		return false;
	}

	mCurrentTexture[actor] = tex;

	return true;
}

bool Mesh::LoadDivTexture(const std::string & fileName, Renderer * renderer, const Actor * actor, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame, int animIndex)
{
	bool ret = false;

	Texture * textures = nullptr;

	AnimationChips * animChips = new AnimationChips();
	size_t frameMass = animChips->Load(renderer, fileName, allNum, xNum, yNum, chipW, chipH, secondPerFrame, textures);

	if (frameMass == allNum)
	{
		ret = true;

		if (mAnimations[actor].find(animIndex) != mAnimations[actor].end())
		{
			SDL_Log("Specified index has already been used.\n");
		}
		else
		{
			mAnimations[actor][animIndex] = animChips;
		}
	}

	// アニメーション使用モードとして設定
	mFlags |= mAnimModeFlagMask;

	return ret;
}

Texture* Mesh::GetAnimFrameTexture(const Actor* actor, int index)
{
	Texture * ret = nullptr;

	if (mAnimations[actor].count(index))
	{
		ret = mAnimations[actor][index]->GetCurrentTexture();
	}

	return ret;
}

AnimationChips * Mesh::GetAnimChips(const Actor* actor, int index)
{
	if (mAnimations[actor].count(index))
	{
		return mAnimations[actor][index];
	}

	return nullptr;
}

