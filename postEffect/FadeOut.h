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

	//シーン描画前処理
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	//シーン描画前処理
	void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

	void SetAlpha(float arg_alpha) { alpha = arg_alpha; };
	void SetDelayTime(int delayTime) { this->delayTime = delayTime; };

	float GetFadeOutCount() { return fadeOutCount; }

	float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
private:
	//テクスチャバッファ
	ComPtr<ID3D12Resource>texBuff;
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource>depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;
	//新規
	int delayTime = 0;
	float alpha = 1.0f;
	float fadeOutCount = 1.0f;
	bool fadeOutFlag;

public:
	void SetFadeOutFlag(bool arg_fadeOutFlag);
};

