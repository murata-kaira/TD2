#pragma once
#include"Player.h"
#include<vector>
#include"Skydome.h"
#include"MapChipField.h"
#include "CameraController.h"
#include "Boss.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "IScene.h" // ISceneをインクルード

// ゲームシーン
class GameScene : public IScene { // ISceneを継承
public:
	~GameScene();
	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	void GenerateBlocks();

	void CheckAllCollisions();

	bool IsFinished() const override { return finished_; }

private:
	enum class Phase {
		kFadeIn, // フェードイン
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
		kVictory, // クリア演出
		kGolf,  // ゴルフモード
		kFadeOut, // フェードアウト
	};
	Phase phase_;

	void ChangePhase();

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;

	//カメラ
	KamataEngine::Camera camera_;
	// 自キャラ
	Player* player_ = nullptr;
	// プレイヤーモデル
	KamataEngine::Model* player_model_ = nullptr;

	// ブロックモデル
	KamataEngine::Model* block_model_ = nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;


	// スカイドーム
	Skydome* skydome_ = nullptr;

	// 3Dモデル
	KamataEngine:: Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	CameraController* CController_ = nullptr;

	Boss* boss_ = nullptr;

	KamataEngine::Model* boss_model_ = nullptr;

	DeathParticles* deathParticles_ = nullptr;

	Model* deathParticle_model_ = nullptr;

	bool finished_ = false;

	Fade* fade_ = nullptr;


	// 勝利演出用タイマー
	float victoryTimer_ = 0.0f;
	static inline const float kVictoryDuration = 2.0f;

	// フレームレート
	static inline const float kFrameRate = 60.0f;

	// ゴルフモード用
	enum class GolfPhase {
		kAim,     // 狙いを定める
		kPower,   // パワーを決める
		kShot,    // ショット後のボール移動
		kResult,  // 結果表示
	};
	GolfPhase golfPhase_ = GolfPhase::kAim;
	
	WorldTransform worldTransformBall_;
	Model* modelBall_ = nullptr;
	Vector3 ballVelocity_ = {0.0f, 0.0f, 0.0f};
	
	WorldTransform worldTransformHole_;
	Model* modelHole_ = nullptr;
	
	float golfAimAngle_ = 0.0f;
	float golfPower_ = 0.0f;
	float golfPowerDirection_ = 1.0f;
	int golfShotCount_ = 0;
	float golfResultTimer_ = 0.0f;
	
	// ゴルフ用定数
	static inline const float kGolfHoleRadius = 1.0f;
	static inline const float kGolfMaxPower = 30.0f;
	static inline const float kGolfPowerScale = 0.1f;
	static inline const float kGolfFriction = 0.95f;
	static inline const float kGolfStopThreshold = 0.1f;
	static inline const float kGolfResultDuration = 3.0f;

};
