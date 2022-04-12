#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"

class Title : public GameScene
{
public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Title();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Title();

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="arg_score">�X�R�A</param>
	void Initialize(unsigned int arg_score)override;

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	void CreateAllObject(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="arg_debugText">�f�o�b�O�e�L�X�g</param>
	void Update(DebugText* arg_debugText)override;

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_debugText">�f�o�b�O�e�L�X�g</param>
	void DrawRenderTexture(ID3D12GraphicsCommandList* arg_cmdList, DebugText* arg_debugText)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	void PostEffectDraw(ID3D12GraphicsCommandList* arg_cmdList)override;

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_debugText">�f�o�b�O�e�L�X�g</param>
	void Draw(ID3D12GraphicsCommandList* arg_cmdList, DebugText* arg_debugText)override;

	/// <summary>
	/// ���̃V�[����
	/// </summary>
	/// <returns>�V�[���񋓌^</returns>
	Scene Next()override;
private:
	std::unique_ptr<GameObjectManager> gameObjectManager;
	std::unique_ptr<Sprite> titleTexture;
	std::unique_ptr<Sprite> titleIcon;
	std::unique_ptr<Sprite> titleButtomTexture;
	std::unique_ptr<Bloom> bloom;
	std::vector<std::vector<std::unique_ptr<Sprite>>>bgHexagon;
	std::vector<std::vector<int>> gradationMap;
	Vector4 blue = { 0.0f,0.0f,1.0f,1.0f };
	Vector4 yellow = { 1.0f,1.0f,0.0f,1.0f };
	Vector4 red = { 1.0f,0.0f,0.0f,1.0f };
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	bool pushButtonFlag = false;
	int particleLugtime = 0;
	int alphaCount = 0;
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

