#pragma once
#include"DirectXBase.h"
#include"Collision.h"
#include"Input.h"
#include"DebugText.h"
#include"Light.h"
#include"LightObject.h"
#include"Audio.h"
#include<time.h>
#include<vector>
#include"Title.h"
#include"Stage1.h"
#include"BossScene1.h"
#include"Stage2.h"
#include"BossScene2.h"
#include"Stage3.h"
#include"BossScene3.h"
#include"GameClear.h"
#include"GameOver.h"
#include"ResultScene.h"
#include"Object3d.h"
#include"MineMath.h"
#include"OBJCharacter.h"
#include"OBJLoader.h"
#include"OBJModel.h"
#include"PostEffect.h"
#include"SceneManager.h"
#include "Bullet.h"
#include"OBJHighModel.h"
#include"OBJHighCharacter.h"
#include"PipelineState.h"
#define ENDTIME 2880
#define UP 2
#define DOWN -2
#define LEFT -1
#define RIGHT 1

class Application
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Application();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Application();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �e�p�C�v���C���̐���
	/// </summary>
	void CreatePipeline();

	/// <summary>
	/// �I�u�W�F�N�g�̐ÓI������
	/// </summary>
	void CameraSetAllObject();

	/// <summary>
	/// �V�[���̒ǉ�
	/// </summary>
	void AddScene();

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

	/// <summary>
	/// Imgui�̕`�揈��
	/// </summary>
	void DrawUI();

	/// <summary>
	/// �V�[���I���t���O�̎擾
	/// </summary>
	/// <returns>�V�[���I���t���O</returns>
	bool GetEndFlag() { return endFlag; }

	/// <summary>
	/// �V�[���I���t���O�̃Z�b�g
	/// </summary>
	/// <param name="endFlag">�V�[���I���t���O</param>
	void SetEndFlag(bool arg_endFlag) { endFlag = arg_endFlag; }
private:
	DebugText debugText;
	DirectXBase* directXBase = nullptr;
	bool endFlag = false;

	Camera* camera = nullptr;

	SceneManager* sceneManager = nullptr;

	std::unique_ptr<Title> title;
	std::unique_ptr<Stage1> stage;
	std::unique_ptr<BossScene1> bossScene1;
	std::unique_ptr<Stage2> stage2;
	std::unique_ptr<BossScene2> bossScene2;
	std::unique_ptr<Stage3> stage3;
	std::unique_ptr<BossScene3> bossScene3;
	std::unique_ptr<GameClear> gameClear;
	std::unique_ptr<GameOver> gameOver;
	std::unique_ptr<ResultScene> resultScene;
	int score = 0;
private: // �ÓI�����o�ϐ�
	static const int debugTextTexNumber = 0;
};