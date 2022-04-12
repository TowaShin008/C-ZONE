#include "Bloom.h"
#include<d3dx12.h>
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
using namespace DirectX;


Bloom::Bloom() :Sprite(
	100,
	{ 0,0 },
	{ 500.0f,500.0f },
	{ 1,1,1,1 },
	{ 0.0f,0.0f }
)
{

}

void Bloom::Barrier(ID3D12GraphicsCommandList* cmdList,ID3D12Resource* p,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after) {
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(p, before, after, 0));
}

void Bloom::Initialize()
{
	HRESULT result;


	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WindowSize::window_width,
		(UINT)WindowSize::window_height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	
	{
		//テクスチャバッファの生成
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
			IID_PPV_ARGS(&texBuff));
		assert(SUCCEEDED(result));

		const UINT pixelCount = WindowSize::window_width * WindowSize::window_height;

		const UINT rowPitch = sizeof(UINT) * WindowSize::window_width;

		const UINT depthPitch = rowPitch * WindowSize::window_height;

		UINT* img = new UINT[pixelCount];

		for (int j = 0; j < pixelCount; j++) { img[j] = 0xff000000; }

		result = texBuff->WriteToSubresource(0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));

		//テクスチャバッファの生成
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
			IID_PPV_ARGS(&gaussianBuffer));
		assert(SUCCEEDED(result));

		result = gaussianBuffer->WriteToSubresource(0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[]img;
	}


	{
		//テクスチャバッファの生成
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
			IID_PPV_ARGS(&bloomBuffer[0]));
		assert(SUCCEEDED(result));

		const UINT pixelCount = WindowSize::window_width * WindowSize::window_height;

		const UINT rowPitch = sizeof(UINT) * WindowSize::window_width;

		const UINT depthPitch = rowPitch * WindowSize::window_height;

		UINT* img = new UINT[pixelCount];

		for (int j = 0; j < pixelCount; j++) { img[j] = 0xff000000; }

		result = bloomBuffer[0]->WriteToSubresource(0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[]img;
	}

	{
		//テクスチャバッファの生成
		result = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
				D3D12_MEMORY_POOL_L0),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(
				DXGI_FORMAT_R8G8B8A8_UNORM,
				WindowSize::window_width/2,
				(UINT)WindowSize::window_height,
				1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clearColor),
			IID_PPV_ARGS(&bloomBuffer[1]));
		assert(SUCCEEDED(result));

		const UINT pixelCount = WindowSize::window_width * WindowSize::window_height;

		const UINT rowPitch = sizeof(UINT) * WindowSize::window_width;

		const UINT depthPitch = rowPitch * WindowSize::window_height;

		UINT* img = new UINT[pixelCount];

		for (int j = 0; j < pixelCount; j++) { img[j] = 0xff000000; }

		result = bloomBuffer[1]->WriteToSubresource(0, nullptr,
			img, rowPitch, depthPitch);
		assert(SUCCEEDED(result));
		delete[]img;
	}
	



	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 4;

	result = device->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;


	//シェーダーリソースビューの生成
	device->CreateShaderResourceView(texBuff.Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	for (int i = 0; i < 2; i++)
	{
		//シェーダーリソースビューの生成
		device->CreateShaderResourceView(bloomBuffer[i].Get(),
			&srvDesc,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapSRV->GetCPUDescriptorHandleForHeapStart(), i + 1,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}

	//シェーダーリソースビューの生成
	device->CreateShaderResourceView(gaussianBuffer.Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetCPUDescriptorHandleForHeapStart(), 3,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 4;
	result = device->CreateDescriptorHeap(&rtvDescHeapDesc,
		IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));


	//レンダーターゲットビューの生成 通常テクスチャ
	device->CreateRenderTargetView(texBuff.Get(),
		nullptr,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
	);

	for (int i = 0; i < 2; i++)
	{
		//レンダーターゲットビューの生成 高輝度抽出 縮小
		device->CreateRenderTargetView(bloomBuffer[i].Get(),
			nullptr,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i + 1,
				device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
		);
	}

	//レンダーターゲットビューの生成 ガウシアンブラー
	device->CreateRenderTargetView(gaussianBuffer.Get(),
		nullptr,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(), 3,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
	);

	D3D12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			WindowSize::window_width,
			WindowSize::window_height,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	//デプスバッファの生成
	result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descHeapDesc.NumDescriptors = 1;
	//デスクリプタヒープの生成
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(depthBuff.Get(),
		&dsvDesc,
		descHeapDSV->GetCPUDescriptorHandleForHeapStart());

	CreateVertexBuffer();

	CreateConstBuffer();
}

void Bloom::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// 定数バッファにデータ転送
	BlurConstBuffer* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);

	//フェードアウトを有効にした時のみ
	if (fadeOutFlag)
	{
		if (fadeOutCount > 0.0f)
		{
			fadeOutCount -= 0.004f;
		}
	}
	else
	{
		if (fadeOutCount < 1.0f)
		{
			fadeOutCount += 0.004f;
		}
	}

	constMap->effectCount = fadeOutCount;
	constBuff->Unmap(0, nullptr);

	cmdList->SetPipelineState(PipelineState::bloomPostEffectPipelineState.Get());

	cmdList->SetGraphicsRootSignature(RootSignature::bloomPostEffectSignature.Get());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	// デスクリプタヒープをセット
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	//cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), this->texNumber, descriptorSize));
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 0,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);


	cmdList->SetGraphicsRootDescriptorTable(2,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 1,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	cmdList->SetGraphicsRootDescriptorTable(3,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 2,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
	// 描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);
	//ガウシアンブラー描画
	DrawGaussianBlur(cmdList);
	//縮小バッファ描画
	DrawShrinkTextureForBlur(cmdList);
}

void Bloom::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	Barrier(cmdList, texBuff.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	Barrier(cmdList, bloomBuffer[0].Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);


	D3D12_CPU_DESCRIPTOR_HANDLE rtvH[2];
	
	for (int i = 0; i < 2; i++)
	{
		rtvH[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i,
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);
	}
	

	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();

	cmdList->OMSetRenderTargets(2, rtvH, false, &dsvH);


	CD3DX12_VIEWPORT viewports[2];
	CD3DX12_RECT scissorRects[2];

	for (int i = 0; i < 2; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, WindowSize::window_width, WindowSize::window_height);
		scissorRects[i] = CD3DX12_RECT(0, 0, WindowSize::window_width, WindowSize::window_height);
	}

	cmdList->RSSetViewports(2, viewports);

	cmdList->RSSetScissorRects(2, scissorRects);

	for (int i = 0; i < 2; i++)
	{
		cmdList->ClearRenderTargetView(rtvH[i], clearColor, 0, nullptr);
	}

	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Bloom::PostDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	Barrier(cmdList, texBuff.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Bloom::DrawGaussianBlur(ID3D12GraphicsCommandList* cmdList)
{
	Barrier(cmdList, gaussianBuffer.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvBaseHandle = descHeapRTV->GetCPUDescriptorHandleForHeapStart();

	rtvBaseHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)*3;

	//レンダーターゲットセット
	cmdList->OMSetRenderTargets(1, &rtvBaseHandle, false, nullptr);

	auto srvHandle = descHeapSRV->GetGPUDescriptorHandleForHeapStart();

	//テクスチャは1枚目の３つめのレンダーターゲット
	srvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	cmdList->SetGraphicsRootDescriptorTable(1, srvHandle);
	cmdList->SetPipelineState(PipelineState::gaussianPostEffectPipelineState.Get());
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->DrawInstanced(4, 1, 0, 0);

	Barrier(cmdList, gaussianBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Bloom::DrawShrinkTextureForBlur(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(PipelineState::shrinkTexturePipelineState.Get());
	cmdList->SetGraphicsRootSignature(RootSignature::shrinkTextureRootsignature.Get());

	//頂点バッファセット
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList->IASetVertexBuffers(0, 1, &vbView);

	//高輝度成分バッファはシェーダリソースに
	Barrier(cmdList,bloomBuffer[0].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//縮小バッファ(ブルーム用)はレンダーターゲットに
	Barrier(cmdList, bloomBuffer[1].Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);


	auto rtvBaseHandle = descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	auto srvHandle = descHeapSRV->GetGPUDescriptorHandleForHeapStart();

	rtvBaseHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * 2;

	//レンダーターゲットセット
	cmdList->OMSetRenderTargets(1, &rtvBaseHandle, false, nullptr);

	//テクスチャは1枚目の３つめのレンダーターゲット
	srvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//1パス目高輝度をテクスチャとして使用
	cmdList->SetDescriptorHeaps(1, descHeapSRV.GetAddressOf());
	cmdList->SetGraphicsRootDescriptorTable(2, srvHandle);

	auto desc = bloomBuffer[0]->GetDesc();
	D3D12_VIEWPORT vp = {};
	D3D12_RECT sr = {};
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.Height = (FLOAT)desc.Height / 2.0f;
	vp.Width = (FLOAT)desc.Width / 2.0f;
	sr.top = 0;
	sr.left = 0;
	sr.right = (LONG)vp.Width;
	sr.bottom = (LONG)vp.Height;
	for (int i = 0; i < 4; ++i) {
		cmdList->RSSetViewports(1, &vp);
		cmdList->RSSetScissorRects(1, &sr);
		cmdList->DrawInstanced(4, 1, 0, 0);
		//書いたら下にずらして次を書く準備
		sr.top += (LONG)vp.Height;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = (FLOAT)sr.top;
		//幅も高さも半分に
		vp.Width /= 2.0f;
		vp.Height /= 2.0f;
		sr.bottom = sr.top + (LONG)vp.Height;
	}
	//縮小バッファをシェーダリソースにに
	Barrier(cmdList,bloomBuffer[1].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

bool Bloom::CreateConstBuffer() {
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	auto gWeights = GetGaussianValues(8, 5.0f);

	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(
		AligmentedValue((unsigned int)gWeights.size() * (unsigned int)sizeof(gWeights[0]), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)
	);

	auto result = device->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf()));

	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	result = device->CreateDescriptorHeap(&descriptorHeapDesc,
		IID_PPV_ARGS(&descHeapCBV));

	assert(SUCCEEDED(result));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)resDesc.Width;
	device->CreateConstantBufferView(&cbvDesc, descHeapCBV->GetCPUDescriptorHandleForHeapStart());

	float* mappedWeights = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&mappedWeights);
	copy(gWeights.begin(), gWeights.end(), mappedWeights);
	constBuff->Unmap(0, nullptr);
	assert(SUCCEEDED(result));

	return true;

}

bool Bloom::CreateVertexBuffer()
{
	//頂点バッファの生成
	auto result = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	VertexPosUv vertices[vertNum] = {
		{{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},
		{{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},
		{{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},
		{{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},
	};

	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	return true;
}

std::vector<float> Bloom::GetGaussianValues(size_t count, float s) {
	std::vector<float> weight(count);
	float total = 0;
	for (int i = 0; i < count; ++i) {
		float x = static_cast<float>(i);
		auto wgt = expf(-(x * x) / (2 * s * s));
		weight[i] = wgt;
		total += wgt;
	}


	total = total * 2 - weight[0];
	for (auto& wgt : weight) {
		wgt /= total;
	}

	return weight;

}

unsigned int Bloom:: AligmentedValue(unsigned int size, unsigned int alignment) {
	return (size + alignment - (size % alignment));
}

void Bloom::SetFadeOutFlag(bool arg_fadeOutFlag)
{
	if (fadeOutFlag == arg_fadeOutFlag)
	{
		return;
	}

	fadeOutFlag = arg_fadeOutFlag;
	if (fadeOutFlag)
	{
		fadeOutCount = 1.0f;
	}
	else
	{
		fadeOutCount = 0.0f;
	}
}

