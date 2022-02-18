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
#include"HomingBullet.h"
#include"NormalEnemy.h"
#include"ParticleManager.h"
#include"Block.h"
#include"FinalBossEye.h"
#define BULLETMAXNUM 10
#define MAXENEMYLUGTIME 60
#define LANCHMAXTIME 800
#define TENTACLESNUM 40
#define BIGBULLETMAXNUM 3
//�{�X�N���X
class FinalBoss :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	FinalBoss(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~FinalBoss();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static FinalBoss* Create(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList, Vector3 position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	void CreateConstBuffer(ID3D12Device* device);

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update(const Vector3& incrementValue, const Vector3& playerPosition);

	/// <summary>
	/// �`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void Draw();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="objModel">���f��</param>
	void SetOBJModel(OBJModel* eyeModel, OBJModel* bodyModel, OBJModel* wingModel, OBJHighModel* bulletModel);

	/// <summary>
	/// �̂̃��f���̃��f���̃Z�b�g
	/// </summary>
	/// <param name="bulletModel">���f��</param>
	void SetBodyModel(OBJModel* bodyModel);

	/// <summary>
	/// �ڂ̃��f���̃��f���̃Z�b�g
	/// </summary>
	/// <param name="bulletModel">���f��</param>
	void SetEyeModel(OBJModel* eyeModel);

	/// <summary>
	/// ���̃��f���̃Z�b�g
	/// </summary>
	/// <param name="wingModel"></param>
	void SetWingModel(OBJModel* wingModel);

	/// <summary>
	/// �e�ۃ��f���̃Z�b�g
	/// </summary>
	/// <param name="bulletModel">���f��</param>
	void SetBulletModel(OBJHighModel* bulletModel);

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
	static void SetCamera(Camera* camera) { FinalBoss::camera = camera; }

	/// <summary>
	/// �̂����T�C�Y����������
	/// </summary>
	void AttachBody(ID3D12Device* device);

	/// <summary>
	/// �ڂ����T�C�Y����������
	/// </summary>
	void AttachEye(ID3D12Device* device);

	/// <summary>
	/// ���������T�C�Y����������
	/// </summary>
	void AttachLeftWing(ID3D12Device* device);

	/// <summary>
	/// �E�������T�C�Y����������
	/// </summary>
	void AttachRightWing(ID3D12Device* device);

	/// <summary>
	/// �e�����T�C�Y����������
	/// </summary>
	void AttachBullet(ID3D12Device* device);
private:
	UINT descpriptorSize;
	static ID3D12GraphicsCommandList* cmdList;
	ComPtr<ID3D12Resource>constBuffB0;
	ComPtr<ID3D12Resource>constBuffB1;

	//�J�����֌W
	//�r���[�s��
	static XMMATRIX matView;
	//�v���W�F�N�V�����s��
	static XMMATRIX matProjection;
	//�J�����N���X
	static Camera* camera;

	//�e�N���X
	FinalBoss* parent = nullptr;


	//�Ĕ��˂���܂ł̎���
	int shotLugTime = 60;

	//���݂̓����̒i�K���i�[����ϐ�
	MOVEPHASE currentPhase = MOVEPHASE::PHASE1;

	//���݂̓����̎�ނ��i�[����ϐ�
	MOVETYPE  currentType = MOVETYPE::TRIANGLE;

	OBJModel* bodyModel = nullptr;
	Block* bodyBlock = nullptr;

	FinalBossEye* finalBossEye = nullptr;

	Block* parentLeftWing = nullptr;
	std::vector<Block*>leftWing;

	Block* parentRightWing = nullptr;
	std::vector<Block*>rightWing;

	bool moveEndFlag = false;
	float curveTime = 0.0f;
	bool shotFlag = true;
	float centerPosition = 0.0f;
	int hp = 400;

	std::unique_ptr<ParticleManager> deathParticle;
	bool deathParticleFlag = false;

	MOVEPHASE bodyAttackPhase = MOVEPHASE::PHASE1;
	Vector3 previousParentLeftWingRotation = { 0,0,0 };
	Vector3 previousParentRightWingRotation = { 0,0,0 };

	float attackFlag = false;
	int rnd = 0;
	float tentacleAngle = 0.0f;
	float bodyAngle = 0.0f;
	float homePositionX = 20.0f;
	bool blinkFlag = false;
	float blinkAngle = 0.0f;
	bool entryFlag = true;
	bool bodyAttackFlag = false;

	std::vector<HomingBullet*> bigBullets;
public:
	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	void TransferConstBuff();

	/// <summary>
	/// �`���[�W�e�̎擾
	/// </summary>
	/// <returns>�`���[�W�e</returns>
	std::vector<HomingBullet*> GetBigBullet() { return bigBullets; }

	/// <summary>
	///�@�����̐e�I�u�W�F�N�g�̎擾
	/// </summary>
	Block* GetLeftParentWing() { return parentLeftWing; }

	/// <summary>
	/// �E���̐e�I�u�W�F�N�g�̎擾
	/// </summary>
	Block* GetRightParentWing() { return parentRightWing; }
	/// <summary>
	/// ���[�u�t�F�C�Y�̃Z�b�g
	/// </summary>
	/// <param name="movePhase">���[�u�t�F�C�Y</param>
	void SetMovePhase(const MOVEPHASE& movePhase) { currentPhase = movePhase; }

	/// <summary>
	/// ���[�u�t�F�C�Y�̎擾
	/// </summary>
	/// <returns>���[�u�t�F�C�Y</returns>
	MOVEPHASE GetMovePhase() { return currentPhase; }

	/// <summary>
	/// ���[�u�^�C�v�̎擾
	/// </summary>
	/// <returns>���[�u�^�C�v</returns>
	MOVETYPE GetMoveType() { return currentType; }

	/// <summary>
	/// �ړ�����
	/// </summary>
	void Move();

	/// <summary>
	/// ������x�i��Œ�~���鏈������
	/// </summary>
	/// <param name="incrementValue">�X�N���[����</param>
	/// <param name="playerPosition">�v���C���[�|�W�V����</param>
	void StayMove();

	/// <summary>
	/// ���[�u�G���h�t���O�̃Z�b�g
	/// </summary>
	/// <param name="moveEndFlag">���[�u�G���h�t���O</param>
	void SetMoveEndFlag(bool arg_moveEndFlag) { moveEndFlag = arg_moveEndFlag; }

	/// <summary>
	/// ���[�u�G���h�t���O�̏���
	/// </summary>
	/// <returns>���[�u�G���h�t���O</returns>
	bool GetMoveEndFlag() { return moveEndFlag; }

	/// <summary>
	/// isDeadFlag��true�ɂ���
	/// </summary>
	void IsDead();

	/// <summary>
	/// �ˌ��ł��邩�̃t���O���Z�b�g����
	/// </summary>
	/// <param name="shotFlag">�ˌ��ł��邩�ǂ����̃t���O</param>
	void SetShotFlag(bool arg_shotFlag) { shotFlag = arg_shotFlag; }

	/// <summary>
	/// ���˂܂ł̃��O�^�C��
	/// </summary>
	/// <param name="arg_shotLugTime">���O�^�C��</param>
	void SetShotLugTime(int arg_shotLugTime) { shotLugTime = arg_shotLugTime; }

	/// <summary>
	/// �̓�����t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_bodyAttackFlag">�̓�����t���O</param>
	void SetBodyAttackFlag(bool arg_bodyAttackFlag) { bodyAttackFlag = arg_bodyAttackFlag; }

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="incrementValue">�|�W�V�����̑�����</param>
	void SetScrollIncrement(const Vector3& incrementValue);

	/// <summary>
	/// �Z���^�[�|�W�V�����̃Z�b�g
	/// </summary>
	/// <param name="centerPosition"></param>
	void SetCenterPos(float arg_centerPosition) { centerPosition = arg_centerPosition; }

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
	/// �_���[�W����
	/// </summary>
	void Damage()override;

	/// <summary>
	/// ��_���[�W����
	/// </summary>
	void CriticalDamage()override;

	/// <summary>
	/// �_���[�W���o
	/// </summary>
	void DamageEffect();

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
	bool IsCollision(GameObject* arg_otherObject);

	FinalBossEye* GetCollisionEye() { return finalBossEye; }

	/// <summary>
	/// �U���t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_attackFlag"></param>
	void SetAttackFlag(bool arg_attackFlag) { attackFlag = arg_attackFlag; }

	/// <summary>
	/// �ċz���o
	/// </summary>
	void BreathMove();

	/// <summary>
	/// �ˌ�����
	/// </summary>
	void ShotBullet(const Vector3& incrementValue, const Vector3& playerPosition);

	/// <summary>
	/// �u������
	/// </summary>
	void Blink();

	/// <summary>
	/// �o�ꉉ�o
	/// </summary>
	void EntryPerformance();

	/// <summary>
	/// �����p��
	/// </summary>
	void InitialPosture();

	/// <summary>
	/// �ʏ�p��
	/// </summary>
	void NormalPosture();

	/// <summary>
	/// ���S���o
	/// </summary>
	void EndPerformance();

	/// <summary>
	/// �̓�����U������
	/// </summary>
	void BodyAttack();
};

