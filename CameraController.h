#pragma once
#include "KamataEngine.h"

// 前方宣言
class Player;
using namespace KamataEngine;

/// <summary>
/// カメラの制御を管理するクラス
/// プレイヤーなどのターゲットに追従し、特定の範囲内にカメラを制限する機能を提供します。
/// </summary>
class CameraController {
public:
	/// <summary>
	/// カメラの移動可能範囲を定義する構造体
	/// </summary>
	struct Rect {
		float left = 0.0f;   // 左端
		float right = 1.0f;  // 右端
		float bottom = 0.0f; // 下端
		float top = 1.0f;    // 上端
	};
	/// <summary>
	/// 初期化処理
	/// カメラオブジェクトへのポインタを設定します。
	/// </summary>
	/// <param name="camera">制御するカメラオブジェクトのポインタ</param>
	void Initialize(Camera* camera);

	/// <summary>
	/// 更新処理
	/// 毎フレーム呼び出され、カメラの位置を更新します。
	/// ターゲットへの追従、速度バイアスによる予測移動、移動範囲の制限などを行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 追従対象のプレイヤーを設定します。
	/// </summary>
	/// <param name="target">追従するPlayerオブジェクトのポインタ</param>
	void SetTarget(Player* target) { target_ = target; }

	/// <summary>
	/// カメラの位置をリセットします。
	/// 主にシーン開始時などにターゲットの位置にカメラを瞬時に移動させるために使用します。
	/// </summary>
	void Reset();

	/// <summary>
	/// カメラの移動可能範囲を設定します。
	/// </summary>
	/// <param name="area">カメラが移動できる矩形範囲</param>
	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	Camera* camera_ = nullptr; // 制御対象のカメラオブジェクト
	Player* target_ = nullptr; // 追従対象のプレイヤーオブジェクト
	
	// 追従対象とカメラの座標の差（オフセット）
	// カメラがターゲットに対してどれだけ離れた位置に配置されるかを設定します。
	Vector3 targetOffset_ = {0, 0, -20.0f};

	// カメラ移動範囲
	// カメラが移動できるワールド座標系での矩形範囲を定義します。
	Rect movableArea_ = {0, 100, 0, 100};

	// カメラが目指す最終的な座標
	Vector3 destination_;

	// カメラの線形補間レート
	// 値が小さいほど滑らかに、大きいほど素早く目的地に近づきます。
	static inline const float kInterpolationRate = 0.1f;

	// ターゲットの速度に基づいてカメラの予測移動を行う際のバイアス値
	// ターゲットの移動速度にこの値を乗算し、将来の位置を予測してカメラを先行させます。
	static inline const float kVelocityBias = 30.0f;

	// ターゲットの追従範囲マージン
	// カメラがターゲットの移動に追従する際に、ターゲットを中心にどれだけの余裕を持たせるかを定義します。
	// 例えば、ターゲットがこのマージン内を移動している間はカメラは動かず、マージンを超えるとカメラが動き始めます。
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};
};
