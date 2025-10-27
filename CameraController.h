#pragma once
#include "KamataEngine.h"

class Player;

class CameraController {

public:

	Player* target_ = nullptr;
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	static inline const Rect targetMargin = {
		-9.0f, 9.0f, -5.0f, 5.0f};


	void Initialize();

	void Update();

	void Reset();

	Rect movableArea_ = {0,100,0,100};

	void SetTarget(Player* target) { target_ = target; }

	void SetMovableArea(const Rect& area) { movableArea_ = area; }

	KamataEngine::Vector3 targetoffest_ = {0, 0, -15.0f};

	KamataEngine::Vector3 targetPosition_;

	static inline const float kInterpolationRate = 0.05f;

	static inline const float kVelocityBias = 15.0f;

	const KamataEngine::Camera& GetViewProjection() const {return camera_;}

private:
	// カメラ
	KamataEngine::Camera camera_;

};
