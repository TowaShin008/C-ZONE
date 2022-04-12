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

#define DIRECTINPUT_VERSION    0x0800//DirectInput�̃o�[�W�����w��
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
private: // �����o�ϐ�
// �E�B���h�E�Y�A�v���P�[�V�����Ǘ�

	// Direct3D�֘A
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
	/// �R���X�g���N�^
	/// </summary>
	DirectXBase();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~DirectXBase();

	/// <summary>
	/// �E�B���h�E�v���V�[�W��
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void CreateAllObject();
	
	/// <summary>
	/// �E�B���h�E�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateGameWindow();

	/// <summary>
	/// �f�o�C�X�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateDevice();

	/// <summary>
	/// �X���b�v�`�F�[���̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateSwapChain();

	/// <summary>
	/// �R�}���h���X�g�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateCMD();

	/// <summary>
	/// �����_�[�^�[�Q�b�g�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateRenderTarget();

	/// <summary>
	/// �f�v�X�o�b�t�@�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateDepthBuffer();

	/// <summary>
	/// ���b�Z�[�W�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateMessage();

	/// <summary>
	/// �[�x�̃N���A
	/// </summary>
	void DepthClear();

	/// <summary>
	/// �`��O����
	/// </summary>
	void BeginDraw();

	/// <summary>
	/// �`��㏈��
	/// </summary>
	void EndDraw();

	/// <summary>
	/// �t�F���X�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateFence();

	/// <summary>
	/// �E�B���h�E�̔j��
	/// </summary>
	void EndWindow();

	/// <summary>
	/// Imgui�̏�����
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateImgui();

	/// <summary>
	/// �f�o�C�X�̎擾
	/// </summary>
	/// <returns>�f�o�C�X</returns>
	ComPtr<ID3D12Device> GetDevice() { return device; }

	/// <summary>
	/// �R�}���h���X�g�̎擾
	/// </summary>
	/// <returns>�R�}���h���X�g</returns>
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return cmdList; }

	/// <summary>
	/// HWND�N���X�̎擾
	/// </summary>
	/// <returns>HWND�N���X</returns>
	const HWND& Gethwnd() { return hwnd; }

	/// <summary>
	/// �E�B���h�E�N���X�̎擾
	/// </summary>
	/// <returns>�E�B���h�E�N���X</returns>
	const WNDCLASSEX& GetWndClass() { return wndClass; }

	/// <summary>
	/// �N���A�J���[�̃Z�b�g
	/// </summary>
	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
private:
	HWND hwnd = nullptr;
	WNDCLASSEX wndClass{};
	ComPtr<ID3D12DescriptorHeap> imguiHeap;
};