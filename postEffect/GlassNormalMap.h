#pragma once
#include "Sprite.h"
#include"Input.h"
class GlassNormalMap : public Sprite
{
public:
	struct BlurConstBuffer
	{
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GlassNormalMap();

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
private:
	//テクスチャバッファ
	ComPtr<ID3D12Resource>texBuff;
	ComPtr<ID3D12Resource>effectTexBuff;
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource>depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;
};

