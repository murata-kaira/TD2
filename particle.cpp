#include "Particle.h"
#include "MyMath.h"
using namespace KamataEngine::MathUtility;

void Particle::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;

	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	counter_ = 0;
	isFinished_ = false;
}

void Particle::Update() {
	if (isFinished_) {

		return;
	}

	for (uint32_t i = 0; i < 8; ++i) {
		Vector3 velocity = {kSpeed, 0, 0};
		float angle = kAngleUnit * i;
		Matrix4x4 rot = MakeRotateZMatrix(angle);
		velocity = Transform(velocity, rot);
		worldTransforms_[i].translation_ += velocity;

		worldTransforms_[i].matWorld_ = MakeAffineMatrix(worldTransforms_[i].scale_, worldTransforms_[i].rotation_, worldTransforms_[i].translation_);
		worldTransforms_[i].TransferMatrix();
	}

	counter_ += 1.0f / 60.0f;

	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}
}

void Particle::Draw() {
	if (isFinished_) {

		return;
	}

	for (uint32_t i = 0; i < 8; ++i) {
		model_->Draw(worldTransforms_[i], *camera_);
	}
}
