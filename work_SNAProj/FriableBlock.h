#pragma once
#include "Block.h"

class FriableBlock : public Block
{
public:
	FriableBlock(const std::string & texturePath, bool isGroundFlag);
	~FriableBlock();
};
