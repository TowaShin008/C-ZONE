#include "MotionBlur.h"

MotionBlur::MotionBlur()
{

}

MotionBlur::~MotionBlur()
{
	for (int i = 0; i < _countof(postEffects) ; i++)
	{
		delete postEffects[i];
	}
}

void MotionBlur::Initialize()
{
	for (int i = 0; i < _countof(postEffects); i++)
	{
		postEffects[i] = new PostEffect();
		postEffects[i]->Initialize();
		postEffects[i]->SetAlpha(1.0f - i * 0.2f);
	}

	blurFlag = false;
	beginFlag = true;
}

void MotionBlur::Update()
{
	if (delayTime < 12)
	{
		delayTime++;
	}
	else
	{
		delayTime = 0;
	}
}

void MotionBlur::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, GameObject* gameObject)
{//引数に渡されたオブジェクトだけに適用
	gameObject->SetMotionBlurFlag(true);

	for (int i = 0; i < _countof(postEffects); i++)
	{
		int drawDelayTime = i * 3;
		if (delayTime == drawDelayTime || beginFlag)
		{
			postEffects[i]->PreDrawScene(cmdList);
			gameObject->Draw();
			postEffects[i]->PostDrawScene(cmdList);
			if (i == 4)
			beginFlag = false;
		}
	}

	gameObject->SetMotionBlurFlag(false);
}

void MotionBlur::Draw(ID3D12GraphicsCommandList* cmdList)
{
	for (int i = 0; i < _countof(postEffects); i++)
	{
		postEffects[i]->Draw(cmdList);
	}
}

