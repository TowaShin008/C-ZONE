#pragma once
#include<map>
#include"GameScene.h"
class SceneManager
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	SceneManager(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~SceneManager();
	
	/// <summary>
	/// �V�[���ǉ�
	/// </summary>
	/// <param name="name">�V�[����</param>
	/// <param name="scene">�V�[���I�u�W�F�N�g</param>
	/// <param name="camera">�J����</param>
	void Add(const Scene& name, GameScene* scene,Camera* camera);


	/// <summary>
	/// �V�[���`�F���W
	/// </summary>
	/// <param name="name">�V�[����</param>
	void ChangeScene(const Scene& name);

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update(DebugText* debugText);

	/// <summary>
	/// �|�X�g�`��O����
	/// </summary>
	void DrawRenderTexture(DebugText* debugText);

	/// <summary>
	/// �|�X�g�`�揈��
	/// </summary>
	void PostEffectDraw();

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw(DebugText* debugText);

private:
	std::map<Scene, GameScene*>scenes;
	//���݂̃V�[��
	GameScene* currentScene = nullptr;

	ID3D12Device* device = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;

	unsigned int score = 0;
public:
	bool IsCurrentScene(const Scene& name);
};

