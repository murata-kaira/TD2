#pragma once
#include "KamataEngine.h"
#include "Fade.h"
#include "IScene.h"

using namespace KamataEngine;

class GolfScene : public IScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kAim,     // 狙いを定める
		kPower,   // パワーを決める
		kShot,    // ショット後のボール移動
		kResult,  // 結果表示
		kFadeOut, // フェードアウト
	};

	~GolfScene();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	bool IsFinished() const override { return finished_; }

private:
	// カメラ
	Camera camera_;

	// ボール
	WorldTransform worldTransformBall_;
	Model* modelBall_ = nullptr;
	Vector3 ballVelocity_ = {0.0f, 0.0f, 0.0f};

	// ホール（目標）
	WorldTransform worldTransformHole_;
	Model* modelHole_ = nullptr;

	// 地面
	WorldTransform worldTransformGround_;
	Model* modelGround_ = nullptr;

	// フェード
	Fade* fade_ = nullptr;

	// フェーズ
	Phase phase_ = Phase::kFadeIn;

	// 角度と方向
	float aimAngle_ = 0.0f;
	float power_ = 0.0f;
	float powerDirection_ = 1.0f; // パワーゲージの方向

	// 打数
	int shotCount_ = 0;

	// 完了フラグ
	bool finished_ = false;

	// 結果表示用タイマー
	float resultTimer_ = 0.0f;
	static inline const float kResultDuration = 3.0f;

	// 定数
	static inline const float kBallRadius = 0.5f;
	static inline const float kHoleRadius = 1.0f;
	static inline const float kMaxPower = 30.0f;
	static inline const float kFriction = 0.95f;
	static inline const float kStopThreshold = 0.1f;
};
