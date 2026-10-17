<p align="center">
  <img width="9984" height="2496" alt="rogo" src="https://github.com/user-attachments/assets/bb628493-eee7-4ffb-bbf2-9849a5d9ccef" />
</p>

<p align="center">
  <img src="https://img.shields.io/badge/status-prototype-orange" alt="status: prototype">
  <img src="https://img.shields.io/badge/hardware-open%20source-blue" alt="hardware">
  <img src="https://img.shields.io/badge/made%20in-Japan-red" alt="made in Japan">
</p>

<p align="center">
  <img width="1920" height="1080" alt="無題 2026-10-08 00-23-11" src="https://github.com/user-attachments/assets/7e952981-597b-4a5d-8f6a-17d4d8fed84d" />

</p>

---

## プロジェクトのステータス

> **このプロジェクトは現在「プロトタイプ段階」です。**
> 日本の高校生が開発しており、将来的な**製品化**を目指しています。
> 仕様・回路・ファームウェアは予告なく変更されることがあります。

| 項目 | 内容 |
|---|---|
| 現在のフェーズ | プロトタイプ(Rev.2.0) |
| 目標 | 製品化・販売 |
| 開発者 | 日本の高校生2人 |
| 最終更新 | 2026/10/18 |

---

## 📖 目次

- [概要](#-概要)
- [デモ](#-デモ)
- [特徴](#-特徴)
- [仕様](#-仕様)
- [システム構成](#-システム構成)
- [ハードウェア](#-ハードウェア)
- [ファームウェア](#-ファームウェア)
- [ビルドと組み立て](#-ビルドと組み立て)
- [使い方](#-使い方)
- [リポジトリ構成](#-リポジトリ構成)
- [ロードマップ](#-ロードマップ)
- [既知の問題](#-既知の問題)
- [コントリビュート](#-コントリビュート)
- [ライセンス](#-ライセンス)
- [開発メンバー・連絡先](#-開発メンバー連絡先)

---

## 🎯 概要

ポケットサイズのシンセサイザーはそのサイズ故に機能が制限され、シンセサイザーの持つ表現の幅や**触った時の面白さ**が低下してしまうのが課題でした。
そこで私は演奏体験をより良いものにしようとハプティクスの概念をシンセサイザーに持ち込むことにしました。
これにより操作しているだけで楽しく、直感的で自然に操作になじむことができ、またユーザーに新たな体験をもたらしてくれる楽器を製作しようと考えたのです。
**それがChordionです。**

【開発のきっかけ、既存の製品にない点、目指す体験を3〜5行で書く】

<!-- 開発の動機を書くと、読んだ人に伝わりやすくなります -->

---

## 🎬 デモ

<p align="center">
  <img src="docs/images/demo.gif" alt="動作デモ" width="640">
</p>

- 動画(YouTube等):【URL】
- SNS(開発の様子):【URL】

---

## ✨ 特徴

- **【特徴1】**:【説明】
- **【特徴2】**:【説明】
- **【特徴3】**:【説明】

<p align="center">
  <img src="docs/images/feature1.jpg" alt="特徴1" width="30%">
  <img src="docs/images/feature2.jpg" alt="特徴2" width="30%">
  <img src="docs/images/feature3.jpg" alt="特徴3" width="30%">
</p>

---

## 📐 仕様

| 項目 | 内容 |
|---|---|
| マイコン/SoC | 【例:〇〇】 |
| 動作電圧 | 【例:5V (USB) / バッテリー〇V】 |
| 消費電流 | 【typ. 〇mA / max 〇mA】 |
| 入力 | 【ボタン・鍵盤・ノブ・センサー等】 |
| 出力 | 【オーディオ/MIDI/USB など】 |
| 通信 | 【USB-MIDI / UART / I2C / SPI / BLE など】 |
| 寸法 | 【W × D × H mm】 |
| 重量 | 【g】 |
| 電源 | 【USB給電 / 電池 / ACアダプタ】 |

---

## 🧩 システム構成

### ブロック図

<p align="center">
  <img src="docs/images/block-diagram.png" alt="システムブロック図" width="720">
</p>

【各ブロックの役割を簡潔に説明】

### ピンアサイン

| ピン | 信号名 | 接続先 | 備考 |
|---|---|---|---|
| 【例:D1】 | 【例:MIDI_TX】 | 【例:MIDIコネクタ】 | |
| | | | |

---

## 🔧 ハードウェア

### 回路図

<p align="center">
  <img src="docs/images/schematic.png" alt="回路図" width="720">
</p>

- 回路図データ(KiCad / Eagle 等):[`hardware/schematic/`](hardware/schematic/)
- PDF:[`docs/schematic.pdf`](docs/schematic.pdf)

### 基板(PCB)

<p align="center">
  <img src="docs/images/pcb-front.png" alt="基板 表面" width="45%">
  <img src="docs/images/pcb-back.png" alt="基板 裏面" width="45%">
</p>

| 項目 | 内容 |
|---|---|
| 層数 | 【2層 / 4層】 |
| 基板サイズ | 【mm】 |
| 板厚 | 【1.6mm】 |
| 製造 | 【JLCPCB / PCBWay など】 |
| ガーバーデータ | [`hardware/gerber/`](hardware/gerber/) |

### 部品表(BOM)

| 部品番号 | 部品名 | 型番 | 数量 | 購入先 |
|---|---|---|---|---|
| U1 | 【マイコン】 | 【型番】 | 1 | 【秋月/Digi-Key/LCSC など】 |
| C1 | 【コンデンサ】 | 【0.1µF】 | 【数】 | |
| | | | | |

- 全部品リスト(CSV):[`hardware/bom.csv`](hardware/bom.csv)
- 概算コスト(1台あたり):【約〇〇円】

### 筐体・機構

<p align="center">
  <img src="docs/images/enclosure.png" alt="筐体CAD" width="45%">
  <img src="docs/images/enclosure-photo.jpg" alt="筐体の写真" width="45%">
</p>

- CAD データ(STEP / STL / Fusion360 等):[`hardware/enclosure/`](hardware/enclosure/)
- 製作方法:【3Dプリント / レーザーカット / 市販ケース加工】
- 3Dプリント設定:【材質 / 積層ピッチ / インフィル】

---

## 💾 ファームウェア

| 項目 | 内容 |
|---|---|
| 開発環境 | 【Arduino IDE / PlatformIO / Pico SDK など】 |
| 言語 | 【C++ / MicroPython など】 |
| 依存ライブラリ | 【ライブラリ名とバージョン】 |
| ソース | [`firmware/`](firmware/) |

### 書き込み手順

```bash
# 例:PlatformIO の場合
git clone https://github.com/【ユーザー名】/chordion.git
cd chordion/firmware
pio run -t upload
```

【書き込みモードへの入り方(ボタン長押しなど)があれば記載】

---

## 🛠 ビルドと組み立て

### 必要な道具

- はんだごて、はんだ、フラックス
- 【ピンセット、テスターなど】

### 手順

1. 基板を発注する(ガーバーデータは `hardware/gerber/`)
2. 部品を調達する(BOM を参照)
3. 表面実装部品 → スルーホール部品の順にはんだ付けする
4. 導通・電源電圧を確認する(**初回通電は電流制限付き電源を推奨**)
5. ファームウェアを書き込む
6. 筐体に組み込む

<p align="center">
  <img src="docs/images/assembly-1.jpg" alt="組み立て手順1" width="30%">
  <img src="docs/images/assembly-2.jpg" alt="組み立て手順2" width="30%">
  <img src="docs/images/assembly-3.jpg" alt="組み立て手順3" width="30%">
</p>

---

## 🎹 使い方

1. 【電源の入れ方】
2. 【基本操作】
3. 【接続方法(PC・スマホ・他機器)】

| 操作 | 動作 |
|---|---|
| 【ボタンA 短押し】 | 【動作】 |
| 【ボタンA 長押し】 | 【動作】 |

---

## 📁 リポジトリ構成

```
chordion/
├── README.md
├── LICENSE
├── docs/
│   └── images/          # README用の画像
├── hardware/
│   ├── schematic/       # 回路図
│   ├── pcb/             # 基板データ
│   ├── gerber/          # ガーバーデータ
│   ├── enclosure/       # 筐体CAD
│   └── bom.csv          # 部品表
└── firmware/            # ファームウェア
```

---

## 🗺 ロードマップ

- [x] 試作機(プロトタイプ)の製作
- [ ] 【機能追加・不具合修正】
- [ ] 基板の改版(Rev.B)
- [ ] ノイズ・安全性・耐久性の検証
- [ ] 量産に向けた設計の見直し
- [ ] 製品版の販売

---

## ⚠️ 既知の問題

| 内容 | 状態 | 対処・回避策 |
|---|---|---|
| 【例:〇〇でノイズが乗る】 | 調査中 | 【回避策】 |
| | | |

---

## 🤝 コントリビュート

プロトタイプ段階のため、フィードバックを歓迎します。

- バグ・要望:[Issues](../../issues)
- 改善提案:[Pull Request](../../pulls)
- 質問・感想:【SNS / Discussions】

---

## 📄 ライセンス

| 対象 | ライセンス |
|---|---|
| ハードウェア(回路・基板・筐体) | 【例:CERN-OHL-P / CERN-OHL-S など】 |
| ファームウェア | 【例:MIT / GPL-3.0 など】 |
| ドキュメント・画像 | 【例:CC BY 4.0】 |

> 製品化を予定しているため、商用利用の扱いは【方針を記載】。

---

## 👥 開発メンバー・連絡先

日本の高校生によるプロジェクトです。

- 開発者:【名前/ハンドルネーム】
- SNS:【URL】
- 連絡先:【メール or フォームのURL】

---

## 🙏 謝辞

【参考にしたプロジェクト、使用したライブラリ、協力してくれた人など】
