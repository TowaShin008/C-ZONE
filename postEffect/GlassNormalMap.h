#pragma once
#include "Sprite.h"
#include"Input.h"
class GlassNormalMap : public Sprite
{
public:
	struct BlurConstBuffer
	{
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GlassNormalMap();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="input">�C���v�b�g�N���X</param>
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �N���A�J���[�̃Z�b�g
	/// </summary>
	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
private:
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource>texBuff;
	ComPtr<ID3D12Resource>effectTexBuff;
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource>depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;
};

