#include <algorithm>
#include "CameraController.h"
#include "Math.h"
#include <iostream> // std::cout を使用するために追加
#include "Player.h"

// 線形補間を行うヘルパー関数
// start: 開始座標, end: 終了座標, t: 補間係数 (0.0f～1.0f)
Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;
	result.x = start.x + (end.x - start.x) * t;
	result.y = start.y + (end.y - start.y) * t;
	result.z = start.z + (end.z - start.z) * t;
	return result;
}

/// <summary>
/// カメラコントローラーの初期化
/// </summary>
/// <param name="camera">制御対象のカメラインスタンスへのポインタ</param>
void CameraController::Initialize(Camera* camera) { 
	camera_ = camera; // 制御するカメラを設定
}

/// <summary>
/// カメラコントローラーの更新処理
/// 毎フレーム呼び出され、カメラの位置を計算して更新します。
/// </summary>
void CameraController::Update() {
	// ターゲットのワールド座標を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// ターゲットの速度を取得
	const Vector3& targetVelocity = target_->GetVelocity();
	
	// カメラの目標位置を計算
	// ターゲットの現在位置にオフセットを加え、さらにターゲットの速度に応じた予測移動を加味します。
	// kVelocityBias: 速度バイアス。値が大きいほど、ターゲットの移動方向へカメラが先行します。
	destination_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	destination_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	destination_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

	// 現在のカメラ位置から目標位置へ線形補間（Lerp）で滑らかに移動させます。
	// kInterpolationRate: 補間レート。値が小さいほど移動が滑らかになります。
	camera_->translation_ = Lerp(camera_->translation_, destination_, kInterpolationRate);

	// ここから下の3つのブロックは、カメラの追従と移動範囲の制限ロジックです。

	// 追従対象のプレイヤー位置を基準としたカメラ位置の直接設定
	// オフセットを適用し、ターゲットを中心にカメラを配置します。
	camera_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_->translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_->translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	// targetMargin（ターゲットマージン）によるカメラ移動の制限
	// カメラがターゲットを中心に一定のマージン内でしか動かないように制限します。
	// これにより、ターゲットがマージン内を移動している間はカメラが不動になり、より安定した視点を提供します。
	camera_->translation_.x = max(camera_->translation_.x, destination_.x + targetMargin.left);
	camera_->translation_.x = min(camera_->translation_.x, destination_.x + targetMargin.right);
	camera_->translation_.y = max(camera_->translation_.y, destination_.y + targetMargin.bottom);
	camera_->translation_.y = min(camera_->translation_.y, destination_.y + targetMargin.top);

	// movableArea_（移動可能範囲）によるカメラ移動の制限
	// カメラがワールドの特定の矩形範囲内から出ないように最終的な制限をかけます。
	camera_->translation_.x = max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = max(camera_->translation_.y, movableArea_.bottom); 
	camera_->translation_.y = min(camera_->translation_.y, movableArea_.top);


	// カメラの変換行列を更新
	// カメラの位置が変更された後に必ず呼び出す必要があります。
	camera_->UpdateMatrix();

	// デバッグ出力 (Log関数がない場合)
	std::cout << "Camera Y: " << camera_->translation_.y << ", Camera Z: " << camera_->translation_.z << std::endl;
}

/// <summary>
/// カメラをリセットする処理
/// ターゲットの現在位置にオフセットを適用した位置にカメラを瞬時に移動させます。
/// 主にシーンの開始時や、ゲームオーバー後のリスタート時などに使用されます。
/// </summary>
void CameraController::Reset() { 
	// ターゲットのワールド座標を取得
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// カメラをターゲットの位置 + オフセットに直接設定
	camera_->translation_ = {targetWorldTransform.translation_.x + targetOffset_.x, targetWorldTransform.translation_.y + targetOffset_.y, targetWorldTransform.translation_.z + targetOffset_.z};
}
