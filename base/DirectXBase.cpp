#include "DirectXBase.h"
//FPS�Œ�p
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include<sstream>


#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT DirectXBase::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return 1;
	}
	// ���b�Z�[�W�ŕ���
	switch (msg)
	{
	case WM_DESTROY: // �E�B���h�E���j�����ꂽ
		PostQuitMessage(0); // OS�ɑ΂��āA�A�v���̏I����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // �W���̏������s��
}

DirectXBase::DirectXBase()
{
}

DirectXBase::~DirectXBase()
{
}

void DirectXBase::CreateAllObject()
{
	//�E�B���h�E�̐���
	CreateGameWindow();
	//�f�o�C�X�̐���
	CreateDevice();
	//�R�}���h���X�g�̐���
	CreateCMD();
	//�X���b�v�`�F�[���̐���
	CreateSwapChain();
	//�����_�[�^�[�Q�b�g�̐���
	CreateRenderTarget();
	//�[�x�o�b�t�@�̐���
	CreateDepthBuffer();
	//�t�F���X�̐���
	CreateFence();
	//Imgui�̐���
	CreateImgui();
}

bool DirectXBase::CreateGameWindow()
{
	// �E�B���h�E�N���X�̐ݒ�
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WindowProc; // �E�B���h�E�v���V�[�W��
	wndClass.lpszClassName = L"MineLib"; // �E�B���h�E�N���X��
	wndClass.hInstance = GetModuleHandle(nullptr); // �E�B���h�E�n���h��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW); // �J�[�\���w��

	RegisterClassEx(&wndClass); // �E�B���h�E�N���X��OS�ɓo�^

	// �E�B���h�E�T�C�Y{ X���W Y���W ���� �c�� }
	RECT wrc = { 0, 0, WindowSize::window_width, WindowSize::window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false); // �����ŃT�C�Y�␳

	// �E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(wndClass.lpszClassName, // �N���X��
		L"MineLib",			// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	// �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			// �\��X���W�iOS�ɔC����j
		CW_USEDEFAULT,			// �\��Y���W�iOS�ɔC����j
		wrc.right - wrc.left,	// �E�B���h�E����
		wrc.bottom - wrc.top,	// �E�B���h�E�c��
		nullptr,				// �e�E�B���h�E�n���h��
		nullptr,				// ���j���[�n���h��
		wndClass.hInstance,			// �Ăяo���A�v���P�[�V�����n���h��
		nullptr);				// �I�v�V����

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);
	if (hwnd == nullptr)
	{
		assert(0);
	}
	return true;
}

bool DirectXBase::CreateDevice()
{
	HRESULT result = S_FALSE;

#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugController;
	//�f�o�b�O���C���[���I����	
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	// �Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter1>> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter1> tmpAdapter;
	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter);	// ���I�z��ɒǉ�����
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);	// �A�_�v�^�[�̏����擾

		// �\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		std::wstring strDesc = adesc.Description;	// �A�_�v�^�[��

		// Intel UHD Graphics�i�I���{�[�h�O���t�B�b�N�j�����
		if (strDesc.find(L"Intel") == std::wstring::npos)
		{
			tmpAdapter = adapters[i];	// �̗p
			break;
		}
	}

	D3D_FEATURE_LEVEL featureLevel;

	result = S_FALSE;
	for (int i = 0; i < _countof(levels); i++)
	{
		// �f�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&device));
		if (SUCCEEDED(result))
		{
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	return true;
}

bool DirectXBase::CreateSwapChain()
{
	HRESULT result = S_FALSE;

	// �e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = WindowSize::window_width;
	swapchainDesc.Height = WindowSize::window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �F���̏�������ʓI�Ȃ��̂�
	swapchainDesc.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;	// �o�b�N�o�b�t�@�Ƃ��Ďg����悤��
	swapchainDesc.BufferCount = 2;	// �o�b�t�@�����Q�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �t���b�v��͑��₩�ɔj��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // �t���X�N���[���؂�ւ�������	
	ComPtr<IDXGISwapChain1> swapchain1;
	//HWND hwnd = this->hwnd;
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);
	if (FAILED(result)) {
		assert(0);
		return false;
	}
	swapchain1.As(&swapchain);

	return true;
}

bool DirectXBase::CreateCMD()
{
	HRESULT result = S_FALSE;

	// �R�}���h�A���P�[�^�𐶐�
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �R�}���h���X�g�𐶐�
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	result = device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	return true;
}

bool DirectXBase::CreateRenderTarget()
{
	HRESULT result = S_FALSE;

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �e��ݒ�����ăf�B�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// �����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = swcDesc.BufferCount;
	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// ���\�̂Q���ɂ���
	backBuffers.resize(swcDesc.BufferCount);
	for (int i = 0; i < backBuffers.size(); i++)
	{
		// �X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (FAILED(result)) {
			assert(0);
			return false;
		}

		// �f�B�X�N���v�^�q�[�v�̃n���h�����擾
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), i, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		// �����_�[�^�[�Q�b�g�r���[�̐���
		device->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);
	}

	return true;
}

bool DirectXBase::CreateDepthBuffer()
{
	HRESULT result = S_FALSE;

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WindowSize::window_width,
		WindowSize::window_height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	// ���\�[�X�̐���
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // �[�x�l�������݂Ɏg�p
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1; // �[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // �f�v�X�X�e���V���r���[
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // �[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool DirectXBase::CreateMessage()
{
	MSG msg{};	// ���b�Z�[�W

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // ���b�Z�[�W������H
	{
		TranslateMessage(&msg); // �L�[���̓��b�Z�[�W�̏���
		DispatchMessage(&msg); // �E�B���h�E�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
	}

	if (msg.message == WM_QUIT) // �I�����b�Z�[�W�������烋�[�v�𔲂���
	{
		return true;
	}

	return false;
}

void DirectXBase::BeginDraw()
{
	// �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	// ���\�[�X�o���A��ύX�i�\����ԁ��`��Ώہj
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), bbIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());
	// �����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);


	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// �[�x�o�b�t�@�̃N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// �r���[�|�[�g�̐ݒ�
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, WindowSize::window_width, WindowSize::window_height));
	// �V�U�����O��`�̐ݒ�
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, WindowSize::window_width, WindowSize::window_height));
}

void DirectXBase::EndDraw()
{
	// ���\�[�X�o���A��ύX�i�`��Ώہ��\����ԁj
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// ���߂̃N���[�Y
	cmdList->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdLists[] = { cmdList.Get() }; // �R�}���h���X�g�̔z��
	cmdQueue->ExecuteCommandLists(1, cmdLists);

	// �o�b�t�@���t���b�v
	swapchain->Present(1, 0);

	// �R�}���h���X�g�̎��s������҂�
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	cmdAllocator->Reset(); // �L���[���N���A
	cmdList->Reset(cmdAllocator.Get(), nullptr);	// �ĂуR�}���h���X�g�𒙂߂鏀��
}

void DirectXBase::DepthClear()
{
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());

	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

bool DirectXBase::CreateFence()
{
	HRESULT result = S_FALSE;

	// �t�F���X�̐���
	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	return true;
}

void DirectXBase::EndWindow()
{
	//�E�B���h�E�N���X��o�^����
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

bool DirectXBase::CreateImgui()
{
	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&imguiHeap));
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �X���b�v�`�F�[���̏����擾
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	if (ImGui::CreateContext() == nullptr) {
		assert(0);
		return false;
	}
	if (!ImGui_ImplWin32_Init(Gethwnd())) {
		assert(0);
		return false;
	}
	if (!ImGui_ImplDX12_Init(
		GetDevice().Get(),
		swcDesc.BufferCount,
		swcDesc.BufferDesc.Format,
		imguiHeap.Get(),
		imguiHeap->GetCPUDescriptorHandleForHeapStart(),
		imguiHeap->GetGPUDescriptorHandleForHeapStart()
	)) {
		assert(0);
		return false;
	}

	return true;
}
