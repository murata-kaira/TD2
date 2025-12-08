#include "SceneManager.h"
#include "TitleScene.h" // TitleSceneをインクルード
#include "GameScene.h"  // GameSceneをインクルード
#include "GameClear.h"  // ClearSceneをインクルード
#include "GameOver.h"   // OverSceneをインクルード

SceneManager* SceneManager::GetInstance() {
    static SceneManager instance;
    return &instance;
}

void SceneManager::Initialize() {
    // 初期シーンの設定はmain.cppで行うため、ここでは何もしない
}

void SceneManager::Update() {
    if (currentScene_) {
        currentScene_->Update();
        // シーンが終了したら次のシーンに切り替える
        if (currentScene_->IsFinished()) {
            if (currentSceneType_ == SceneType::kTitle) {
                ChangeScene(SceneType::kGame);
            } else if (currentSceneType_ == SceneType::kGame) {
                // GameSceneから次のシーンを取得
                int nextScene = currentScene_->GetNextScene();
                if (nextScene == IScene::kNextSceneClear) {
                    ChangeScene(SceneType::kClear);
                } else if (nextScene == IScene::kNextSceneOver) {
                    ChangeScene(SceneType::kOver);
                } else {
                    ChangeScene(SceneType::kTitle);
                }
            } else if (currentSceneType_ == SceneType::kClear || currentSceneType_ == SceneType::kOver) {
                ChangeScene(SceneType::kTitle);
            }
        }
    }
}

void SceneManager::Draw() {
    if (currentScene_) {
        currentScene_->Draw();
    }
}

void SceneManager::ChangeScene(SceneType newSceneType) {
    if (currentScene_) {
        // 現在のシーンを解放
        currentScene_.reset();
    }

    currentSceneType_ = newSceneType;

    switch (newSceneType) {
    case SceneType::kTitle:
        currentScene_.reset(new TitleScene());
        break;
    case SceneType::kGame:
        currentScene_.reset(new GameScene());
        break;
    case SceneType::kClear:
        currentScene_.reset(new ClearScene());
        break;
    case SceneType::kOver:
        currentScene_.reset(new OverScene());
        break;
    default:
        // 未知のシーンタイプの場合の処理 (エラーログなど)
        break;
    }

    if (currentScene_) {
        currentScene_->Initialize();
    }
}
