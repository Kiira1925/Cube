#pragma once
#include "framework.h"
#include <DirectXMath.h>
#include <memory>


#define FbxMax 10
#define Gravity 0.25f

class Scene
{
protected:
    virtual void ImGuiUpdate();
    void ImGuiRender();

public:
    Scene() {}
    virtual ~Scene() {}

    virtual void Initialize() = 0;
    //virtual void Release() = 0;
    virtual void Update(float elapsedTime) = 0;
    virtual void Render(float elapsedTime) = 0;
    
};

class SceneTitle : public Scene
{
public:
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
    DirectX::XMFLOAT4 lightDirection;

public:
    SceneTitle() {}
    ~SceneTitle() {}

    void Initialize()override;
    void Update(float elapsedTime)override;
    void Render(float elapsedTime)override;
};

class SceneGame : public Scene
{
private:


private:
    int timer;

public:
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
    DirectX::XMFLOAT4 lightDirection;
    float angle = 0;

public:
    SceneGame() {}
    ~SceneGame(){}
    void Initialize()override;
    void Update(float elapsedTime)override;
    void Render(float elapsedTime)override;

};


//*******************************************************************


//**********************************************
//
// scene�̊Ǘ�
//
//**********************************************
class SceneManager
{
private:
    std::unique_ptr<Scene> currentScene;
private:
    SceneManager() {}
    ~SceneManager() {}
public:
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    void Update();
    void Render();
    void ChangeScene(Scene *newScene);
};
