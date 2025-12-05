#include "Boss.h"
#include "Math.h"
#include "Player.h"
#include <cassert>
#include <numbers>

void Boss::Initialize(Model* model, Camera* camera, const Vector3& position) {

	assert(model);

	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// ボスのスケールを設定
	worldTransform_.scale_ = {kScale, kScale, kScale};

	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer = 0.0f;

	// HP初期化
	hp_ = kMaxHP;
	isDead_ = false;
	isInvincible_ = false;
	invincibleTimer_ = 0.0f;

	// ジャンプ攻撃初期化
	isOnGround_ = true;
	groundY_ = position.y;
	jumpTimer_ = 0.0f;
}

void Boss::Update() {

	// 死亡時は更新しない
	if (isDead_) {
		return;
	}

	// 無敵時間の処理
	if (isInvincible_) {
		invincibleTimer_ -= 1.0f / kFrameRate;
		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;
			invincibleTimer_ = 0.0f;
		}
	}

	// 状態に応じた更新
	switch (state_) {
	case State::kPatrol:
		UpdatePatrol();
		break;
	case State::kCharge:
		UpdateCharge();
		break;
	case State::kCooldown:
		UpdateCooldown();
		break;
	case State::kJump:
		UpdateJump();
		break;
	}

	WorldTransformUpdate(worldTransform_);
}

void Boss::UpdatePatrol() {
	// 通常の移動
	worldTransform_.translation_ += velocity_;

	// 端に到達したら反転
	if (worldTransform_.translation_.x <= leftLimit_) {
		worldTransform_.translation_.x = leftLimit_;
		ReverseDirection();
	} else if (worldTransform_.translation_.x >= rightLimit_) {
		worldTransform_.translation_.x = rightLimit_;
		ReverseDirection();
	}

	walkTimer += 1.0f / kFrameRate;
	worldTransform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

	// 突進攻撃のタイマー
	chargeTimer_ += 1.0f / kFrameRate;
	if (chargeTimer_ >= kChargeInterval && player_ != nullptr) {
		StartCharge();
	}

	// ジャンプ攻撃のタイマー
	jumpTimer_ += 1.0f / kFrameRate;
	if (jumpTimer_ >= kJumpInterval && player_ != nullptr && isOnGround_) {
		StartJump();
	}
}

void Boss::UpdateCharge() {
	// 突進移動
	worldTransform_.translation_ += velocity_;

	// 端に到達したら反転して突進継続
	if (worldTransform_.translation_.x <= leftLimit_) {
		worldTransform_.translation_.x = leftLimit_;
		velocity_.x = kChargeSpeed;
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	} else if (worldTransform_.translation_.x >= rightLimit_) {
		worldTransform_.translation_.x = rightLimit_;
		velocity_.x = -kChargeSpeed;
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}

	// 突進時は前傾姿勢
	worldTransform_.rotation_.x = 0.3f;

	// 突進時間経過でクールダウンへ
	stateTimer_ += 1.0f / kFrameRate;
	if (stateTimer_ >= kChargeDuration) {
		state_ = State::kCooldown;
		stateTimer_ = 0.0f;
		// 通常速度に戻す
		velocity_.x = (velocity_.x > 0) ? kWalkSpeed : -kWalkSpeed;
	}
}

void Boss::UpdateCooldown() {
	// クールダウン中は停止
	worldTransform_.rotation_.x = 0.0f;

	stateTimer_ += 1.0f / kFrameRate;
	if (stateTimer_ >= kCooldownDuration) {
		state_ = State::kPatrol;
		stateTimer_ = 0.0f;
		chargeTimer_ = 0.0f;
	}
}

void Boss::StartCharge() {
	state_ = State::kCharge;
	stateTimer_ = 0.0f;
	chargeTimer_ = 0.0f;

	// プレイヤーの方向に突進
	if (player_ != nullptr) {
		Vector3 playerPos = player_->GetWorldPosition();
		Vector3 bossPos = GetWorldPosition();

		if (playerPos.x < bossPos.x) {
			// プレイヤーが左にいる
			velocity_.x = -kChargeSpeed;
			worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
		} else {
			// プレイヤーが右にいる
			velocity_.x = kChargeSpeed;
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
		}
	}
}
void Boss::Draw() {

	// 死亡時は描画しない
	if (isDead_) {
		return;
	}
	// 無敵時は点滅（フレームカウント偶数時のみ描画）
	if (isInvincible_) {
		int frame = static_cast<int>(invincibleTimer_ * kFrameRate);
		if (frame % kBlinkCycle < kBlinkOnFrames) {
			return; // 点滅のため描画しない
		}
	}

	model_->Draw(worldTransform_, *camera_);
}

AABB Boss::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

Vector3 Boss::GetWorldPosition() {
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Boss::OnCollision(const Player* player) {
	(void)player;
	//
}

void Boss::TakeDamage(int damage) {

	// 無敵時間中はダメージを受けない
	if (isInvincible_ || isDead_) {
		return;
	}
	hp_ -= damage;
	if (hp_ <= 0) {
		hp_ = 0;
		isDead_ = true;
	} else {
		// ダメージを受けたら無敵時間開始
		isInvincible_ = true;
		invincibleTimer_ = kInvincibleTime;
	}
}

void Boss::SetMovementBounds(float leftLimit, float rightLimit) {

	leftLimit_ = leftLimit;
	rightLimit_ = rightLimit;
}

void Boss::ReverseDirection() {

	// 速度を反転
	velocity_.x = -velocity_.x;
	// 向きを反転（Y軸回転）
	if (velocity_.x > 0) {
		// 右向き
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	} else {
		// 左向き
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}
}

void Boss::StartJump() {
	state_ = State::kJump;
	jumpTimer_ = 0.0f;
	isOnGround_ = false;

	// 上方向の速度を設定
	velocity_.y = kJumpSpeed;

	// プレイヤーの方向に水平移動
	if (player_ != nullptr) {
		Vector3 playerPos = player_->GetWorldPosition();
		Vector3 bossPos = GetWorldPosition();

		if (playerPos.x < bossPos.x) {
			// プレイヤーが左にいる
			velocity_.x = -kJumpMoveSpeed;
			worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
		} else {
			// プレイヤーが右にいる
			velocity_.x = kJumpMoveSpeed;
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
		}
	}
}

void Boss::UpdateJump() {
	// 重力を適用
	velocity_.y -= kGravity;

	// 移動
	worldTransform_.translation_ += velocity_;

	// 端に到達したら反転（水平移動のみ）
	if (worldTransform_.translation_.x <= leftLimit_) {
		worldTransform_.translation_.x = leftLimit_;
		velocity_.x = kJumpMoveSpeed;
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	} else if (worldTransform_.translation_.x >= rightLimit_) {
		worldTransform_.translation_.x = rightLimit_;
		velocity_.x = -kJumpMoveSpeed;
		worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}

	// 地面に着地したかチェック
	if (worldTransform_.translation_.y <= groundY_) {
		worldTransform_.translation_.y = groundY_;
		velocity_.y = 0.0f;
		isOnGround_ = true;
		// パトロール状態に戻る
		state_ = State::kPatrol;
		// 通常速度に戻す
		velocity_.x = (velocity_.x > 0) ? kWalkSpeed : -kWalkSpeed;
		jumpTimer_ = 0.0f;
		worldTransform_.rotation_.x = 0.0f;
	} else {
		// 空中では少し前傾姿勢
		worldTransform_.rotation_.x = 0.2f;
	}
}
