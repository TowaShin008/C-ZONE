#pragma once
#include<d3d12.h>
#define DIRECTINPUT_VERSION    0x0800//DirectInputのバージョン指定
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
	//マウス用構造体
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
	//ローダー削除処理
	static void Destroy();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="w">ウィンドウクラス</param>
	/// <param name="hwnd">HWNDクラス</param>
	void Initialize(const WNDCLASSEX& w, const HWND& hwnd);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();


	/// <summary>
	/// キーボード入力判定
	/// </summary>
	/// <param name="keyState">キーの種類(DirectInputのマクロ名で指定)</param>
	/// <returns>押されたかどうか</returns>
	bool KeyState(BYTE keyState);

	/// <summary>
	/// キーボード入力トリガー判定
	/// </summary>
	/// <param name="keyState">キーの種類(DirectInputのマクロ名で指定)</param>
	/// <returns>押されたかどうか</returns>
	bool KeyTriggerState(BYTE keyState);

	/// <summary>
	/// マウス左入力判定
	/// </summary>
	/// <returns>押されたかどうか</returns>
	bool PushMouseLeft();

	/// <summary>
	/// マウス中入力判定
	/// </summary>
	/// <returns>押されたかどうか</returns>
	bool PushMouseMiddle();

	/// <summary>
	/// マウス左入力トリガー判定
	/// </summary>
	/// <returns>押されたかどうか</returns>
	bool TriggerMouseLeft();

	/// <summary>
	/// マウス中入力トリガー判定
	/// </summary>
	/// <returns>押されたかどうか</returns>
	bool TriggerMouseMiddle();

	/// <summary>
	/// マウス状態の取得
	/// </summary>
	/// <returns>マウス構造体</returns>
	MouseMove GetMouseMove();

	/// <summary>
	/// マウスポジションの取得
	/// </summary>
	/// <returns>マウスポジション</returns>
	POINT GetMousePosition();

	/// <summary>
	/// ゲームパッドボタン入力判定
	/// </summary>
	/// <param name="padButton">ボタンの種類(XINPUT_GAMEPAD構造体で指定)</param>
	/// <returns>押されたかどうか</returns>
	bool PadButtonState(int padButton);

	/// <summary>
	/// ゲームパッドボタン入力トリガー判定
	/// </summary>
	/// <param name="padButton">ボタンの種類(XINPUT_GAMEPAD構造体で指定)</param>
	/// <returns>押されたかどうか</returns>
	bool PadButtonTrigger(int padButton);

	/// <summary>
	/// ゲームパッド左スティック入力判定
	/// </summary>
	/// <returns>ALLOWTYPE列挙型(UP　DOWN　RIGHT LEFT)</returns>
	DIRTYPE PadLStickState();

	/// <summary>
	/// ゲームパッド左スティック入力トリガー判定
	/// </summary>
	/// <returns>ALLOWTYPE列挙型(UP　DOWN　RIGHT LEFT)</returns>
	DIRTYPE PadLStickTriggerState();

	/// <summary>
	/// ゲームパッド右スティック入力判定
	/// </summary>
	/// <returns>ALLOWTYPE列挙型(UP　DOWN　RIGHT LEFT)</returns>
	DIRTYPE PadRStickState();

private:
	//キーボード用変数
	IDirectInput8* dinput = nullptr;
	IDirectInputDevice8* devkeyboard = nullptr;
	BYTE currentKey[256] = {};
	BYTE previousKey[256] = {};

	//マウス用変数
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 previousMouseState = {};

	//ゲームパッド用変数
	XINPUT_STATE padState;
	XINPUT_STATE previousPadState;

	//スティック用変数
	Vector2 rStickPower;
	Vector2 lStickPower;
	Vector2 previousLStickPower;
	WNDCLASSEX windowClass;
protected:
	/// <summary>
	/// 自身の変数
	/// </summary>
	static Input* input;
	/// <summary>
	/// 明示的なデフォルト定義
	/// </summary>
	Input() = default;
	~Input() = default;
};

