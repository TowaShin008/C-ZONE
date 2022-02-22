#pragma once
#include"GameScene.h"
#include"GameObjectManager.h"
#include"MotionBlur.h"
#define ENEMYNUM 10
#define MAXWAVE 5
#define MAXNEXTWAVECOUNT 900
class Stage1 : public GameScene
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Stage1();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Stage1();

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
	void WaveProcessing();

	void StageStartProcessing();

	void Wave1();

	void Wave2();

	void Wave3();

	void Wave4();

	void Wave5();

	/// <summary>
	/// ���̃V�[����
	/// </summary>
	/// <returns>�V�[���񋓌^</returns>
	Scene Next()override;

private:
	std::unique_ptr<GameObjectManager> gameObjectManager;

	std::unique_ptr<OBJHighModel> planeModel;
	std::unique_ptr<OBJHighModel> aircraftCarrierPlaneModel;
	std::unique_ptr<OBJHighModel> enemyPlaneModel;
	std::unique_ptr<OBJHighModel> sphereModel;
	std::unique_ptr<OBJHighModel> missileModel;

	Player* playerObject = nullptr;
	std::vector<NormalEnemy*> normalEnemyObjects;
	std::vector<std::vector<Block*>>blocks;
	std::vector<std::vector<Block*>>blocks_2;
	std::vector<TankEnemy*>tankEnemys;
	AircraftCarrier* aircraftCarrier = nullptr;

	std::unique_ptr<OBJModel> yellowBulletModel;
	std::unique_ptr<OBJModel> redBulletModel;
	std::unique_ptr<OBJModel> tankBulletModel;
	std::unique_ptr<OBJModel> blockModel;
	std::unique_ptr<OBJModel> scoreModel;
	std::unique_ptr<OBJHighModel> unionModel;
	std::unique_ptr<OBJModel> alienModel;
	std::unique_ptr<OBJHighModel> laserModel;
	std::unique_ptr<ParticleManager> gaugeParticle;

	std::vector<std::vector<short>> map;
	std::vector<std::vector<short>> map_2;

	//OBJHighModel* laserGaugeModel = nullptr;
	std::unique_ptr<OBJHighCharacter> laserGauge;

	std::unique_ptr<MotionBlur> motionBlur;
	std::unique_ptr<FadeOut> fadeOut;
	std::unique_ptr<Bloom> bloom;

	std::unique_ptr<Sprite> stage1Texture;
	std::unique_ptr<Sprite> lifeTexture;
	std::unique_ptr<Sprite> gaugeFrame;

	float rotateY = 0.0f;
	int delayTime = 0;
	bool blurFlag = false;
	bool beginFlag = true;
	float incrementValue = 0.02f;
	float scrollPos = incrementValue;
	short wave = 0;
	bool allEnemyDeadFlag = false;
	unsigned int nextWaveCount = 0;
	int chargeCount = 1;
	int particleLugtime = 0;
	int alphaCount = 0;
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

	/// <summary>
	/// �}�b�v�f�[�^�̃Z�b�g
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	void SetMapData(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �Q�[���I�u�W�F�N�g�̐���
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	void CreateGameObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList);
};

