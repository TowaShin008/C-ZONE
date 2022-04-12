#pragma once
#include "Sprite.h"
#include"Input.h"
class PostEffect : public Sprite
{
public:
	struct BlurConstBuffer
	{
		float alpha;
		float noiseCount;
	};

	PostEffect();

	void Initialize();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�V�[���`��O����
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	//�V�[���`��O����
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	void SetAlpha(float alpha) { this->alpha = alpha; };
	void SetDelayTime(int delayTime) { this->delayTime = delayTime; };

	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
private:
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource>texBuff[2];
	//SRV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//�[�x�o�b�t�@
	ComPtr<ID3D12Resource>depthBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;
	//�V�K
	int delayTime = 0;
	float alpha = 1.0f;
	float noiseCount = 0.0f;
};

