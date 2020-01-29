#include "Sprite.h"
#include "System.h"
#include "Texture.h"
#include "Shader.h"

const Sprite::FlagType Sprite::mDrawFlagMask = 1 << 0;

Sprite::Sprite(const char * texPath):
	mFlags(mDrawFlagMask),
	mPosition(Vector2D::zero),
	mSize(Vector2D(100, 100))
{
	System::GetInstance().ResisterSprite(this);

	if (texPath)
	{
		mTexture = System::GetInstance().GetRenderer()->GetTexture(texPath);
	}
}

Sprite::~Sprite()
{
	System::GetInstance().DeresisterSprite(this);
}

void Sprite::Draw()
{
	if (mFlags & mDrawFlagMask && mTexture)
	{
		mTexture->SetActive();

		Vector2D leftUp, rightDown;
		CalculatePosition(mPosition, leftUp);
		CalculatePosition(mPosition + mSize, rightDown);

		/*
		// Scale the quad by the width/height of texture
		Matrix4 scaleMat = Matrix4::CreateScale(
			mSize.x,
			mSize.y,
			1.0f);
		// Translate to position on screen
		Matrix4 transMat = Matrix4::CreateTranslation(
			Vector3D(mPosition.x, mPosition.y, 0.0f));
		// Set world transform
		Matrix4 world = scaleMat * transMat;
		System::GetInstance().GetRenderer()->SetMeshShader(world);
		*/

		//glDrawElements(GL_POLYGON, 2, GL_UNSIGNED_INT, nullptr);

		glBegin(GL_POLYGON);
		const bool test = true;
		if (!test)
		{
			glTexCoord2f(0, 0); glVertex2f(leftUp.x, leftUp.y);
			glTexCoord2f(0, 1); glVertex2f(leftUp.x, rightDown.y);
			glTexCoord2f(1, 0); glVertex2f(rightDown.x, leftUp.y);
			glTexCoord2f(1, 1); glVertex2f(rightDown.x, rightDown.y);
		}
		else
		{
			glTexCoord2f(0, 0); glVertex2f(-1.0f, 1.0f);
			glTexCoord2f(0, 1); glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(1, 0); glVertex2f(1.0f, 1.0f);
			glTexCoord2f(1, 1); glVertex2f(1.0f, -1.0f);
		}
		glEnd();
	}
}

void Sprite::CalculatePosition(const Vector2D & pos, Vector2D & result)
{
	float screenW = System::GetInstance().GetRenderer()->GetScreenWidth();
	float screenH = System::GetInstance().GetRenderer()->GetScreenHeight();

	result.x = (pos.x / (screenW / 2)) - 1.0f;
	result.y = -((pos.y / (screenH / 2)) - 1.0f);
}
