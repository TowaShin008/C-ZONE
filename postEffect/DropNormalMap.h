#pragma once
#include "Sprite.h"
#include"Input.h"
class DropNormalMap : public Sprite
{
public:
	struct BlurConstBuffer
	{
		float scrollCount;
		float scrollWidth;
		float effectCount;
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	DropNormalMap();

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
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource>texBuff;//�ʏ�F
	ComPtr<ID3D12Resource>normalMapTexBuff;//���H
	ComPtr<ID3D12Resource>normalMapTexBuff_2;//���H��
	ComPtr<ID3D12Resource>dissolveTexBuff;//�f�B�]���u
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource>depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;

	float scrollCount = 0.0f;

	float fadeOutCount = 1.0f;
	bool fadeOutFlag = 1.0f;
};

