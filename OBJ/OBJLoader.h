#pragma once
#include<d3d12.h>
#include<string>
#include"OBJModel.h"
#include<fstream>
#include<sstream>
#include<d3d12.h>
#include<memory>

class OBJLoader
{//シングルトンパターンで実装
public:
	/// <summary>
	/// たった一つの実体を返す関数
	/// </summary>
	/// <returns>自身の実体</returns>
	static OBJLoader* GetInstance()
	{
		return objLoader;
	}

	/// <summary>
	/// 生成処理
	/// </summary>
	static void Create();

	/// <summary>
	/// 削除処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// モデルのロード
	/// </summary>
	/// <param name="modelName">モデル名</param>
	/// <returns>モデル</returns>
	OBJModel* LoadModel(const std::string& modelName);

	/// <summary>
	/// マテリアル読み込み処理
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイルネーム</param>
	/// <param name="objModel">紐づけたいモデルクラス</param>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename, OBJModel* objModel);
	
	/// <summary>
	/// OBJファイルからのデータ読み込み
	/// </summary>
	/// <param name="modelName">モデルネーム</param>
	/// <param name="objModel">紐づけたいモデルクラス</param>
	void CreateCharacter(const std::string& modelName, OBJModel* objModel);
	
	/// <summary>
	/// デバイスのセット処理 必ずCreate()関数の後に記述する
	/// </summary>
	/// <param name="device">デバイス</param>
	void SetDevice(ID3D12Device* device) { this->device = device; };
private:
	ID3D12Device* device = nullptr;
protected:
	/// <summary>
	/// 自身の変数
	/// </summary>
	static OBJLoader* objLoader;
	/// <summary>
	/// 明示的なデフォルト定義
	/// </summary>
	OBJLoader() = default;
	~OBJLoader() = default;
};

