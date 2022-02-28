#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include<string>
#include"OBJModel.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#include"GameObject.h"
#include"Camera.h"
#include"Input.h"
#include"Bullet.h"
#define SCREENTOP 10.0f
#define SCREENBUTTOM -10.0f
#define SCREENLEFT -17.0f
#define SCREENRIGHT 17.0f
#define SCREENBACK 50.0f
#define BULLETMAXNUM 10
#define MAXPLAYERLUGTIME 30
//�v���C���[�̎q�@�N���X
class UnionCharacter :public GameObject
{
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	UnionCharacter(ID3D12GraphicsCommandList* arg_cmdList);

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~UnionCharacter();

	/// <summary>
	/// �I�u�W�F�N�g�̐�������
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	/// <param name="arg_cmdList">�R�}���h���X�g</param>
	/// <param name="arg_position">�|�W�V����</param>
	/// <returns>�L�����N�^�[�I�u�W�F�N�g</returns>
	static UnionCharacter* Create(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList,const Vector3& arg_position = { 0.0f,0.0f,0.0f });


	/// <summary>
	/// �萔�o�b�t�@�̐���
	/// </summary>
	/// <param name="arg_device">�f�o�C�X</param>
	void CreateConstBuffer(ID3D12Device* arg_device);

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="arg_player">�v���C���[�I�u�W�F�N�g</param>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void Update(GameObject* arg_player, const Vector3& arg_incrementValue);

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw();

	/// <summary>
	/// ���f���̃Z�b�g
	/// </summary>
	/// <param name="arg_objModel">���f��</param>
	/// <param name="arg_bulletModel">�e�ۃ��f��</param>
	void SetOBJModel(OBJHighModel* arg_objModel, OBJModel* arg_bulletModel);

	/// <summary>
	/// �e�ۃ��f���̃Z�b�g
	/// </summary>
	/// <param name="arg_bulletModel">���f��</param>
	void SetBulletModel(OBJModel* arg_bulletModel);

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
	static void SetCamera(Camera* arg_camera) { UnionCharacter::camera = arg_camera; }

	/// <summary>
	/// �e�����T�C�Y����������
	/// </summary>
	/// <param name="arg_device"></param>
	void AttachBullet(ID3D12Device* arg_device);

	/// <summary>
	/// �|�W�V�����̈ړ�
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void SetScrollIncrement(const Vector3& arg_incrementValue);
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
	UnionCharacter* parent = nullptr;
	std::vector<Bullet*>bullets;
	int shotLugTime = MAXPLAYERLUGTIME;
	Vector3 accel = { 0.0f,0.0f,0.0f };
	bool bossSceneFlag = false;
	bool lanchFlag = false;
	bool playerFrontFlag = false;
	bool playerBackFlag = false;
	int maxWireLength = 4;
	bool shotFlag = true;
public:
	/// <summary>
	/// �e�e�̔z����擾
	/// </summary>
	/// <returns>�e�e�̔z��</returns>
	std::vector<Bullet*> GetBullets() { return bullets; }
	/// <summary>
	/// �{�X�V�[���Ɉڂ��Ă��邩
	/// </summary>
	/// <param name="arg_bossSceneFlag">�{�X�t���O</param>
	void SetBossSceneFlag(bool arg_bossSceneFlag);

	/// <summary>
	/// �v���C���[���甭�˂��ꂽ��
	/// </summary>
	/// <param name="arg_lanchFlag">���˃t���O</param>
	void SetLanchFlag(bool arg_lanchFlag);
	bool GetLanchFlag() { return lanchFlag; }

	/// <summary>
	/// �v���C���[�ɓ������Ă��邩�ǂ���
	/// </summary>
	/// <param name="arg_otherPosition">�v���C���[�|�W�V����</param>
	/// <param name="arg_otherRadius">�v���C���[�����蔻�蔼�a</param>
	/// <returns>�������Ă��邩</returns>
	bool IsPlayerCollision(const Vector3& arg_otherPosition, float arg_otherRadius);

	/// <summary>
	/// �v���C���[�̂����܂��ɂ��邩�̃Z�b�^�[�ƃQ�b�^�[
	/// </summary>
	void SetPlayerFrontFlag(bool arg_playerFrontFlag) { playerFrontFlag = arg_playerFrontFlag; }
	bool GetPlayerFrontFlag() { return playerFrontFlag; }

	/// <summary>
	/// �v���C���[�̂������ɂ��邩�̃Z�b�^�[�ƃQ�b�^�[
	/// </summary>
	void SetPlayerBackFlag(bool arg_playerBackFlag) { playerBackFlag = arg_playerBackFlag; }
	bool GetPlayerBackFlag() { return playerBackFlag; }

	/// <summary>
	/// �V���b�g�t���O�̃Z�b�g
	/// </summary>
	/// <param name="arg_shotFlag">�V���b�g�t���O</param>
	void SetShotFlag(bool arg_shotFlag) { shotFlag = arg_shotFlag; }

	/// <summary>
	/// �e�̔��ˏ���
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void ShotBullet(const Vector3& arg_incrementValue);

	/// <summary>
	/// �萔�o�b�t�@�̓]��
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void TransferConstBuff(const Vector3& arg_incrementValue);

	/// <summary>
	/// �ړ�����
	/// </summary>
	/// <param name="arg_player">�v���C���[�I�u�W�F�N�g</param>
	void Move(GameObject* arg_player);

	/// <summary>
	/// �����蔻��
	/// </summary>
	/// <param name="arg_otherObject">����̃I�u�W�F�N�g</param>
	/// <returns>�����������ǂ���</returns>
	bool IsCollision(GameObject* arg_otherObject);

	/// <summary>
	/// �U����i�̍X�V����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void UpdateAttack(const Vector3& arg_incrementValue);
};

