#include "GameScene.h"
#include "Math.h"
#include <numbers>

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
	delete modelBall_;
	delete modelHole_;
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

	// ゴルフモード用の初期化
	modelBall_ = Model::CreateFromOBJ("player"); // ボールとしてプレイヤーモデルを使用
	modelHole_ = Model::CreateFromOBJ("goal");   // ホールとしてゴールモデルを使用
	
	worldTransformBall_.Initialize();
	worldTransformBall_.translation_ = {0.0f, 0.5f, -10.0f};
	worldTransformBall_.scale_ = {0.5f, 0.5f, 0.5f};
	
	worldTransformHole_.Initialize();
	worldTransformHole_.translation_ = {0.0f, 0.1f, 30.0f};
	worldTransformHole_.scale_ = {1.5f, 0.2f, 1.5f};
	
	golfPhase_ = GolfPhase::kAim;
	golfShotCount_ = 0;
	golfAimAngle_ = 0.0f;
	golfPower_ = 0.0f;
	ballVelocity_ = {0.0f, 0.0f, 0.0f};
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
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kDeath;
		}


		break;
	case Phase::kVictory:
		fade_->Update();
		if (fade_->IsFinished()) {
			// 勝利後はゴルフモードへ遷移
			phase_ = Phase::kGolf;
			golfPhase_ = GolfPhase::kAim;
			golfShotCount_ = 0;
			golfAimAngle_ = 0.0f;
			golfPower_ = 0.0f;
			ballVelocity_ = {0.0f, 0.0f, 0.0f};
			// ボールとホールの位置をリセット
			worldTransformBall_.translation_ = {0.0f, 0.5f, -10.0f};
			worldTransformHole_.translation_ = {0.0f, 0.1f, 30.0f};
		}

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
	case Phase::kGolf:
		// ゴルフモードの更新
		switch (golfPhase_) {
		case GolfPhase::kAim:
			// 左右キーで角度を調整
			if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				golfAimAngle_ += 0.05f;
			}
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				golfAimAngle_ -= 0.05f;
			}

			// スペースキーでパワー調整フェーズへ
			if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
				golfPhase_ = GolfPhase::kPower;
				golfPower_ = 0.0f;
				golfPowerDirection_ = 1.0f;
			}
			break;

		case GolfPhase::kPower:
			// パワーゲージを往復させる
			golfPower_ += golfPowerDirection_ * 0.5f;
			if (golfPower_ >= kGolfMaxPower) {
				golfPower_ = kGolfMaxPower;
				golfPowerDirection_ = -1.0f;
			} else if (golfPower_ <= 0.0f) {
				golfPower_ = 0.0f;
				golfPowerDirection_ = 1.0f;
			}

			// スペースキーで打つ
			if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
				// 速度を設定
				ballVelocity_.x = std::sin(golfAimAngle_) * golfPower_ * kGolfPowerScale;
				ballVelocity_.z = std::cos(golfAimAngle_) * golfPower_ * kGolfPowerScale;
				golfShotCount_++;
				golfPhase_ = GolfPhase::kShot;
			}
			break;

		case GolfPhase::kShot:
			// ボールの移動
			worldTransformBall_.translation_.x += ballVelocity_.x;
			worldTransformBall_.translation_.z += ballVelocity_.z;

			// 摩擦による減速
			ballVelocity_.x *= kGolfFriction;
			ballVelocity_.z *= kGolfFriction;

			// ボールが止まったか判定（速度の2乗で比較）
			float speedSquared = ballVelocity_.x * ballVelocity_.x + 
			                     ballVelocity_.z * ballVelocity_.z;
			if (speedSquared < kGolfStopThreshold * kGolfStopThreshold) {
				ballVelocity_ = {0.0f, 0.0f, 0.0f};

				// ホールに入ったか判定（距離の2乗で比較）
				float dx = worldTransformBall_.translation_.x - worldTransformHole_.translation_.x;
				float dz = worldTransformBall_.translation_.z - worldTransformHole_.translation_.z;
				float distanceSquared = dx * dx + dz * dz;

				if (distanceSquared < kGolfHoleRadius * kGolfHoleRadius) {
					// ホールイン！
					golfPhase_ = GolfPhase::kResult;
					golfResultTimer_ = 0.0f;
				} else {
					// 次のショット
					golfPhase_ = GolfPhase::kAim;
				}
			}
			break;

		case GolfPhase::kResult:
			golfResultTimer_ += 1.0f / kFrameRate;
			if (golfResultTimer_ >= kGolfResultDuration) {
				fade_->Start(Fade::Status::FadeOut, 1.0f);
				phase_ = Phase::kFadeOut;
			}
			break;
		}

		// ゴルフ用のカメラ更新（ボールに追従）
		camera_.translation_.x = worldTransformBall_.translation_.x;
		camera_.translation_.y = 20.0f;
		camera_.translation_.z = worldTransformBall_.translation_.z - 30.0f;
		camera_.rotation_.x = 0.5f;
		camera_.TransferMatrix();

		// ボールとホールの行列更新
		WorldTransformUpdate(worldTransformBall_);
		WorldTransformUpdate(worldTransformHole_);

		// 地面（ブロック）の更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
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

	// ゴルフモードの場合
	if (phase_ == Phase::kGolf) {
		// 地面（ブロック）を描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				block_model_->Draw(*worldTransformBlock, camera_);
			}
		}

		// ホールを描画
		modelHole_->Draw(worldTransformHole_, camera_);

		// ボールを描画
		modelBall_->Draw(worldTransformBall_, camera_);
	} else {
		// 通常モードの描画
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