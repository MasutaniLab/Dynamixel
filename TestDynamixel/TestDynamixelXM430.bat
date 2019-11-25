@echo off
: Dynamixelのテスト


:ネーミングサービスの確認
rtls /localhost > nul
if errorlevel 1 (
  echo ネーミングサーバが見つかりません
  pause
  exit /b 1
  rem /bオプションは親を終わらせないために必須
)

:コンポーネントの起動
call ..\Dynamixel.bat
start "" TkSliderCommandXM430.pyw
start "" TkSliderSpeedXM430.pyw
start "" TkSliderGainXM430.pyw
start "" TkMonitorSliderPositionXM430.pyw
start "" TkMonitorSliderMoving.pyw

:コンポーネント名を変数化
set c=/localhost/TkSliderCommandXM4300.rtc
set s=/localhost/TkSliderSpeedXM4300.rtc
set g=/localhost/TkSliderGainXM4300.rtc
set p=/localhost/TkMonitorSliderPositionXM4300.rtc
set m=/localhost/TkMonitorSliderMoving0.rtc
set d=/localhost/Dynamixel0.rtc

:時間待ち
timeout 10

:コンフィギュレーション
rtconf %d% -s xm430 act

:接続
rtcon %c%:slider %d%:goalPosition
rtcon %s%:slider %d%:movingSpeed
rtcon %g%:slider %d%:pGain
rtcon %d%:presentPosition %p%:value
rtcon %d%:moving %m%:value

:アクティベート
rtact %c% %s% %g% %p% %m% %d%

:loop
set /p ans="終了しますか？ (y/n)"
if not "%ans%"=="y" goto loop

rtdeact %c% %s% %g% %p% %m% %d%

:終了（rtexitは，引数を一つずつ）
for %%i in (%c% %s% %g% %p% %m% %d%) do (
  rtexit %%i
)
