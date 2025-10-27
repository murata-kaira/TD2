#pragma once
#include "KamataEngine.h"
#include <array>

using namespace KamataEngine;

class DeathParticles {
public:
	/// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// 更新
	void Update();

	/// 描画
	void Draw();

	bool IsFinished() const { return isFinished_; }


private:
	KamataEngine::WorldTransform worldTransform_; // ワールドトランスふぉーむ
	KamataEngine::Model* model_ = nullptr;        // モデル
	KamataEngine::Camera* camera_ = nullptr;      // カメラ
	
	static inline const uint32_t kNumParticles = 8;

	static inline const float kDuration = 2.0f;
	
	static inline const float kSpeed = 0.07f;
		
	static inline const float kAngleUint = 2.0f * 3.14f / 8.0f;

	bool isFinished_ = false;

	float counter_ = 0.0f;

	//色変更obj
	ObjectColor objectColor_;

	//色の数値
	Vector4 color_;

	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;
};