#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Enemy;

class MapChipField;

class Player {
	enum class LRDirection {
		kRight,
		kLeft,
	};

	LRDirection lrDirection_ = LRDirection::kRight;

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner
	};

public:
	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		KamataEngine::Vector3 move;
	};

	/// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void InputMove();
	void AnimateTurn();
	/// 更新
	void Update();
	/// 描画
	void Draw();

	float turnFirstRotationY_ = 0.0f;

	float turnTimer_ = 0.0f;

	bool prevOnGround_ = false;

	float angle_ = 0.0f;
	float cosValue_ = 0.0f;
	float sinValue_ = 0.0f;

	bool onGround_ = true;

	//回転の所要時間
	static inline const float kTimeTurn = 0.3f;

	// プレイヤー加速度 
	static inline const float kAcceleration = 0.04f;

	static inline const float kAttenuation = 0.4f;


	// 重力加速度　小さくするとジャンプがゆっくりになるｙお
	static inline const float kGravityAcceleration = 0.05f;

	// 落下速度の上限
	static inline const float kLimitFallSpeed = 0.5f;

	// ジャンプ初速　ジャンプのたかさ
	static inline const float kJumpAcceleration = 0.6f;

	static inline const float kWidth = 0.8f;

	static inline const float kHeight = 0.8f;

	KamataEngine::Vector3 velocity_ = {};

	KamataEngine::Camera* camera_ = nullptr;

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; };

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void CheckMapMove(const CollisionMapInfo& info);

	void CheckMapWall(const CollisionMapInfo& info);

	void CheckMapCeiling(const CollisionMapInfo& info);

	// 接地状態に切れ変え処理
	void CheckMapLanding(const CollisionMapInfo& info);
	// ワールド座標取得
	KamataEngine::Vector3 GetWorldPosition();

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	void SetGrab(bool grab) { isGrab_ = grab; }
	void SetGrabPosition(const KamataEngine::Vector3& pos) { grabPosition_ = pos; }
	const KamataEngine::Vector3& GetGrabPosition() const { return grabPosition_; }
	bool IsGrab() const { return isGrab_; }

	// げった
	bool isDead() const { return isDead_; }

private:
	// 音声再生ハンドル
	uint32_t soundJump_ = 0;


	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	MapChipField* mapChipField_ = nullptr;

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	// 隙間
	static inline const float kBlank = 0.1f;
	// 着地時の速度減衰
	static inline const float kAttenuationLanding = 0.8f;
	// 微小な数値
	static inline const float kGroundSearchHeight = 0.1f;
	// 着地時の距離減衰率
	static inline const float kAttenuationWall = 0.5f;

	bool prevSpace_ = false;

	bool isGrab_ = false;

	// ですフラグ
	bool isDead_ = false;

	KamataEngine::Vector3 grabPosition_;
};
