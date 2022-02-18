#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include"MineMath.h"

class Light
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMVECTOR lightv;//ライトへの方向を表すベクトル
		Vector3 lightcolor;//ライトの色
	};
private://静的メンバ変数
	//デバイス
	static ID3D12Device* device;
public://静的メンバ関数
	///<summary>
	///静的初期化
	///</summary>
	///<param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);
private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource>constBuff;
	//ライト光線方向(単位ベクトル)
	XMVECTOR lightdir = { 1,0,0,0 };
	//ライト色
	Vector3 lightcolor = { 1,1,1 };
	//ダーティフラグ
	bool dirty = false;
public:
	void Initialize();
	void TransferConstBuffer();
	///<summary>
	///ライト方向をセット
	///</summary>
	///<param name="lightdir">ライト方向</param>
	void SetLightDir(const XMVECTOR& arg_lightdir);

	/// <summary>
	/// ライトカラーのセット
	/// </summary>
	/// <param name="lightcolor"></param>
	void SetLightColor(const Vector3& arg_lightcolor);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	/// <param name="rootParameterIndex">ルートパラメーターインデックス</param>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	/// <summary>
	/// ライト生成処理
	/// </summary>
	/// <returns>ライトクラス</returns>
	static Light* Create();
};