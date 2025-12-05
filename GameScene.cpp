#include "GameScene.h"
#include "Math.h"

using namespace KamataEngine;
GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete debugCamera_;
	delete skydome_;
	delete mapChipField_;
	delete boss_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete deathParticles_;
	delete deathParticle_model_;
}

void GameScene::Initialize() {
	// 3Dモデルデータの生成
	model_ = Model::Create();
	// カメラの初期化
	camera_.Initialize();

	// スカイドームの生成
	skydome_ = new Skydome();
	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// スカイドームの初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// ブロックモデル
	block_model_ = Model::CreateFromOBJ("block");

	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_model_ = Model::CreateFromOBJ("player");
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 15);

	player_->SetMapChipField(mapChipField_);

	player_->Initialize(player_model_, &camera_, playerPosition);

	CController_ = new CameraController();
	CController_->Initialize(&camera_);
	CController_->SetTarget(player_);
	CController_->Reset();

	CameraController::Rect cameraArea = {15.0f, 30 - 15.0f, 11.0f, 11.0f};
	CController_->SetMovableArea(cameraArea);

	boss_model_ = Model::CreateFromOBJ("enemy");

	boss_ = new Boss();
	Vector3 bossPosition = mapChipField_->GetMapChipPositionByIndex(20, 15);

	// ボスのスケールが大きいため、Y位置を調整
	bossPosition.y += 0.5f;

	boss_->Initialize(boss_model_, &camera_, bossPosition);

	// ボスの移動範囲を設定（マップの端で反転）
	Vector3 leftBound = mapChipField_->GetMapChipPositionByIndex(1, 15);
	Vector3 rightBound = mapChipField_->GetMapChipPositionByIndex(28, 15);
	boss_->SetMovementBounds(leftBound.x, rightBound.x);

	// ボスにプレイヤーの参照を設定（突進攻撃用）
	boss_->SetPlayer(player_);

	deathParticle_model_ = Model::CreateFromOBJ("deathParticle");

	phase_ = Phase::kPlay;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:

		if (player_->IsDead()) {
			// 死亡演出
			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(deathParticle_model_, &camera_, deathParticlesPosition);
		}

		// ボスを倒したら勝利演出へ
		if (boss_->IsDead()) {
			phase_ = Phase::kVictory;
			victoryTimer_ = 0.0f;
		}
		break;
	case Phase::kDeath:
		break;
	case Phase::kVictory:
		break;
	}
}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {

		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::Update() {
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kPlay;
		}

		skydome_->Update();
		/*CController_->Update(); */

		player_->Update();
		boss_->Update();

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// フラグをトグル
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock)
					continue;

				// アフィン変換～DirectXに転送
				WorldTransformUpdate(*worldTransformBlock);
			}
		}
		break;
	case Phase::kPlay:
		skydome_->Update();
		CController_->Update();
		// 自キャラの更新
		player_->Update();
		boss_->Update();

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			// フラグをトグル
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		};
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock)
					continue;

				// アフィン変換～DirectXに転送
				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		CheckAllCollisions();
		break;
	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}

		skydome_->Update();
		CController_->Update();

		boss_->Update();

		if (deathParticles_) {
			deathParticles_->Update();
		}
		break;
	case Phase::kVictory:
		// 勝利演出
		victoryTimer_ += 1.0f / kFrameRate;
		if (victoryTimer_ >= kVictoryDuration) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		skydome_->Update();
		CController_->Update();
		player_->Update();
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				// アフィン変換～DirectXに転送
				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}

		skydome_->Update();
		CController_->Update();

		boss_->Update();

		break;
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// スカイドームの描画
	skydome_->Draw();

	if (!player_->IsDead())
		player_->Draw();
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			block_model_->Draw(*worldTransformBlock, camera_);
		}
	}

	boss_->Draw();
	if (player_->IsDead())
		if (deathParticles_) {
			deathParticles_->Draw();
		}

	Model::PostDraw();
	// スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();

	// fade_->Draw();
}

void GameScene::CheckAllCollisions() {

	// 判定対象1と2の座標
	AABB aabb1, aabb2;

#pragma region 自キャラと敵キャラの当たり判定
	{
		// ボスが死んでいる場合は判定しない
		if (boss_->IsDead()) {
			return;
		}

		aabb1 = player_->GetAABB();

		// 自キャラと敵弾全ての当たり判定
		// 敵弾の座標
		aabb2 = boss_->GetAABB();
		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			// 踏みつけ攻撃かチェック
			if (player_->IsStompingBoss(boss_)) {
				// ボスにダメージを与える
				boss_->TakeDamage(1);
				// プレイヤーを跳ね上げる
				player_->BounceFromStomp();
			} else {
				// 通常の衝突時はプレイヤーがダメージ
				player_->OnCollision(boss_);
				boss_->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

// ファイル保存用