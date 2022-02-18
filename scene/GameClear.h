#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"

class GameClear : public GameScene
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GameClear();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameClear();

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
	Sprite* gameClearTexture = nullptr;
	Bloom* bloom = nullptr;

	//OBJHighCharacter* objHighCharacter = nullptr;

	float rotateY = 0.0f;
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	bool pushButtonFlag = false;
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

