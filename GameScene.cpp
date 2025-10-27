#include "GameScene.h"
#include "CameraController.h"
#include "MyMath.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	phase_ = Phase::kFadeIn;
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("./Resources./uvChecker.png");

	// 3Dモデルの生成
	modelBlock_ = Model::CreateFromOBJ("block");
	modelSkydome_ = Model::CreateFromOBJ("SkyDome", true);
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true);
	modelGrab_ = Model::CreateFromOBJ("grab", true);
	modelGoal_ = Model::CreateFromOBJ("goal", true);
	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	// マップチップフィールドの初期化
	mapChipField_->LoadMapchipCsv("Resources/blocks.csv");

	// 自キャラ生成
	player_ = new Player();

	// 自キャラ座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(6, 30);

	// 敵生成

	// 敵座標をマップチップ番号で指定
	std::vector<KamataEngine::Vector2> enemyTilePositions = {
	    {10, 30}, // 1つ目
	    
	};

	// 敵座標をマップチップ番号で指定
	for (const auto& tilePos : enemyTilePositions) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(static_cast<uint32_t>(tilePos.x), static_cast<uint32_t>(tilePos.y));
		Vector3 enemySize = {1.0f, 1.0f, 1.0f};
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

// 自キャラの初期化
	player_->Initialize(modelPlayer_, &camera_, playerPosition);

	skydome_ = new Skydome();

	player_->SetMapChipField(mapChipField_);

	// カメラの初期化
	camera_.Initialize();

	cameraController_ = new CameraController();

	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	CameraController::Rect cameraAera = {12.0f,12.0f, 6.0f, 100 - 6.0f};
	cameraController_->SetMovableArea(cameraAera);

	// 仮生成パーティクル
	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(modelDeathParticle_, &camera_, playerPosition);

	// ゴールの初期化
	Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(14,32); //ゴール置く位置 変える
	Vector3 goalSize = {1.0f, 1.0f, 1.0f};
	goal_.Initialize(goalPosition, goalSize, modelGoal_);
	

	// つかむ場所のマップチップ番号リスト
	std::vector<KamataEngine::Vector2> grabTilePositions = {
	    {12, 32}, // 1つ目
	    {16, 28}, // 2つ目
	    {12, 25},  // 3つ目
	    {14,  23},  // 4つ目
	    {10, 20}  //  5つ目
	};

	// grapの初期化
	for (const auto& tilePos : grabTilePositions) {
		Grab* grabGoal = new Grab();
		Vector3 grabPosition = mapChipField_->GetMapChipPositionByIndex(static_cast<uint32_t>(tilePos.x), static_cast<uint32_t>(tilePos.y));
		Vector3 grabSize = {1.0f, 1.0f, 1.0f};
		grabGoal->Initialize(grabPosition, grabSize, modelGrab_);
		grabs_.push_back(grabGoal);
	}

	// 他の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	GenerateBlocks();

	cameraController_->Reset();

	// カメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成
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

void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	AABB aabb1, aabb2;

	aabb1 = player_->GetAABB();

	for (Enemy* enemy : enemies_) {

		aabb2 = enemy->GetAABB();

		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);

			enemy->OnCollision(player_);
		}
	}

player_->SetGrab(false); // まずリセット

	for (Grab* grab : grabs_) {
		if (IsCollision(aabb1, grab->GetAABB())) {
			player_->SetGrab(true);
			player_->SetGrabPosition(grab->GetPosition());
			break;
		}
	}

	if (IsCollision(aabb1, goal_.GetAABB())) {
		finished_ = true;
		isclear_ = true;
	}


#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズ処理
		if (player_->isDead() == true) {
			phase_ = Phase::kDeath;

			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:

		if (deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}

		break;

	case Phase::kFadeIn:
		if (fade_->isFinished()) {
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kFadeOut:
		if (fade_->isFinished()) {
			finished_ = true;
		}
		break;
	};
}

void GameScene::Update() {

	fade_->Update();

	switch (phase_) {
	case Phase::kPlay:
		goal_.Update();
		for (Grab* grap : grabs_) {
			grap->Update(); 
		}

		CheckAllCollisions();

		break;
	case Phase::kDeath:
		deathParticles_->Update();
		break;
	case Phase::kFadeIn:

		fade_->Update();

		break;
	case Phase::kFadeOut:

		fade_->Update();
		CheckAllCollisions();
		if (phase_ == Phase::kFadeOut) {
			deathParticles_->Update();
		}
		break;
	}

	// 共通の処理
	if (phase_ != Phase::kFadeOut) {
		player_->Update();
	}

	// Skyblock
	skydome_->Update();

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// カメラコントロール
	cameraController_->Update();

	// デバッグカメラの更新
	debugCamera_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			// アフィン変換行列の作成
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	ChangePhase();

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
		camera_.TransferMatrix();
		// camera_.UpdateMatrix();
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	if (skydome_) {
		skydome_->Draw();
	}

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 自キャラの描画

	if (!player_->isDead()) {
		if (phase_ == Phase::kPlay || phase_ == Phase::kFadeIn)
			player_->Draw();
	}

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	goal_.Draw(&camera_);

	for (Grab* goal : grabs_) {
		goal->Draw(&camera_);
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	Model::PostDraw();

	fade_->Draw();
}

GameScene::~GameScene() {
	delete modelBlock_;
	delete debugCamera_;
	delete modelPlayer_;
	delete modelEnemy_;
	delete deathParticles_;
	delete fade_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	for (Grab* goal : grabs_) {
		delete goal;
	}
	grabs_.clear();
	delete modelSkydome_;
	delete mapChipField_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	
	//音声停止
	Audio::GetInstance()->StopWave(voiceHandle_);
}
