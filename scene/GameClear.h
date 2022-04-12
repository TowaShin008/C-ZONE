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
	/// ���̃V�[����
	/// </summary>
	/// <returns>�V�[���񋓌^</returns>
	Scene Next()override;
private:
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<Sprite> gameClearTexture;
	std::unique_ptr<Bloom> bloom;

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

