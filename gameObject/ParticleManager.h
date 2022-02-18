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
	bool Initialize(ID3D12Device* device);

	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	static void PostDraw();

	static ParticleManager* Create(ID3D12Device* device);

	const Vector3& GetEye() { return eye; }

	void SetEye(const Vector3& eye);

	const Vector3& GetTarget() { return target; }

	void SetTarget(const Vector3& target);

	void CameraMoveVector(const Vector3& move);
	void CameraMoveEyeVector(const Vector3& move);

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
	bool InitializeDescriptorHeap();
	void InitializeCamera();
	bool LoadTexture();
	void CreateModel();
	void UpdateViewMatrix();

public:
	bool CreateConstBuff();
	void Update();
	void Draw();
	void ChangeSelectColor();
	void SetSelectColor(int arg_selectColor) { selectColor = arg_selectColor; }

private: // メンバ変数
	ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	int selectColor = 0;
	
	std::forward_list<Particle>particles;
	public:
		UINT GetParticleLength()
		{
			return (UINT)std::distance(particles.begin(), particles.end());
		}
		void Add(int life, const Vector3& position, const Vector3& velocity, const Vector3& accel,float start_scale,float end_scale, const Vector4& start_color, const Vector4& end_color);
};

