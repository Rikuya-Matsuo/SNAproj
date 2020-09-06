#include "Stage.h"
#include "Block.h"
#include "Floor.h"
#include "BGObject.h"
#include <vector>

float Stage::mBlockScale = 1.0f;

Stage::Stage():
	mBlocks(nullptr),
	mBlockMassX(0),
	mBlockMassY(0)
{
}

Stage::~Stage()
{
	// メモリ削除
	if (mBlocks != nullptr)
	{
		for (Uint8 i = 0; i < mBlockMassY; ++i)
		{
			delete[] mBlocks[i];
		}
		delete[] mBlocks;
	}
}

void Stage::LoadMap(const std::string & mapFilePath, const std::string & blockTextureFilePath, const std::string & floorTextureFilePath)
{
	mBlockMassX = mBlockMassY = 0;

	// ファイルオープン
	std::ifstream mapFile;
	mapFile.open(mapFilePath.c_str());

	// 読み込み失敗時
	if (mapFile.fail())
	{
		return;
	}

	std::vector<int> numArray;		// 配列が決まるまで一時的にここに格納する
	std::string buf;				// 数字文字を格納
	buf.reserve(4);
	int mapWidth = 0;				// マップの幅を記録
	char c;							// ファイルの中の文字
	while (true)
	{
		// 文字を入れる
		c = mapFile.get();

		// ファイルが終わりならループを抜ける
		if (mapFile.eof())
		{
			break;
		}

		// カンマじゃなく、改行でもない = 数字文字
		if (c != ',' && c != '\n')
		{
			buf += c;
		}
		else
		{
			// マップ幅記録（マップ幅　=　カンマ、改行の読まれた数　より）
			mapWidth++;

			// 改行は段が変わったとき。
			if (c == '\n')
			{
				mBlockMassY++;

				// 一番広い幅を幅として記録する
				if (mapWidth > mBlockMassX)
				{
					mBlockMassX = mapWidth;
				}
				mapWidth = 0;
			}

			// 数字文字をint型の値として記録
			int num = std::stoi(buf);
			numArray.emplace_back(num);
			buf.clear();
		}
	}

	// メモリ確保
	mBlocks = new Uint8*[mBlockMassY];
	for (int i = 0; i < mBlockMassY; ++i)
	{
		mBlocks[i] = new Uint8[mBlockMassX];
	}

	// 代入
	for (int yloop = 0; yloop < mBlockMassY; ++yloop)
	{
		for (int xloop = 0; xloop < mBlockMassX; ++xloop)
		{
			int loopNum = yloop * mBlockMassX + xloop;
			mBlocks[yloop][xloop] = numArray[loopNum];
		}
	}

	std::vector<int>().swap(numArray);

	mapFile.close();

	// 生成
	Construct(blockTextureFilePath, floorTextureFilePath);
}

void Stage::Construct(const std::string & blockTextureFilePath, const std::string & floorTextureFilePath)
{
	// ブロックがnullなら行わない
	if (mBlocks == nullptr)
	{
		SDL_Log("Stage : Block is not set! Fail to construction.\n");
		return;
	}
	// ステージの縦横ブロック数が設定されていない場合も行わない
	else if (mBlockMassX <= 0 || mBlockMassY <= 0)
	{
		SDL_Log("Stage : Block mass is invalid value. Fail to construction.\n");
		return;
	}

	///////////////////////////////////////
	// ブロック生成
	///////////////////////////////////////
	const float blockSize = Block::mModelSize * mBlockScale;
	for (int yBlock = 0; yBlock < mBlockMassY; ++yBlock)
	{
		for (int xBlock = 0; xBlock < mBlockMassX; ++xBlock)
		{
			const Uint8 blockType = mBlocks[yBlock][xBlock];

			// ブロック配置データ内の値が0であれば生成しない
			if (!blockType)
			{
				continue;
			}

			// 生成
			Block * const bk = new Block(blockTextureFilePath, yBlock == mBlockMassY - 1);
			bk->SetScale(mBlockScale);

			// ブロックの高さの半分を計算
			const float blockHalfHeight = blockSize / 2;

			// 一番下の段のブロックの上面が高さ0となるよう、生成する位置を計算
			// xも左が0となるように。
			Vector3D pos(
				Block::mModelSize * mBlockScale * xBlock + Block::mModelSize * mBlockScale / 2,
				0.0f,
				Block::mModelSize * mBlockScale * (mBlockMassY - (yBlock + 1)) - blockHalfHeight);

			// 位置情報代入
			bk->SetPosition(pos);
		}
	}

	// 終端3ブロックに踏み込んだ時点でゴールとする
	mGoalLineX = blockSize * (mBlockMassX - 3);

	////////////////////////////////////////////////
	// ブロックのさらに下に表示する床の生成
	////////////////////////////////////////////////
	// 生成位置を計算
	Vector3D flrPos;
	flrPos.x = mBlockMassX * Block::mModelSize * mBlockScale / 2.0f;
	flrPos.y = 0.0f;
	flrPos.z = mFloorHeight = -(Block::mModelSize * mBlockScale);

	// 床のスケールを設定
	// マップの長さ + 5000.0f（端っこが見えないように）
	float flrScale = mBlockMassX * blockSize + 5000.0f;

	// 生成
	Floor * const flr = new Floor(floorTextureFilePath);
	flr->SetPosition(flrPos);
	flr->SetScale(flrScale);
}

void Stage::LoadBGObjectMap(const std::string & bgObjMapFilePath, float xScale, float yScale, float zPos)
{
	std::ifstream file;
	file.open(bgObjMapFilePath.c_str());

	std::unordered_map<std::string, BGObjectPallet> pallet;

	LoadBGObjectMapPallet(file, pallet);

	LoadBGObjectMapPosition(file, pallet, xScale, yScale, zPos);
}

void Stage::LoadBGObjectMapPallet(std::ifstream & file, std::unordered_map<std::string, BGObjectPallet>& ret)
{
	enum Section
	{
		Key = 0,
		ModelPath,
		Scale
	};

	std::string key;

	uint8_t currentSection = Section::Key;

	BGObjectPallet palletBuf;

	std::string buf;

	char c;
	while (true)
	{
		c = file.get();

		if (file.eof())
		{
			break;
		}

		if (c != ',' && c != '\n')
		{
			buf += c;
			continue;
		}

		if (buf == "</pallet>")
		{
			break;
		}

		switch (currentSection)
		{
		case Section::Key:
			key = buf;
			buf.clear();
			++currentSection;

		case Section::ModelPath:
			palletBuf.mModelFilePath = buf;
			buf.clear();
			++currentSection;

		case Section::Scale:
			palletBuf.mScale = std::stof(buf);

			ret[key] = palletBuf;

			//ClearPallet(palletBuf);

		default:
			break;
		}
	}
}

void Stage::LoadBGObjectMapPosition(std::ifstream & file, const std::unordered_map<std::string, BGObjectPallet> & pallet, float xScale, float yScale, float zPos)
{
	std::string buf;

	int xCell = 0;
	int yCell = 0;

	std::list<BGObject *> objectList;

	char c;
	while (true)
	{
		c = file.get();

		if (file.eof())
		{
			break;
		}

		if (c != ',' && c != '\n')
		{
			buf += c;
			continue;
		}

		// 生成位置の計算
		// 高さを示す変数は、ファイルを読み終えるまで分からないので、一旦yCellの値を格納
		Vector3D pos;
		pos.x = xCell * xScale;
		pos.y = zPos;
		pos.z = static_cast<float>(yCell);

		// 生成
		BGObject * obj = new BGObject(pallet.at(buf).mModelFilePath);
		obj->SetPosition(pos);
		obj->SetScale(pallet.at(buf).mScale);

		objectList.emplace_back(obj);

		if (c == ',')
		{
			++xCell;
		}
		else
		{
			xCell = 0;
			++yCell;
		}

		buf.clear();
	}

	// 高さの調整
	for (auto itr : objectList)
	{
		Vector3D pos = itr->GetPosition();

		// 下記の式の右辺において変数の果たす意味合い
		// yCell	: 最終的な縦のセルの数
		// pos.z	: 上から何番目のセルか（ゼロオリジン）
		pos.z = (yCell - pos.z) * yScale;

		itr->SetPosition(pos);
	}
}

void Stage::ClearPallet(BGObjectPallet & pallet)
{
	pallet.mModelFilePath.clear();
	pallet.mScale = 0.0f;
}
