#include "Goal.h"
#include "MyMath.h"
#include "Player.h"
#include <cassert>

void Goal::Initialize(const KamataEngine::Vector3& position, const KamataEngine::Vector3& size, KamataEngine::Model* model) {
	assert(model);
	model_ = model;
	position_ = position;
	size_ = size;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position_;
}

void Goal::Update() {
	worldTransform_.translation_ = position_;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Goal::Draw(KamataEngine::Camera* camera) { model_->Draw(worldTransform_, *camera); }

AABB Goal::GetAABB() const {
	return AABB{
	    {position_.x - size_.x * 0.5f, position_.y - size_.y * 0.5f, position_.z - size_.z * 0.5f},

	    {position_.x + size_.x * 0.5f, position_.y + size_.y * 0.5f, position_.z + size_.z * 0.5f}
    };
}
// 衝突判定はプレイヤー側で実施
