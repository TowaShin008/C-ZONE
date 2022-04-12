#pragma once
#include "Sprite.h"
#include"Input.h"
class DropNormalMap : public Sprite
{
public:
	struct BlurConstBuffer
	{
		float scrollCount;
		float scrollWidth;
		float effectCount;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	DropNormalMap();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="input">インプットクラス</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// クリアカラーのセット
	/// </summary>
	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };

	/// <summary>
	/// フェードアウト用のカウントを格納
	/// </summary>
	/// <param name="arg_effectCount">フェードアウト変数</param>
	void SetFadeOutCount(float arg_fadeOutCount) { fadeOutCount = arg_fadeOutCount; }

	/// <summary>
	/// フェードアウト用のカウントを取得
	/// </summary>
	/// <returns>フェードアウト変数</returns>
	float GetFadeOutCount() { return fadeOutCount; }

	/// <summary>
	/// フェードアウトかフェードインかのフラグ
	/// </summary>
	/// <param name="arg_fadeOutFlag"></param>
	void SetFadeOutFlag(bool arg_fadeOutFlag);
private:
	//テクスチャバッファ
	ComPtr<ID3D12Resource>texBuff;//通常色
	ComPtr<ID3D12Resource>normalMapTexBuff;//水滴
	ComPtr<ID3D12Resource>normalMapTexBuff_2;//水滴跡
	ComPtr<ID3D12Resource>dissolveTexBuff;//ディゾルブ
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource>depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;

	float scrollCount = 0.0f;

	float fadeOutCount = 1.0f;
	bool fadeOutFlag = 1.0f;
};

