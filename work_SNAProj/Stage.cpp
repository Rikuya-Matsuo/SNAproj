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

int Stage::LoadBGObjectMap(const std::string & bgObjMapFilePath, float xStartPos, float groundHeight, float depth, float xEmptyCellScale, float yEmptyCellScale, Actor *** generatedActors)
{
	// ファイルオープン
	std::ifstream file;
	file.open(bgObjMapFilePath.c_str());

	if (file.fail())
	{
		printf("Fail to open BGObject File \"%s\"\n", bgObjMapFilePath.c_str());
		return -1;
	}

	// 文字列に関連付けたオブジェクトデータ（パレット）を記録する
	std::unordered_map<std::string, BGObjectPallet> pallet;

	bool successToLoadPallet = LoadBGObjectMapPallet(file, pallet);

	if (successToLoadPallet)
	{
		// オブジェクトの配置データロード及びオブジェクトの生成
		int objMass = LoadBGObjectMapPosition(file, pallet, xStartPos, groundHeight, depth, xEmptyCellScale, yEmptyCellScale, generatedActors);

		return objMass;
	}

	return -1;
}

bool Stage::LoadBGObjectMapPallet(std::ifstream & file, std::unordered_map<std::string, BGObjectPallet>& ret)
{
	// パレットの記録順序
	enum Section
	{
		Key = 0,
		ModelPath,
		Scale
	};

	std::string key;

	Uint8 currentSection = Section::Key;

	BGObjectPallet palletBuf;

	std::string buf;

	bool palletEndFlag = false;

	bool isLastCharNewLine = false;

	const char * errorMassage = "Fail to load stage decoration pallet. The file may not be correct format.\n";

	char c;
	while (true)
	{
		// ストリームから文字を取得
		c = file.get();

		// パレットロード時点ではファイル終端には至らないはずなので、エラーである
		if (file.eof())
		{
			printf(errorMassage);
			return false;
		}

		// 改行文字だったかを記録
		isLastCharNewLine = (c == '\n');

		// カンマでも改行文字でもなければ、バッファに記録
		if (c != ',' && c != '\n')
		{
			buf += c;
			continue;
		}

		// 空文字列だった場合スキップ
		if (buf == "")
		{
			continue;
		}

		// パレットの終わりを示す文字列であればロードを終了する
		if (buf == "</pallet>")
		{
			palletEndFlag = true;
			break;
		}

		// セクションに応じた記録を行う
		switch (currentSection)
		{
		case Section::Key:
			key = buf;
			buf.clear();
			++currentSection;

			break;

		case Section::ModelPath:
			palletBuf.mModelFilePath = buf;
			buf.clear();
			++currentSection;
			
			break;

		case Section::Scale:
			palletBuf.mScale = std::stof(buf);

			ret[key] = palletBuf;

			buf.clear();

			currentSection = Section::Key;

			break;

		default:
			break;
		}
	}

	// パレット終了後も、次の改行文字まで読み進める
	// ただし、前のループで既に、パレット終了記号の次の改行文字を読んでいるなら、この処理を行わない。
	while (palletEndFlag && !isLastCharNewLine)
	{
		c = file.get();

		if (file.eof())
		{
			printf(errorMassage);
			return false;
		}

		if (c == '\n')
		{
			break;
		}
	}

	return true;
}

int Stage::LoadBGObjectMapPosition(std::ifstream & file, const std::unordered_map<std::string, BGObjectPallet> & pallet, float xStartPos, float groundHeight, float depth, float xEmptyCellScale, float yEmptyCellScale, Actor *** generatedActors)
{
	std::string buf;

	int xCell = 0;

	// yCellはデータの形によるが、少なくともexcelからそのまま書きだした形だとゼロオリジンにならないよう。
	// 改行してからeofか、改行せずにeofかの違いだが、両方に対応させてゼロオリジンにしたい。
	int yCell = 0;

	// xCellのとった最大値を記録
	int xCellMax = -1;

	float xOffset = 0.0f;

	// 生成したオブジェクトへのポインタと、記録されていたセルの情報をまとめた構造体
	struct ObjCellData
	{
		BGObject * mObj;
		int xCell;
		int yCell;
	};

	std::list<ObjCellData *> objectList;
	
	// 最後に読んだ文字が改行文字だったかのフラグ
	bool isLastCharNewLine = false;

	// 改行文字を読んだときのラムダ式
	auto onLoadNewLine = [&xCell, &xCellMax, &yCell]()
	{
		if (xCellMax < xCell)
		{
			xCellMax = xCell;
		}

		xCell = 0;
		++yCell;
	};

	char c;
	while (true)
	{
		// 文字の取得
		c = file.get();

		if (file.eof())
		{
			break;
		}

		// ここで改行文字フラグを記録
		isLastCharNewLine = (c == '\n');

		if (c != ',' && c != '\n')
		{
			buf += c;
			continue;
		}

		// ここから先へはcが','か'\n'の場合しか進めない

		// 空白（オブジェクトを生成しないセル）と指定された場合、バッファをクリアしてスキップする
		if (buf == "-1")
		{
			xOffset += xEmptyCellScale;

			buf.clear();
			continue;
		}

		// 空文字列はスキップ
		if (buf == "")
		{
			// 読んだ文字が改行文字なら、その時の処理はここでしておく。
			if (isLastCharNewLine)
			{
				onLoadNewLine();
			}

			continue;
		}

		// 生成位置の計算
		// 高さを示す変数は、ファイルを読み終えるまで分からないので、一旦0として初期化
		Vector3D pos;
		pos.x = xOffset + xStartPos;
		pos.y = depth;
		pos.z = 0.0f;

		// 生成
		BGObject * obj = new BGObject(pallet.at(buf).mModelFilePath);
		obj->SetPosition(pos);
		obj->SetScale(pallet.at(buf).mScale);

		// 次のオブジェクトの生成場所のx座標オフセットを記録
		xOffset += obj->GetModelSize().x * obj->GetScale();

		// オブジェクトへのポインタとセル情報をまとめた構造体インスタンスの生成
		ObjCellData * ocd = new ObjCellData;
		ocd->mObj = obj;
		ocd->xCell = xCell;
		ocd->yCell = yCell;

		objectList.emplace_back(ocd);

		if (c == ',')
		{
			++xCell;
		}
		// '\n'が読まれた場合
		else
		{
			onLoadNewLine();
		}

		buf.clear();
	}

	// 最後に読んだ文字が改行文字なら、ゼロオリジンにするためにyCellから1を引く
	yCell -= isLastCharNewLine;

	// 各オブジェクトの高さを調節するための変数群
	// 各xセル座標のオブジェクトの調整する高さを記録
	std::vector<float> yOffsets;
	yOffsets.resize(xCellMax + 1);
	for (auto itr = yOffsets.begin(); itr != yOffsets.end(); ++itr)
	{
		*itr = 0.0f;
	}

	// 添え字のxセル座標で、最後に位置を調整したyセル座標を記録
	// オブジェクトを生成しないセルが何回連続で読まれたの計算に使う
	std::vector<int> lastAdjustYCell;
	lastAdjustYCell.resize(xCellMax + 1);
	for (auto itr = lastAdjustYCell.begin(); itr != lastAdjustYCell.end(); ++itr)
	{
		*itr = yCell + 1;
	}

	// 高さの調整
	// 性質上リストの反対側から調整をしなければならない。
	// 逆イテレータなるものをこの時知る。使ってみる。
	for (auto itr = objectList.rbegin(); itr != objectList.rend(); ++itr)
	{
		ObjCellData * data = *itr;

		int x = data->xCell;
		int y = data->yCell;

		// 前に調整したオブジェクトから現在調整中のオブジェクトの間の、オブジェクトを生成しないセルの数
		int notGenCellMass;
		if (lastAdjustYCell.size())
		{
			notGenCellMass = (lastAdjustYCell[x] - y) - 1;
		}
		else
		{
			notGenCellMass = x;
		}

		// 調整後の高さ
		float newHeight = groundHeight + yOffsets[x] + yEmptyCellScale * notGenCellMass;
		
		// オフセットの計算
		yOffsets[x] += yEmptyCellScale * notGenCellMass + data->mObj->GetModelSize().z * data->mObj->GetScale();

		// 代入する
		Vector3D pos = data->mObj->GetPosition();

		pos.z = newHeight;

		data->mObj->SetPosition(pos);

		// 次のループに向けたデータの設定
		lastAdjustYCell[x] = y;
	}

	// ロード後にオブジェクトにアクセスできるように、Actor*型の配列を生成し、*generatedActorsに代入する
	if (generatedActors != nullptr)
	{
		*generatedActors = new Actor * [objectList.size()];

		size_t loopCount = 0;
		for (auto itr : objectList)
		{
			(*generatedActors)[loopCount++] = itr->mObj;
		}
	}

	// オブジェクトへのポインタとセル情報をまとめた構造体のメモリ解放
	for (auto itr : objectList)
	{
		delete itr;
	}

	// 生成したオブジェクトの数を返却
	return objectList.size();
}

// 使われていない
void Stage::ClearPallet(BGObjectPallet & pallet)
{
	pallet.mModelFilePath.clear();
	pallet.mScale = 0.0f;
}
