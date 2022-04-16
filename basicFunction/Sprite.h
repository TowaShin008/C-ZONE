#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include"WindowSize.h"
#include"MineMath.h"
#include"PipelineState.h"
#include<map>
#define LENGTH_LINK 100

class Sprite
{
protected:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMMATRIX = DirectX::XMMATRIX;

public:
	struct VertexPosUv
	{
		Vector3 pos; // xyz座標
		Vector2 uv;  // uv座標
	};

	struct ConstBufferData
	{
		Vector4 color;	// 色 (RGBA)
		XMMATRIX mat;	// ３Ｄ変換行列
		float slideUVCount;
	};

public:
	/// <summary>
	/// グラフィックスパイプライン初期化処理
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">スクリーンの横幅</param>
	/// <param name="window_height">スクリーンの縦幅</param>
	/// <returns>処理の可否</returns>
	static bool CreateDescriptorHeap(ID3D12Device* arg_device);
	
	/// <summary>
	/// テクスチャロード処理
	/// </summary>
	/// <param name="texnumber">テクスチャ番号</param>
	/// <param name="filename">ファイルネーム</param>
	/// <returns>処理の可否</returns>
	static bool LoadTexture(const wchar_t*filename);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	static void BeginDraw(ID3D12GraphicsCommandList* arg_cmdList,bool bloomFlag);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void EndDraw();

	/// <summary>
	/// 生成処理
	/// </summary>
	/// <param name="arg_texNumber">テクスチャ番号</param>
	/// <param name="arg_position">ポジション</param>
	/// <param name="arg_color">カラー</param>
	/// <param name="arg_anchorpoint">アンカーポイント</param>
	/// <param name="arg_isFlipX">上下反転するか</param>
	/// <param name="arg_isFlipY">左右反転するか</param>
	/// <returns>スプライト</returns>
	static Sprite* Create(const wchar_t* arg_filename, const Vector2& arg_position, const Vector4& arg_color = { 1, 1, 1, 1 }, const Vector2& arg_anchorpoint = { 0.0f, 0.0f }, bool arg_isFlipX = false, bool arg_isFlipY = false);

protected:
	static const int srvCount = 512;
	static const int vertNum = 4;
	static ID3D12Device* device;
	static UINT descriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	static XMMATRIX matProjection;
	static ComPtr<ID3D12DescriptorHeap> descHeap;
	static std::map<const wchar_t*,int> texKey;
	static ComPtr<ID3D12Resource> texBuff[srvCount];
	static int texNum;

public:
	Sprite(const UINT& arg_texNumber, const Vector2& arg_position, const Vector2& arg_size, const Vector4& arg_color, const Vector2& arg_anchorpoint = { 0, 0 });
	bool Initialize();
	void Draw();
	void SetRotation(float arg_rotation);
	void SetPosition(const Vector2& arg_position);
	void SetAnchorPoint(const Vector2& arg_anchorpoint);
	void SetScale(const Vector2& arg_scale);
	const Vector2& GetPosition() { return position; }

protected: // メンバ変数
	ComPtr<ID3D12Resource> vertBuff;
	ComPtr<ID3D12Resource> constBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	int texNumber = 0;
	float rotation = 0.0f;
	Vector2 position{};
	Vector2 scale = { 100.0f, 100.0f };
	Vector2 anchorpoint = { 0, 0 };
	XMMATRIX matWorld{};
	Vector4 color = { 1, 1, 1, 1 };
	Vector2 texBase = { 0, 0 };
	Vector2 texScale = { 100.0f, 100.0f };

private:

	void UpdateVertices();
	Vector2 velocity = { 0.0f,0.0f };
	Vector2 speed = { 0.0f,0.0f };
	float initSpeed = 5.0f;
	Vector2 accel = { 0.0f,0.0f };
	float fX = 0.0f;
	float fY = 0.0f;
	float g = 0.0f;
	float m = 1.0f;
	float e = 1.0f;
	float time = 0.0f;
	int lugTime = 0;
	int collisionLugtime = 30;
	int changeLugTime = 0;
	int window_width = WindowSize::window_width;
	int window_height = WindowSize::window_height;
	Sprite* parentTop = nullptr;
	Sprite* parentButtom = nullptr;
	bool collisionFlag = true;
	float slideUVCount = 0.0f;
	bool rightSlideFlag = false;
	bool slideFlag = false;
	int stayInvisibleTime = 60;
	bool alphaFlag = false;
public:
	void MoveUpdate();
	void GradatioUpdate();
	void WireUpdate();
	void EndingMoveUpdate();
	void SetColor(const Vector4& arg_color) { color = arg_color; }
	void SetLugTime(int arg_lugTime) { lugTime = arg_lugTime; changeLugTime = arg_lugTime; }
	void SetVelocity(const Vector2& arg_velocity);
	void SetSpeedX(const Vector2& arg_speed) { speed = arg_speed; }
	Vector4 GetColor() { return color; }
	Vector2 GetSpeed() { return speed; }
	Vector2 GetAccel() { return accel; }
	void SetSpeedX(float arg_speedX) { speed.x = arg_speedX; }
	float GetSpeedX() { return speed.x; }
	float GetSpeedY() { return speed.y; }
	float GetM() { return m; }
	float GetFX() { return fX; }
	float GetFY() { return fY; }
	float GetG() { return g; }
	void SetTextureRect(const Vector2& arg_texBase,const Vector2& arg_texScale);
	bool EndingCollision(Sprite* arg_otherBall, float radius);
	void SetSlideTextureFlag(bool arg_slideFlag, bool arg_rightSlideFlag);
	void SetAlphaFlag(bool arg_alphaFlag) { alphaFlag = arg_alphaFlag; }
};