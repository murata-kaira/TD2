#define NOMINMAX
#include "Enemy.h"
#include "MapChipField.h"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;

	// 初期状態
	velocity_ = {-kWalkSpeed, 0, 0}; // 左向きスタート
	walkTimer_ = 0.0f;

	groundY_ = position.y;
	jumpTimer_ = 0.0f;
	jumpState_ = JumpState::None;

	// 突進用
	isDashing_ = false;
	dashTimer_ = 0.0f;
	dashDuration_ = 0.5f;
	direction_ = -1; // 左向き
}

void Enemy::Update() {
	// --- ジャンプタイマー ---
	jumpTimer_ += 1.0f / 60.0f;
	if (jumpTimer_ > 2.0f && jumpState_ == JumpState::None) {
		jumpState_ = JumpState::JumpStart;
		jumpTimer_ = 0.0f;
	}

	// --- ジャンプ処理 ---
	switch (jumpState_) {
	case JumpState::JumpStart:
		jumpVelocity_ = jumpPower_;
		jumpState_ = JumpState::JumpUp;
		break;
	case JumpState::JumpUp:
		worldTransform_.translation_.y += jumpVelocity_;
		jumpVelocity_ -= gravity_;
		if (jumpVelocity_ <= 0.0f)
			jumpState_ = JumpState::JumpDown;
		break;
	case JumpState::JumpDown:
		worldTransform_.translation_.y += jumpVelocity_;
		jumpVelocity_ -= gravity_;
		if (worldTransform_.translation_.y <= groundY_) {
			worldTransform_.translation_.y = groundY_;
			jumpState_ = JumpState::Landed;
		}
		break;
	case JumpState::Landed:
		jumpState_ = JumpState::None;
		// 着地したらプレイヤー方向に突進開始
		if (player_) {
			direction_ = (Player->GetWorldPosition().x > worldTransform_.translation_.x) ? 1 : -1;
		}
		isDashing_ = true;
		dashTimer_ = 0.0f;
		break;
	default:
		break;
	}

	// --- 突進処理 ---
	if (isDashing_) {
		dashTimer_ += 1.0f / 60.0f;
		velocity_.x = kWalkSpeed * 4.0f * direction_; // 突進速度
		if (dashTimer_ >= dashDuration_) {
			isDashing_ = false;
			velocity_.x = kWalkSpeed * direction_; // 突進終了後は通常移動
		}
	} else {
		velocity_.x = kWalkSpeed * direction_; // 通常移動
	}

	// --- 横移動 ---
	worldTransform_.translation_ += velocity_;

	// --- マトリックス更新 ---
	walkTimer_ += 1.0f / 60.0f;
	worldTransform_.rotation_.x = std::sin(walkTimer_ * 5.0f);
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }

Vector3 Enemy::GetWorldPosition() { return {worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]}; }

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	return {
	    {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f},
        {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f}
    };
}

void Enemy::OnCollision(const Player* player) {
	player_ = player; // プレイヤー情報を保持して追尾用
}
