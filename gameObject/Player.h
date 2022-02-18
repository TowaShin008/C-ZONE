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
	Player(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~Player();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static Player* Create(ID3D12Device* device,ID3D12GraphicsCommandList* arg_cmdList,Vector3 position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	void CreateConstBuffer(ID3D12Device* device);

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update(const Vector3& incrementValue);

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="objModel">���@�̃��f��</param>
	/// <param name="bulletModel">���@����o��e�̃��f��</param>
	/// <param name="unionModel">�q�@�̃��f��</param>
	/// <param name="uBulletmodel">�q�@����o��e�̃��f��</param>
	/// <param name="laserModel">���[�U�[�̃��f��</param>
	void SetOBJModel(OBJHighModel* arg_objModel, OBJModel* bulletModel,OBJHighModel* unionModel,OBJModel* uBulletmodel,OBJHighModel* laserModel,OBJHighModel* missileModel);

	/// <summary>
	/// �e�ۃ��f���̃Z�b�g
	/// </summary>
	/// <param name="bulletModel">���f��</param>
	void SetBulletModel(OBJModel* bulletModel);

	/// <summary>
	/// ���[�U�[���f���̃Z�b�g
	/// </summary>
	/// <param name="bulletModel">���f��</param>
	void SetLaserModel(OBJHighModel* laserModel);

	/// <summary>
	/// �~�T�C�����f���̃Z�b�g
	/// </summary>
	/// <param name="bulletModel">���f��</param>
	void SetMissileModel(OBJHighModel* missileModel);

	/// <summary>
	/// ���_���W�̃Z�b�g
	/// </summary>
	/// <param name="eye">���_���W</param>
	static void SetEye(const Vector3& eye);

	/// <summary>
	/// �����_���W�̃Z�b�g
	/// </summary>
	/// <param name="target">�����_���W</param>
	static void SetTarget(const Vector3& target);

	/// <summary>
	/// �r���[�s��̍X�V����
	/// </summary>
	static void UpdateViewMatrix();

	/// <summary>
	/// �J�����̃Z�b�g
	/// </summary>
	/// <param name="camera">�J����</param>
	static void SetCamera(Camera* arg_camera) { Player::camera = arg_camera; }

	/// <summary>
	/// �v���C���[�̒e�����T�C�Y����������
	/// </summary>
	void AttachBullet(ID3D12Device* device);

private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//�J�����֌W
	static XMMATRIX matView;
	static XMMATRIX matProjection;
	static Camera* camera;

	OBJHighModel* objModel = nullptr;
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
	Vector3 deathPosition = { 0,0,0 };
	bool deathParticleFlag = false;
	int life = 0;
	int chargeCount = 1;
	int particleLugtime = 0;
	float centerPosition = 0.0f;
	bool bossSceneFlag = false;
	int laserEnergy = 0;
	bool isAbleToChargeLaser = true;
	int chargeLugTime = 0;
	bool shotFlag = true;
	bool moveFlag = true;
	bool spawnFlag = false;
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
	/// <param name="unionCharacter">���j�I���̃|�C���^</param>
	void SetUnionCharacter(UnionCharacter* arg_unionCharacter) { unionCharacter.reset(arg_unionCharacter); }

	/// <summary>
	/// ���j�I���̎擾
	/// </summary>
	/// <returns>���j�I���̃|�C���^</returns>
	UnionCharacter* GetUnionCharacter() { return unionCharacter.get(); }

	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="particleMan">�p�[�e�B�N���}�l�[�W���[</param>
	void SetChargePerticleManager(ParticleManager* arg_particleManager) { chargeParticleManager.reset(arg_particleManager); }

	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="particleMan">�p�[�e�B�N���}�l�[�W���[</param>
	void SetChargeMaxPerticleManager(ParticleManager* arg_particleMaxManager) { chargeMaxParticleManager.reset(arg_particleMaxManager); }

	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="particleMan">�p�[�e�B�N���}�l�[�W���[</param>
	void SetChargeBulletPerticleManager(ParticleManager* arg_particleBulletManager) { chargeBulletParticleManager.reset(arg_particleBulletManager); }


	/// <summary>
	/// �p�[�e�B�N���̃Z�b�g
	/// </summary>
	/// <param name="particleMan">�p�[�e�B�N���}�l�[�W���[</param>
	void SetJetPerticleManager(ParticleManager* arg_jetParticletManager) { jetParticleManager.reset(arg_jetParticletManager); }

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& incrementValue);

	/// <summary>
	/// moveFlag�̃Z�b�g
	/// </summary>
	/// <param name="arg_moveFlag">���[�u�t���O</param>
	void SetMoveFlag(bool arg_moveFlag) { moveFlag = arg_moveFlag; }

	/// <summary>
	/// HP�̃Z�b�g
	/// </summary>
	/// <param name="hp">HP</param>
	void SetHP(int arg_hp) { hp = arg_hp; }

	/// <summary>
	/// HP�̎擾
	/// </summary>
	/// <returns>HP</returns>
	int GetHP() { return hp; }

	/// <summary>
	/// �{�X��ւ̐؂�ւ�
	/// </summary>
	/// <param name="bossSceneFlag">�{�X��ɂȂ��Ă��邩�ǂ���</param>
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
	void ShotBullet(const Vector3& incrementValue);

	/// <summary>
	/// �`���[�W�e�̔��ˏ���
	/// </summary>
	void ShotChargeBullet(const Vector3& incrementValue);

	/// <summary>
	/// ���[�U�[�̔��ˏ���
	/// </summary>
	void ShotLaser(const Vector3& incrementValue);

	/// <summary>
	/// ���[�U�[�̔��ˏ���
	/// </summary>
	void ShotMissile(const Vector3& incrementValue);

	/// <summary>
	/// ���[�U�[�G�l���M�[�̎擾
	/// </summary>
	/// <returns></returns>
	int GetLaserEnergy() { return laserEnergy; }

	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// �ړ�����
	/// </summary>
	/// <param name="incrementValue"></param>
	void Move(const Vector3& incrementValue);

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
	/// <param name="deathParticle">�p�[�e�B�N��</param>
	void SetDeathParticleManager(ParticleManager* arg_deathParticle) { deathParticle.reset(arg_deathParticle); }

	/// <summary>
	/// ���S���o�̔����t���O
	/// </summary>
	/// <param name="deathParticle">���S�t���O�̔����t���O</param>
	void SetDeathParticleFlag(bool arg_deathParticleFlag) { deathParticleFlag = arg_deathParticleFlag; }

	/// <summary>
	/// ���S�p�[�e�B�N������
	/// </summary>
	void DeathParticleProcessing();

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="otherObject">����̃I�u�W�F�N�g</param>
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
	/// <param name="incrementValue">�X�N���[����</param>
	void UpdateAttack(const Vector3& incrementValue);
};

