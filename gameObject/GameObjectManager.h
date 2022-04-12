#pragma once
#include"GameObject.h"
#include <iomanip>
#include<vector>
#include"Debugtext.h"
class GameObjectManager
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	GameObjectManager();

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameObjectManager();

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="arg_score">�X�R�A</param>
	void Initialize(unsigned int arg_score);

	/// <summary>
	/// �I�u�W�F�N�g�ǉ�����
	/// </summary>
	/// <param name="arg_gameObject">�Q�[���I�u�W�F�N�g</param>
	void AddGameObject(GameObject* arg_gameObject);

	/// <summary>
	/// ���ׂẴI�u�W�F�N�g�̃A�b�v�f�[�g����
	/// </summary>
	/// <param name="arg_incrementValue">�X�N���[����</param>
	void UpdateAllObject(const Vector3& arg_incrementValue);

	/// <summary>
	/// ���ׂẴI�u�W�F�N�g�̕`�揈��
	/// </summary>
	/// <param name="cmdList">�R�}���h���X�g</param>
	void DrawAllObject();

	/// <summary>
	/// �S�ẴI�u�W�F�N�g�̓����蔻��
	/// </summary>
	void AllObjectCollision();

	int GetScore() { return score; }

	/// <summary>
	/// �Q�[���I�u�W�F�N�g�z��̎擾
	/// </summary>
	/// <returns>�Q�[���I�u�W�F�N�g�z��</returns>
	std::vector<GameObject*> GetGameObject() { return gameObjects; }

	/// <summary>
	/// �X�R�A�̕\��
	/// </summary>
	/// <param name="debugText">�f�o�b�O�e�L�X�g</param>
	void DisPlayScore(DebugText* debugText);
private:
	//�Q�[���I�u�W�F�N�g�z��
	std::vector<GameObject*>gameObjects;
	int score = 0;
};

