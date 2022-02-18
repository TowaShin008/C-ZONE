#pragma once
#include<d3d12.h>
#define DIRECTINPUT_VERSION    0x0800//DirectInput�̃o�[�W�����w��
#include<dinput.h>
#include<DirectXMath.h>
#include<Xinput.h>
#include <wrl.h>
#include"MineMath.h"

#define STICKMAXVALUE 32768

enum class DIRTYPE
{
	UP,
	DOWN,
	RIGHT,
	LEFT,
	NONDIR
};

class Input
{
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//�}�E�X�p�\����
	struct MouseMove {
		LONG    lX;
		LONG    lY;
		LONG    lZ;
	};

public:
	static Input* GetInstance()
	{
		return input;
	}
	static void Create();
	//���[�_�[�폜����
	static void Destroy();

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="w">�E�B���h�E�N���X</param>
	/// <param name="hwnd">HWND�N���X</param>
	void Initialize(const WNDCLASSEX& w, const HWND& hwnd);

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();


	/// <summary>
	/// �L�[�{�[�h���͔���
	/// </summary>
	/// <param name="keyState">�L�[�̎��(DirectInput�̃}�N�����Ŏw��)</param>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool KeyState(BYTE keyState);

	/// <summary>
	/// �L�[�{�[�h���̓g���K�[����
	/// </summary>
	/// <param name="keyState">�L�[�̎��(DirectInput�̃}�N�����Ŏw��)</param>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool KeyTriggerState(BYTE keyState);

	/// <summary>
	/// �}�E�X�����͔���
	/// </summary>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool PushMouseLeft();

	/// <summary>
	/// �}�E�X�����͔���
	/// </summary>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool PushMouseMiddle();

	/// <summary>
	/// �}�E�X�����̓g���K�[����
	/// </summary>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool TriggerMouseLeft();

	/// <summary>
	/// �}�E�X�����̓g���K�[����
	/// </summary>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool TriggerMouseMiddle();

	/// <summary>
	/// �}�E�X��Ԃ̎擾
	/// </summary>
	/// <returns>�}�E�X�\����</returns>
	MouseMove GetMouseMove();

	/// <summary>
	/// �}�E�X�|�W�V�����̎擾
	/// </summary>
	/// <returns>�}�E�X�|�W�V����</returns>
	POINT GetMousePosition();

	/// <summary>
	/// �Q�[���p�b�h�{�^�����͔���
	/// </summary>
	/// <param name="padButton">�{�^���̎��(XINPUT_GAMEPAD�\���̂Ŏw��)</param>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool PadButtonState(int padButton);

	/// <summary>
	/// �Q�[���p�b�h�{�^�����̓g���K�[����
	/// </summary>
	/// <param name="padButton">�{�^���̎��(XINPUT_GAMEPAD�\���̂Ŏw��)</param>
	/// <returns>�����ꂽ���ǂ���</returns>
	bool PadButtonTrigger(int padButton);

	/// <summary>
	/// �Q�[���p�b�h���X�e�B�b�N���͔���
	/// </summary>
	/// <returns>ALLOWTYPE�񋓌^(UP�@DOWN�@RIGHT LEFT)</returns>
	DIRTYPE PadLStickState();

	/// <summary>
	/// �Q�[���p�b�h���X�e�B�b�N���̓g���K�[����
	/// </summary>
	/// <returns>ALLOWTYPE�񋓌^(UP�@DOWN�@RIGHT LEFT)</returns>
	DIRTYPE PadLStickTriggerState();

	/// <summary>
	/// �Q�[���p�b�h�E�X�e�B�b�N���͔���
	/// </summary>
	/// <returns>ALLOWTYPE�񋓌^(UP�@DOWN�@RIGHT LEFT)</returns>
	DIRTYPE PadRStickState();

private:
	//�L�[�{�[�h�p�ϐ�
	IDirectInput8* dinput = nullptr;
	IDirectInputDevice8* devkeyboard = nullptr;
	BYTE currentKey[256] = {};
	BYTE previousKey[256] = {};

	//�}�E�X�p�ϐ�
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 previousMouseState = {};

	//�Q�[���p�b�h�p�ϐ�
	XINPUT_STATE padState;
	XINPUT_STATE previousPadState;

	//�X�e�B�b�N�p�ϐ�
	Vector2 rStickPower;
	Vector2 lStickPower;
	Vector2 previousLStickPower;
	WNDCLASSEX windowClass;
protected:
	/// <summary>
	/// ���g�̕ϐ�
	/// </summary>
	static Input* input;
	/// <summary>
	/// �����I�ȃf�t�H���g��`
	/// </summary>
	Input() = default;
	~Input() = default;
};

