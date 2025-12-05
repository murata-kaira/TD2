#pragma once
#include "KamataEngine.h"
#include "Fade.h"
#include "IScene.h" // ISceneをインクルード

using namespace KamataEngine;

class TitleScene : public IScene { // ISceneを継承
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	~TitleScene();

	void Initialize() override;

	void Update() override;

	void Draw() override;

	bool IsFinished() const override { return finished_; }

private:
	static inline const float kTimeTitleMove = 2.0f;

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformPlayer_;

	Model* modelPlayer_ = nullptr;
	Model* modelTitle_ = nullptr;

	float counter_ = 0.0f;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;
};
