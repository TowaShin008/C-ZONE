#include "GameScene.h"

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Initialize(unsigned int arg_score)
{

}

void GameScene::CreateAllObject(ID3D12Device* device, ID3D12GraphicsCommandList* arg_cmdList)
{

}

void GameScene::Update(DebugText* debugText)
{

}

void GameScene::DrawRenderTexture(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{

}

void GameScene::PostEffectDraw(ID3D12GraphicsCommandList* cmdList)
{

}

void GameScene::Draw(ID3D12GraphicsCommandList* cmdList, DebugText* debugText)
{

}

void GameScene::DeleteAllObject()
{

}

Scene GameScene::Next()
{
	return Scene::TITLE;
}