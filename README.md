# Dynamixel用RTコンポーネント

大阪電気通信大学  
澤崎 悠太，升谷 保博  
2019年11月23日

## はじめに

- ROBOTIS社のロボットアクチェータDynamixel複数台とUSBと介して接続し，入力された位置と速度の指令をアクチュエータへ送ったり，アクチェータから取得した現在位置を出力したりするRTコンポーネントです．
- 以下の環境で開発，動作確認しています．
  - Dynamixel AX-12, XL-320, XM-430
  - Windows 10 64bit版
  - Visual Studio 2015
  - OpenRTM-aist 1.2.0 64bit版
  - DynamixelSDK 3.7
- 同じ種類のDynamixelが同じバスに任意個数接続されており，それらのIDが1から個数まで1刻みで付けられていることを前提にしています．

## 仕様

### 入力ポート
- goalPosition
  - 型: RTC::TimedUShortSeq
  - 概要： DynamixelsのControl Tableの`Goal Position`に設定する値
- movingSpeed
  - 型: RTC::TimedUShortSeq
  - 概要： DynamixelsのControl Tableの`Moving Speed`または`Profile Velocity`に設定する値
- pGain
  - 型： RTC::TimedUShortSeq
  - 概要： DynamixelsのControl Tableの`Compliance Slope`または`P Gain`または`Position P Gain`に設定する値．0の場合は特別にトルクオフにする．

### 出力ポート
- presentPosition
  - 型: RTC::TimedUShortSeq
  - 概要： DynamixelsのControl Tableの`Present Position`から読み出した値
- moving
  - 型: RTC::TimedUShortSeq
  - 概要： DynamixelsのControl Tableの`Moving`から読み出した値．エラーが起きている場合はそれも表す．

### コンフィギュレーション

- NUM_ACTUATOR
  - 型: short
  - 概要： アクチュエータの数
- DEVICENAME
  - 型: string
  - 概要： Dynamixelと通信するためのシリアルポートの名前
- BAUDRATE
  - 型: int
  - 概要： 通信速度 [bps]
- MODEL
  - 型: string
  - 概要： アクチュエータの種類（AX12またはXL320またはXM430）
- initialPositionString
  - 型: string
  - 概要： 初期位置．空白で区切った複数の数値の文字列を与えることができる．例：NUM_ACTUATOR = 5の場合，`10`ならば，10,10,10,10,10が設定され，`10 20 30`ならば，10,20,30,30,30が設定される．
- initialSpeedString
  - 型: string
  - 概要： 初期速度．空白で区切った複数の数値の文字列を与えることができる．例：NUM_ACTUATOR = 5の場合，`10`ならば，10,10,10,10,10が設定され，`10 20 30`ならば，10,20,30,30,30が設定される．  

## インストール

- [OpenRTM-aist 1.2.0](https://www.openrtm.org/openrtm/ja/download/openrtm-aist-cpp/openrtm-aist-cpp_1_2_0_release)をインストール．
- [DynamixelSDK](http://emanual.robotis.com/docs/en/software/dynamixel/dynamixel_sdk/overview/)をインストール．
  - [GitHubのDynamixelSDKリポジトリ](https://github.com/ROBOTIS-GIT/DynamixelSDK)をクローン
  - `DynamixelSDK\c++\build\win64\dxl_x64_cpp.sln` を開いてReleaseでビルド
  - `DynamixelSDK\c++\build\win64\output`の内容を例えば`C:\Program Files\DynamixelSDK\lib`へコピー
  - `DynamixelSDK\c++\include\dynamixel_sdk`の内容を例えば`C:\Program Files\DynamixelSDK\include`へコピー
  - DynamixelSDKのヘッダファイルとライブラリを適切な場所にインストールする．例えば，
  - DLLファイルのインストールされたディレクトリのパス（`C:\Program Files\DynamixelSDK\lib`）を環境変数のPathの並びに追加する．
- [Dynamixel](https://github.com/MasutaniLab/Dynamixel)をクローンかダウンロードする．
- CMake
  - ビルドディレクトリはトップ直下の`build`
  - ConfigureはVisual Studio 64bit
  - キャッシュ変数の`DYNAMIXELSDK_DIR`にDynamixelSDKをインストールしたディレクトリを設定する．
  - キャッシュ変数の`DYNAMIXELSDK_LIBRARIES`にDynamixelSDKのファイル名を設定する．
- `build\Dynamixel.sln`をVisual Studioで開く．
- ビルド

## 使い方

- DynamixelをUSB2Dynamixelなどを介してPCのUSBポートに接続する．
- Windowsのデバイスマネージャーの設定で，対応するUSB Serial PortのプロパティでLatency Timeを最小の1(msec)に変更する．
- RTCのコンフィギュレーションで，アクチュエータの数，シリアルポート名，通信速度，モデル，初期位置，初期速度を変更できる．
- アクティベートすると設定した初期速度で設定した初期位置へ移動する．
- ディアクティベートしても安全のためトルクはオフにならない．
- 過大トルクなどでアラームシャットダウン（脱力）に陥ると，コンポーネントがエラー状態へ遷移する．
- エラーをリセットすると，トルクオフし，電源を入れ直さなくても再び使えるようになる．
- TkInterを使ったPythonのRTコンポーネントと組み合わせてテストできるようにしている．
  - [TestDynamixel.bat](TestDynamixel/TestDynamixel.bat): AX-12とXL-320のテスト用バッチファイル
  - [TestDynamixelXM430.bat](TestDynamixel/TestDynamixelXM430.bat): XM-430のテスト用バッチファイル

## 既知の問題・TODO

- モデルはAX12とXL320とXM430にしか対応していない．
- 複数種類のアクチェータを接続することは想定していない．
- IDは1から始まる連続した値であること想定している．
