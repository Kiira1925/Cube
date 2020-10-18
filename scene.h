#pragma once
#include "framework.h"
#include <DirectXMath.h>
#include <memory>
#include "camera.h"
#include "ground_blocks.h"
#include "player.h"


#define FbxMax 10
#define Gravity 0.25f

static FileName cube_texture[6];

class Scene
{
private:

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
private: 
    std::unique_ptr<MainCamera> camera1;
    std::unique_ptr<Player> player;
    std::unique_ptr<GroundBlockManager> block;

public:
    DirectX::XMFLOAT4 lightDirection;

public:
    SceneTitle() {}
    ~SceneTitle() {}

    void Initialize()override;
    void Update(float elapsedTime)override;
    void Render(float elapsedTime)override;

    static SceneTitle* getInstance()
    {
        static SceneTitle instance;
        return &instance;
    }
};

class SceneSerect : public Scene
{
private:
    std::unique_ptr<MainCamera> camera1;
    std::unique_ptr<Player> player;
    std::unique_ptr<GroundBlockManager> block;

public:
    DirectX::XMFLOAT4 lightDirection;

public:
    SceneSerect() {}
    ~SceneSerect() {}

    void Initialize()override;
    void Update(float elapsedTime)override;
    void Render(float elapsedTime)override;

    static SceneSerect* getInstance()
    {
        static SceneSerect instance;
        return &instance;
    }
};


class SceneGame : public Scene
{
private:
    std::unique_ptr<MainCamera> camera;
    std::unique_ptr<GroundBlockManager> blocks;
    std::unique_ptr<Player> player;

private:
    int timer;
    bool pause_flg;
    bool SetPosflg;

public:
    DirectX::XMFLOAT4 lightDirection;
    float angle = 0;

public:
    SceneGame() {}
    ~SceneGame(){}
    void Initialize()override;
    void Update(float elapsedTime)override;
    void Render(float elapsedTime)override;

    static SceneGame* getInstance()
    {
        static SceneGame instance;
        return &instance;
    }
};


//*******************************************************************


//**********************************************
//
// scene‚ÌŠÇ—
//
//**********************************************
class SceneManager
{
private:
    Scene* currentScene;
    int stageNum = 0;
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

public:
    int GetStageNum() { return stageNum; }
public:
    void SetStageNum(int stageNum) { this->stageNum = stageNum; }
};
