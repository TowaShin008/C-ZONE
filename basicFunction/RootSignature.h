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
	/// �e�Ȃ��`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeSimpleRootSignature(ID3D12Device* device);
	/// <summary>
	/// �ʏ�`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeBasicRootSignature(ID3D12Device* device);

	/// <summary>
	/// �X�v���C�g�`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeSpriteRootSignature(ID3D12Device* device);

	/// <summary>
	/// �X�v���C�g�`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeSlideSpriteRootSignature(ID3D12Device* device);

	/// <summary>
	/// �u���[���X�v���C�g�`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeBloomSpriteRootSignature(ID3D12Device* device);

	/// <summary>
	/// FBX�`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeFBXRootSignature(ID3D12Device* device);

	/// <summary>
	/// ���C�g�t���I�u�W�F�N�g�`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeLightObjectRootSignature(ID3D12Device* device);

	/// <summary>
	/// �e�ە`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeBulletRootSignature(ID3D12Device* device);

	/// <summary>
	/// �p�[�e�B�N���`��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeParticleRootSignature(ID3D12Device* device);

	/// <summary>
	/// �u���[���p�k���o�b�t�@�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeShrinkTextureRootSignature(ID3D12Device* device);

	/// <summary>
	/// �t�F�[�h�A�E�g�p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeFadeOutPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// �u���[���p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device"></param>
	static void InitializeBloomPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// ���H�p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeDropNormalMapPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// �Ђъ���Č��p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeGlassNormalMapPostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// �X�L�����m�C�Y�p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeNoisePostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// �|�X�g�G�t�F�N�g�p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializePostEffectRootSignature(ID3D12Device* device);

	/// <summary>
	/// �f�B�]���u�p�̃��[�g�V�O�l�`������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
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

