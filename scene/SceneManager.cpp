#include "SceneManager.h"

SceneManager::SceneManager(ID3D12Device* arg_device, ID3D12GraphicsCommandList* arg_cmdList)
{
    device = arg_device;
    cmdList = arg_cmdList;
}

SceneManager::~SceneManager()
{
    currentScene->DeleteAllObject();
    //scenes.clear();
}


void SceneManager::Add(const Scene& name, GameScene* scene,Camera* camera)
{
    //シーンの追加
    scenes[name] = scene;
    scenes[name]->SetSceneName(name);

    scenes[name]->Initialize(score);
    scenes[name]->SetCamera(camera);
    //最初に代入されたシーンは現在のシーンに代入する
    if (currentScene == nullptr)
    {
        currentScene = scenes[name];
        currentScene->CreateAllObject(device,cmdList);
    }
}

void SceneManager::ChangeScene(const Scene& name)
{
    //何かシーンが登録されていたら
    if (currentScene != nullptr)
    {
        //現在のシーンの終了処理
        currentScene->DeleteAllObject();
    }
    const Scene previousScene = currentScene->GetSceneName();
    //マップから次のシーンを取り出し、
    //現在のシーンに設定
    currentScene = scenes[name];


    currentScene->SetPreviousScene(previousScene);
    //シーンの初期化
    currentScene->Initialize(score);
    currentScene->CreateAllObject(device,cmdList);
}

void SceneManager::Update(DebugText* debugText)
{
    if (currentScene == nullptr)
    {
        return;
    }

    //現在のシーンの更新
    currentScene->Update(debugText);


    if (currentScene->GetEndFlag())
    {
        score = currentScene->GetScore();
        //次のシーンを取り出し、シーン切り替え
        ChangeScene(currentScene->Next());
    }
}

void SceneManager::DrawRenderTexture(DebugText* debugText)
{
    currentScene->DrawRenderTexture(cmdList,debugText);
}

void SceneManager::PostEffectDraw()
{
    currentScene->PostEffectDraw(cmdList);
}

void SceneManager::Draw(DebugText* debugText)
{
    //現在のシーンを描画
    currentScene->Draw(cmdList, debugText);
}

bool SceneManager::IsCurrentScene(const Scene& name)
{
    if (currentScene == scenes[name])
    {
        return true;
    }

    return false;
}

