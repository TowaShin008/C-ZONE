#pragma once
#include"PostEffect.h"
#include"GameObject.h"
class MotionBlur
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	MotionBlur();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MotionBlur();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, GameObject* gameObject);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	PostEffect* postEffects[5];

	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
};

