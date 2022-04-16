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
	/// �e�Ȃ��`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeSimplePipelineState(ID3D12Device* device);
	/// <summary>
	/// �ʏ�`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeBasicPipelineState(ID3D12Device* device);

	/// <summary>
	/// �X�v���C�g�`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeSpritePipelineState(ID3D12Device* device);

	/// <summary>
	/// �X���C�h����X�v���C�g�`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeSlideSpritePipelineState(ID3D12Device* device);

	/// <summary>
	/// �u���[���X�v���C�g�`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeBloomSpritePipelineState(ID3D12Device* device);

	/// <summary>
	/// FBX�`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeFBXPipelineState(ID3D12Device* device);

	/// <summary>
	/// ���C�g�t���I�u�W�F�N�g�`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeLightObjectPipelineState(ID3D12Device* device);

	/// <summary>
	/// �e�ە`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeBulletPipelineState(ID3D12Device* device);

	/// <summary>
	/// �p�[�e�B�N���`��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeParticlePipelineState(ID3D12Device* device);

	/// <summary>
	/// �u���[���p�k���o�b�t�@�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeShrinkTexturePipelineState(ID3D12Device* device);

	/// <summary>
	/// �t�F�[�h�A�E�g�p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeFadeOutPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// �K�E�V�A���ڂ����p�̃p�C�v���C������
	/// </summary>
	/// <param name="device"></param>
	static void InitializeGaussianPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// �u���[���p�̃p�C�v���C������
	/// </summary>
	/// <param name="device"></param>
	static void InitializeBloomPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// ���H�p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeDropNormalMapPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// �Ђъ���Č��p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeGlassNormalMapPostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// �X�L�����m�C�Y�p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializeNoisePostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// �|�X�g�G�t�F�N�g�p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	static void InitializePostEffectPipelineState(ID3D12Device* device);

	/// <summary>
	/// �f�B�]���u�p�̃p�C�v���C������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
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

	//�u���[���p�O���t�B�b�N�X�p�C�v���C��
	//ComPtr<ID3D12PipelineState>bloomPipelineState;
};

