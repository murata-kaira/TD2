#include "KamataEngine.h"
#include <Windows.h>
#include "SceneManager.h" // SceneManagerをインクルード

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	using namespace KamataEngine;
	// エンジンの初期化
	KamataEngine::Initialize(L" LE2C_12_サトウ_シオン_AL3");
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	
	// 最初のシーンを設定
	SceneManager::GetInstance()->ChangeScene(SceneManager::SceneType::kTitle);

	while (true) {
	//エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}
		
		// シーンマネージャーの更新
		SceneManager::GetInstance()->Update();

		//描画開始
		dxCommon->PreDraw();

		// シーンマネージャーの描画
		SceneManager::GetInstance()->Draw();

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		// プリミティブ描画のリセット
		PrimitiveDrawer::GetInstance()->Reset();

		//描画終了
		dxCommon->PostDraw();
	}
	
	//エンジンの終了処理
	KamataEngine::Finalize();
	return 0;
}
