#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Player;

class Enemy {
public:
	/// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	// 歩行スピード

	 static inline const float kGravity = 0.05f;  // 重力加速度
	static inline const float kJumpSpeed = 0.5f; // ジャンプ初速
	static inline const float kGroundY = 0.0f;   // 接地高さ（地面のy座標）

	float walkTimer_ = 0.0f;

	KamataEngine::Vector3 velocity_ = {};
	// AABBを取得
	AABB GetAABB();
	KamataEngine::Vector3 GetWorldPosition();

	//衝突応答
	void OnCollision(const Player* player);

private:
	KamataEngine::WorldTransform worldTransform_; // ワールドトランスふぉーむ
	KamataEngine::Model* model_ = nullptr;        // モデル
	KamataEngine::Camera* camera_ = nullptr;      // カメラ
	;
};
