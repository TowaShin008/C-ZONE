#pragma once
#include<d3d12.h>
#include"Sprite.h"
#include<vector>
#include"Input.h"
#include"ParticleManager.h"
#include"DirectXBase.h"
#include"OBJCharacter.h"
#include"OBJLoader.h"
#include"Camera.h"
#include"Player.h"
#include"NormalEnemy.h"
#include"Alien.h"
#include"Block.h"
#include"UnionCharacter.h"
#include"DebugText.h"
#include"AircraftCarrier.h"
#include"Boss.h"
#include"Squid.h"
#include"FinalBoss.h"
#include"FinalBossEye.h"
#include"FadeOut.h"
#include"Bloom.h"
#include"GlassNormalMap.h"
#include"DropNormalMap.h"
#include"NoisePostEffect.h"
#include"Model.h"
#include"object3d.h"
#include"FbxLoader.h"
#include"OBJHighModel.h"
#include"OBJHighCharacter.h"
#include"TankEnemy.h"
#include<memory>
using namespace Microsoft::WRL;
enum class Scene
{
	TITLE,
	STAGE1,
	BOSS1,
	STAGE2,
	BOSS2,
	STAGE3,
	BOSS3,
	GAMECLEAR,
	GAMEOVER,
	RESULT,
};

class GameScene
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GameScene();

	/// <summary>
	/// ���z�f�X�g���N�^
	/// </summary>
	virtual ~GameScene();

	/// <summary>
	/// ����������
	/// </summary>
	virtual void Initialize(unsigned int arg_score);

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="device"></param>
	virtual void CreateAllObject(ID3D12Device* device,ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">�C���v�b�g�N���X</param>
	virtual void Update(DebugText* debugText);

	/// <summary>
	/// �|�X�g�`��O����
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	virtual void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText);

	/// <summary>
	/// �|�X�g�`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	virtual void PostEffectDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	virtual void Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText);

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̍폜����
	/// </summary>
	virtual void DeleteAllObject();

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J�����I�u�W�F�N�g</param>
	void SetCamera(Camera* camera) { this->camera = camera; }

	/// <summary>
	/// �V�[���I���t���O�̃Z�b�g
	/// </summary>
	/// <param name="endFlag">�V�[���I���t���O</param>
	void SetEndFlag(bool endFlag) { this->endFlag = endFlag; }

	/// <summary>
	/// �V�[���I���t���O�̎擾
	/// </summary>
	/// <returns>�V�[���I���t���O</returns>
	bool GetEndFlag() { return endFlag; }

	/// <summary>
	/// �X�R�A���擾
	/// </summary>
	/// <returns></returns>
	unsigned int GetScore() { return score; }

	/// <summary>
	/// ���̃V�[����
	/// </summary>
	/// <returns>�V�[���񋓌^</returns>
	virtual Scene Next();

	Scene GetPreviousScene() { return previousScene; }

	void SetPreviousScene(Scene arg_previousScene) { previousScene = arg_previousScene; }

	Scene GetSceneName() { return sceneName; }

	void SetSceneName(Scene arg_sceneName) { sceneName = arg_sceneName; }

protected:
	Camera* camera = nullptr;
	bool endFlag = false;
	unsigned int score = 0;
	Scene previousScene = Scene::TITLE;
	Scene sceneName = Scene::TITLE;
};

