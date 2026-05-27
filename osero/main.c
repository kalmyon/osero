#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#define SIDE 8 // 一辺の長さ 
#define ASIDE (SIDE + 2) // 局面用配列の一辺 緩衝地帯分２を足す Side for array, add 2
#define BOARDSIZE (ASIDE * ASIDE) // ボードの大きさ　Size of board
#define UP (-ASIDE)
#define DOWN ASIDE
#define RIGHT 1
#define LEFT (-1)
// Piece number
#define B 1 // BLACK 1
#define W 2 // WHITE 2
#define N 3 // 番兵 Sentinel (out of board) 3
#define BLACK_TURN 0 
#define WHITE_TURN 1


//座標からX座標またはY座標を計算するマクロ
#define X(pos) (pos % ASIDE)
#define Y(pos) (pos / ASIDE)
#define TURNCOLOR( turn ) (turn + 1)
#define OPPONENT(turn) !turn
#define PASSMOVE 99 //パス手には99を入れる 
#define MOVENUM 32

#define TRUE 1
#define FALSE 0

///////////////////////////AI用に追加　ここから
#define MAXDEPTH 5 //探索を行う最大深さ
#define INFINITY 1000 //十分大きい数を無限大として扱う
#define SEARCH_LIMIT_DEPTH 128 //探索深さの上限
///////////////////////////AI用に追加　ここまで
//表示に使う文字定数
const char* piece[3] = { "  ", "黒", "〇" };


const char* abc[9] = { "a","b","c","d","e","f","g","h" ,"" };
int turn; // 手番
int ply; //手数 

typedef unsigned char Move;//手だとわかりやすくするため型を宣言（しなくてもよい）
Move nextmove;//次の手 
unsigned char stonenum[2];//石の数を記憶する変数

//2次元の座標を一次元に変換
int getposition(int x, int y) { return y * ASIDE + x; };

//一番大事な変数、ここでは一次元で表現
unsigned char board[BOARDSIZE] = //intでも良いが、データ格納のことを考えてcharにしている
{//とりあえずはこんな感じで初期化
	N, N, N, N, N, N, N, N, N, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, B, W, 0, 0, 0, N,
	N, 0, 0, 0, W, B, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, 0, 0, 0, 0, 0, 0, 0, 0, N,
	N, N, N, N, N, N, N, N, N, N
};
///////////////////////////AI用に追加　ここから
struct Position //過去の局面などを記憶するための構造体
{
	unsigned char board[BOARDSIZE];
	unsigned char stonenum[2];
};
struct Position history[SEARCH_LIMIT_DEPTH];
//場所による評価を与える配列
//角が良いことを教えることが一番の目的
//本当はパターンによる評価が出来ればよいが、
//この程度でもそれっぽくすることは可能
int evalboard[BOARDSIZE] =
{
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  99, -99,  20,   4,   4,  20, -99,  99,   0,
   0, -99, -99,  -5,  -5,  -5,  -5, -99, -99,   0,
   0,  20,  -5,  15,   3,   3,  15,  -5,  20,   0,
   0,	4,  -5,   3,   3,   3,   3,  -5,   4,   0,
   0,	4,  -5,   3,   3,   3,   3,  -5,   4,   0,
   0,  20,  -5,  15,   3,   3,  15,  -5,  20,   0,
   0, -99, -99,  -5,  -5,  -5,  -5, -99, -99,   0,
   0,  99, -99,  20,   4,   4,  20, -99,  99,   0,
   0,	0,   0,   0,   0,   0,   0,   0,   0,   0
};


///////////////////////////AI用に追加　ここまで


// 合法手かどうか判定する関数 
// 合法手ならTRUE、違ったらFALSEを返す
int isLegalMove(Move pos)
{
	int dirx, diry, dir;
	int pos1;
	// 自分の色、相手の色は何か変数に入れておく
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	//空きマスでないかCheck
	if (board[pos] != 0)
		return FALSE;

	// posの周り8方向を調べ相手石が取れるか調べる
	for (dirx = -1; dirx <= 1; dirx++)
	{

		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0)//方向０は意味ないのでパス
				continue;
			pos1 = pos + dir;//posの隣のマス
			//以下は board[pos1] の中身をチェックしながら、
			//pos1を +=dirしていく
			//だめなら continue

			//まず相手の石があるか調べる
			if (board[pos1] != opponentcolor)
				continue;
			do // 相手の石がある間は次を調べる
			{
				pos1 += dir;//次の石
			} while (board[pos1] == opponentcolor);
			// ひっくり返すためには最後に自分の石がないといけない
			if (board[pos1] != color)
				continue;
			//最後まで来たら成功
			return TRUE;
		}
	}
	return FALSE;
}


//手の生成 generation of moves
//生成した手の数を返している 
int generateMoves(Move moves[])
{
	int num = 0;//生成する合法手の数
	int pos;
	// 左上から順に石を置き、合法手か調べる
	for (pos = 11; pos <= 88; pos++)
	{
		if (isLegalMove(pos))
			moves[num++] = pos;//num番目の配列にposを入れる
	}
	return num;
}

// ゲーム終了局面ならTRUEを返す Return TRUE in case of GAME OVER
// 生成した手が０の時呼ばれる

int isTerminalNode(int isPass)
{
	int num;
	Move moves[MOVENUM];
	if (!isPass)
	{
		num = generateMoves(moves);
		if (num > 0)
			return FALSE;

	}
	turn = OPPONENT(turn);
	num = generateMoves(moves);
	turn = OPPONENT(turn);
	if (num == 0)
		return TRUE;
	return FALSE;
}



// 表示関数 display function
void output()
{//ヒントを参考に、とにかく表示する関数を作りましょう
	int x, y = 0;
	printf("  a  b  c  d  e  f  g  h \n");
	for (y = 1; y <= SIDE; y++)
	{

		printf("%d|", y);
		for (x = 1; x <= SIDE; x++)
		{
			printf("%s|", piece[board[getposition(x, y)]]);//ちょっと簡潔に書くためのヒント

		}
		printf("\n");
	}

}

///////////////////////////AI用に追加　ここから

// ゲーム終了局面の評価値を返す 勝ち ∞ 引き分け 0 負け －∞
int getTerminalValue()
{
	//石数の差を計算　自分の石ー相手の石
	int diff = stonenum[turn] - stonenum[OPPONENT(turn)];

	if (diff > 0)
		return INFINITY;
	else if (diff < 0)
		return -INFINITY;
	else
		return 0;
}

// 評価関数の計算　Calculation of evaluation function
int getEvaluationValue()
{
	int pos, value, c;
	Move moves[MOVENUM];
	// 合法手数の差を評価関数とする(自由度)
	value = generateMoves(moves);//自分の合法手数を足す
	turn = OPPONENT(turn);
	value -= generateMoves(moves);//相手の合法手数を引く
	turn = OPPONENT(turn);//手番を戻す

	value *= 30;//自由度１を30点としておく（適当）
	for (pos = 11; pos <= 88; pos++)
	{
		c = board[pos];
		if (c == 0)
			continue;
		else if (c == TURNCOLOR(turn))
			value += evalboard[pos];
		else
			value -= evalboard[pos];
	}
	return value;
}
///////////////////////////AI用に追加　ここまで


// 実際に手を進める
void makeMove(Move pos, int depth)
{
	int pos1, dir, dirx, diry, count = 0;
	int isLegal = TRUE;
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));

	///////////////////////////AI用に追加　ここから
	// 局面を配列historyに保存
	memcpy(history[depth].board, board, sizeof(board));// 配列の中身をコピーするのはmemcpy()を使うと簡単
	memcpy(history[depth].stonenum, stonenum, sizeof(stonenum));
	///////////////////////////AI用に追加　ここまで

	if (pos == PASSMOVE)//パス手の場合
	{
		turn = OPPONENT(turn);
		return;
	}
	board[pos] = color;

	// posの周り8方向を調べ相手石をひっくり返す
	for (dirx = -1; dirx <= 1; dirx++)
	{
		for (diry = -ASIDE; diry <= ASIDE; diry += ASIDE)
		{
			dir = dirx + diry;
			if (dir == 0)
				continue;
			pos1 = pos + dir;//posの隣のマス
			//以下は board[pos1] の中身をチェックしながら、
			//pos1を +=dirしていく
			//だめなら continue
			if (board[pos1] != opponentcolor)//相手の石があるか
				continue;
			do // 相手の石がある間は次を調べる
			{
				pos1 += dir;
			} while (board[pos1] == opponentcolor);
			// ひっくり返すためには最後に自分の石がないといけない
			if (board[pos1] != color)
				continue;

			//最後まで来たら成功!実際にひっくり返す
			pos1 = pos + dir;
			do // 相手の石がある間ひっくり返す　
			{
				board[pos1] = color;
				pos1 += dir;
				count++;
			} while (board[pos1] == opponentcolor);
		}
	}
	stonenum[turn] += count + 1;
	stonenum[OPPONENT(turn)] -= count;
	turn = OPPONENT(turn);
}


///////////////////////////AI用に追加　ここから

// 手を戻す
void unmakeMove(int depth)
{
	// 配列の中身をコピーするのはmemcpy()を使うと簡単
	// 逆の事をmakeMove()でしないといけない
	// 局面を配列historyから復元
	memcpy(board, history[depth].board, sizeof(board));
	memcpy(stonenum, history[depth].stonenum, sizeof(stonenum));
	turn = OPPONENT(turn);
}

// 探索して最も評価の高い手を選ぶ
int search(int depth)
{
	int i;
	int movenum;//手の数
	Move moves[MOVENUM];//手を入れる配列 an array of moves
	int value;
	int bestvalue = -INFINITY - 1;//まず最小値を入れる

	//手を生成
	movenum = generateMoves(moves);
	if (movenum == 0)
	{
		if (isTerminalNode(TRUE))// Game Over
			return getTerminalValue();
		else // パス
			moves[movenum++] = PASSMOVE;
	}
	for (i = 0; i < movenum; i++)
	{
		makeMove(moves[i], depth);//一手進め
		value = getEvaluationValue();//評価関数の値を計算
		output();//for Debug
		printf("i = %d, value = %d, move = %d\n", i, value, moves[i]);//for Debug
		unmakeMove(depth);//一手戻る
		//最大かどうか調べ、最大ならnextmoveに代入する（スライド参照）
		if (value > bestvalue)
		{
			bestvalue = value;
			nextmove = moves[i];
		}



	}
	return bestvalue;
}

//COMの手を生成する関数
void comPlayer()
{
	int value;
	printf("Com Thinking...\n");
	value = search(0);
	printf("value = %d\n", value);
	if (value == INFINITY)
		printf("Computer Finds Win!!\n");
}

///////////////////////////AI用に追加　ここまで

//人間の入力を管理する関数
Move manPlayer()
{
	//入力をさせ、合法手かチェック
	char line[256];
	int x, y, num, move;

	//パスのチェック　関数generateMovesが完成したらコメントアウトを外す
	Move moves[MOVENUM];
	num = generateMoves(moves);
	// 合法手が無い場合
	if (num == 0)
	{
		printf("あなたはおける場所がありません\nパスします(Please Enter)\n");// パスであることを表示する
		// 何か入力させる（Enter押すだけなど）
		while (1)
		{
			fgets(line, 256, stdin);
			if (line[0] == '\n')
				break;
		}
		return PASSMOVE;
	}

	do
	{
		// input X coordinate
		do
		{
			printf("x(a-h):");
			fgets(line, 256, stdin);
		} while (line[0] < 'a' || line[0] > 'h');
		x = line[0] - 'a' + 1;
		// input Y coordinate
		do
		{
			printf("y(1-8):");
			fgets(line, 256, stdin);
		} while (line[0] < '1' || line[0] > '8');
		y = line[0] - '1' + 1;
		// Check legal move
		move = getposition(x, y);
		if (isLegalMove(move))
		{
			break;
		}
		printf("Illegal Move!\n");
	} while (1);
	return (Move)move;
}

//ランダムなプレイヤー
Move randPlayer()
{
	int num;
	Move moves[MOVENUM];
	num = generateMoves(moves);
	printf("RandPlayer\n");
	if (num == 0)
		return PASSMOVE;

	else
		return moves[rand() % num];

}


//グローバル変数などを初期化
void init()
{
	turn = BLACK_TURN;
	ply = 0;
	stonenum[BLACK_TURN] = 2;
	stonenum[WHITE_TURN] = 2;
}
int main()
{
	srand((unsigned int)time(NULL));//乱数の初期化
	//まずは変数宣言、初期化など
	int result;
	char line[256];
	int manturn = BLACK_TURN;
	init();
	output();//局面の表示
	while (1)//一回の対局が終わるまでループ
	{
		if (isTerminalNode(FALSE))//終局かチェック
		{
			//石の数で勝ち負け判定し表示
			result = stonenum[BLACK_TURN] - stonenum[WHITE_TURN];//こんな感じで
			// result により表示を変える
			if (result == 0)
				printf("GAMEOVER!  DRAW!!\n");
			else
				printf("GAMEOVER! %s WIN!!\n", (result > 0 ? "BLACK" : "WHITE"));
			return 0;
		}
		if (turn == manturn)
			nextmove = manPlayer();
		//randplayer();
		else
			//nextmove = randPlayer();
			comPlayer();
		makeMove(nextmove, 0);
		ply++;
		printf("ply = %d\n", ply);
		output();
		printf("%s -> %s%d\n\n", (turn != BLACK_TURN ? "BLACK" : "WHITE"), abc[X(nextmove) - 1], Y(nextmove));
	}
	printf("Press any key and Enter\n");
	fgets(line, 256, stdin);
	return 0;
}