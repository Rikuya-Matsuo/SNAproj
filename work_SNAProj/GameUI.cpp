#include "GameUI.h"
#include "Player.h"
#include "AnimationChips.h"
#include "System.h"
#include "Texture.h"
#include <vector>
#include <cstdlib>

GameUI::GameUI(const Player * player):
	UIScreen(),
	mPlayer(player),
	mLifeMax(player->GetLife())
{
	mLifeAnimTextures = new AnimationChips[mLifeMax];
	for (char i = 0; i < mLifeMax; ++i)
	{
		const int frameMass = 9;

		mLifeAnimTextures[i].Load(System::GetInstance().GetRenderer(), "Assets/flame_parts1.png", frameMass, 3, 3, 1024, 1024, 0.1f);

		const bool useRandonFlame = true;
		if (useRandonFlame)
		{
			// �A�j���[�V�����`�b�v�̏��Ԃ��̂��Ƃ̃����_���ɂ���
			std::vector<int> routine;

			// �ŏ��̃t���[���̃C���f�b�N�X��ݒ�
			int tmp = rand() % frameMass;
			routine.emplace_back(tmp);

			// ��Őݒ肵���t���[���ƍ��킹�āA6~8�t���[���̃��[�`���Ƃ���
			int routineFrameMass = 5 + rand() % 3;

			for (int j = 0; j < routineFrameMass; ++j)
			{
				// �O�̃t���[�����1~2�t���[���i�߂����̂��C���f�b�N�X�Ƃ��Đݒ肷��
				tmp += 1 + rand() % 2;

				// tmp���t���[�����𒴂��ē���΁A����ȓ��Ɏ��߂�
				tmp %= frameMass;

				// �ݒ�
				routine.emplace_back(tmp);
			}

			// routine�ɋL�^���ꂽ�t���[���̏��Ԃ�mLifeAnimTextures[i]�ɓo�^
			mLifeAnimTextures[i].SetRoutine(routine.data(), routine.size());
		}
	}

	mGuide = System::GetInstance().GetRenderer()->GetTexture("Assets/guide.png");
}

GameUI::~GameUI()
{
	delete[] mLifeAnimTextures;
}

void GameUI::Update()
{
	UIScreen::Update();

	for (char i = 0; i < mLifeMax; ++i)
	{
		mLifeAnimTextures[i].Update();
	}
}

void GameUI::Draw(Shader * shader) const
{
	// �̗�UI
	for (char i = 0; i < mPlayer->GetLife(); ++i)
	{
		Texture * tex = mLifeAnimTextures[i].GetCurrentTexture();
		if (!tex)
		{
			printf("Return is null...\n");
		}

		float scale = 0.3f;

		Vector2D pos;
		pos.x = -(System::GetInstance().GetRenderer()->GetScreenWidth() / 2) + (tex->GetWidth() / 3 * scale / 2) + (tex->GetWidth() / 3 * scale * i);
		pos.y = System::GetInstance().GetRenderer()->GetScreenHeight() / 2 - tex->GetHeight() * scale / 2 + tex->GetHeight() * scale / 5;

		DrawTexture(shader, tex, pos, scale);
	}

	// �K�C�hUI
	Vector2D guidePos;
	guidePos.x = 0.0f;
	guidePos.y = -System::GetInstance().GetRenderer()->GetScreenHeight() / 2 + mGuide->GetHeight() / 2;

	DrawTexture(shader, mGuide, guidePos);
}
