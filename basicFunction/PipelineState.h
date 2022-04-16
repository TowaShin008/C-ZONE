#pragma once
#include<d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include<d3dx12.h>
#include<wrl.h>
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include<string>
#include<DirectXMath.h>
#include<math.h>
#include"RootSignature.h"
#include"Helper.h"
using namespace Microsoft::WRL;
using namespace DirectX;

class PipelineState
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	/// <summary>
	/// 影なし描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeSimplePipelineState(ID3D12Device* device);
	/// <summary>
	/// 通常描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeBasicPipelineState(ID3D12Device* device);

	/// <summary>
	/// スプライト描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeSpritePipelineState(ID3D12Device* device);

	/// <summary>
	/// スライドするスプライト描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeSlideSpritePipelineState(ID3D12Device* device);

	/// <summary>
	/// ブルームスプライト描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeBloomSpritePipelineState(ID3D12Device* device);

	/// <summary>
	/// FBX描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeFBXPipelineState(ID3D12Device* device);

	/// <summary>
	/// ライト付きオブジェクト描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeLightObjectPipelineState(ID3D12Device* device);

	/// <summary>
	/// 弾丸描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeBulletPipelineState(ID3D12Device* device);

	/// <summary>
	/// パーティクル描画用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeParticlePipelineState(ID3D12Device* device);

	/// <summary>
	/// ブルーム用縮小バッファのパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeShrinkTexturePipelineState(ID3D12Device* device);

	/// <summary>
	/// フェードアウト用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeFadeOutPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// ガウシアンぼかし用のパイプライン生成
	/// </summary>
	/// <param name="device"></param>
	static void InitializeGaussianPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// ブルーム用のパイプライン生成
	/// </summary>
	/// <param name="device"></param>
	static void InitializeBloomPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// 水滴用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeDropNormalMapPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// ひび割れ再現用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeGlassNormalMapPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// スキャンノイズ用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeNoisePostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// ポストエフェクト用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializePostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// ディゾルブ用のパイプライン生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeDissolvePostEffectPipelineState(ID3D12Device* device);

	static ComPtr<ID3D12PipelineState> simplePipelineState;
	static ComPtr<ID3D12PipelineState> basicPipelineState;
	static ComPtr<ID3D12PipelineState> spritePipelineState;
	static ComPtr<ID3D12PipelineState> slideSpritePipelineState;
	static ComPtr<ID3D12PipelineState> bloomSpritePipelineState;
	static ComPtr<ID3D12PipelineState> fbxPipelineState;
	static ComPtr<ID3D12PipelineState> lightObjectPipelineState;
	static ComPtr<ID3D12PipelineState> bulletPipelineState;
	static ComPtr<ID3D12PipelineState> particlePipelineState;
	static ComPtr<ID3D12PipelineState> shrinkTexturePipelineState;
	static ComPtr<ID3D12PipelineState> fadeOutPostEffectPipelineState;
	static ComPtr<ID3D12PipelineState> gaussianPostEffectPipelineState;
	static ComPtr<ID3D12PipelineState> bloomPostEffectPipelineState;
	static ComPtr<ID3D12PipelineState> dropNormalMapPostEffectPipelineState;
	static ComPtr<ID3D12PipelineState> glassNormalMapPostEffectPipelineState;
	static ComPtr<ID3D12PipelineState> noisePostEffectPipelineState;
	static ComPtr<ID3D12PipelineState> postEffectPipelineState;
	static ComPtr<ID3D12PipelineState> dissolvePostEffectPipelineState;

	//ブルーム用グラフィックスパイプライン
	//ComPtr<ID3D12PipelineState>bloomPipelineState;
};

