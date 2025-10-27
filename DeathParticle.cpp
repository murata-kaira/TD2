#include"DeathParticle.h"
#include"KamataEngine.h"
#include"Player.h"
#include"GameScene.h"
#include"cassert"
#include<numbers>
#include<algorithm>

#define NOMINMAX
#include<algorithm>
#include"MapChipField.h"
using namespace KamataEngine;
using namespace MathUtility;

void DeathParticle::Initialize(Model* model, Camera* camera, KamataEngine::Vector3 position)
{
	// NULLポイントチェック
	assert(model);

	model_ = model;

	// textureHandle_ = textureHandle;
	for (KamataEngine::WorldTransform& worldTransform : WorldTransforms_)
	{
		worldTransform.translation_ = position;
		worldTransform.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	}
	

	camera_ = camera;

	
	//worldTransform_.Initialize();


	//ワールド変換の初期化
	for (KamataEngine::WorldTransform& worldTransform :WorldTransforms_)
	{
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};

}

void DeathParticle::Update()
{

	
	for (KamataEngine::WorldTransform& worldTransform : WorldTransforms_)
	{
		// アフィン変換行列
		worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.TransferMatrix(); // プレイヤーの座標の計算

	}
	
	for (uint32_t i = 0; i < 8; i++)
	{
		//基本となる速度ベクトル
		KamataEngine::Vector3 velocity = {kSpeed, 0, 0};
		//回転角を計算する
		float angle = kAngleUnit * i;
		//Z軸まわり回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
	    //基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(velocity, matrixRotation);
	    //移動処理
		WorldTransforms_[i].translation_ += velocity;
	}
	//カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;
	
	
	
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	//色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);
	
	
	
	
	
	
	//存続時間の上限に達したら
	if (counter_>= kDuration)
	{
		counter_ = kDuration;
		//終了扱いにする
		isFinished_ = true;
	}
	//終了なら何もしない
	if (isFinished_)
	{
		return;
	}



}



void DeathParticle::Draw()
{ 


	for (KamataEngine::WorldTransform& worldTransform : WorldTransforms_)
	{
		model_->Draw(worldTransform, *camera_,&objectColor_); 
	}

	// 終了なら何もしない
	if (isFinished_)
	{
		return;
	}
}



