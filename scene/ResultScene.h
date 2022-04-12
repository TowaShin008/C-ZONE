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
	/// <param name="score">�X�R�A</param>
	void Initialize(unsigned int score)override;

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="input">�C���v�b�g�N���X</param>
	void Update(DebugText* debugText)override;

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="debugText">�f�o�b�O�e�L�X�g</param>
	void DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void PostEffectDraw(ID3D12GraphicsCommandList* cmdList)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	/// <param name="debugText">�f�o�b�O�e�L�X�g</param>
	void Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	/// <summary>
	/// �E�F�[�u�؂�ւ�����
	/// </summary>
	/// <param name="debugText">�f�o�b�O�e�L�X�g</param>
	void SceneProcessing(DebugText* debugText);

	void Scene1();

	void Scene2();

	void Scene3(DebugText* debugText);

	/// <summary>
	/// ���̃V�[����
	/// </summary>
	/// <returns>�V�[���񋓌^</returns>
	Scene Next()override;
private:
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<Sprite> windowTexture;
	std::unique_ptr<Sprite> stage_clearTexture;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<ObjFileModel> playerModel;
	ObjFileCharacter* playerObject = nullptr;
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
	/// ���f���̃��[�h
	/// </summary>
	void LoadAllModel();
};

