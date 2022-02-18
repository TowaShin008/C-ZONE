#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include"MineMath.h"

class Light
{
private://�G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public://�T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		XMVECTOR lightv;//���C�g�ւ̕�����\���x�N�g��
		Vector3 lightcolor;//���C�g�̐F
	};
private://�ÓI�����o�ϐ�
	//�f�o�C�X
	static ID3D12Device* device;
public://�ÓI�����o�֐�
	///<summary>
	///�ÓI������
	///</summary>
	///<param name="device">�f�o�C�X</param>
	static void StaticInitialize(ID3D12Device* device);
private://�����o�ϐ�
	//�萔�o�b�t�@
	ComPtr<ID3D12Resource>constBuff;
	//���C�g��������(�P�ʃx�N�g��)
	XMVECTOR lightdir = { 1,0,0,0 };
	//���C�g�F
	Vector3 lightcolor = { 1,1,1 };
	//�_�[�e�B�t���O
	bool dirty = false;
public:
	void Initialize();
	void TransferConstBuffer();
	///<summary>
	///���C�g�������Z�b�g
	///</summary>
	///<param name="lightdir">���C�g����</param>
	void SetLightDir(const XMVECTOR& arg_lightdir);

	/// <summary>
	/// ���C�g�J���[�̃Z�b�g
	/// </summary>
	/// <param name="lightcolor"></param>
	void SetLightColor(const Vector3& arg_lightcolor);

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="rootParameterIndex">���[�g�p�����[�^�[�C���f�b�N�X</param>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	/// <summary>
	/// ���C�g��������
	/// </summary>
	/// <returns>���C�g�N���X</returns>
	static Light* Create();
};