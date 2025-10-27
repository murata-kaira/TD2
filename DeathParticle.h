#pragma once
#include "KamataEngine.h"
#include"MyMath.h"
#include<array>
#define _USE_MATH_DEFINES
#include<math.h>

class DeathParticle
{
public:

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3 position);

	void Update();
	
	void Draw();

	//void IsFinished();

	//パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<KamataEngine::WorldTransform, kNumParticles> WorldTransforms_;





	//存続時間
	static inline const float kDuration = 1.0f;
	
	//移動の速さ
	static inline const float kSpeed = 0.1f;
	
	//分割した1個分の角度
	static inline const float kAngleUnit = 2 * float(M_PI) / 8.0f;


	//終了フラグ
	bool isFinished_ = false;
	//経過時間カウント
	float counter_ = 0.0f;
	
	//色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;
	//色の数値
	KamataEngine::Vector4 color_;





private:

	// ワールド変換データ
	//KamataEngine::WorldTransform worldTransform_;
	
	// カメラ
	KamataEngine::Camera* camera_;
	
	// モデル
	KamataEngine::Model* model_;

};