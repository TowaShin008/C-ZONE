#pragma once

#include "Sprite.h"
#include <Windows.h>
#include <string>

class DebugText
{
public:
	static const int maxCharCount = 256;	// 最大文字数
	static const int fontWidth = 36;		// フォント画像内1文字分の横幅
	static const int fontHeight = 50;		// フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;	// フォント画像内1行分の文字数

	/// <summary>
	/// コンストラクタ
	/// </summary>
	DebugText();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugText();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	void Initialize(UINT texnumber);
	
	/// <summary>
	/// 文字生成処理
	/// </summary>
	/// <param name="text">生成する文字</param>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="size">フォントサイズ</param>
	void Print(const std::string & text, float x, float y, float size);

	/// <summary>
	/// 全ての文字の描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void DrawAll(ID3D12GraphicsCommandList * cmdList);

private:
	// スプライトデータの配列
	Sprite* spriteDatas[maxCharCount] = {};
	// スプライトデータ配列の添え字番号
	int spriteIndex = 0;
};
