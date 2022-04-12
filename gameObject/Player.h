#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#include"Bullet.h"
#include"Missile.h"
#include"Laser.h"
#include"UnionCharacter.h"
#include"ParticleManager.h"
#include"ObjFileCharacter.h"

#define BULLETMAXNUM 10
#define MISSILEMAXNUM 4
#define MAXPLAYERLUGTIME 30
#define SPAWN 10
#define CHARGEMAXTIME 200
#define CHARGELUTIME 5
//�v���C���[�N���X
class Player :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_device">�f�o�C�X</param>
	Player(ID3D12GraphicsCommandList* arg_cmdList, ID3D12Device* arg_device);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Player();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static Player* Create(ID3D12Device* arg_device,ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void Update(const Vector3& arg_incrementValue);

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_objModel">���@�̃��f��</param>
	/// <param name="arg_bulletModel">���@����o��e�̃��f��</param>
	/// <param name="arg_unionModel">�q�@�̃��f��</param>
	/// <param name="arg_uBulletmodel">�q�@����o��e�̃��f��</param>
	/// <param name="arg_laserModel">���[�U�[�̃��f��</param>
	/// <param name="arg_missileModel">�~�T�C���̃��f��</param>
	void SetOBJModel(ObjFileModel* arg_objModel, ObjFileModel* arg_bulletModel, ObjFileModel* arg_unionModel, ObjFileModel* arg_uBulletmodel, ObjFileModel* arg_laserModel, ObjFileModel* arg_laserGaugeModel, ObjFileModel* arg_missileModel);

	/// <summary>
	/// �e�ۃ��f���̃Z�b�g
	/// </summary>
	/// <param name="arg_bulletModel">���f��</param>
	void SetBulletModel(ObjFileModel* arg_bulletModel);

	/// <summary>
	/// ���[�U�[���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_laserModel">���f��</param>
	void SetLaserModel(ObjFileModel* arg_laserModel);

	/// <summary>
	/// ���[�U�[�Q�[�W���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_laserGaugeModel">���f��</param>
	void SetLaserGaugeModel(ObjFileModel* arg_laserGaugeModel);

	/// <summary>
	/// �~�T�C�����f���̃Z�b�g
	/// </summary>
	/// <param name="arg_missileModel">���f��</param>
	void SetMissileModel(ObjFileModel* arg_missileModel);

	/// <summary>
	/// ���_���W�̃Z�b�g
	/// </summary>
	/// <param name="arg_eye">���_���W</param>
	static void SetEye(const Vector3& arg_eye);

	/// <summary>
	/// �����_���W�̃Z�b�g
	/// </summary>
	/// <param name="arg_target">�����_���W</param>
	static void SetTarget(const Vector3& arg_target);

	/// <summary>
	/// �r���[�s��̍X�V����
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="arg_camera">�J����</param>
	static void SetCamera(Camera* arg_camera) { Player::camera = arg_camera; }

	/// <summary>
	/// �e�����T�C�Y����������
	/// </summary>
	void AttachBullet();

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//�J�����֌W
	static XMMATRIX matView;
	static XMMATRIX matProjection;
	static Camera* camera;
	static ID3D12Device* device;

	ObjFileModel* objModel = nullptr;
	std::unique_ptr<ObjFileCharacter>laserGauge;
	ObjFileModel* laserGaugeModel = nullptr;
	Player* parent = nullptr;
	std::vector<Bullet*>bullets;
	std::unique_ptr<Bullet> chargeBullet;
	std::unique_ptr<Laser> laser;
	std::vector<Missile*>missiles;
	int shotLugTime = MAXPLAYERLUGTIME;
	int shotMissileLugTime = MAXPLAYERLUGTIME;
	std::unique_ptr<UnionCharacter> unionCharacter;
	int spawnTime = 0;
	std::unique_ptr<ParticleManager> deathParticle;
	std::unique_ptr<ParticleManager> chargeParticleManager;
	std::unique_ptr<ParticleManager> chargeMaxParticleManager;
	std::unique_ptr<ParticleManager> chargeBulletParticleManager;
	std::unique_ptr<ParticleManager> jetParticleManager;
	std::unique_ptr<ParticleManager> laserGaugeParticleManager;
	Vector3 deathPosition = { 0,0,0 };
	bool deathParticleFlag = false;
	int life = 0;
	int chargeCount = 1;
	int particleLugtime = 0;
	int laserParticleLugtime = 0;
	float centerPosition = 0.0f;
	bool bossSceneFlag = false;
	int laserEnergy = 0;
	bool isAbleToChargeLaser = true;
	int chargeLugTime = 0;
	bool shotFlag = true;
	bool moveFlag = true;
	bool spawnFlag = false;
	bool disPlayLaserGaugeFlag = true;
public:
	/// <summary>
	/// �e�e�̔z��̎擾
	/// </summary>
	/// <returns>�e�e�̔z��</returns>
	std::vector<Bullet*> GetBullets() { return bullets; }

	/// <summary>
	/// �`���[�W�e�̎擾
	/// </summary>
	/// <returns>�`���[�W�e</returns>
	Bullet* GetChargeBullet() { return chargeBullet.get(); }

	/// <summary>
	/// �~�T�C���̔z��̎擾
	/// </summary>
	/// <returns>�e�e�̔z��</returns>
	std::vector<Missile*> GetMissiles() { return missiles; }

	/// <summary>
	/// ���j�I���̏e�e�̔z��̎擾
	/// </summary>
	/// <returns>���j�I���̏e�e�̔z��</returns>
	std::vector<Bullet*> GetUnionBullets() { return unionCharacter->GetBullets(); }

	/// <summary>
	/// ���j�I���̃Z�b�g
	/// </summary>
	/// <param name="arg_unionCharacter">���j�I���̃|�C���^</param>
	void SetUnionCharacter(UnionCharacter* arg_unionCharacter) { unionCharacter.reset(arg_unionCharacter); }

	/// <summary>
	/// ���j�I���̎擾
	/// </summary>
	/// <returns>���j�I���̃|�C���^</returns>
	UnionCharacter* GetUnionCharacter() { return unionCharacter.get(); }

	/// <summary>
	/// ���j�I���̃Z�b�g
	/// </summary>
	/// <param name="arg_laserGauge">���j�I���̃|�C���^</param>
	void SetLaserGauge(ObjFileCharacter* arg_laserGauge) { laserGauge.reset(arg_laserGauge); }

	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_particleManager">�p�[�e�B�N���}�l�[�W���[</param>
	void SetChargePerticleManager(ParticleManager* arg_particleManager) { chargeParticleManager.reset(arg_particleManager); }

	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_particleMaxManager">�p�[�e�B�N���}�l�[�W���[</param>
	void SetChargeMaxPerticleManager(ParticleManager* arg_particleMaxManager) { chargeMaxParticleManager.reset(arg_particleMaxManager); }

	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_chargeBulletParticleManager">�p�[�e�B�N���}�l�[�W���[</param>
	void SetChargeBulletPerticleManager(ParticleManager* arg_chargeBulletParticleManager) { chargeBulletParticleManager.reset(arg_chargeBulletParticleManager); }


	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_jetParticletManager">�p�[�e�B�N���}�l�[�W���[</param>
	void SetJetPerticleManager(ParticleManager* arg_jetParticletManager) { jetParticleManager.reset(arg_jetParticletManager); }

	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_laserGaugeParticletManager">�p�[�e�B�N���}�l�[�W���[</param>
	void SetLaserGaugePerticleManager(ParticleManager* arg_laserGaugeParticletManager) { laserGaugeParticleManager.reset(arg_laserGaugeParticletManager); }

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="arg_incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);

	/// <summary>
	/// moveFlag�̃Z�b�g
	/// </summary>
	/// <param name="arg_moveFlag">���[�u�t���O</param>
	void SetMoveFlag(bool arg_moveFlag) { moveFlag = arg_moveFlag; }

	/// <summary>
	/// HP�̃Z�b�g
	/// </summary>
	/// <param name="arg_hp">HP</param>
	void SetHP(int arg_hp) { hp = arg_hp; }

	/// <summary>
	/// HP�̎擾
	/// </summary>
	/// <returns>HP</returns>
	int GetHP() { return hp; }

	/// <summary>
	/// �{�X��ւ̐؂�ւ�
	/// </summary>
	/// <param name="arg_bossSceneFlag">�{�X��ɂȂ��Ă��邩�ǂ���</param>
	void SetBossSceneFlag(bool arg_bossSceneFlag);

	/// <summary>
	/// �V���b�g�t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_shotFlag">�V���b�g�t���O</param>
	void SetShotFlag(bool arg_shotFlag);

	/// <summary>
	/// �V���b�g�t���O�̎擾
	/// </summary>
	/// <returns></returns>
	bool GetShotFlag() { return shotFlag; }

	/// <summary>
	/// �e�̔��ˏ���
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void ShotBullet(const Vector3& arg_incrementValue);

	/// <summary>
	/// �`���[�W�e�̔��ˏ���
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void ShotChargeBullet(const Vector3& arg_incrementValue);

	/// <summary>
	/// ���[�U�[�̔��ˏ���
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void ShotLaser(const Vector3& incrementValue);

	/// <summary>
	/// ���[�U�[�̔��ˏ���
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void ShotMissile(const Vector3& arg_incrementValue);

	/// <summary>
	/// ���[�U�[�G�l���M�[�̎擾
	/// </summary>
	/// <returns>���[�U�[�G�l���M�[</returns>
	int GetLaserEnergy() { return laserEnergy; }

	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// �ړ�����
	/// </summary>
	/// <param name="arg_incrementValue"></param>
	void Move(const Vector3& arg_incrementValue);

	/// <summary>
	/// HP���P���炷
	/// </summary>
	void Damage()override;

	/// <summary>
	/// ��_���[�W����
	/// </summary>
	void CriticalDamage()override;

	/// <summary>
	/// ���S���o�p�̃p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="arg_deathParticle">�p�[�e�B�N��</param>
	void SetDeathParticleManager(ParticleManager* arg_deathParticle) { deathParticle.reset(arg_deathParticle); }

	/// <summary>
	/// ���S���o�̔����t���O
	/// </summary>
	/// <param name="arg_deathParticle">���S�t���O�̔����t���O</param>
	void SetDeathParticleFlag(bool arg_deathParticleFlag) { deathParticleFlag = arg_deathParticleFlag; }

	/// <summary>
	/// ���S�p�[�e�B�N������
	/// </summary>
	void DeathParticleProcessing();

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="arg_otherObject">����̃I�u�W�F�N�g</param>
	/// <returns>�����������ǂ���</returns>
	bool IsCollision(GameObject* arg_otherObject)override;

	/// <summary>
	/// ���[�U�[�̎��S�t���O�̎擾
	/// </summary>
	/// <returns>���S����</returns>
	bool GetLaserIsDead() { return laser->GetIsDeadFlag(); }

	/// <summary>
	/// �o�����o
	/// </summary>
	/// <returns>��ʒu�ɂ��ǂ蒅������</returns>
	bool SpawnProcessing();

	/// <summary>
	/// �X�|�[���t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_spawnFlag"></param>
	void SetSpawnFlag(bool arg_spawnFlag) { spawnFlag = arg_spawnFlag; }
	
	/// <summary>
	/// �U����i�̍X�V����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void UpdateAttack(const Vector3& arg_incrementValue);

	/// <summary>
	/// ���[�U�[�Q�[�W����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void LaserGaugeProcessing(const Vector3& arg_incrementValue);

	/// <summary>
	/// ���[�U�[�Q�[�W�\���t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_disPlayLaserGaugeFlag">���[�U�[�Q�[�W�\���t���O</param>
	void SetDisPlayLaserGaugeFlag(bool arg_disPlayLaserGaugeFlag) { disPlayLaserGaugeFlag = arg_disPlayLaserGaugeFlag; }
};

