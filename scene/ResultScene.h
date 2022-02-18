#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"

class ResultScene : public GameScene
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	ResultScene();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~ResultScene();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize(unsigned int arg_score)override;

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	void CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">�C���v�b�g�N���X</param>
	void Update(DebugText* debugText)override;

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�R�}���h����</param>
	void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	/// <summary>
	/// �|�X�g�`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h����</param>
	void PostEffectDraw(ID3D12GraphicsCommandList* cmdList)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h����</param>
	void Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	void SceneProcessing(DebugText* debugText);

	void Scene1();

	void Scene2();

	void Scene3(DebugText* debugText);


	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̍폜����
	/// </summary>
	void DeleteAllObject()override;

	/// <summary>
	/// ���̃V�[����
	/// </summary>
	/// <returns>�V�[���񋓌^</returns>
	Scene Next()override;
private:
	GameObjectManager* gameObjectManager = nullptr;
	Sprite* windowTexture = nullptr;
	Sprite* stage_clearTexture = nullptr;
	Bloom* bloom = nullptr;
	OBJHighModel* playerModel = nullptr;
	OBJHighCharacter* playerObject = nullptr;
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	bool pushButtonFlag = false;
	int sceneWave = 1;
	float easingTime = 0.0f;
	float windowTextureAlpha = 0.0f;
	int blinkCount = 0;
public:
	/// <summary>
	/// �|�X�g�G�t�F�N�g�̐���
	/// </summary>
	void CreatePostEffect();
	/// <summary>
	/// ���[�U�[�Q�[�W����
	/// </summary>
	void LaserGaugeProcessing();
	/// <summary>
	/// ���f���̃��[�h
	/// </summary>
	void LoadAllModel();
};

