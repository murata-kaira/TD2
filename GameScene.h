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

};
