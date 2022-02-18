#pragma once
#include "Sprite.h"
#include"Input.h"
#include<vector>
class Bloom : public Sprite
{
public:
	/// <summary>
	/// �萔�o�b�t�@�̍\����
	/// </summary>
	struct BlurConstBuffer
	{
		Vector4 weights[2];
		float effectCount;
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Bloom();

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

	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };

	/// <summary>
	/// ���P�x�k���`��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void DrawShrinkTextureForBlur(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �K�E�V�A���u���[�`��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void DrawGaussianBlur(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �o���A�֐�
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="p">�e�N�X�`���o�b�t�@</param>
	/// <param name="before">�O�̏��</param>
	/// <param name="after">��̏��</param>
	void Barrier(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* p,
		D3D12_RESOURCE_STATES before,
		D3D12_RESOURCE_STATES after);

	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	/// <returns>�����̉�</returns>
	bool CreateConstBuffer();

	/// <summary>
	/// ���_�o�b�t�@�̐���
	/// </summary>
	/// <returns>���������̉�</returns>
	bool CreateVertexBuffer();

	/// <summary>
	/// �K�E�V�A���u���[�Ɏg�p����E�F�C�g�l�̌v�Z
	/// </summary>
	/// <param name="count">�v�f��</param>
	/// <param name="s">�W���΍�</param>
	/// <returns></returns>
	std::vector<float> GetGaussianValues(size_t count, float s);

	/// <summary>
	/// �T�C�Y�̃A���C�����g
	/// </summary>
	/// <param name="size">�A���C�����g�����T�C�Y</param>
	/// <param name="alignment">�A���C�����g����T�C�Y</param>
	/// <returns></returns>
	unsigned int AligmentedValue(unsigned int size, unsigned int alignment);

	/// <summary>
	/// �t�F�[�h�A�E�g�p�̃J�E���g���i�[
	/// </summary>
	/// <param name="arg_effectCount">�t�F�[�h�A�E�g�ϐ�</param>
	void SetFadeOutCount(float arg_fadeOutCount) { fadeOutCount = arg_fadeOutCount; }

	/// <summary>
	/// �t�F�[�h�A�E�g�p�̃J�E���g���擾
	/// </summary>
	/// <returns>�t�F�[�h�A�E�g�ϐ�</returns>
	float GetFadeOutCount() { return fadeOutCount; }

	/// <summary>
	/// �t�F�[�h�A�E�g���t�F�[�h�C�����̃t���O
	/// </summary>
	/// <param name="arg_fadeOutFlag"></param>
	void SetFadeOutFlag(bool arg_fadeOutFlag);
private:
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource>texBuff;
	// �ʏ�F �K�E�V�A���u���[
	ComPtr<ID3D12Resource>gaussianBuffer;
	// ���P�x �k���o�b�t�@
	ComPtr<ID3D12Resource>bloomBuffer[2];
	//CBV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapCBV;
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource>depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;

	float fadeOutCount = 1.0f;
	bool fadeOutFlag = 1.0f;
};

