#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include"Input.h"
#include<forward_list>
#include"PipelineState.h"
#include"WindowSize.h"
#define BLUE 0
#define YELLOW 1
#define RED 2
class ParticleManager
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	struct VertexPos
	{
		Vector3 pos;//xyz座標
		float scale;//スケール
		Vector4 color;//スケール
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		//XMFLOAT4 color;	// 色 (RGBA)
		XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX matBillboard;//ビルボード行列
	};
	struct Particle
	{
		//座標	
		Vector3 position = {};
		//速度
		Vector3 velocity = {};
		//加速度
		Vector3 accel = {};

		//スケール
		Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
		//初期値
		Vector4 s_color = { 0.0f,0.0f,0.0f,0.0f };
		//最終値
		Vector4 e_color = { 1.0f,1.0f,1.0f,1.0f };

		//スケール
		float scale = 1.0f;
		//初期値
		float s_scale = 1.0f;
		//最終値
		float e_scale = 0.0f;
		//現在フレーム
		int frame = 0;
		//終了フレーム
		int num_frame = 0;
	};

public:
	static const int vertexCount = 80;
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <returns>生成の可否</returns>
	bool Initialize(ID3D12Device* arg_device);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="arg_cmdList"></param>
	static void PreDraw(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// オブジェクトの生成
	/// </summary>
	/// <param name="arg_device">デバイス</param>
	/// <returns>キャラクターオブジェクト</returns>
	static ParticleManager* Create(ID3D12Device* arg_device);

	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>視点座標</returns>
	const Vector3& GetEye() { return eye; }

	/// <summary>
	/// 視点座標のセット
	/// </summary>
	/// <param name="arg_eye">視点座標</param>
	void SetEye(const Vector3& arg_eye);

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>注視点座標</returns>
	const Vector3& GetTarget() { return target; }

	/// <summary>
	/// 注視点座標のセット
	/// </summary>
	/// <param name="arg_target">注視点座標</param>
	void SetTarget(const Vector3& arg_target);

	/// <summary>
	/// 視点座標と注視点座標のスクロール
	/// </summary>
	/// <param name="arg_move">スクロール量</param>
	void CameraMoveVector(const Vector3& arg_move);

	/// <summary>
	/// 視点座標のスクロール
	/// </summary>
	/// <param name="arg_move">スクロール量</param>
	void CameraMoveEyeVector(const Vector3& arg_move);

private:
	ID3D12Device* device;
	UINT descriptorHandleIncrementSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12DescriptorHeap> descHeap;
	ComPtr<ID3D12Resource> vertBuff;
	ComPtr<ID3D12Resource> texbuff;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	XMMATRIX matView;
	XMMATRIX matProjection;
	Vector3 eye = { 0, 0, -20.2f };
	Vector3 target = { 0, 0, 0 };
	Vector3 up = { 0, 1, 0 };
	D3D12_VERTEX_BUFFER_VIEW vbView;
	VertexPos vertices[vertexCount];
	XMMATRIX matBillboard;
	XMMATRIX matBillboardY;

private:
	/// <summary>
	/// デスクリプターヒープの生成
	/// </summary>
	/// <returns></returns>
	bool InitializeDescriptorHeap();

	/// <summary>
	/// カメラの初期化
	/// </summary>
	void InitializeCamera();

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <returns></returns>
	bool LoadTexture();

	/// <summary>
	/// モデルの生成
	/// </summary>
	void CreateModel();

	/// <summary>
	/// ビュー行列の更新
	/// </summary>
	void UpdateViewMatrix();

public:

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateConstBuff();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// カラーの切り替え処理
	/// </summary>
	void ChangeSelectColor();

	/// <summary>
	/// カラーのセット
	/// </summary>
	/// <param name="arg_selectColor">カラー 0:BLUE 1:YELLOW 2:RED</param>
	void SetSelectColor(int arg_selectColor) { selectColor = arg_selectColor; }

private: // メンバ変数
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	int selectColor = 0;
	
	std::forward_list<Particle>particles;
public:
	UINT GetParticleLength(){ return (UINT)std::distance(particles.begin(), particles.end()); }

	/// <summary>
	/// パーティクルの追加
	/// </summary>
	/// <param name="arg_life">生存時間</param>
	/// <param name="arg_position">ポジション</param>
	/// <param name="arg_velocity">移動方向</param>
	/// <param name="arg_accel">加速度</param>
	/// <param name="arg_start_scale">開始スケール</param>
	/// <param name="arg_end_scale">終了スケール</param>
	/// <param name="arg_start_color">開始カラー</param>
	/// <param name="arg_end_color">終了カラー</param>
	void Add(int life, const Vector3& arg_position, const Vector3& arg_velocity, const Vector3& arg_accel,float arg_start_scale,float arg_end_scale, const Vector4& arg_start_color, const Vector4& arg_end_color);
};

