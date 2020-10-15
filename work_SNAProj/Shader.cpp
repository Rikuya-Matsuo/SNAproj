#include "Shader.h"
#include "Texture.h"
#include <SDL/SDL.h>
#include <fstream>
#include <sstream>

Shader::Shader()
	: mShaderProgram(0)
	, mVertexShader(0)
	, mFragShader(0)
{

}

Shader::~Shader()
{

}

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
	// 頂点シェーダー、ピクセル（フラグメント）シェーダーをコンパイル
	if (!CompileShader(vertName,
		GL_VERTEX_SHADER,
		mVertexShader) ||
		!CompileShader(fragName,
			GL_FRAGMENT_SHADER,
			mFragShader))
	{
		return false;
	}

	//ここでシェーダープログラムができたので、 頂点シェーダー・フラグメントシェーダーをリンクさせる
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);

	// プログラムのリンクが成功したか？
	if (!IsValidProgram())
	{
		return false;
	}

	return true;
}

void Shader::Unload()
{
	// シェーダ、プログラムのdelete
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

void Shader::SetActive()
{
	// シェーダープログラムをアクティブにする
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// シェーダー変数(uniform)を名前で検索する
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 行列データをシェーダー変数に送る
	glUniformMatrix4fv(loc, 1, GL_TRUE, matrix.GetAsFloatPtr());
}

void Shader::SetMatrixUniforms(const char* name, Matrix4* matrices, unsigned count)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 行列配列データをシェーダー変数に送る
	glUniformMatrix4fv(loc, count, GL_TRUE, matrices->GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector3D& vector)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// ベクトル値をシェーダー変数に送る
	glUniform3fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetFloatUniform(const char* name, float value)
{
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// float値をシェーダー変数に送る
	glUniform1f(loc, value);
}

bool Shader::CompileShader(const std::string& fileName,
	GLenum shaderType,
	GLuint& outShader)
{
	// ファイルを開く
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// テキストを読みこんで、文字列に変換する
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		// シェーダーをshaderTypeに従って作成する
		outShader = glCreateShader(shaderType);
		// ソース文字列のコンパイルを試みる
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			printf("シェーダーコンパイルに失敗 %s", fileName.c_str());
			return false;
		}
	}
	else
	{
		printf("シェーダーファイルが見つかりませんでした: %s", fileName.c_str());
		return false;
	}

	return true;
}

bool Shader::IsCompiled(GLuint shader)
{
	GLint status;
	// コンパイルの状態を問い合わせる
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		printf("GLSL compileに失敗:\n%s", buffer);
		return false;
	}

	return true;
}

bool Shader::IsValidProgram()
{

	GLint status;
	// リンク状態を問い合わせる
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		printf("GLSL Link 状態:\n%s", buffer);
		return false;
	}

	return true;
}
