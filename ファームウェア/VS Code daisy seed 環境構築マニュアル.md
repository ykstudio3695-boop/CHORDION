# VS Code + Daisy Seed 環境構築マニュアル(Windows 11)

> 作成日: 2026-10-21
> 対象: Windows 11 / VS Code / Daisy Seed(STM32H750)
> 目的: C++ で Daisy Seed を開発できる環境を、あとから**自力で再現**できるようにする

---

## 0. このマニュアルの使い方

- 手順は **1 → 10** の順番に進めます。順番には意味があります。
  - ツールチェーンがないとライブラリをビルドできない
  - ライブラリがないとプロジェクトをビルドできない
- 各手順の最後に「**確認**」があります。ここが通ってから次へ進んでください。
- 詰まったら、巻末の「**トラブルシューティング**」を見てください。

### 全体像

| 層 | 中身 | 手順 |
|---|---|---|
| 1. 基本ツール | VS Code、Git for Windows | 1 |
| 2. ツールチェーン | make、arm-none-eabi-gcc、dfu-util、openocd | 2 |
| 3. エディタ設定 | VS Code 拡張機能 | 3 |
| 4. ライブラリ | DaisyExamples、libDaisy、DaisySP | 4〜5 |
| 5. USB 書き込み | Zadig(WinUSB ドライバ) | 6 |
| 6. 動作確認 | Blink の書き込み | 7 |
| 7. 自分の開発 | 新規プロジェクトの作成と書き込み | 8〜10 |

### この環境で確認できたバージョン(2026-10-21 時点)

| ツール | 確認したバージョン |
|---|---|
| GNU Make | 4.3 |
| arm-none-eabi-gcc | 10.2.1(GNU Arm Embedded Toolchain 10-2020-q4-major) |
| dfu-util | 0.10 |
| xPack OpenOCD | 0.11.0 |
| Python | 3.10 |

---

## 1. VS Code と Git for Windows

### 1-1. VS Code

インストーラ(`VSCodeUserSetup-x64-○○.exe`)を実行します。「追加タスクの選択」では次にチェックを入れます。

- [x] PATH への追加
- [x] エクスプローラーのファイルのコンテキストメニューに [Code で開く] アクションを追加する
- [x] エクスプローラーのディレクトリのコンテキストメニューに [Code で開く] アクションを追加する

### 1-2. Git for Windows

`git-scm.com` から Windows 用をダウンロードしてインストールします。

| 画面 | 選択 |
|---|---|
| Choosing the default editor | Use Visual Studio Code as Git's default editor |
| Adjusting your PATH environment | Git from the command line and also from 3rd-party software(デフォルト) |
| Configuring the line ending conversions | Checkout as-is, commit Unix-style line endings(推奨) |
| それ以外 | デフォルトのまま |

> 改行コードを「Checkout as-is」にする理由: デフォルトの CRLF 変換だと、clone した `.sh` スクリプトが Git Bash で動かないことがあるため。

### 確認

**新しく開いた** PowerShell で実行します。

```powershell
code --version
git --version
```

どちらもバージョン番号が出れば OK。スタートメニューから「Git Bash」も起動できることを確認します。

---

## 2. Daisy Toolchain のインストール

### 2-1. 事前確認

過去に GNU Arm Embedded Toolchain や GnuWin32 の make などを**個別に**入れたことがある場合は、環境変数の Path から古い項目を削除してから進めます(古い版が優先されるのを防ぐため)。

### 2-2. ダウンロード

公式の **DaisyToolchain** の Windows 用 zip を入手します。

- 旧 Wiki(`github.com/daisyaudio/DaisyWiki`)は 2026-08-27 にアーカイブされ、現在は `daisy.audio` / `docs.daisy.audio` に案内が移っています。
- 「Setting Up Your Development Environment」または「Daisy Toolchain」のページから Windows 用リンクをたどります。
- **リリースのファイル名・バージョンは変わる可能性があります。ダウンロード元の表記を優先してください。**

### 2-3. 展開と登録

1. zip を、**パスに日本語やスペースを含まない場所**に展開する(例: `C:\DaisyToolchain`)
2. 展開したフォルダ内の `install.bat` をダブルクリックする
   - `windows\bin` をユーザーの PATH に追加するだけの処理です。ウィンドウがすぐ閉じても完了しています。

> ⚠️ `install.bat` は展開した場所のパスを PATH に登録します。**あとからフォルダを移動すると PATH が壊れます。** 置き場所は最初に決めてください。

### 確認

**新しく開いた** PowerShell で、4 つとも通ることを確認します。

```powershell
make --version
arm-none-eabi-gcc --version
dfu-util --version
openocd --version
```

---

## 3. VS Code 拡張機能

| 拡張機能 | 発行元 | ID | 用途 |
|---|---|---|---|
| C/C++ | Microsoft | `ms-vscode.cpptools` | 補完・定義ジャンプ・エラー表示 |
| Cortex-Debug | marus25 | `marus25.cortex-debug` | デバッグ(ST-Link 等のプローブ使用時) |

```powershell
code --install-extension ms-vscode.cpptools
code --install-extension marus25.cortex-debug
```

### 確認

```powershell
code --list-extensions
```

2 つが一覧に含まれていれば OK。

> Cortex-Debug は、USB(DFU)書き込みだけなら使いません。新規プロジェクトのテンプレートに設定が含まれるため、入れておくだけです。

---

## 4. DaisyExamples の取得

作業フォルダは **日本語・スペースを含まない短いパス**にします。OneDrive 同期フォルダ(デスクトップ・ドキュメント)は避けます。

```powershell
cd C:\
mkdir dev
cd dev
git clone --recursive https://github.com/electro-smith/DaisyExamples
```

- `--recursive` が必須です(libDaisy / DaisySP / stmlib がサブモジュールのため)。
- GitHub の「Download ZIP」は**使わない**でください(サブモジュールの中身が入りません)。
- 容量は数百 MB、環境によっては数分〜十数分かかります。

### 確認

次のように `checked out` が出ていれば OK(libDaisy、DaisySP、stmlib の 3 つ)。

```
Submodule path 'DaisySP': checked out '...'
Submodule path 'libDaisy': checked out '...'
Submodule path 'stmlib': checked out '...'
```

空だった場合は次で取り直せます。

```bash
git submodule update --init --recursive
```

---

## 5. ライブラリのビルド

**Git Bash** で行います(PowerShell だと失敗しやすい)。

エクスプローラーで `C:\dev\DaisyExamples` を開き、空白部分を右クリック →「その他のオプションを確認」→「Git Bash Here」。

```bash
make -C libDaisy
make -C DaisySP
```

- libDaisy は数分かかります。大量のコンパイルログが流れるのは正常です。

### 確認

```bash
ls libDaisy/build/libdaisy.a
ls DaisySP/build/libdaisysp.a
```

2 つともファイル名が表示されれば OK。

> ライブラリを更新(`git pull` など)したら、再ビルドが必要です。

---

## 6. Zadig で USB ドライバを WinUSB にする

`dfu-util` が Daisy と通信するために、DFU モード時のデバイスに WinUSB ドライバを割り当てます。

### 6-1. DFU モードにする

1. Seed を USB ケーブルで PC に接続(**データ通信対応のケーブル**。充電専用は不可)
2. **BOOT ボタンを押したまま RESET を押して離す**
3. その後 BOOT も離す

→ LED は消灯のまま。デバイスマネージャーに「STM32 BOOTLOADER」と出れば DFU モードです。

### 6-2. Zadig での設定

1. `zadig.akeo.ie` から Zadig をダウンロード(インストール不要)
2. **DFU モードのまま**起動(管理者権限は「はい」)
3. メニュー **Options → List All Devices** にチェック
4. ドロップダウンから **「STM32 BOOTLOADER」**(USB ID: `0483 DF11`)を選ぶ
5. ドライバ欄が **WinUSB** になっていることを確認
6. **Replace Driver** をクリック

> ⚠️ **他のデバイス(マウス・キーボードなど)を選ばない**でください。必ず `0483 DF11` を確認します。

### 確認

DFU モードのまま:

```powershell
dfu-util -l
```

`Found DFU: [0483:df11]` を含む行が出れば OK(`Internal Flash` と `Option Bytes` の 2 行が出ます)。

---

## 7. Blink で動作確認

### 7-1. VS Code で開く

**Blink フォルダそのもの**を開きます。

- 右クリックの「Code で開く」が使えない場合は、次のどれかで開きます。
  - 「その他のオプションを確認」→「Code で開く」
  - Git Bash / PowerShell で `cd` してから `code .`
  - VS Code の **ファイル → フォルダーを開く** で `C:\dev\DaisyExamples\seed\Blink` を選ぶ

> ⚠️ `DaisyExamples` や `seed` を開いても、タスクは表示されません。タイトルバーに `Blink` と出ているか確認します。

### 7-2. ビルドと書き込み

`Ctrl+Shift+P` → `Tasks: Run Task`

| タスク | 内容 | DFU モード |
|---|---|---|
| `build` | コンパイルだけ(Seed と通信しない) | 不要 |
| `build_and_program_dfu` | ビルド + USB 書き込み | **必要** |

書き込み前に、毎回 BOOT+RESET で DFU モードにします。

### 確認

書き込み後、Seed の LED が点滅すれば成功です。

---

## 8. 自分のプロジェクトを作る

### 8-1. 前提

- Python 3 が必要(`python --version` で確認)
- プロジェクトは **`DaisyExamples\seed\` の直下**に作る
  - Makefile が `../../libDaisy` のような相対パスでライブラリを参照するため
- 名前は英数字のみ(例: `ChordionFarmware`)

### 8-2. 作成

Git Bash で:

```bash
cd /c/dev/DaisyExamples
python helper.py create seed/ChordionFarmware --board seed
```

`done` と出れば成功です。

### 確認

```bash
ls -la seed/ChordionFarmware
```

次のものがあれば OK。

| ファイル | 内容 |
|---|---|
| `Makefile` | ビルド設定(`TARGET` がプロジェクト名と一致していること) |
| `ChordionFarmware.cpp` | ソース(テンプレートは入力をそのまま出力に流すパススルー) |
| `.vscode/` | VS Code 設定(Cortex-Debug 含む) |
| `README.md` | 説明 |

### 8-3. 開く

```bash
cd seed/ChordionFarmware
code .
```

---

## 9. 最初のコード: サイン波を出す

### 9-1. ロジック

パススルーは、`AudioCallback` が入力を出力にコピーするだけでした。シンセでは、コールバックの中で**自分で波形を作って出力に入れます**。

1. `Oscillator`(DaisySP)をグローバルに宣言
2. **サンプルレートを設定した後**で `Init`、波形・周波数・振幅を設定
3. コールバックで `Process()` を呼び、1 サンプルずつ左右に出力
4. LED を点灯して、配線なしでも起動を確認できるようにする

### 9-2. コード(`ChordionFarmware.cpp` を丸ごと置き換え)

```cpp
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
Oscillator osc;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		float sig = osc.Process(); // 次の1サンプルを生成する
		out[0][i] = sig;           // 左
		out[1][i] = sig;           // 右
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	// サンプルレートを設定した後で初期化する
	osc.Init(hw.AudioSampleRate());
	osc.SetWaveform(Oscillator::WAVE_SIN);
	osc.SetFreq(440.0f); // ラの音
	osc.SetAmp(0.2f);    // 控えめな音量

	hw.SetLed(true); // 動作確認用: Seed の LED を点灯する

	hw.StartAudio(AudioCallback);
	while(1) {}
}
```

### 9-3. ビルドと書き込み

1. `Tasks: Run Task` → `build`
2. Seed を DFU モードにする
3. `Tasks: Run Task` → `build_and_program_dfu`
4. **LED が点灯**すれば、新しいプログラムが動いています

> 音の確認には、Seed のオーディオ出力ピンからアンプ等へ配線します。ピン配置は**必ず公式のピン配置図で確認**してください。最初はアンプの音量を絞った状態から始めます。

---

## 10. 日常の開発フロー(まとめ)

```
① VS Code で seed/<プロジェクト名> フォルダを開く
② .cpp を編集して保存
③ Tasks: Run Task → build        (エラーがないか確認)
④ Seed を DFU モードに (BOOT押しながらRESET)
⑤ Tasks: Run Task → build_and_program_dfu
⑥ Error 74 が出ても、"File downloaded successfully" があれば書き込み成功
```

### ビルド結果のメモリ使用量の見方

```
FLASH:  64520 B / 128 KB  49.22%
```

- **空に近いプログラムでも FLASH を約 60KB 使います**(土台の libDaisy など)。残りは約 64KB。
- 機能を増やすと 128KB を超えることがあります。超えた場合は QSPI フラッシュ(8MB)への配置を検討します(手順は公式資料で要確認)。

---

## トラブルシューティング

### A. VS Code のターミナルで `make` が見つからない

```
make : 用語 'make' は、コマンドレット、関数、スクリプト ファイル、または操作可能なプログラムの名前として認識されません。
```

**原因**: VS Code が古い PATH のまま起動している(`install.bat` より前から起動していた、または完全に終了していなかった)。

**対処**:
1. VS Code のウィンドウを**すべて**閉じる
2. **新しく**開いた PowerShell から起動する
   ```powershell
   code C:\dev\DaisyExamples\seed\Blink
   ```
3. それでも直らなければ、Windows のサインアウト→サインイン(または再起動)

### B. `rm -fR build` のエラーが出る

```
process_begin: CreateProcess(NULL, rm -fR build, ...) failed.
make: [...Makefile:306: clean] Error 2 (ignored)
```

**これは無視して問題ありません。** `(ignored)` と表示される通り、make が無視する設定です。PowerShell に `rm.exe` がないため出ます。

- 古い `build` を確実に消したいときは、Git Bash で `make clean` を実行するか、PowerShell で `Remove-Item -Recurse -Force build`。

### C. 書き込みの最後に `Error 74`

```
File downloaded successfully
Error during download get_status
make: *** [...Makefile:344: program-dfu] Error 74
```

**書き込みは成功しています。** `:leave` 指定で、書き込み後に Seed がプログラムへ移って USB 接続が切れるため、確認の応答が取れずに出るエラーです。VS Code 上では「終了コード 1」で失敗扱いになりますが、実害はありません。

- 成否は `File downloaded successfully` の有無と、実機の動作(LED・音)で判断します。

### D. `python helper.py create` で `FileExistsError`

```
FileExistsError: [WinError 183] 既に存在するファイルを作成することはできません。
```

**原因**: 作成先のフォルダが、すでに存在している。

**対処**:
```bash
ls -la seed/<プロジェクト名>        # 中身を確認
rmdir seed/<プロジェクト名>         # 空ならこれで削除(中身があれば失敗するので安全)
# 自分のコードがあるなら、削除せず退避する
mv seed/<プロジェクト名> seed/<プロジェクト名>_old
```
その後、`create` をやり直します。**`rm -rf` は、中身を確認せずに使わない**でください。

### E. VS Code でタスク(`build` など)が表示されない

**原因**: プロジェクトフォルダそのものを開いていない。
**対処**: タイトルバーにプロジェクト名(`Blink` など)が出ているか確認し、出ていなければフォルダを開き直す。

### F. 「Code で開く」が使えない

「その他のオプションを確認」の中を見る / `code .` を使う / VS Code のメニュー「ファイル → フォルダーを開く」を使う。

### G. `dfu-util -l` で何も表示されない

- DFU モードに入れていない(BOOT を押しながら RESET をやり直す)
- 充電専用ケーブルを使っている(データ通信対応のものに替える)
- Zadig で WinUSB を割り当てていない(手順 6)

### H. ビルド時に `cannot find -ldaisy` などのリンクエラー

ライブラリが未ビルド。手順 5 を実施する。

### I. PowerShell で make が不安定

シェルの違いによる失敗の可能性があります。VS Code 下部のターミナルを **Git Bash** に切り替えて、`make` / `make program-dfu` を直接実行します。

---

## 注意点・弱点(正直なメモ)

- **公式 Wiki は移転済み**で、ネット上の古い解説記事は旧方式のままのことが多い。混ぜると失敗します。
- **Windows ネイティブ環境は不具合報告が多い**。公式のテスト環境も Git Bash(MINGW64)です。詰まったら Git Bash に寄せるのが安全です。
- **書き込みのたびに BOOT+RESET が必要**。頻繁に書き換える段階では、ST-Link などのプローブ導入も選択肢です(追加機材が必要)。
- **DaisyExamples の中に自作コードを置くと、公式の更新(`git pull`)と混ざる恐れ**があります。販売を見据えるなら、後で自分専用の Git リポジトリに切り出します。その際は Makefile の `LIBDAISY_DIR` / `DAISYSP_DIR` を指定します。公式の README によると、Windows では**絶対パス(Windows 形式)で指定する必要がある**場合があります。
- **ブロックサイズ 4** はテンプレートの設定です。大きいほど CPU のオーバーヘッドは減りますが、遅延は増えます。重い処理を載せるときは見直します。
- **サイン波は動作確認向き**です。ノコギリ波などを使うときは、折り返しノイズを抑える `WAVE_POLYBLEP_SAW` などが選択肢になります(処理は少し重くなる)。
- `helper.py update` は既存ファイルを**上書き**します。自分のプロジェクトでは使わないでください。

---

## 次のステップ(候補)

| 順番 | テーマ |
|---|---|
| 10 | ポット(ADC)で周波数や音量を変える |
| 11 | MIDI 入力で音程を弾く |
| 12 | エンベロープ(ADSR)とフィルター |
| 13 | 自分専用の Git リポジトリへの切り出し |

---

## 参考資料

| 資料 | URL |
|---|---|
| DaisyExamples(README に clone・ビルド・helper.py の説明) | https://github.com/electro-smith/DaisyExamples |
| Daisy 公式の C++ 開発環境ガイド | https://daisy.audio/tutorials/cpp-dev-env/ |
| Electro-Smith フォーラム: How to set up development tools on Windows? | https://forum.electro-smith.com/t/how-to-set-up-development-tools-on-windows/443 |
| フォーラム: Get Started Faster!(新しいツールチェーンの入れ方) | https://forum.electro-smith.com/t/get-started-faster-a-newer-simpler-way-to-install-the-toolchain/1523 |
| フォーラム: "build_all" (and others) not showing up in VS Code | https://forum.electro-smith.com/t/build-all-and-others-not-showing-up-in-vs-code/3667 |
| フォーラム: Failure to launch(Windows の make 失敗例) | https://forum.electro-smith.com/t/failure-to-launch/3188 |
| フォーラム: Make program-dfu error | https://forum.electro-smith.com/t/make-program-dfu-error/1063 |
| フォーラム: Is 4 the best buffer size for most effect based projects? | https://forum.electro-smith.com/t/is-4-the-best-buffer-size-for-most-effect-based-projects/8948 |
