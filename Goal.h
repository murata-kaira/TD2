#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

struct AABB;

class Goal {
public:
	void Initialize(const KamataEngine::Vector3& position, const KamataEngine::Vector3& size, KamataEngine::Model* model);
	void Update();
	void Draw(KamataEngine::Camera* camera);

	// AABB 取得
	AABB GetAABB() const;

	// ゴールしたか？
	bool IsReached() const { return reached_; }

private:
	KamataEngine::Vector3 position_;
	KamataEngine::Vector3 size_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;

	bool reached_ = false;
};
