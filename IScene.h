#pragma once

class IScene {
public:
    virtual ~IScene() = default;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual bool IsFinished() const = 0;
    virtual int GetNextScene() const { return -1; } // デフォルトは-1（次のシーン指定なし）
};
