#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"
#include"MotionBlur.h"
#define MAXWAVE 5
class BossScene3 :
	public GameScene
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	BossScene3();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~BossScene3();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize(unsigned int score)override;

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
	/// <param name="cmdList"></param>
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
	void Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)override;

	/// <summary>
	/// �E�F�[�u�؂�ւ�����
	/// </summary>
	void SceneProcessing();

	void Scene1();

	void Scene2();

	void Scene3();

	void Scene4();

	void Scene5();

	/// <summary>
	/// ���̃V�[����
	/// </summary>
	/// <returns>�V�[���񋓌^</returns>
	Scene Next()override;

private:
	std::unique_ptr<GameObjectManager> gameObjectManager;

	std::unique_ptr<OBJHighModel> planeModel;
	std::unique_ptr<OBJHighModel> missileModel;

	Player* playerObject = nullptr;
	FinalBoss* boss = nullptr;

	std::unique_ptr<OBJModel> yellowBulletModel;
	std::unique_ptr<OBJHighModel> fragmentBulletModel;
	std::unique_ptr<OBJModel> bossBodyModel;
	std::unique_ptr<OBJModel> eyeModel;
	std::unique_ptr<OBJModel> wingModel;
	std::unique_ptr<OBJHighModel> unionModel;
	std::unique_ptr<OBJHighModel> laserModel;
	std::unique_ptr<ParticleManager> gaugeParticle;

	std::unique_ptr<Sprite> warningTexture;
	std::unique_ptr<Sprite> warningBarTexture_1;
	std::unique_ptr<Sprite> warningBarTexture_2;
	float warningTextureAlpha = 1.0f;
	float angleCounter = 0.0f;
	bool incrementFlag = false;

	std::unique_ptr<MotionBlur> motionBlur;
	std::unique_ptr<FadeOut> fadeOut;
	std::unique_ptr<Bloom> bloom;

	//OBJHighModel* laserGaugeModel = nullptr;
	std::unique_ptr<OBJHighCharacter> laserGauge;

	std::unique_ptr<Sprite> lifeTexture;
	std::unique_ptr<Sprite> gaugeFrame;

	float rotateY = 0.0f;
	bool beginFlag = true;
	float incrementValue = 0.0f;
	float scrollPos = incrementValue;
	short sceneWave = 1;
	bool allEnemyDeadFlag = false;
	unsigned int nextWaveCount = 3;
	int chargeCount = 1;
	int particleLugtime = 0;
	float easingTime = 0.0f;
	Vector3 bossPosition;
	bool key = false;
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

