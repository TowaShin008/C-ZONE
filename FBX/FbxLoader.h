#pragma once

#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>
#include "Model.h"

class FbxLoader
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	/// <summary>
	/// 生成処理
	/// </summary>
	/// <param name="device">デバイス</param>
	void Create(ID3D12Device* device);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private:
	using string = std::string;
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

	ID3D12Device* device = nullptr;
	//FBXSDKを使うのに必要
	FbxManager* fbxManager = nullptr;
	//FBXファイルを読み込むのに必要
	FbxImporter* fbximporter = nullptr;

public:
	static const string baseDirectory;

	//void LoadModelFromFile(const string& modelName);
	Model* LoadModelFromFile(const string& modelName);

	void ParseNodeRecursive(Model* model, FbxNode* fbxNode, Node* parent = nullptr);

	void ParseMesh(Model* model, FbxNode* fbxNode);
	//頂点座標読み取り
	void ParseMeshVertices(Model* model, FbxMesh* fbxMesh);
	//面情報読み取り
	void ParseMeshFaces(Model* model, FbxMesh* fbxMesh);
	//マテリアル読み込み
	void ParseMaterial(Model* model, FbxNode* fbxNode);

	void ParseSkin(Model* model, FbxMesh* fbxMesh);
	//テクスチャ読み込み
	void LoadTexture(Model* model, const std::string& fullpath);
	//ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);
	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName;

	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);
};