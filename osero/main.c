#include <stdio.h>
#include <stdlib.h>

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
//表示に使う文字定数
const char* piece[3] = { "  ", "黒", "白" };
const char* abc[8] = { "a","b","c","d","e","f","g","h" };
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
	for (pos = getposition(1, 1); pos <= getposition(SIDE, SIDE); pos++)
	{
		if (isLegalMove(pos))
			moves[num++] = pos;//num番目の配列にposを入れる
	}
	return num;
}

// ゲーム終了局面ならTRUEを返す Return TRUE in case of GAME OVER
// 生成した手が０の時呼ばれる
int isTerminalNode()
{
	int num;
	Move moves[MOVENUM];
	turn = OPPONENT(turn);//手番変更
	num = generateMoves(moves);//手を生成して数を見る
	turn = OPPONENT(turn);//手番戻す
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
// 実際に手を進める
void makeMove(Move pos)
{

	// まずisLegalMoveの中身をコピー
	// 合法手であることを確認してから呼び出されるので、
	//	if ( board[pos] != 0 )//空きマスでないか
	//  の部分は必要ない。代わりにboard[pos]に石を置く必要あり。
	// return TRUE;　や return FALSE; も必要ない
	int dirx, diry, dir;
	int pos1;

	int count = 0;
	// 自分の色、相手の色は何か変数に入れておく
	int color = TURNCOLOR(turn);
	int opponentcolor = TURNCOLOR(OPPONENT(turn));
	
	board[pos] = color; //石を置く
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
			// 最後まで来たら成功!のところから、実際にひっくり返すコードを追加
			// ひっくり返す時に、ひっくり返した石の数を変数 int count に記憶させる
			for (int i = pos+dir ; i != pos1; i += dir)
			{

				board[i] = color;
				count++;
			}
		}
	}
	

	
	// 最後に石の数をグローバル変数stonenum[]に足す（もしくは引く）
	stonenum[turn] += count;
	stonenum[OPPONENT(turn)] -= count;
	turn = OPPONENT(turn);
}


//人間の入力を管理する関数
Move manPlayer()
{
	//入力をさせ、合法手かチェック
	char line[256];
	int x, y, num, move;

	  //パスのチェック　関数generateMovesが完成したらコメントアウトを外す
	Move moves[MOVENUM];
	num = generateMoves( moves );
	// 合法手が無い場合
	if(num == 0)
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
		y = line[0] - '1'+1;
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
	//まずは変数宣言、初期化など
	int result;
	int num;
	Move moves[MOVENUM];
	int manturn = BLACK_TURN;
	char line[256];
	init();
	output();//局面の表示
	while (1)//一回の対局が終わるまでループ
	{
		num = generateMoves(moves);
		if (num == 0 && isTerminalNode())//終局かチェック
		{
			//石の数で勝ち負け判定し表示
			result = stonenum[BLACK_TURN] - stonenum[WHITE_TURN];//こんな感じで
			// result により表示を変える
			if (result == 0)
				printf("GAMEOVER!  DRAW!!\n");
			else
				printf("GAMEOVER! %s WIN!!\n", (result > 0 ? "BLACK" : "WHITE"));
			printf("Press ENTER\n");
			fgets(line, 256, stdin);
			return 0;
		}
		if (turn == manturn)
			nextmove = manPlayer();
		else
			nextmove = randPlayer();
		if (nextmove != PASSMOVE)
			makeMove(nextmove);
		else
			turn = OPPONENT(turn);
		ply++;
		printf("ply = %d\n", ply);
		output();
		if (nextmove != PASSMOVE) {
			printf("%s -> %s%d\n\n",
				(turn != BLACK_TURN ? "BLACK" : "WHITE"),
				abc[X(nextmove) - 1], Y(nextmove));
		}
		else {
			printf("PASS\n\n");
		}
	}
	return 0;
}