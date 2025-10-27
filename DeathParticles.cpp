#include "DeathParticles.h"
#include "MyMath.h"
#include <algorithm>
using namespace KamataEngine::MathUtility;

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) 
{
	model_ = model;
	camera_ = camera;

	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {
	for (KamataEngine::WorldTransform& particleTransform : worldTransforms_) {
		particleTransform.matWorld_ = MakeAffineMatrix(
			particleTransform.scale_, 
			particleTransform.rotation_,
			particleTransform.translation_);
		particleTransform.TransferMatrix();
	}

	for (uint32_t i = 0; i < 8; ++i) {
	//基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		//回転角を計算
		float angle = kAngleUint * i;
		//Z軸周り回転行列
		Matrix4x4 matrixRotation = MathUtility::MakeRotateZMatrix(angle);
	//基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(velocity, matrixRotation);
	//移動
		worldTransforms_[i].translation_ += velocity;
	}

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f,1.0f);
	objectColor_.SetColor(color_);
	//消す
	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		//終わる扱い
		isFinished_ = true;
	}

	//終わったら何もしない
	if (isFinished_) {
		return;
	}
}

void DeathParticles::Draw() {
	for (KamataEngine::WorldTransform& particleTransform : worldTransforms_) {
		model_->Draw(particleTransform, *camera_, &objectColor_);
	}

		// 終わったら何もしない
	if (isFinished_) {
		return;
	}
}
