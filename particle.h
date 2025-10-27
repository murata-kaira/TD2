#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;

class Particle {
public:
	Particle() {
		counter_ = 0;
		isFinished_ = true;
	}

	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	bool IsFinished() const { return isFinished_; }

private:
	Model* model_;
	Camera* camera_;
	WorldTransform worldTransforms_[8]; // 8個の火花

	float counter_;
	bool isFinished_;

	static inline const float kSpeed = 0.1f;
	static inline const float kAngleUnit = 3.14159f * 2.0f / 8.0f;
	static inline const float kDuration = 0.5f; // 半秒で消える
};
