#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>

class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:

	void OnVoiceProcessingPassStart(UINT32 BytesRequired) {};
	//ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
	//バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd) (THIS) {};
	//バッファの使用開始時
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
	//バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {
		//バッファを解放する
		delete[] pBufferContext;
	};
	//再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
	//ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};

class Audio
{
private://エイリアス
	//Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://サブクラス
	//チャンクヘッダ
	struct Chunk
	{
		char	id[4]; // チャンク毎のID
		int		size;  // チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader
	{
		Chunk	chunk;   // "RIFF"
		char	type[4]; // "WAVE"
	};

	//FMTチャンク
	struct FormatChunk
	{
		Chunk		chunk; // "fmt "
		WAVEFORMAT	fmt;   // 波形フォーマット
	};

public: // メンバ関数

	static Audio* GetInstance()
	{
		return audio;
	}
	static void Create();
	//ローダー削除処理
	static void Destroy();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <returns>生成の可否</returns>
	bool Initialize();

	/// <summary>
	/// サウンドファイルの再生処理
	/// </summary>
	/// <param name="filename">再生したいサウンドファイル名</param>
	void PlayWave(const char* filename);

	/// <summary>
	/// サウンドファイルのループ再生
	/// </summary>
	/// <param name="filename">再生したいサウンドファイル名</param>
	void PlayLoopWave(const char* filename);

private: //メンバ変数
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2VoiceCallback voiceCallback;
protected:
	/// <summary>
	/// 自身の変数
	/// </summary>
	static Audio* audio;
	/// <summary>
	/// 明示的なデフォルト定義
	/// </summary>
	Audio() = default;
	~Audio() = default;
};

