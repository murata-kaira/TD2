#include "GameScene.h"
#include "Math.h"
#include <numbers>

GameScene::~GameScene() {
	delete modelBall_;
	delete modelHole_;
	delete modelGround_;
	delete fade_;
}

void GameScene::Initialize() {
	// モデルの読み込み
	modelBall_ = Model::CreateFromOBJ("player");  // ボールとしてプレイヤーモデルを使用
	modelHole_ = Model::CreateFromOBJ("cube");    // ホールとしてゴールモデルを使用
	modelGround_ = Model::CreateFromOBJ("block"); // 地面としてブロックモデルを使用

	// カメラの初期化
	camera_.Initialize();
	camera_.translation_ = {0.0f, 20.0f, -30.0f};
	camera_.rotation_.x = 0.5f;

	// ボールの初期化
	worldTransformBall_.Initialize();
	worldTransformBall_.translation_ = {0.0f, 0.5f, -10.0f};
	worldTransformBall_.scale_ = {0.5f, 0.5f, 0.5f};

	// ホールの初期化
	worldTransformHole_.Initialize();
	worldTransformHole_.translation_ = {0.0f, 0.1f, 30.0f};
	worldTransformHole_.scale_ = {1.5f, 0.2f, 1.5f};

	// 地面の初期化
	worldTransformGround_.Initialize();
	worldTransformGround_.translation_ = {0.0f, -0.5f, 10.0f};
	worldTransformGround_.scale_ = {30.0f, 0.5f, 50.0f};

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 初期状態
	phase_ = Phase::kFadeIn;
	shotCount_ = 0;
	aimAngle_ = 0.0f;
	power_ = 0.0f;
	ballVelocity_ = {0.0f, 0.0f, 0.0f};
}

void GameScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kAim;
		}
		break;

	case Phase::kAim:
		// 左右キーで角度を調整
		if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			aimAngle_ += 0.05f;
		}
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			aimAngle_ -= 0.05f;
		}

		// スペースキーでパワー調整フェーズへ
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kPower;
			power_ = 0.0f;
			powerDirection_ = 1.0f;
		}
		break;

	case Phase::kPower:
		// パワーゲージを往復させる
		power_ += powerDirection_ * 0.5f;
		if (power_ >= kMaxPower) {
			power_ = kMaxPower;
			powerDirection_ = -1.0f;
		} else if (power_ <= 0.0f) {
			power_ = 0.0f;
			powerDirection_ = 1.0f;
		}

		// スペースキーで打つ
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// 速度を設定
			ballVelocity_.x = std::sin(aimAngle_) * power_ * kPowerScale;
			ballVelocity_.z = std::cos(aimAngle_) * power_ * kPowerScale;
			shotCount_++;
			phase_ = Phase::kShot;
		}
		break;

	case Phase::kShot:
		// ボールの移動
		worldTransformBall_.translation_.x += ballVelocity_.x;
		worldTransformBall_.translation_.z += ballVelocity_.z;

		// 摩擦による減速
		ballVelocity_.x *= kFriction;
		ballVelocity_.z *= kFriction;

		{
			// ボールが止まったか判定（速度の2乗で比較）
			float speedSquared = ballVelocity_.x * ballVelocity_.x + ballVelocity_.z * ballVelocity_.z;
			if (speedSquared < kStopThreshold * kStopThreshold) {
				ballVelocity_ = {0.0f, 0.0f, 0.0f};

				// ホールに入ったか判定（距離の2乗で比較）
				float dx = worldTransformBall_.translation_.x - worldTransformHole_.translation_.x;
				float dz = worldTransformBall_.translation_.z - worldTransformHole_.translation_.z;
				float distanceSquared = dx * dx + dz * dz;

				if (distanceSquared < kHoleRadius * kHoleRadius) {
					// ホールイン！
					phase_ = Phase::kResult;
					resultTimer_ = 0.0f;
				} else {
					// 次のショット
					phase_ = Phase::kAim;
				}
			}
		}
		break;

	case Phase::kResult:
		resultTimer_ += 1.0f / 60.0f;
		if (resultTimer_ >= kResultDuration) {
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

	// カメラをボールに追従（ゲームプレイ中のみ）
	if (phase_ == Phase::kAim || phase_ == Phase::kPower || phase_ == Phase::kShot) {
		camera_.translation_.x = worldTransformBall_.translation_.x;
		camera_.translation_.z = worldTransformBall_.translation_.z - 30.0f;
	}

	// 行列を更新
	camera_.TransferMatrix();
	WorldTransformUpdate(worldTransformBall_);
	WorldTransformUpdate(worldTransformHole_);
	WorldTransformUpdate(worldTransformGround_);
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	Model::PreDraw(commandList);

	// 地面を描画
	modelGround_->Draw(worldTransformGround_, camera_);

	// ホールを描画
	modelHole_->Draw(worldTransformHole_, camera_);

	// ボールを描画
	modelBall_->Draw(worldTransformBall_, camera_);

	Model::PostDraw();

	// フェードを描画
	fade_->Draw();
}
