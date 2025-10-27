#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

enum class Scene {

	kUnknow = 0,

kTitle,
kGame,
};

Scene scene = Scene::kUnknow;

using namespace KamataEngine;

void ChangeScene();

void UpdateScene();

void DrawScene();

GameScene* gameScene = nullptr;

TitleScene* titleScene = nullptr;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2D_25_ヨシオカ_シュンタ_AL3");

	// DirectXCommon*インスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	

	scene = Scene::kTitle;

	//ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();

	titleScene = new TitleScene;
	titleScene->Initialize();

	// メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		//ゲームシーンの更新
		ChangeScene();

		// 描画開始
		UpdateScene();
		dxCommon->PreDraw();

		//ゲームシーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	//ゲームシーンの解放
	delete gameScene;


	//nullptrの代入
	gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}

void ChangeScene() {
	switch (scene) { 
		case Scene::kTitle:
		if (titleScene->isFinished()) {
		//scene変化
			scene = Scene::kGame;
			//旧scene開放
			delete titleScene;
			titleScene = nullptr;
			//新scene生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;

		case Scene::kGame:
		    // ゲームシーンの更新処理
		    if (gameScene) {
			 
			    if (gameScene->isFinished()) {
				    scene = Scene::kTitle;
				    delete gameScene;
				    gameScene = nullptr;
				    titleScene = new TitleScene;
				    titleScene->Initialize();
			    }
		    }
		    break;
	}

}

void UpdateScene() { 
	switch (scene) { 
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}

}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene) {
			titleScene->Draw();
		}
		break;

	case Scene::kGame:
		if (gameScene) {
			gameScene->Draw();
		}
		break;
	}
}
