#pragma once

class IScene {
public:
    // 次のシーンを示す定数
    static constexpr int kNoNextScene = -1;
    static constexpr int kNextSceneClear = 2;
    static constexpr int kNextSceneOver = 3;

    virtual ~IScene() = default;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual bool IsFinished() const = 0;
    virtual int GetNextScene() const { return kNoNextScene; } // デフォルトは次のシーン指定なし
};
