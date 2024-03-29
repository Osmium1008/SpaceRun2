﻿#include <Siv3D.hpp> // OpenSiv3D v0.3.1
#include <bits/stdc++.h>
using namespace std;

struct Rocket {
	double x, y, degree;
};


struct Brock {
	double x, y, degree;
	double speed, size;
	int color_red, color_green, color_blue;
	int near;
};

//ファイル入出力
ifstream fin;
ofstream fout;

void Main() {
	srand((unsigned)(time(NULL)));
	Window::Resize(1200, 600);
	Window::SetTitle(U"Space Run2");
	Window::Centering();

	//シチュエーション管理
	int now_situation = 0;
	int next_situation = 1;

	//難易度管理
	int difficulty = 1;

	//フォント
	Font font_120_italy(120, Typeface::Regular, FontStyle::Italic);
	Font font_75_italy(75, Typeface::Regular, FontStyle::Italic);
	Font font_35_italy(35, Typeface::Regular, FontStyle::Italic);
	Font font_25_italy(25, Typeface::Regular, FontStyle::Italic);
	Font font_15_italy(15, Typeface::Regular, FontStyle::Italic);
	//テクスチャ
	Texture rocket_texture(U"rocket.png", TextureDesc::Mipped);
	Rocket rocket;
	rocket.x = 600., rocket.y = 450, rocket.degree = 0.;
	Texture twitter_rink(U"twitter.png");
	//弾
	vector<Brock>brock;
	//タイム管理
	Stopwatch gaming_time;
	int game_stop = 0;
	//FPS上限
	Graphics::SetTargetFrameRateHz(60);
	//弾の色管理
	int red_brock = -1;
	//スコア
	int game_hp = 100, near_score = 0;
	//順位表
	vector<int>scores[10];
	bool get_scores = false;
	bool write_scores = false;

	while (System::Update()) {

		//start画面
		if (now_situation == 0) {
			//順位表を取得

			if (!get_scores) {
				fin.open("Scores.txt", ios::in);
				while (!fin.eof()) {
					string sc;
					getline(fin, sc);
					if (sc == "")break;
					int newsc = stoi(sc);
					scores[newsc / 1000000].push_back(newsc % 1000000);
				}
				fin.close();
				get_scores = true;
				for (int i = 0; i < 10; i++) {
					sort(scores[i].begin(), scores[i].end(), greater<>());
				}
			}

			//表示
			font_120_italy(U"Space Run2").drawAt(600, 120, Palette::Deepskyblue);
			Rect(180, 270, 360, 60).shearedX(120).draw(Palette::Turquoise);
			Rect(180, 360, 360, 60).shearedX(120).draw(Palette::Turquoise);
			Rect(180, 450, 360, 60).shearedX(120).draw(Palette::Turquoise);
			font_35_italy(U"チュートリアル").drawAt(360, 300);
			font_35_italy(U"プレイ").drawAt(360, 390);
			font_35_italy(U"作者").drawAt(360, 480);
			font_25_italy(U"...はじめてプレイする方向け").draw(750, 270);
			font_25_italy(U"...ゲームをプレイ").draw(750, 360);
			font_25_italy(U"...このゲームを作った人").draw(750, 450);
			font_25_italy(U"↑↓キーで選択、Enterキーで決定").drawAt(600, 570);
			if (KeyDown.down())next_situation = min(3, next_situation + 1);
			if (KeyUp.down())next_situation = max(1, next_situation - 1);
			Rect(180, 180 + next_situation * 90, 360, 60).shearedX(120).drawFrame(7, Palette::Yellow);
			if (KeyEnter.down()) {
				now_situation = next_situation;
				next_situation = 1;
			}
		}
		else if (now_situation == 1) {
			//文字の描画
			font_75_italy(U"チュートリアル").drawAt(600, 120, Palette::Deepskyblue);
			font_25_italy(U"降ってくるカラフルな隕石を避けましょう！").drawAt(600, 225);
			font_25_italy(U"隕石に近ければ近いほどスコアが高くなります").drawAt(600, 265);
			font_25_italy(U"操作方法：マウスを動かしてロケットを動かす").drawAt(600, 300);
			font_25_italy(U"ロケットはカーソルのある方向に飛んでいきます。").drawAt(600, 340);
			font_25_italy(U"練習してみましょう！").drawAt(600, 375);
			font_25_italy(U"Enterキーでホーム画面に戻る").drawAt(600, 570);
			//ロケットの描画
			rocket_texture.scaled(0.5*0.75).rotatedAt(rocket_texture.width() / 4 * 0.75, rocket_texture.height() / 4 * 0.75, ToRadians(rocket.degree)).drawAt(rocket.x, rocket.y);

			//ロケットの移動

			if (abs(rocket.x - Cursor::Pos().x) <= 15 && abs(rocket.y - Cursor::Pos().y) <= 15)goto loop;
			rocket.x += cos(ToRadians(90 - rocket.degree)) * System::DeltaTime() * 600 * 0.75;
			rocket.x = max(rocket.x, 100.);
			rocket.x = min(rocket.x, 1100.);
			rocket.y -= sin(ToRadians(90 - rocket.degree)) * System::DeltaTime() * 600 * 0.75;
			rocket.y = max(rocket.y, 100.);
			rocket.y = min(rocket.y, 500.);
		loop:;
			rocket.degree = 90 - ToDegrees(atan2(rocket.y - Cursor::Pos().y, Cursor::Pos().x - rocket.x));
			if (KeyEnter.down()) {
				now_situation = 0;
				rocket.x = 600., rocket.y = 450, rocket.degree = 0.;
			}
		}
		else if (now_situation == 2) {
			//文字・選択バーの描画
			font_75_italy(U"難易度選択").drawAt(800 * 0.75, 160 * 0.75, Palette::Deepskyblue);
			Rect(240 * 0.75, 320 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 400 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 480 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 560 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			Rect(240 * 0.75, 640 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).draw(Palette::Turquoise);
			font_25_italy(U"レベル１").drawAt(480 * 0.75, 340 * 0.75);
			font_25_italy(U"レベル２").drawAt(480 * 0.75, 420 * 0.75);
			font_25_italy(U"レベル３").drawAt(480 * 0.75, 500 * 0.75);
			font_25_italy(U"レベル４").drawAt(480 * 0.75, 580 * 0.75);
			font_25_italy(U"レベル５").drawAt(480 * 0.75, 660 * 0.75);
			if (scores[1].size())font_25_italy(U"HighScore: ", scores[1][0]).draw(1000 * 0.75, 320 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 320 * 0.75);
			if (scores[2].size())font_25_italy(U"HighScore: ", scores[2][0]).draw(1000 * 0.75, 400 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 400 * 0.75);
			if (scores[3].size())font_25_italy(U"HighScore: ", scores[3][0]).draw(1000 * 0.75, 480 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 480 * 0.75);
			if (scores[4].size())font_25_italy(U"HighScore: ", scores[4][0]).draw(1000 * 0.75, 560 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 560 * 0.75);
			if (scores[5].size())font_25_italy(U"HighScore: ", scores[5][0]).draw(1000 * 0.75, 640 * 0.75);
			else font_25_italy(U"HighScore: -").draw(1000 * 0.75, 640 * 0.75);
			font_25_italy(U"数字キーを使って選ぶこともできます。").drawAt(800 * 0.75, 760 * 0.75);

			//選択
			if (KeyDown.down())difficulty = min(5, difficulty + 1);
			if (KeyUp.down())difficulty = max(1, difficulty - 1);
			Rect(240 * 0.75, 240 * 0.75 + difficulty * 80 * 0.75, 480 * 0.75, 40 * 0.75).shearedX(120).drawFrame(10 * 0.75, Palette::Yellow);
			if (KeyEnter.down()) {
				now_situation = 4;
			}
			//コマンド
			if ((Key1 | KeyNum1).down()) {
				difficulty = 1;
			}
			if ((Key2 | KeyNum2).down()) {
				difficulty = 2;
			}
			if ((Key3 | KeyNum3).down()) {
				difficulty = 3;
			}
			if ((Key4 | KeyNum4).down()) {
				difficulty = 4;
			}
			if ((Key5 | KeyNum5).down()) {
				difficulty = 5;
			}
		}
		else if (now_situation == 3) {
			font_75_italy(U"作者").drawAt(600, 120, Palette::Deepskyblue);
			font_35_italy(U"原案・デザイン・プログラム：define").drawAt(600, 300);
			Rect twitter_click(800 * 0.75 - twitter_rink.width() / 8 * 0.75, 450 * 0.75, twitter_rink.width() / 4 * 0.75, twitter_rink.height() / 4 * 0.75);
			twitter_rink.scaled(0.25*0.75).draw(800 * 0.75 - twitter_rink.width() / 8 * 0.75, 450 * 0.75);
			if (twitter_click.leftClicked()) {
				System::LaunchBrowser(U"https://twitter.com/define_AC");
			}
			font_25_italy(U"Enterキーでホーム画面に戻る").drawAt(800 * 0.75, 750 * 0.75);
			if (KeyEnter.down()) {
				now_situation = 0;
			}
		}
		else if (now_situation == 4) {
			//デバッグ用コマンド
			if (KeyE.down())now_situation = 5;

			Triangle rocket_upper(rocket.x, rocket.y - 54 * 0.75, rocket.x + 12 * 0.75, rocket.y + 16 * 0.75, rocket.x - 12 * 0.75, rocket.y + 16 * 0.75);
			Rect rocket_lower(rocket.x - 22 * 0.75, rocket.y + 16 * 0.75, 44 * 0.75, 39 * 0.75);
			//ロケットの描画
			rocket_texture.scaled(0.5*0.75).rotatedAt(rocket_texture.width() / 4 * 0.75, rocket_texture.height() / 4 * 0.75, ToRadians(rocket.degree)).drawAt(rocket.x, rocket.y);


			if (game_stop % 2 == 0) {
				if (MouseL.down()) {
					gaming_time.start();
					game_stop++;
					continue;
				}
				for (auto &p : brock) {
					Circle brock_p(p.x, p.y, p.size);
					if (gaming_time.s() > red_brock)brock_p.draw(Color(p.color_red + 100, p.color_green + 100, p.color_blue + 100));
					else brock_p.draw(Palette::Red);
				}
				if (game_stop)font_75_italy(U"クリックして再開").drawAt(800 * 0.75, 400 * 0.75);
				else {
					font_75_italy(U"クリックしてスタート").drawAt(800 * 0.75, 400 * 0.75);
					font_25_italy(U"途中でクリックすると一時停止します").drawAt(800 * 0.75, 500 * 0.75);
				}
				//スコアの表示
				font_25_italy(U"タイム").draw(100 * 0.75, 150 * 0.75);
				font_25_italy(gaming_time.s()).draw(100 * 0.75, 200 * 0.75);
				font_25_italy(U"HP").draw(100 * 0.75, 250 * 0.75);
				font_25_italy(game_hp).draw(100 * 0.75, 300 * 0.75);
				font_25_italy(U"スコア").draw(100 * 0.75, 350 * 0.75);
				font_25_italy(near_score).draw(100 * 0.75, 400 * 0.75);
				font_25_italy(U"難易度").draw(100 * 0.75, 450 * 0.75);
				font_25_italy(difficulty).draw(100 * 0.75, 500 * 0.75);
				//HPバー
				Rect(10 * 0.75, 740 * 0.75, 1590 * 0.75, 50 * 0.75).draw(Palette::Black);
				Rect(10 * 0.75, 740 * 0.75, 1590 * game_hp / 100 * 0.75, 50 * 0.75).draw(Palette::Red);
				Rect(10 * 0.75, 740 * 0.75, 1590 * 0.75, 50 * 0.75).drawFrame(10 * 0.75, 0);
				continue;
			}
			else if (MouseL.down()) {
				game_stop++;
				gaming_time.pause();
				continue;
			}
			//ロケットの移動

			if (abs(rocket.x - Cursor::Pos().x) <= 50 && abs(rocket.y - Cursor::Pos().y) <= 50)goto loop3;
			rocket.x += cos(ToRadians(90 - rocket.degree)) * System::DeltaTime() * 60 * 4 * max(3, difficulty)* 0.75;
			rocket.x = max(rocket.x, 100.);
			rocket.x = min(rocket.x, 1100.);
			rocket.y -= sin(ToRadians(90 - rocket.degree)) * System::DeltaTime() * 60 * 4 * max(3, difficulty)* 0.75;
			rocket.y = max(rocket.y, 100.);
			rocket.y = min(rocket.y, 500.);
		loop3:;
			rocket.degree = 90 - ToDegrees(atan2(rocket.y - Cursor::Pos().y, Cursor::Pos().x - rocket.x));

			//ブロックの追加
			if (System::FrameCount() % (120 / (difficulty + 1)) == 0) {
				double start_x = rand() % 2000 - 400, end_x = rand() % 1200;
				double degree = 90 - ToDegrees(atan2(500, end_x - start_x));
				brock.push_back({ start_x,0.,degree,gaming_time.s() / 20 * difficulty* 0.75 + 2 * (difficulty + 1)* 0.75,gaming_time.s() / 10 * difficulty*0.75 + 5,Random(0,125),Random(0,125),Random(0,125),10000 });
			}
			else if (System::FrameCount() % (120 / (difficulty + 1)) == 120 / (difficulty + 1) / 2) {
				double start_x = rand() % 2000 - 400, end_x = rocket.x + Random(-300, 300);
				double degree = 90 - ToDegrees(atan2(rocket.y, end_x - start_x));
				brock.push_back({ start_x,0.,degree,gaming_time.s() / 20 * difficulty* 0.75 + 2 * (difficulty + 1)* 0.75,gaming_time.s() / 10 * difficulty * 0.75 + 5,Random(0,125),Random(0,125),Random(0,125),10000 });
			}
			//ブロックの当たり判定
			for (auto &p : brock) {
				Circle brock_p(p.x, p.y, p.size);
				//ロケットの当たり判定
				if (rocket_upper.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)).intersects(brock_p) || rocket_lower.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)).intersects(brock_p)) {
					game_hp -= 5;
					red_brock = gaming_time.s() + 1;
					p = brock.back();
					brock.pop_back();
					continue;
				}
				else if (gaming_time.s() > red_brock) {
					p.near = min(p.near, (int)(Geometry2D::Distance(rocket_upper.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)), brock_p)));
					p.near = min(p.near, (int)(Geometry2D::Distance(rocket_lower.rotatedAt(rocket.x, rocket.y, ToRadians(rocket.degree)), brock_p)));
				}
				//範囲判定
				if (p.y > 600.) {
					if (p.near) {
						near_score += 75 / max(1, p.near - 75);
					}
					p = brock.back();
					brock.pop_back();
					continue;
				}
				if (gaming_time.s() > red_brock)brock_p.draw(Color(p.color_red + 100, p.color_green + 100, p.color_blue + 100));
				else brock_p.draw(Palette::Red);
				//ブロックの移動
				p.x += cos(ToRadians(90 - p.degree)) * System::DeltaTime()*p.speed * 60;
				p.y += sin(ToRadians(90 - p.degree)) * System::DeltaTime()*p.speed * 60;
			}
			//HPの回復
			if (System::FrameCount() % 300 == 0)game_hp = min(game_hp + 1, 100);

			//スコアの表示
			font_25_italy(U"タイム").draw(100 * 0.75, 150 * 0.75);
			font_25_italy(gaming_time.s()).draw(100 * 0.75, 200 * 0.75);
			font_25_italy(U"HP").draw(100 * 0.75, 250 * 0.75);
			font_25_italy(game_hp).draw(100 * 0.75, 300 * 0.75);
			font_25_italy(U"スコア").draw(100 * 0.75, 350 * 0.75);
			font_25_italy(near_score).draw(100 * 0.75, 400 * 0.75);
			font_25_italy(U"難易度").draw(100 * 0.75, 450 * 0.75);
			font_25_italy(difficulty).draw(100 * 0.75, 500 * 0.75);
			//HPバー
			Rect(10 * 0.75, 740 * 0.75, 1590 * 0.75, 50 * 0.75).draw(Palette::Black);
			Rect(10 * 0.75, 740 * 0.75, 1590 * game_hp / 100 * 0.75, 50 * 0.75).draw(Palette::Red);
			Rect(10 * 0.75, 740 * 0.75, 1590 * 0.75, 50 * 0.75).drawFrame(10 * 0.75, 0);
			//ゲーム終了
			if (game_hp <= 0 || gaming_time.s() >= 60) {
				now_situation = 5;
			}
		}
		else if (now_situation == 5) {
			gaming_time.pause();
			//スコア表示
			font_75_italy(U"結果").drawAt(800 * 0.75, 160 * 0.75, Palette::Deepskyblue);
			font_35_italy(U"   難易度：", difficulty, U"    タイム：", gaming_time.s(), U"秒", U"    スコア：", near_score).drawAt(800 * 0.75, 300 * 0.75);

			//スコア書き込み
			if (!write_scores) {
				scores[difficulty].push_back(near_score);
				sort(scores[difficulty].begin(), scores[difficulty].end(), greater<>());
				fout.open("Scores.txt", ios::app);
				fout << 1000000 * difficulty + near_score << endl;
				fout.close();
				write_scores = true;
			}

			//順位表
			for (int i = 0; i < min((int)scores[difficulty].size(), 5); i++) {
				font_35_italy(i + 1, U"位 ", scores[difficulty][i]).draw(700 * 0.75, 350 * 0.75 + 65 * i* 0.75, Palette::Yellow);
			}

			font_25_italy(U"Enterキーでホーム画面に戻る").drawAt(800 * 0.75, 750 * 0.75);
			if (KeyEnter.down()) {
				//初期化
				now_situation = 0;
				next_situation = 1;
				near_score = 0;
				game_hp = 100;
				difficulty = 1;
				brock.clear();
				red_brock = -1;
				rocket.x = 600., rocket.y = 450, rocket.degree = 0.;
				game_stop = 0;
				gaming_time.reset();
				get_scores = false;
				write_scores = false;
				for (int i = 0; i < 10; i++)scores[i].clear();
			}
		}
	}
}