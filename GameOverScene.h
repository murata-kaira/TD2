#pragma once
#include "KamataEngine.h"
#include "Fade.h"
#include "IScene.h"

using namespace KamataEngine;

class GameOverScene : public IScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	~GameOverScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

	bool IsFinished() const override { return finished_; }

private:
	// カメラ
	Camera camera_;

	// スプライト
	Sprite* gameOverSprite_ = nullptr;
	uint32_t gameOverTextureHandle_ = 0;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;

	// フレームレート
	static inline const float kFrameRate = 60.0f;
	// 表示時間
	static inline const float kDisplayDuration = 3.0f;
	float displayTimer_ = 0.0f;
};
