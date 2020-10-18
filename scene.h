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
    virtual void Finalize() = 0;
    
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
    void Finalize()override;

    static SceneTitle* getInstance()
    {
        static SceneTitle instance;
        return &instance;
    }
};

class SceneSelect : public Scene
{
private:
    std::unique_ptr<MainCamera> camera1;
    std::unique_ptr<Player> player;
    std::unique_ptr<GroundBlockManager> block;

public:
    DirectX::XMFLOAT4 lightDirection;

public:
    SceneSelect() {}
    ~SceneSelect() {}

    void Initialize()override;
    void Update(float elapsedTime)override;
    void Render(float elapsedTime)override;
    void Finalize()override;

    static SceneSelect* getInstance()
    {
        static SceneSelect instance;
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
    void Finalize()override;

    void Reload(int stage_num);

    static SceneGame* getInstance()
    {
        static SceneGame instance;
        return &instance;
    }
};


//*******************************************************************

enum CHANGE_PROCESS { Set, Out, In, Fin };

//**********************************************
//
// scene‚ÌŠÇ—
//
//**********************************************
class SceneManager
{
private:
   // Scene* currentScene;
    int stageNum = 0;
    Scene*  currentScene;
    Scene*  nextScene;
    int     change_timer;
    bool    change_flg;

    CHANGE_PROCESS currentProcess;
private:
    SceneManager();
    ~SceneManager() {}
public:
    void changeProcessing();
    void changeProcessDraw();

    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    void Update();
    void Render();
    void SetScene(Scene* newScene);
    void ChangeScene(Scene *newScene);

public:
    int GetStageNum() { return stageNum; }
public:
    void SetStageNum(int stageNum) { this->stageNum = stageNum; }
    void ChangeScenePerformance(Scene* newScene);
};
