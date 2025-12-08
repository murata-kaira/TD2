#include "SceneManager.h"
#include "TitleScene.h"     // TitleSceneをインクルード
#include "GameScene.h"      // GameSceneをインクルード
#include "GameOverScene.h"  // GameOverSceneをインクルード
#include "GameClearScene.h" // GameClearSceneをインクルード

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
                // GameSceneの場合は次のシーンを判定
                GameScene* gameScene = dynamic_cast<GameScene*>(currentScene_.get());
                if (gameScene) {
                    if (gameScene->GetNextScene() == GameScene::NextScene::kGameOver) {
                        ChangeScene(SceneType::kGameOver);
                    } else if (gameScene->GetNextScene() == GameScene::NextScene::kGameClear) {
                        ChangeScene(SceneType::kGameClear);
                    } else {
                        ChangeScene(SceneType::kTitle);
                    }
                } else {
                    ChangeScene(SceneType::kTitle);
                }
            } else if (currentSceneType_ == SceneType::kGameOver) {
                ChangeScene(SceneType::kTitle);
            } else if (currentSceneType_ == SceneType::kGameClear) {
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
    case SceneType::kGameOver:
        currentScene_.reset(new GameOverScene());
        break;
    case SceneType::kGameClear:
        currentScene_.reset(new GameClearScene());
        break;
    default:
        // 未知のシーンタイプの場合の処理 (エラーログなど)
        break;
    }

    if (currentScene_) {
        currentScene_->Initialize();
    }
}
