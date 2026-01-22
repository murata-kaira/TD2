#include "Golf.h"
#include "Player.h"
#include <algorithm>
#include <cmath>

using namespace KamataEngine;
using namespace MathUtility;

void Golf::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	assert(model);

	model_ = model;

	camera_ = camera;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;

	worldTransform_.scale_ = {kBallRadius, kBallRadius, kBallRadius};

	velocity_ = {0, 0, 0};

	isInHole_ = false;

	isCharging_ = false;
	chargePower_ = 0.0f;
	shotCount_ = 0;
	lastShotDistance_ = 0.0f;
	shotStartPosition_ = position;
	chargingUp_ = true;

}

void Golf::Update() {

	if (isInHole_) {
		return;
	}

	if (isCharging_) {
		UpdateCharging();
	}


	ApplyPhysics();

	CheckGoal();

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void Golf::ApplyPhysics() {

	// Apply gravity
	velocity_.y -= kGravityAcceleration;

	// Apply velocity
	worldTransform_.translation_ += velocity_;

	// Calculate distance traveled (only horizontal distance)
	if (shotCount_ > 0) {
		float dx = worldTransform_.translation_.x - shotStartPosition_.x;
		float dz = worldTransform_.translation_.z - shotStartPosition_.z;
		lastShotDistance_ = std::sqrt(dx * dx + dz * dz);
	}



	// Ground collision
	if (worldTransform_.translation_.y - kBallRadius <= kGroundHeight) {
		worldTransform_.translation_.y = kGroundHeight + kBallRadius;

		// Bounce
		if (std::abs(velocity_.y) > 0.01f) {
			velocity_.y = -velocity_.y * kBounceDamping;
		} else {
			velocity_.y = 0;
		}

		// Apply friction on ground
		velocity_.x *= kFriction;
		velocity_.z *= kFriction;
	}

	// Rotate ball based on movement
	float speed = std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	if (speed > 0.001f) {
		worldTransform_.rotation_.x += speed * 0.5f;
	}
}

void Golf::CheckGoal() {

	Vector3 toGoal = {goalPosition_.x - worldTransform_.translation_.x, goalPosition_.y - worldTransform_.translation_.y, goalPosition_.z - worldTransform_.translation_.z};

	float distance = std::sqrt(toGoal.x * toGoal.x + toGoal.y * toGoal.y + toGoal.z * toGoal.z);

	if (distance < kGoalRadius && std::abs(velocity_.x) < 0.1f && std::abs(velocity_.y) < 0.1f && std::abs(velocity_.z) < 0.1f) {
		isInHole_ = true;
		velocity_ = {0, 0, 0};
	}
}

void Golf::Draw() {
	if (!isInHole_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

Vector3 Golf::GetWorldPosition() {

	// Return the translation directly since it's kept up-to-date
	return worldTransform_.translation_;
}

AABB Golf::GetAABB() {

	Vector3 worldPos = GetWorldPosition();
	AABB aabb;

	aabb.min = {worldPos.x - kBallRadius, worldPos.y - kBallRadius, worldPos.z - kBallRadius};
	aabb.max = {worldPos.x + kBallRadius, worldPos.y + kBallRadius, worldPos.z + kBallRadius};

	return aabb;
}



void Golf::StartCharging() {
	if (!isCharging_ && std::abs(velocity_.x) < 0.01f && std::abs(velocity_.y) < 0.01f && std::abs(velocity_.z) < 0.01f) {
		isCharging_ = true;
		chargePower_ = kMinPower;
		chargingUp_ = true;
	}
}
void Golf::UpdateCharging() {
	// Oscillate power smoothly between min and max
	if (chargingUp_) {
		chargePower_ += kChargeSpeed;
		if (chargePower_ >= kMaxPower) {
			chargePower_ = kMaxPower;
			chargingUp_ = false;
		}
	} else {
		chargePower_ -= kChargeSpeed;
		if (chargePower_ <= kMinPower) {
			chargePower_ = kMinPower;
			chargingUp_ = true;
		}
	}
}
void Golf::Hit(const Player* player, float power) { // Calculate direction from player to ball
	Vector3 playerPos = player->GetWorldTransform().translation_;
	Vector3 ballPos = GetWorldPosition();

	Vector3 direction = {ballPos.x - playerPos.x, 0, ballPos.z - playerPos.z};

	float length = std::sqrt(direction.x * direction.x + direction.z * direction.z);

	if (length > 0.001f) {
		direction.x /= length;
		direction.z /= length;

			// Store start position for distance calculation
		shotStartPosition_ = worldTransform_.translation_;

		// Apply hit force with power
		velocity_.x = direction.x * power;
		velocity_.y = kUpwardVelocityBase * (power / kMaxPower); // Scale upward velocity with power
		velocity_.z = direction.z * power;

		// Increment shot counter
		shotCount_++;

		// Reset charging
		isCharging_ = false;
		chargePower_ = 0.0f;
	}
}