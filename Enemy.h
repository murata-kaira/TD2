#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "Player.h"

class MapChipField;
class Player;
class Enemy {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	AABB GetAABB();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void OnCollision(const Player* player);

	 KamataEngine::Vector3 GetWorldPosition();

	 void SetStageLimits(float left, float right) {
		 leftLimit_ = left;
		 rightLimit_ = right;
	 }

	  void SetPlayer(const Player* p) { player_ = p; }
	  void TakeDamage(int damage);
	 int GetHP() const { return hp_; }

	
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	MapChipField* mapChipField_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	float walkTimer_ = 0.0f;

	static inline const float kWalkSpeed = 0.05f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;


	  // ジャンプ用
	enum class JumpState { None, JumpUp, JumpDown, Landed, JumpStart };
	JumpState jumpState_ = JumpState::None;
	float jumpPower_ = 0.8f;
	float gravity_ = 0.1f;
	float jumpVelocity_ = 0.0f;
	float jumpTimer_ = 0.0f;
	float groundY_ = 0.0f;

	// 突進用
	bool isDashing_ = false;
	float dashSpeed_ = 0.15f;
	float dashDuration_ = 0.5f;
	float dashTimer_ = 0.0f;

   // 移動範囲制御
	int direction_ = -1; // 左向きスタート
	float leftLimit_ = 1.0f;
	float rightLimit_ = 20.0f;

	 // HP管理
	int hp_ = 200;
	int phase_ = 1;

	  // プレイヤー追尾用（後で拡張可能）
	const Player* player_ = nullptr;


};
