#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Player;
class Golf {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	AABB GetAABB();

	

		void StartCharging();

	void Hit(const Player* player, float power);

	KamataEngine::Vector3 GetWorldPosition();

	bool IsInHole() const { return isInHole_; }

	void SetGoalPosition(const KamataEngine::Vector3& goalPosition) { goalPosition_ = goalPosition; }

		bool IsCharging() const { return isCharging_; }
	float GetChargePower() const { return chargePower_; }
	int GetShotCount() const { return shotCount_; }
	float GetLastShotDistance() const { return lastShotDistance_; }

	static constexpr float kMaxPower = 2.0f;

private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	KamataEngine::Vector3 goalPosition_ = {};

	bool isInHole_ = false;


		bool isCharging_ = false;
	float chargePower_ = 0.0f;
	int shotCount_ = 0;
	float lastShotDistance_ = 0.0f;
	KamataEngine::Vector3 shotStartPosition_ = {};


	static inline const float kGravityAcceleration = 0.05f;

static inline const float kFriction = 0.98f;

	static inline const float kBallRadius = 0.5f;

	static inline const float kGoalRadius = 0.5f;

	static inline const float kGroundHeight = 0.0f;

	static inline const float kBounceDamping = 0.6f;

	static inline const float kUpwardVelocityBase = 0.4f;
	static inline const float kChargeSpeed = 0.02f;
	static inline const float kMinPower = 0.3f;

	bool chargingUp_ = true;


	void ApplyPhysics();

	void CheckGoal();


	void UpdateCharging();
};