#include "Math.h"
#include "GameOver.h"
#include <numbers>

OverScene::~OverScene() {
	delete modelPlayer_;
	delete modelTitle_;
	delete fade_;
}

void OverScene::Initialize() {

	

	// カメラ初期化
	camera_.Initialize();

	const float kPlayerOver = 2.0f;

	worldTransformOver_.Initialize();

	worldTransformOver_.scale_ = {kPlayerOver, kPlayerOver, kPlayerOver};

	const float kPlayerScale = 10.0f;

	worldTransformPlayer_.Initialize();

	worldTransformPlayer_.scale_ = {kPlayerScale, kPlayerScale, kPlayerScale};

	worldTransformPlayer_.rotation_.y = 0.95f * std::numbers::pi_v<float>;

	worldTransformPlayer_.translation_.x = -2.0f;

	worldTransformPlayer_.translation_.y = -10.0f;

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void OverScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
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



	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeOverMove);

	float angle = counter_ / kTimeOverMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformOver_.translation_.y = std::sin(angle) + 10.0f;

	camera_.TransferMatrix();

	// アフィン変換～DirectXに転送(タイトル座標)
	WorldTransformUpdate(worldTransformOver_);

	// アフィン変換～DirectXに転送（プレイヤー座標）
	WorldTransformUpdate(worldTransformPlayer_);
}

void OverScene::Draw() {

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);


	Model::PostDraw();

	fade_->Draw();
}
