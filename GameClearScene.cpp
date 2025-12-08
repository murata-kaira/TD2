#include "GameClearScene.h"

GameClearScene::~GameClearScene() {
	delete gameClearSprite_;
	delete fade_;
}

void GameClearScene::Initialize() {
	// カメラ初期化
	camera_.Initialize();

	// ゲームクリアスプライトの初期化
	gameClearTextureHandle_ = TextureManager::Load("gameclear.png");
	
	// スプライトの作成（画面中央に配置）
	Vector2 gameClearPos = {(WinApp::kWindowWidth - 800.0f) / 2.0f, (WinApp::kWindowHeight - 200.0f) / 2.0f};
	gameClearSprite_ = Sprite::Create(gameClearTextureHandle_, gameClearPos);

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	phase_ = Phase::kFadeIn;
	displayTimer_ = 0.0f;
}

void GameClearScene::Update() {
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

void GameClearScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	
	// ゲームクリアスプライト描画
	gameClearSprite_->Draw();
	
	// スプライト描画後処理
	Sprite::PostDraw();

	// フェード描画
	fade_->Draw();
}
