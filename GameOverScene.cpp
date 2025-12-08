#include "GameOverScene.h"

GameOverScene::~GameOverScene() {
	delete gameOverSprite_;
	delete fade_;
}

void GameOverScene::Initialize() {
	// カメラ初期化
	camera_.Initialize();

	// ゲームオーバースプライトの初期化
	gameOverTextureHandle_ = TextureManager::Load("gameover.png");
	
	// スプライトの作成（画面中央に配置）
	Vector2 gameOverPos = {(WinApp::kWindowWidth - 800.0f) / 2.0f, (WinApp::kWindowHeight - 200.0f) / 2.0f};
	gameOverSprite_ = Sprite::Create(gameOverTextureHandle_, gameOverPos);

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	phase_ = Phase::kFadeIn;
	displayTimer_ = 0.0f;
}

void GameOverScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		// 一定時間表示、またはスペースキーでタイトルへ
		displayTimer_ += 1.0f / kFrameRate;
		if (displayTimer_ >= kDisplayDuration || Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	camera_.TransferMatrix();
}

void GameOverScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	
	// ゲームオーバースプライト描画
	gameOverSprite_->Draw();
	
	// スプライト描画後処理
	Sprite::PostDraw();

	// フェード描画
	fade_->Draw();
}
