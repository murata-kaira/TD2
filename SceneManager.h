#pragma once
#include "IScene.h"
#include <memory> // std::unique_ptr を使用するために必要

class SceneManager {
private:
    // シングルトンパターン
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    const SceneManager& operator=(const SceneManager&) = delete;

public:
    static SceneManager* GetInstance();

    enum class SceneType {
        kTitle,
        kGame,
    };

    // シーンの初期化
    void Initialize();
    // シーンの更新
    void Update();
    // シーンの描画
    void Draw();

    // シーンの切り替え
    void ChangeScene(SceneType newSceneType);

private:
    std::unique_ptr<IScene> currentScene_ = nullptr;
    SceneType currentSceneType_ = SceneType::kTitle; // 初期シーンをタイトルに設定
};
