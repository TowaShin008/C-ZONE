#pragma once
#include "Sprite.h"
#include"Input.h"
#include<vector>
class Bloom : public Sprite
{
public:
	/// <summary>
	/// 定数バッファの構造体
	/// </summary>
	struct BlurConstBuffer
	{
		Vector4 weights[2];
		float effectCount;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Bloom();

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

	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };

	/// <summary>
	/// 高輝度縮小描画
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void DrawShrinkTextureForBlur(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// ガウシアンブラー描画
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	void DrawGaussianBlur(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// バリア関数
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="p">テクスチャバッファ</param>
	/// <param name="before">前の状態</param>
	/// <param name="after">後の状態</param>
	void Barrier(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* p,
		D3D12_RESOURCE_STATES before,
		D3D12_RESOURCE_STATES after);

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateConstBuffer();

	/// <summary>
	/// 頂点バッファの生成
	/// </summary>
	/// <returns>生成生成の可否</returns>
	bool CreateVertexBuffer();

	/// <summary>
	/// ガウシアンブラーに使用するウェイト値の計算
	/// </summary>
	/// <param name="count">要素数</param>
	/// <param name="s">標準偏差</param>
	/// <returns></returns>
	std::vector<float> GetGaussianValues(size_t count, float s);

	/// <summary>
	/// サイズのアライメント
	/// </summary>
	/// <param name="size">アライメントされるサイズ</param>
	/// <param name="alignment">アライメントするサイズ</param>
	/// <returns></returns>
	unsigned int AligmentedValue(unsigned int size, unsigned int alignment);

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
	// テクスチャバッファ
	ComPtr<ID3D12Resource>texBuff;
	// 通常色 ガウシアンブラー
	ComPtr<ID3D12Resource>gaussianBuffer;
	// 高輝度 縮小バッファ
	ComPtr<ID3D12Resource>bloomBuffer[2];
	//CBV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapCBV;
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource>depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;

	float fadeOutCount = 1.0f;
	bool fadeOutFlag = 1.0f;
};

