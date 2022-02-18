#pragma once
#include"PostEffect.h"
#include"GameObject.h"
class MotionBlur
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	MotionBlur();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~MotionBlur();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �`��O����
	/// </summary>
	void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, GameObject* gameObject);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	PostEffect* postEffects[5];

	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
};

