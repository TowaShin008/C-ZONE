#pragma once
#include "Sprite.h"
#include"Input.h"
class FadeOut : public Sprite
{
public:
	struct BlurConstBuffer
	{
		float alpha;
		float effectCount;
	};

	FadeOut();

	void Initialize();

	void Draw(ID3D12GraphicsCommandList* cmdList);

	//�V�[���`��O����
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	//�V�[���`��O����
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	void SetAlpha(float arg_alpha) { alpha = arg_alpha; };
	void SetDelayTime(int delayTime) { this->delayTime = delayTime; };

	float GetFadeOutCount() { return fadeOutCount; }

	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
private:
	//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource>texBuff;
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
	float fadeOutCount = 1.0f;
	bool fadeOutFlag;

public:
	void SetFadeOutFlag(bool arg_fadeOutFlag);
};

