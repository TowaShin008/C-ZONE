#pragma once
#include<Windows.h>

#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include<vector>
#include<math.h>
#include<DirectXMath.h>//XMFLOAT
using namespace DirectX;

#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#define DIRECTINPUT_VERSION    0x0800//DirectInputのバージョン指定
#include<dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include<DirectXTex.h>

#include"Input.h"
#include<wrl.h>
#include<d3dx12.h>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include"Light.h"
#include"WindowSize.h"
#include"PipelineState.h"
using namespace Microsoft::WRL;
using namespace std;
//const int window_width = 1280;//横幅
//const int window_height = 720;//縦幅

//static ComPtr<ID3D12PipelineState>pipelineState;
//static XMMATRIX matProjection = XMMatrixOrthographicOffCenterLH(
//	0.0f, window_width, window_height, 0.0f, 0.0f, 1.0f);//射影行列
class LightObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector4 color;	// 色 (RGBA)
		XMMATRIX mat;	// ３Ｄ変換行列
	};
	//マテリアル
	struct Material
	{
		std::string name;
		Vector3 ambient;
		Vector3 diffuse;
		Vector3 specular;
		float alpha;
		std::string textureFilename;
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	// 定数バッファ用データ構造体B0
	struct ConstBufferDataB0
	{
		//XMMATRIX mat;	// ３Ｄ変換行列
		XMMATRIX viewproj;//ビュープロジェクション行列
		XMMATRIX world;//ワールド行列
		Vector3 cameraPos;//カメラ座標(ワールド座標)
	};

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
	LightObject();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LightObject();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void CreateDescriptorHeap(ID3D12Device* device);

	/// <summary>
	/// マテリアル読み込み処理
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイルネーム</param>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// モデルの読み込み処理
	/// </summary>
	/// <param name="modelName">モデル名</param>
	void CreateCharacter(const string& modelName);

	/// <summary>
	/// 定数バッファ生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	void CreateConstBuffer(ID3D12Device* device);
	
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="cmdList">コマンド処理</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 視点座標のセット
	/// </summary>
	/// <param name="eye">視点座標</param>
	void SetEye(const Vector3& eye) { this->eye = eye; }

	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>視点座標</returns>
	const Vector3& GetEye() { return eye; }

	/// <summary>
	/// 注視点座標のセット
	/// </summary>
	/// <param name="target">注視点座標</param>
	void SetTerget(const Vector3& target) { this->target = target; };

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>注視点座標の取得</returns>
	const Vector3& GetTarget() { return target; }

	/// <summary>
	/// ポジションの取得
	/// </summary>
	/// <returns>ポジション</returns>
	const Vector3& GetPosition() { return position; }

	/// <summary>
	/// ポジションのセット
	/// </summary>
	/// <param name="position">ポジション</param>
	void SetPosition(const Vector3& position) { this->position = position; }

	/// <summary>
	/// 死亡フラグの取得
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool GetIsDeadFlag() { return isDeadFlag; }

	/// <summary>
	/// 死亡判定セット
	/// </summary>
	/// <param name="isDeadFlag"></param>
	void SetIsDeadFlag(bool isDeadFlag) { this->isDeadFlag = isDeadFlag; }

	/// <summary>
	/// スケールの取得
	/// </summary>
	/// <returns>スケール</returns>
	const Vector3& GetScale() { return scale; }

	/// <summary>
	/// スケールのセット
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) { this->scale = scale; }

	/// <summary>
	/// 回転角度の取得
	/// </summary>
	/// <returns>回転角度</returns>
	const Vector3& GetRotasion() { return rotation; }

	/// <summary>
	/// 回転角度のセット
	/// </summary>
	/// <param name="rotation">回転角度</param>
	void SetRotation(const Vector3& rotation) { this->rotation = rotation; }

	/// <summary>
	/// カラーの取得
	/// </summary>
	/// <returns>カラー</returns>
	const Vector4& GetColor() { return color; }

	/// <summary>
	/// カラーのセット
	/// </summary>
	/// <param name="color">カラー</param>
	void SetColor(const Vector4& color) { this->color = color; }

	/// <summary>
	/// ライトオブジェクトのセット
	/// </summary>
	/// <param name="light">ライトオブジェクト</param>
	static void SetLight(Light* light) {
		LightObject::light = light;
	}

	/// <summary>
	/// ２つ目のライトオブジェクトのセット
	/// </summary>
	/// <param name="light_2">ライトオブジェクト</param>
	static void SetLight_2(Light* light_2) {
		LightObject::light_2 = light_2;
	}

	//マテリアル
	static Material material;
private:
	static ID3D12Device* device;
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	static ComPtr<ID3D12DescriptorHeap>descHeap;
	static ComPtr<ID3D12Resource>vertBuff;
	static ComPtr<ID3D12Resource>indexBuff;
	static ComPtr<ID3D12Resource>texBuff;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//カメラ関係
	static XMMATRIX matView;
	static XMMATRIX matProjection;
	static Vector3 eye;
	static Vector3 target;
	static Vector3 up;

	//ライト
	static Light* light;
	static Light* light_2;

	static D3D12_VERTEX_BUFFER_VIEW vbView;
	static D3D12_INDEX_BUFFER_VIEW ibView;
	static vector<VertexPosNormalUv>vertices;
	static vector<unsigned short>indices;
	Vector3 position = { 0.0f,0.0f,0.0f };
	XMMATRIX matWorld;
	Vector3 scale = { 2.0f,2.0f,2.0f };
	Vector3 rotation = { 0.0f,0.0f,0.0f };
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	bool isDeadFlag;
public:
	static LightObject* Create(ID3D12Device* device);
};

