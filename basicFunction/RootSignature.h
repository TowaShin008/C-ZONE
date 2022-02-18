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
using namespace Microsoft::WRL;
using namespace DirectX;

class RootSignature
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	/// <summary>
	/// 影なし描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeSimpleRootSignature(ID3D12Device* device);
	/// <summary>
	/// 通常描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeBasicRootSignature(ID3D12Device* device);

	/// <summary>
	/// スプライト描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeSpriteRootSignature(ID3D12Device* device);

	/// <summary>
	/// スプライト描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeSlideSpriteRootSignature(ID3D12Device* device);

	/// <summary>
	/// ブルームスプライト描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeBloomSpriteRootSignature(ID3D12Device* device);

	/// <summary>
	/// FBX描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeFBXRootSignature(ID3D12Device* device);

	/// <summary>
	/// ライト付きオブジェクト描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeLightObjectRootSignature(ID3D12Device* device);

	/// <summary>
	/// 弾丸描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeBulletRootSignature(ID3D12Device* device);

	/// <summary>
	/// パーティクル描画用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeParticleRootSignature(ID3D12Device* device);

	/// <summary>
	/// ブルーム用縮小バッファのルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeShrinkTextureRootSignature(ID3D12Device* device);

	/// <summary>
	/// フェードアウト用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeFadeOutPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// ブルーム用のルートシグネチャ生成
	/// </summary>
	/// <param name="device"></param>
	static void InitializeBloomPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// 水滴用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeDropNormalMapPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// ひび割れ再現用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeGlassNormalMapPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// スキャンノイズ用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeNoisePostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// ポストエフェクト用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializePostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// ディゾルブ用のルートシグネチャ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	static void InitializeDissolvePostEffectRootSignature(ID3D12Device* device);

	static ComPtr<ID3D12RootSignature>simpleRootsignature;
	static ComPtr<ID3D12RootSignature>basicRootsignature;
	static ComPtr<ID3D12RootSignature>spriteRootsignature;
	static ComPtr<ID3D12RootSignature>slideSpriteRootsignature;
	static ComPtr<ID3D12RootSignature>bloomSpriteRootsignature;
	static ComPtr<ID3D12RootSignature>fbxRootsignature;
	static ComPtr<ID3D12RootSignature>lightObjectRootsignature;
	static ComPtr<ID3D12RootSignature>bulletRootsignature;
	static ComPtr<ID3D12RootSignature>particleRootsignature;
	static ComPtr<ID3D12RootSignature>shrinkTextureRootsignature;
	static ComPtr<ID3D12RootSignature>fadeOutPostEffectSignature;
	static ComPtr<ID3D12RootSignature>bloomPostEffectSignature;
	static ComPtr<ID3D12RootSignature>dropNormalMapPostEffectSignature;
	static ComPtr<ID3D12RootSignature>glassNormalMapPostEffectSignature;
	static ComPtr<ID3D12RootSignature>noisePostEffectSignature;
	static ComPtr<ID3D12RootSignature>postEffectSignature;
	static ComPtr<ID3D12RootSignature>dissolvePostEffectSignature;
};

