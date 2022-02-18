#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"
#define ENDINGMOVEMAXTIME 500

class GameOver : public GameScene
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GameOver();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameOver();

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
	Sprite* gameOverTexture = nullptr;
	Bloom* bloom = nullptr;

	std::vector<std::vector<Sprite*>>bgHexagon;
	std::vector<std::vector<int>> gradationMap;

	float rotateY = 0.0f;
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	int endingMoveTime = 0;
	bool pushButtonFlag = false;
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

