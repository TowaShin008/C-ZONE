#include "Input.h"

#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")

Input* Input::input = nullptr;

//���[�_�[��������
void Input::Create()
{//�k���ł���ꍇ�̂ݐ���������
	if (input == nullptr)
	{
		input = new Input;
	}
}
//���[�_�[�폜����
void Input::Destroy()
{
	if (input != nullptr)
	{
		delete input;
	}

	input = nullptr;
}

void Input::Initialize(const WNDCLASSEX& w, const HWND& hwnd)
{
	HRESULT result = S_FALSE;
	windowClass = w;
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);

	result = devkeyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);


	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // �W���`��
	// �r�����䃌�x���̃Z�b�g
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	ZeroMemory(&padState, sizeof(XINPUT_STATE));
	ZeroMemory(&previousPadState, sizeof(XINPUT_STATE));
}

void Input::Update()
{
	HRESULT result = S_FALSE;

	result = devkeyboard->Acquire();

	memcpy(previousKey, currentKey, sizeof(currentKey));

	result = devkeyboard->GetDeviceState(sizeof(currentKey), currentKey);

	result = devMouse->Acquire();
	previousMouseState = mouseState;
	result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);

	previousPadState = padState;
	XInputGetState(0, &padState);
}

bool Input::KeyState(BYTE keyState)
{
	if (currentKey[keyState])
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

bool Input::KeyTriggerState(BYTE keyState)
{
	if (!previousKey[keyState] && currentKey[keyState]) {
		return true;
	}

	return false;
}

bool Input::PushMouseLeft()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::PushMouseMiddle()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::TriggerMouseLeft()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!previousMouseState.rgbButtons[0] && mouseState.rgbButtons[0]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

bool Input::TriggerMouseMiddle()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!previousMouseState.rgbButtons[2] && mouseState.rgbButtons[2]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

Input::MouseMove Input::GetMouseMove()
{
	MouseMove tmp;
	tmp.lX = mouseState.lX;
	tmp.lY = mouseState.lY;
	tmp.lZ = mouseState.lZ;
	return tmp;
}

POINT Input::GetMousePosition()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(FindWindowA("MineLib", nullptr), &mousePos);

	return mousePos;
}

bool Input::PadButtonState(int padButton)
{
	if (padState.Gamepad.wButtons & padButton)
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

bool Input::PadButtonTrigger(int padButton)
{
	if (!(previousPadState.Gamepad.wButtons & padButton) && padState.Gamepad.wButtons & padButton)
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

DIRTYPE Input::PadLStickState()
{//�X�e�B�b�N�̓��͋
	lStickPower.x = padState.Gamepad.sThumbLX;
	lStickPower.y = padState.Gamepad.sThumbLY;
	//�������v�Z
	if (lStickPower.x < 0) {
		lStickPower.x /= STICKMAXVALUE;
	}
	else
	{
		lStickPower.x /= STICKMAXVALUE - 1;
	}

	if (lStickPower.x < -0.5f)
	{//������
		return DIRTYPE::LEFT;
	}
	else if (lStickPower.x > 0.5f)
	{//�E����
		return DIRTYPE::RIGHT;
	}
	//�c�����v�Z
	if (lStickPower.y < 0)
	{
		lStickPower.y /= STICKMAXVALUE;
	}
	else
	{
		lStickPower.y /= STICKMAXVALUE -1;
	}

	if (lStickPower.y < -0.5f)
	{//������
		return DIRTYPE::DOWN;
	}
	else if (lStickPower.y > 0.5f)
	{//�㔻��
		return DIRTYPE::UP;
	}

	return DIRTYPE::NONDIR;
}

DIRTYPE Input::PadLStickTriggerState()
{//���͈ȑO�̃X�e�B�b�N�̓��͏��
	previousLStickPower.x = previousPadState.Gamepad.sThumbLX;
	previousLStickPower.y = previousPadState.Gamepad.sThumbLY;


	lStickPower.x = padState.Gamepad.sThumbLX;
	lStickPower.y = padState.Gamepad.sThumbLY;
	//�����͌v�Z
	if (lStickPower.x < 0)
	{
		lStickPower.x /= STICKMAXVALUE;
	}
	else
	{
		lStickPower.x /= STICKMAXVALUE - 1;
	}
	//�c���͌v�Z
	if (lStickPower.y < 0)
	{
		lStickPower.y /= STICKMAXVALUE;
	}
	else
	{
		lStickPower.y /= STICKMAXVALUE - 1;
	}


	//�����͌v�Z
	if (previousLStickPower.x < 0) {
		previousLStickPower.x /= STICKMAXVALUE;
	}
	else
	{
		previousLStickPower.x /= STICKMAXVALUE - 1;
	}
	//�c���͌v�Z
	if (previousLStickPower.y < 0)
	{
		previousLStickPower.y /= STICKMAXVALUE;
	}
	else
	{
		previousLStickPower.y /= STICKMAXVALUE - 1;
	}


	if (previousLStickPower.x >= -0.5f && lStickPower.x < -0.5f)
	{//������
		return DIRTYPE::LEFT;
	}
	else if (previousLStickPower.x <= 0.5f && lStickPower.x > 0.5f)
	{//�E����
		return DIRTYPE::RIGHT;
	}

	if (previousLStickPower.y >= -0.5f && lStickPower.y < -0.5f)
	{//������
		return DIRTYPE::DOWN;
	}
	else if (previousLStickPower.y <= 0.5f && lStickPower.y > 0.5f)
	{//�����
		return DIRTYPE::UP;
	}

	return DIRTYPE::NONDIR;
}

DIRTYPE Input::PadRStickState()
{//�X�e�B�b�N�̓��͋
	rStickPower.x = padState.Gamepad.sThumbRX;
	rStickPower.y = padState.Gamepad.sThumbRY;
	//�����͌v�Z
	if (rStickPower.x < 0) {
		rStickPower.x /= STICKMAXVALUE;
	}
	else
	{
		rStickPower.x /= STICKMAXVALUE - 1;
	}

	if (rStickPower.x < -0.5f)
	{
		return DIRTYPE::LEFT;
	}
	else if (rStickPower.x > 0.5f)
	{
		return DIRTYPE::RIGHT;
	}
	//�c���͌v�Z
	if (rStickPower.y < 0)
	{
		rStickPower.y /= STICKMAXVALUE;
	}
	else
	{
		rStickPower.y /= STICKMAXVALUE - 1;
	}

	if (rStickPower.y < -0.5f)
	{//������
		return DIRTYPE::DOWN;
	}
	else if (rStickPower.y > 0.5f)
	{//�����
		return DIRTYPE::UP;
	}

	return DIRTYPE::NONDIR;
}