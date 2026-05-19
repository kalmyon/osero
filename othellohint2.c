/*******************************************************************

	オセロ プログラム に追加する関数など　　　　Othellohint2.c 
	
	松江工業高等専門学校　情報工学科　准教授　橋本　剛
	ゲームプログラミング　授業用

*******************************************************************/

#define PASSMOVE 99 //パス手には99を入れる 
#define MOVENUM 32


// 合法手かどうか判定する関数 
// 合法手ならTRUE、違ったらFALSEを返す
int isLegalMove(Move pos)
{
	int dirx, diry, dir;
	int pos1;
	// 自分の色、相手の色は何か変数に入れておく
	int color = TURNCOLOR( turn );
	int opponentcolor = TURNCOLOR( OPPONENT(turn));

	//空きマスでないかCheck
	if ( board[pos] != 0)
		return FALSE;
	
	// posの周り8方向を調べ相手石が取れるか調べる
	for ( dirx = -1; dirx <= 1; dirx++ )
	{
		for ( diry = -ASIDE; diry <= ASIDE; diry += ASIDE )
		{
			dir = dirx + diry;
			if ( dir == 0 )//方向０は意味ないのでパス
				continue;
			pos1 = pos + dir;//posの隣のマス
			//以下は board[pos1] の中身をチェックしながら、
			//pos1を +=dirしていく
			//だめなら continue

			//まず相手の石があるか調べる
			if( board[pos1] != opponentcolor)	
				continue;
			do // 相手の石がある間は次を調べる
			{
				pos1 += dir;//次の石
			} while (board[pos1] == opponentcolor);
			// ひっくり返すためには最後に自分の石がないといけない
			if(board[pos1]!= color )

			//最後まで来たら成功
			return TRUE;
		}
	}
	return FALSE;	
}

//手の生成 generation of moves
//生成した手の数を返している 
int generateMoves( Move moves[] )	
{
	int num = 0;//生成する合法手の数
	int pos;
	// 左上から順に石を置き、合法手か調べる
	for ( pos を使ってループ )
	{
		if (合法手なら)
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
	手番変更
	手を生成して数を見る
	手番戻す
	if ( 合法手 == 0 )
		return TRUE;
	return FALSE;
}

// 実際に手を進める
void makeMove( Move pos )
{

	// まずisLegalMoveの中身をコピー

	// 合法手であることを確認してから呼び出されるので、
	//	if ( board[pos] != 0 )//空きマスでないか
	//  の部分は必要ない。代わりにboard[pos]に石を置く必要あり。
	// return TRUE;　や return FALSE; も必要ない

	// 最後まで来たら成功!のところから、実際にひっくり返すコードを追加
	// ひっくり返す時に、ひっくり返した石の数を変数 int count に記憶させる

	// 最後に石の数をグローバル変数stonenum[]に足す（もしくは引く）
	stonenum[turn] += 足される数;
	stonenum[OPPONENT(turn)] -= 引かれる数;
	turn = OPPONENT(turn);
}


//人間の入力を管理する関数
Move manPlayer()
{
	//入力をさせ、合法手かチェック
	char line[256];
	int x, y, num, move ;

/*  //パスのチェック　関数generateMovesが完成したらコメントアウトを外す
	Move moves[MOVENUM];
	num = generateMoves( moves );
	// 合法手が無い場合
	if(num == 0)
	{
		// パスであることを表示する
		// 何か入力させる（Enter押すだけなど）
		return PASSMOVE;
	}
*/
	do
	{
		// input X coordinate
		do
		{
			printf("x(a-h):");
			fgets(line,256,stdin);
		}while(line[0] < 'a' || line[0] > 'h');
		x = line[0] - 'a' + 1;
		// input Y coordinate
		//自分で書こう

		// Check legal move
		move = getposition( x, y );
		if(合法手なら)
		{
			break;
		}
		printf("Illegal Move!\n");
	}while(1);
	return (Move)move;
}

//ランダムなプレイヤー
Move randPlayer()
{
	int num;
	Move moves[MOVENUM];
	num = generateMoves( moves );
	printf("RandPlayer\n");
	if ( num == 0 )
		//nextmove に パスを入れて return;
	else
		//nextmove に ランダムな手を入れて return;

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
	while(1)//一回の対局が終わるまでループ
	{
		num = generateMoves( moves );
		if ( num == 0 && isTerminalNode())//終局かチェック
		{
			//石の数で勝ち負け判定し表示
			result = 黒石 － 白石;//こんな感じで
			// result により表示を変える
			if ( result == 0 )
				printf ( "GAMEOVER!  DRAW!!\n");
			else 
				printf ( "GAMEOVER! %s WIN!!\n", ( result > 0 ? "BLACK": "WHITE"));
			printf ( "Press ENTER\n");
			fgets(line,256,stdin);
			return 0;
		}
		if ( turn == manturn )
			nextmove = manPlayer();
		else
			nextmove = randPlayer();
		makeMove( nextmove );
		ply++;
		printf( "ply = %d\n", ply );
		output();
		printf ( "%s -> %s%d\n\n", ( turn != BLACK_TURN? "BLACK": "WHITE"), abc[X(nextmove)-1], Y(nextmove));
	}
	return 0;
}