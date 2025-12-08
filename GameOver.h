#pragma once
#include "Fade.h"
#include "IScene.h" // ISceneをインクルード
#include "KamataEngine.h"

using namespace KamataEngine;

class OverScene : public IScene { // ISceneを継承
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	~OverScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

	bool IsFinished() const override { return finished_; }

private:
	static inline const float kTimeOverMove = 2.0f;

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformOver_;
	WorldTransform worldTransformPlayer_;

	Model* modelPlayer_ = nullptr;
	Model* modelTitle_ = nullptr;

	float counter_ = 0.0f;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
};
