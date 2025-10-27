#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Skydome {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw();

	KamataEngine::Camera* camera_ = nullptr;

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;

};
