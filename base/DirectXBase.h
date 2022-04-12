#pragma once
#include<Windows.h>

#include<d3d12.h>
#include<dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include<vector>
#include<math.h>
#include<DirectXMath.h>
using namespace DirectX;

#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#define DIRECTINPUT_VERSION    0x0800//DirectInputのバージョン指定
#include<dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include<DirectXTex.h>

#include<wrl.h>
#include<d3dx12.h>
#include<vector>
#include<imgui.h>
#include"WindowSize.h"
#include"Input.h"
using namespace Microsoft::WRL;

class DirectXBase
{
private: // メンバ変数
// ウィンドウズアプリケーション管理

	// Direct3D関連
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<IDXGISwapChain4> swapchain;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DirectXBase();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DirectXBase();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void CreateAllObject();
	
	/// <summary>
	/// ウィンドウの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateGameWindow();

	/// <summary>
	/// デバイスの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateDevice();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateSwapChain();

	/// <summary>
	/// コマンドリストの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateCMD();

	/// <summary>
	/// レンダーターゲットの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateRenderTarget();

	/// <summary>
	/// デプスバッファの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateDepthBuffer();

	/// <summary>
	/// メッセージの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateMessage();

	/// <summary>
	/// 深度のクリア
	/// </summary>
	void DepthClear();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void BeginDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void EndDraw();

	/// <summary>
	/// フェンスの生成
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateFence();

	/// <summary>
	/// ウィンドウの破棄
	/// </summary>
	void EndWindow();

	/// <summary>
	/// Imguiの初期化
	/// </summary>
	/// <returns>生成の可否</returns>
	bool CreateImgui();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ComPtr<ID3D12Device> GetDevice() { return device; }

	/// <summary>
	/// コマンドリストの取得
	/// </summary>
	/// <returns>コマンドリスト</returns>
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return cmdList; }

	/// <summary>
	/// HWNDクラスの取得
	/// </summary>
	/// <returns>HWNDクラス</returns>
	const HWND& Gethwnd() { return hwnd; }

	/// <summary>
	/// ウィンドウクラスの取得
	/// </summary>
	/// <returns>ウィンドウクラス</returns>
	const WNDCLASSEX& GetWndClass() { return wndClass; }

	/// <summary>
	/// クリアカラーのセット
	/// </summary>
	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
private:
	HWND hwnd = nullptr;
	WNDCLASSEX wndClass{};
	ComPtr<ID3D12DescriptorHeap> imguiHeap;
};