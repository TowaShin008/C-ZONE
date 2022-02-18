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
    //�V�[���̒ǉ�
    scenes[name] = scene;
    scenes[name]->SetSceneName(name);

    scenes[name]->Initialize(score);
    scenes[name]->SetCamera(camera);
    //�ŏ��ɑ�����ꂽ�V�[���͌��݂̃V�[���ɑ������
    if (currentScene == nullptr)
    {
        currentScene = scenes[name];
        currentScene->CreateAllObject(device,cmdList);
    }
}

void SceneManager::ChangeScene(const Scene& name)
{
    //�����V�[�����o�^����Ă�����
    if (currentScene != nullptr)
    {
        //���݂̃V�[���̏I������
        currentScene->DeleteAllObject();
    }
    const Scene previousScene = currentScene->GetSceneName();
    //�}�b�v���玟�̃V�[�������o���A
    //���݂̃V�[���ɐݒ�
    currentScene = scenes[name];


    currentScene->SetPreviousScene(previousScene);
    //�V�[���̏�����
    currentScene->Initialize(score);
    currentScene->CreateAllObject(device,cmdList);
}

void SceneManager::Update(DebugText* debugText)
{
    if (currentScene == nullptr)
    {
        return;
    }

    //���݂̃V�[���̍X�V
    currentScene->Update(debugText);


    if (currentScene->GetEndFlag())
    {
        score = currentScene->GetScore();
        //���̃V�[�������o���A�V�[���؂�ւ�
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
    //���݂̃V�[����`��
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

