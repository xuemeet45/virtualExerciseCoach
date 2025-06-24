# バーチャルエクササイズコーチ

C++、GTKmm、OpenCV、TensorFlow Liteで構築された、リアルタイム姿勢検出とエクササイズ指導のための最新デスクトップアプリケーションです。

## 特徴
- MoveNet（TensorFlow Lite）によるリアルタイム姿勢検出
- 骨格オーバーレイとキーポイントの可視化
- クリーンなGTK4ベースのGUI
- 拡張しやすいモジュラー構成

## ビルド手順

### CMake（推奨）
```sh
mkdir build
cd build
cmake ..
make
./VirtualExerciseCoach
```

### シェルスクリプトを使う場合
```sh
./compile_and_run_gtkmm.sh
```

## 必要環境
- GTKmm 4
- OpenCV 4
- TensorFlow Lite（ビルド済み静的ライブラリ）
- PostgreSQLクライアントライブラリ（データベース機能用）
- C++17対応コンパイラ（clang++またはg++）

## モデル
MoveNetモデルファイル（`movenet_singlepose_lightning.tflite`）を`models/`ディレクトリに配置してください。ビルド・実行時に自動的にコピーされます。

## 使い方
- アプリを起動し、カメラアクセスを許可してください。
- 骨格オーバーレイ付きでリアルタイムに姿勢が表示されます。
- GUIからエクササイズや各種機能にアクセスできます。

## プロジェクト構成
- `src/` — C++ソース・ヘッダファイル
- `models/` — TensorFlow Liteモデル
- `images/` — アプリ画像・アセット
- `build/` — CMakeビルドディレクトリ
- `compile_and_run_gtkmm.sh` — レガシービルドスクリプト

## ライセンス
MIT（またはご希望のライセンス）

---
[English README](README.md) 