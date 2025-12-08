# ゲームオーバー・ゲームクリアシーン実装

## 概要
ゲームオーバーとゲームクリアのための専用シーンを実装しました。

## 実装内容

### 新しいシーンクラス

#### 1. GameOverScene (ゲームオーバーシーン)
- **ファイル**: `GameOverScene.h`, `GameOverScene.cpp`
- **機能**:
  - 赤い "GAME OVER" テキストを画面中央に表示
  - フェードイン・フェードアウト演出
  - 3秒間自動表示、またはスペースキーでスキップ可能
  - 終了後にタイトル画面に戻る

#### 2. GameClearScene (ゲームクリアシーン)
- **ファイル**: `GameClearScene.h`, `GameClearScene.cpp`
- **機能**:
  - 緑の "GAME CLEAR" テキストを画面中央に表示
  - フェードイン・フェードアウト演出
  - 3秒間自動表示、またはスペースキーでスキップ可能
  - 終了後にタイトル画面に戻る

### シーン管理の更新

#### SceneManager の拡張
- **新しいシーンタイプ追加**:
  - `SceneType::kGameOver`
  - `SceneType::kGameClear`

- **シーン遷移フロー**:
  ```
  Title → Game → GameOver → Title
               ↓
               GameClear → Title
  ```

#### GameScene の更新
- **NextScene 列挙型追加**: ゲーム終了時の次のシーンを指定
- **シーン遷移ロジック**:
  - プレイヤー死亡時: `GameOverScene` へ
  - ボス撃破時: `GameClearScene` へ

### 表示タイミング

#### ゲームオーバーシーン
- プレイヤーが死亡
- デスパーティクルが終了
- フェードアウト
- **GameOverScene 表示**
- フェードアウト
- タイトル画面へ戻る

#### ゲームクリアシーン
- ボスを倒す
- 勝利演出（2秒）
- フェードアウト
- **GameClearScene 表示**
- フェードアウト
- タイトル画面へ戻る

## 使用方法

### シーンの表示時間変更
各シーンクラスの `kDisplayDuration` を変更:
```cpp
static inline const float kDisplayDuration = 3.0f; // 秒単位
```

### スキップ機能の変更
`Update()` メソッドの条件を変更:
```cpp
if (displayTimer_ >= kDisplayDuration || Input::GetInstance()->PushKey(DIK_SPACE))
```

## 変更されたファイル

### 新規作成
- `GameOverScene.h` - ゲームオーバーシーンのヘッダー
- `GameOverScene.cpp` - ゲームオーバーシーンの実装
- `GameClearScene.h` - ゲームクリアシーンのヘッダー
- `GameClearScene.cpp` - ゲームクリアシーンの実装

### 更新
- `SceneManager.h` - 新しいシーンタイプを追加
- `SceneManager.cpp` - シーン遷移ロジックを更新
- `GameScene.h` - NextScene列挙型を追加
- `GameScene.cpp` - シーン遷移フラグを設定

## テクスチャ

既存のテクスチャを使用:
- `Resources/gameover.png` - ゲームオーバー用テキスト
- `Resources/gameclear.png` - ゲームクリア用テキスト

## 動作確認

1. ゲームを起動
2. プレイヤーでボスに当たる → ゲームオーバーシーンが表示
3. ゲームを起動
4. ボスを倒す → ゲームクリアシーンが表示
5. 両方のシーンが3秒後またはスペースキーでタイトルに戻ることを確認
