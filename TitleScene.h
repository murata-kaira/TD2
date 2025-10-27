#pragma once
#include <KamataEngine.h>

#include "Fade.h"
class TitleScene {
public:

	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	void Initialize();
	void Update();
	void Draw();
	~TitleScene();

	KamataEngine::Model* model_;
	KamataEngine::Model* modelPlayer_;

	KamataEngine::Camera camera_;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::WorldTransform worldTransformPlayer_;

	bool isFinished() const { return finished_; }

private:
	bool finished_ = false;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;

	//音声再生ハンドル
	uint32_t soundHandle_ = 0;
	uint32_t voiceHandle_ = 0;

};
