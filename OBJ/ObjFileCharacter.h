#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include"ObjFileModel.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#include"PipelineState.h"
class ObjFileCharacter :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector4 color;	// 色 (RGBA)
		XMMATRIX mat;	// ３Ｄ変換行列
	};


	//定数バッファ用データ00
	struct  ConstBufferDataB0
	{
		Vector4 color;
		XMMATRIX mat;
	};

	//定数バッファ用データ01
	struct ConstBufferDataB1
	{
		Vector3 ambient;
		float pad1;
		Vector3 diffuse;
		float pad2;
		Vector3 specular;
		float alpha;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_device">デバイス</param>
	ObjFileCharacter(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ObjFileCharacter();

	/// <summary>
	/// オブジェクトの生成処理
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <param name="arg_cmdList">コマンドリスト</param>
	/// <param name="arg_position">ポジション</param>
	/// <returns>キャラクターオブジェクト</returns>
	static ObjFileCharacter* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="arg_incrementValue">スクロール量</param>
	void Update(const Vector3& arg_incrementValue);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;

	/// <summary>
	/// モデルのセット
	/// </summary>
	/// <param name="arg_objModel">モデル</param>
	void SetOBJModel(ObjFileModel* arg_objModel) { objModel = arg_objModel; }

	/// <summary>
	/// 視点座標のセット
	/// </summary>
	/// <param name="arg_eye">視点座標</param>
	static void SetEye(const Vector3& arg_eye);

	/// <summary>
	/// 注視点座標のセット
	/// </summary>
	/// <param name="arg_target">注視点座標</param>
	static void SetTarget(const Vector3& arg_target);

	/// <summary>
	/// ビュー行列の更新処理
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="arg_camera">カメラ</param>
	static void SetCamera(Camera* arg_camera) { camera = arg_camera; }

	/// <summary>
	/// ポジションの移動
	/// </summary>
	/// <param name="arg_incrementValue">ポジションの増加量</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// 当たり判定
	/// </summary>
	/// <param name="arg_otherObject">相手のオブジェクト</param>
	/// <returns>当たったかどうか</returns>
	bool IsCollision(GameObject* arg_otherObject);
private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//カメラ関係
	static XMMATRIX matView;
	static XMMATRIX matProjection;
	static Camera* camera;
	static ID3D12Device* device;

	ObjFileModel* objModel = nullptr;
	ObjFileCharacter* parent = nullptr;
};

