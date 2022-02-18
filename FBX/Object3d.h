#pragma once

#include"Model.h"
#include"Camera.h"

#include<Windows.h>
#include<wrl.h>
#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXMath.h>
#include<string>
#include"FbxLoader.h"
#include"PipelineState.h"

class Object3d
{
protected:
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

	using XMMATRIX = DirectX::XMMATRIX;

public:

	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;
		XMMATRIX world;
		Vector3 cameraPos;
	};
public://Ã“Iƒƒ“ƒoŠÖ”
	static void SetDevice(ID3D12Device* device) { Object3d::device = device; }
	static void SetCamera(Camera* camera) { Object3d::camera = camera; }
public:
	void Initialize();
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void SetModel(Model* model) { this->model = model; }
	void SetRotation(const Vector3& rotation) { this->rotation = rotation; }
	void SetRotationX(float rotationX) { this->rotation.x = rotationX; }
	void SetRotationY(float rotationY) { this->rotation.y = rotationY; }
	void SetRotationZ(float rotationZ) { this->rotation.z = rotationZ; }

	void PlayAnimation();
private:
	static ID3D12Device* device;

	static Camera* camera;

	static const int MAX_BONES = 32;

	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};
protected:
	ComPtr<ID3D12Resource>constBufferDataTransform;

	Vector3 scale = { 2,2,2 };

	Vector3 rotation = { 0,0,0 };

	Vector3 position = { 0,-10,0, };

	XMMATRIX matWorld = DirectX::XMMatrixIdentity();

	Model* model = nullptr;

	ComPtr<ID3D12Resource>constBuffSkin;

	FbxTime frameTime;

	FbxTime startTime;

	FbxTime endTime;

	FbxTime currentTime;

	bool isPlay = false;
};

