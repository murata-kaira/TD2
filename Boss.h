#pragma once

#include "KamataEngine.h"
#include "Math.h"

using namespace KamataEngine;
class Player;

class Boss {
public:
	// ボスの状態
	enum State {
		kPatrol,  // 通常パトロール
		kCharge,  // 突進攻撃
		kCooldown // 突進後のクールダウン
	};

	void Initialize(Model* model, Camera* camera, const Vector3& position);

	void Update();

	void Draw();

	AABB GetAABB();

	Vector3 GetWorldPosition();

	void OnCollision(const Player* player);

	// ダメージを受ける（踏みつけ攻撃）
	void TakeDamage(int damage);
	// 死亡判定
	bool IsDead() const { return isDead_; }
	// 現在のHP取得
	int GetHP() const { return hp_; }
	// 最大HP取得
	int GetMaxHP() const { return kMaxHP; }
	// 高さ取得
	float GetHeight() const { return kHeight; }

	// 移動範囲の設定
	void SetMovementBounds(float leftLimit, float rightLimit);

	// プレイヤーの参照を設定（突進攻撃用）
	void SetPlayer(Player* player) { player_ = player; }
	// 突進中かどうか
	bool IsCharging() const { return state_ == State::kCharge; }

private:
	WorldTransform worldTransform_;

	Model* model_ = nullptr;

	Camera* camera_ = nullptr;

	static inline const float kWalkSpeed = 0.02f;

	Vector3 velocity_ = {};

	static inline const float kWalkMotionAngleStart = 0.0f;

	static inline const float kWalkMotionAngleEnd = 30.0f;

	static inline const float kWalkMotionTime = 1.0f;

	float walkTimer = 0.0f;

	static inline const float kWidth = 1.6f;
	static inline const float kHeight = 1.6f;

	// 移動範囲の制限
	float leftLimit_ = 0.0f;
	float rightLimit_ = 100.0f;
	// 反転処理
	void ReverseDirection();

	// プレイヤー参照（突進攻撃用）
	Player* player_ = nullptr;
	// ボスの状態管理
	State state_ = State::kPatrol;
	// 突進攻撃関連
	static inline const float kChargeSpeed = 0.15f;     // 突進速度
	static inline const float kChargeInterval = 3.0f;   // 突進間隔（秒）
	static inline const float kChargeDuration = 1.0f;   // 突進継続時間（秒）
	static inline const float kCooldownDuration = 1.0f; // クールダウン時間（秒）
	float chargeTimer_ = 0.0f;                          // 突進用タイマー
	float stateTimer_ = 0.0f;                           // 状態遷移用タイマー
	// 突進攻撃を開始
	void StartCharge();
	// パトロール状態の更新
	void UpdatePatrol();
	// 突進状態の更新
	void UpdateCharge();

	// クールダウン状態の更新
	void UpdateCooldown();

	// ボスのスケール
	static inline const float kScale = 2.0f;
	// ボスのHP関連
	static inline const int kMaxHP = 3;
	int hp_ = kMaxHP;
	bool isDead_ = false;
	// ダメージ時の無敵時間
	static inline const float kInvincibleTime = 1.0f;
	float invincibleTimer_ = 0.0f;
	bool isInvincible_ = false;

	// フレームレート
	static inline const float kFrameRate = 60.0f;
	// 点滅パターン
	static inline const int kBlinkCycle = 4;
	static inline const int kBlinkOnFrames = 2;
};
