//==============================================================
// Simple IME Ver1.00   (STPP.04)
//     for PSP CFW3.80 M33-5
// STEAR 2009
//--------------------------------------------------------------
// PSP用の簡易IMEを作成してみた。
// ソフトウェアキーボードと熟語漢字変換を実装。
//--------------------------------------------------------------

#include <pspuser.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "zenkaku.h"
#include "draw.h"
#include "sime.h"

//----- マクロ -----


//----- 定数設定 -----

#define SIMENAME	"Simple IME Ver1.00"

#define DICFILE1	"SIME.DIC"					// 辞書ファイル名
#define DICFILE2	"ms0:/PSP/COMMON/SIME.DIC"
#define INIFILE1	"SIME.INI"					// 環境設定ファイル
#define INIFILE2	"ms0:/PSP/COMMON/SIME.INI"

#define KBPOSX1		268							// ソフトウェアキーの位置X（５０音配列アシストカーソル）
#define KBPOSY1		157							// ソフトウェアキーの位置Y（５０音配列アシストカーソル）
#define KBPOSX3		303							// ソフトウェアキーの位置X（×５かな配置）
#define KBPOSY3		105							// ソフトウェアキーの位置Y（×５かな配置）
#define MEPOSX		131							// 設定画面の位置
#define MEPOSY		53							// 設定画面の位置
#define KEYREP1		20							// キーリピート開始までの時間
#define KEYREP2		3							// キーリピートの間隔
#define FONTX		6							// フォントの長さ
#define FONTY		12							// フォントの高さ
#define CHLEN		32							// 変換ラインの最大文字数
#define KLIST		6							// 漢字候補リストの行数
#define CBTIME		25							// カーソル点滅周期
#define CURSTEP		7							// アシストカーソルの移動所要時間
#define YOMILMAX	24							// この数以下に「よみ」候補数がなったらリストを表示させる

#define CORFL1		0xC0A0A0					// 枠
#define CORFL2		0xF0D0D0					// 明るい枠
#define CORFL3		0x707070					// 暗い枠
#define CORIN		0x600060					// ウィンドウ内背景
#define CORFR		0xA08080					// ウィンドウ内前景
#define CORCUR		0xFF8080					// カーソル
#define CORCUR1		0x7070FF					// カーソル○
#define CORCUR2		0x60D060					// カーソル△
#define CORCUR3		0xA0A0E0					// カーソル□
#define CORCUR4		0xFF8080					// カーソル×
#define CORRBAR		0x70E070					// スクロールバー
#define CORWCHR		0xFFFFFF					// ウィンドウ内文字
#define CORSCHR		0xB0FFB0					// ウィンドウ内特殊文字
#define CORFCHR		0x000000					// ウィンドウ枠文字
#define CORCHBK		0xFFFFFF					// 変換ライン背景
#define CORCHCR		0x000000					// 変換ライン文字
#define CORCHCU		0xFF8080					// 変換ライン選択領域

//----- プロトタイプ宣言 -----

static unsigned int	fcode(char *str);
static unsigned int	getInt(char *str);
static void kList(char wordList[][33],int count,int index);
static void getkList(char wordList[][33],int count);
static void putkList(void);

//------ グローバル変数 -----

static char			gKeyName1[7][25] = {
						"１：全角 ひらがな",
						"２：全角 カタカナ",
						"３：全角 アルファベット",
						"４：全角 記号",
						"５：半角 アルファベット",
						"６：半角 記号",
						"キーガイダンス"
					};
static char			gKeyTable1[6][161] = {
						"０あいうえお１かきくけこ２さしすせそ３たちつてと４なにぬねの５はひふへほ６まみむめも７やゆよ゛゜８らりるれろ９わをん、。！ぁぃぅぇぉ？ゃゅょゎっ”ー「」・　",
						"０アイウエオ１カキクケコ２サシスセソ\３タチツテト４ナニヌネノ５ハヒフヘホ６マミムメモ７ヤユヨ゛゜８ラリルレロ９ワヲン、。！ァィゥェォ？ャュョヮッ”ー「」・　",
						"０ＡＢＣＤＥ１ＦＧＨＩＪ２ＫＬＭＮＯ３ＰＱＲＳＴ４ＵＶＷＸＹ５Ｚａｂｃｄ６ｅｆｇｈｉ７ｊｋｌｍｎ８ｏｐｑｒｓ９ｔｕｖｗｘ！ｙｚ－，．？［（＜｛”’］）＞｝　",
						"０！”＃（％１？’＆）＄２↑↓←→＝３＊＋－±×４／＜｛，；５＼＞｝．：６［＿＠￥＾７］｜～・…８℃※〒♪☆９△□○×◎▽◇ゝゞ∀Λξ∩∪⊂⊃φωД彡∑⊿　",
						"0 A B C D E 1 F G H I J 2 K L M N O 3 P Q R S T 4 U V W X Y 5 Z a b c d 6 e f g h i 7 j k l m n 8 o p q r s 9 t u v w x ! y z - , . ? [ ( < { : / ] ) > }   ",
						"7 4 1 0 + * 8 5 2 , - / 9 6 3 . = #             ( < [ { : ! ) > ] } ; ? \" $ % & @ \\ ' ^ _ | ~                                                               "
					};

static char			gKeyTable3[6][161] = {
						"あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもらりるれろやゆよわをゃゅょゎんぁぃぅぇぉっ゛゜、。ー「！」？０１２３４５６７８９・　　　　",
						"アイウエオカキクケコサシスセソ\タチツテトナニヌネノハヒフヘホマミムメモラリルレロヤユヨワヲャュョヮンァィゥェォッ゛゜、。ー「！」？０１２３４５６７８９・ヱヵヶ　",	// 「ソ\」注意！
						"０１２３４５６７８９ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ’，－！（．）？＜［”］＞：｛／｝　",
						"０１２３４５６７８９”＃＄％＆，（＝）．＊＋±－×：／＠＼；！＜＿＞？＾［￥］’｜｛～｝・…℃※〒♪↑←☆→↓△□◎○×▽◇ゝゞΛ∩⊂∀⊃∪ξφΦωД彡∑⊿л　",
						"0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z ' , - ! ( . ) ? < [ \" ] > : { / }   ",
						"0 1 2 3 4 5 6 7 8 9 ! \" # $ % & ( ' ) * + , - . / : < = > ; ? [ \\ ] @ ^ { _ } | ' ~                                                                             "
					};
static long			gHEDtbl[85],gDicWordPos,gDicSize;
static long			gBackBuf[(194+24)*(166) +4],							// ソフトキー背景待避バッファ
					gInLnBuf[480*FONTY +4],									// 変換ライン背景待避バッファ
					gkListBuf[(CHLEN*FONTX+6+4+2) * ((FONTY+1)*KLIST + 4)];	// 漢字候補リスト背景待避バッファ
static char			*gDic,gDicFile[33],gIniFile[33],gSt[CHLEN +1];
static int			gMode,gBDFlag,gSaveFlag,gSave,gIni,gKey;
static int			*gCx,*gCy,gCxw;


//==============================================================
// 辞書ファイルの読み込み
//--------------------------------------------------------------
// 戻り値：0 辞書の読み込み成功
//         1 不正な辞書ファイル
//         2 メモリが足りない
//         3 辞書ファイルが開けない（存在しない？）
//--------------------------------------------------------------

static int DicLoad(char *DicFile)
{
	int		i,fa,fd;
	long	pos,filesize;

	fa = 0;
	fd = sceIoOpen( DicFile, PSP_O_RDONLY, 0777 );
	if (fd>=0){
		filesize = sceIoLseek(fd, 0, SEEK_END);
		sceIoLseek(fd, 0, SEEK_SET);
		pos = sceIoRead( fd,gHEDtbl, sizeof(gHEDtbl) );
		if (pos!=sizeof(gHEDtbl)){
			fa = 1;								// ジャンプテーブルが足りない
		} else {
			for (i=0; i<85 ;i++){
				if (gHEDtbl[i]!=0xFFFFFFFF && (gHEDtbl[i]>filesize-pos))
					fa = 1;						// ジャンプ先がデータ外を示している
			}
			if (!fa){
				gDic = (char*) malloc( filesize - pos );
				if (gDic==NULL){
					fa = 2;						// メモリを確保できない
				} else {
					gDicSize = filesize - pos;
					sceIoRead( fd,gDic, gDicSize );
				}
			}
		}
		sceIoClose(fd);
	} else {
		fa = 3;
	}

	if (fa)										// 辞書ファイルが開けなかった
		for (i=0; i<85 ;i++)
			gHEDtbl[i] = 0xFFFFFFFF;

	return (fa);
}


//==============================================================
// 辞書ファイルの読み込みと各種初期化
//--------------------------------------------------------------
// flag    0 ウィンドウ消去時に背景を復元する
//      以外 ウィンドウ消去時に何もしない（メイン側で作画するべし）
// 戻り値：0 辞書の読み込み成功
//         1 不正な辞書ファイル
//         2 メモリが足りない
//         3 辞書ファイルが開けない（存在しない？）
//--------------------------------------------------------------
// 辞書ファイルとIME環境設定を読み込みます。
// 環境設定ファイルは辞書ファイルと同じフォルダに配置されます。
//--------------------------------------------------------------

int InitSIME(int flag)
{
	char	str[256],*p;
	int		ret,type,val;
	FILE*	fp1;

	gMode = 0;
	gCxw = 2;									// カーソルの大きさ
	gDicWordPos = -1;
	gDicSize = 0;
	gSaveFlag = 0;
	gSave = 0;									// 終了時に辞書の記載順を保存する
	gIni = 0;
	gKey = 0;									// ソフトキーの種類
	if (flag){
		gBDFlag = 1;							// ウィンドウ消去時に何もしない
	} else {
		gBDFlag = 0;							// ウィンドウ消去時に背景を復元する
	}

	ret = DicLoad( DICFILE1 );
	strcpy( gDicFile ,DICFILE1 );
	strcpy( gIniFile ,INIFILE1 );
	if (ret){
		ret = DicLoad( DICFILE2 );				// COMMONフォルダ
		strcpy( gDicFile ,DICFILE2 );
		strcpy( gIniFile ,INIFILE2 );
		if (ret){
			gDicFile[0] = '\0';
			strcpy( gIniFile ,INIFILE1 );
		}
	}

	fp1 = fopen( gIniFile, "r" );				// 環境設定ファイルの読み込み
	if (fp1!=NULL){
		while (1){
			if (fgets( str,256,fp1 )==NULL) break;
			p = strtok( str," =\t" );
			type = 0;
			if (strstr( p,"KEYTYPE" )!=NULL) type = 1;
			if (strstr( p,"SAVEMODE" )!=NULL) type = 2;
			p = strtok( NULL," =\t" );
			val = (int)strtol( p,NULL,0 );		// 文字列→数値変換
			if (val==0){						// 数字ではないっぽいので
				if (strstr( p,"YES" )!=NULL) val = 1;
				if (strstr( p,"NO" )!=NULL) val = 2;
				if (type==2 && val>0) gSave = val -1;
			} else {
				if (type==1 && val>0 && val<=2) gKey = val -1;
			}
		}
		fclose (fp1);
	}

	return (ret);
}


//==============================================================
// 辞書ファイルへ書き込み
//--------------------------------------------------------------
// 漢字候補の並べ替え結果を辞書に書き込む
//--------------------------------------------------------------

static void DicSave(void)
{
	int	fd;

	if (!gSave && gSaveFlag && gDicSize){
		fd = sceIoOpen( gDicFile, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777 );
		if (fd>=0){
			sceIoWrite(fd, gHEDtbl, sizeof(gHEDtbl));
			sceIoWrite(fd, gDic, sizeof(gDic));
			sceIoClose(fd);
		}
	}
}


//==============================================================
// 終了処理
//--------------------------------------------------------------
// メモリに保持していた辞書をファイルに書き戻した後、開放する。
// これを実行せずに電源を切る等をすると漢字候補の並べ替えが次回に反映されません。
// また、IME環境データを設定ファイルに保存します。
// 設定ファイルの位置は辞書ファイルと同じフォルダになりますが、
// 辞書が見つからなかった場合は EBOOT.PBP と同じフォルダになります。
//--------------------------------------------------------------

void EndSIME(void)
{
	FILE*	fp1;
	char	str[256],item[2][4] = {"YES","NO"},s[3] = {"1\n"};

	if (gIni){
		fp1 = fopen( gIniFile, "w" );
		if (fp1!=NULL){
			strcpy( str,"KEYTYPE = " );
			s[0] = '1' + gKey;
			strcat( str,s );
			fputs( str,fp1 );
			strcpy( str,"SAVEMODE = " );
			strcat( str,item[gSave] );
			strcat( str,"\n" );
			fputs( str,fp1 );
			fclose(fp1);
		}
	}
	DicSave();
	free(gDic);
}


//==============================================================
// 文字カーソル指定
//--------------------------------------------------------------
// xw  カーソルのX方向の幅（0以下：カーソル表示なし）
// x,y カーソル位置へのポインタ
//--------------------------------------------------------------
// カーソル座標を指定する変数とカーソル形状を指定する。
// カーソル表示はSgets()を実行している間に更新されます。
// カーソルはI型で、Y方向の大きさは指定できません。
//--------------------------------------------------------------

void SIMEcursor(int xw,int *x,int *y)
{
	gCx = x;
	gCy = y;
	gCxw = xw;
	if (gCxw<0) gCxw = 0;
	if (gCxw>12) gCxw =12;
}


//==============================================================
// 文字カーソルの作画
//--------------------------------------------------------------

static void DrawCursorSub()
{
	int i,j,x,cx,cy;

	if (gKey){
		cx = KBPOSX3;
		cy = KBPOSY3;
	} else {
		cx = KBPOSX1;
		cy = KBPOSY1;
	}

	x = *gCx;
	if (*gCy+FONTY>cy && gMode==1)				// ソフトキーと重なるなら開始位置をずらす
		if (x+gCxw>cx) x = cx - gCxw;

	for (j=0; j<gCxw ;j++)
		for (i=0; i<12 ;i++)
			VRAM[*gCy+i][x+j] ^= 0xFFFFFF;
}


//==============================================================
// 文字カーソルを表示
//--------------------------------------------------------------
// cr    0：カーソルを点滅させる
//    以外：カーソルを消去/表示しない
//--------------------------------------------------------------

static void DrawCursor(int ch)
{
	static int bk = 0,count = 0;

	if (ch){
		if (bk && !gBDFlag) DrawCursorSub();	// キー入力があったらカーソルを消す
		bk = 0;
		count = CBTIME;							// 次回は直ぐ表示させる
	} else {
		count++;
		if (count>CBTIME){						// カーソル点滅周期
			count = 0;
			bk ^= 1;
		}
		if (gBDFlag){
			if (bk) DrawCursorSub();
		} else {
			if (!count) DrawCursorSub();
		}
	}
}

//==============================================================
// キー入力
//--------------------------------------------------------------
// 戻り値 0:入力なし
//        →:0x1C ←:0x1D ↑:0x1E ↓:0x1F
//        ○:0x0D（[Enter]） ×:0x1B（[Esc]） □:0x08（[BS]） △:0x02 L:0x03 R:0x04
//        [START]:0x05 [SELECT]:0x06
//--------------------------------------------------------------
// 各ボタンの状態をキーボード風なキーリピートありで取得する。
// 同時押しの場合は最後に押し下げたボタンに対応するコードを返す。
//--------------------------------------------------------------

int SIMEgetchar(SceCtrlData pad1)
{
	static SceCtrlData	pad2;
	static int			rep = 0,kcode = 0,button = 0;

	if ((pad1.Buttons & PSP_CTRL_UP) && !(pad2.Buttons & PSP_CTRL_UP)){			// 押し下げた瞬間を検出
		rep = 0;																// リピートカウンタリセット
		kcode = 0x1E;															// 出力コード
		button = PSP_CTRL_UP;													// リピートを監視するボタン
	}
	if ((pad1.Buttons & PSP_CTRL_DOWN) && !(pad2.Buttons & PSP_CTRL_DOWN)){
		rep = 0;
		kcode = 0x1F;
		button = PSP_CTRL_DOWN;
	}
	if ((pad1.Buttons & PSP_CTRL_LEFT) && !(pad2.Buttons & PSP_CTRL_LEFT)){
		rep = 0;
		kcode = 0x1D;
		button = PSP_CTRL_LEFT;
	}
	if ((pad1.Buttons & PSP_CTRL_RIGHT) && !(pad2.Buttons & PSP_CTRL_RIGHT)){
		rep = 0;
		kcode = 0x1C;
		button = PSP_CTRL_RIGHT;
	}
	if ((pad1.Buttons & PSP_CTRL_TRIANGLE) && !(pad2.Buttons & PSP_CTRL_TRIANGLE)){
		rep = 0;
		kcode = 0x02;
		button = PSP_CTRL_TRIANGLE;
	}
	if ((pad1.Buttons & PSP_CTRL_CROSS) && !(pad2.Buttons & PSP_CTRL_CROSS)){
		rep = 0;
		kcode = 0x1B;
		button = PSP_CTRL_CROSS;
	}
	if ((pad1.Buttons & PSP_CTRL_SQUARE) && !(pad2.Buttons & PSP_CTRL_SQUARE)){
		rep = 0;
		kcode = 0x08;
		button = PSP_CTRL_SQUARE;
	}
	if ((pad1.Buttons & PSP_CTRL_CIRCLE) && !(pad2.Buttons & PSP_CTRL_CIRCLE)){
		rep = 0;
		kcode = 0x0D;
		button = PSP_CTRL_CIRCLE;
	}
	if ((pad1.Buttons & PSP_CTRL_LTRIGGER) && !(pad2.Buttons & PSP_CTRL_LTRIGGER)){
		rep = 0;
		kcode = 0x03;
		button = PSP_CTRL_LTRIGGER;
	}
	if ((pad1.Buttons & PSP_CTRL_RTRIGGER) && !(pad2.Buttons & PSP_CTRL_RTRIGGER)){
		rep = 0;
		kcode = 0x04;
		button = PSP_CTRL_RTRIGGER;
	}
	if ((pad1.Buttons & PSP_CTRL_START) && !(pad2.Buttons & PSP_CTRL_START)){
		rep = 0;
		kcode = 0x05;
		button = PSP_CTRL_START;
	}
	if ((pad1.Buttons & PSP_CTRL_SELECT) && !(pad2.Buttons & PSP_CTRL_SELECT)){
		rep = 0;
		kcode = 0x06;
		button = PSP_CTRL_SELECT;
	}

	pad2 = pad1;

	if (pad1.Buttons & button){
		rep++;
		if (rep>KEYREP1) rep -= KEYREP2;					// キーリピート
		if (rep==1 || rep==KEYREP1) return (kcode);			// 押していた
	} else {
		rep = 0;
		kcode = 0;
		button = 0;
	}

	return (0);												// 何も押していない
}


//==============================================================
// テキスト枠の表示
//--------------------------------------------------------------
// 凹んだフレームにテキストを表示します。
//--------------------------------------------------------------

static void DrawTextbox(char *str,int x,int y,int wx,long txcor,long bkcor)
{
	BoxFill( x, y, wx, FONTY+2, CORFL2, bkcor );
	HLine( x, y, wx-1, CORFL3 );				// 上
	VLine( x, y, FONTY+2-1, CORFL3 );			// 左
	mh_print( x+2, y+1, str, txcor );			// 文字
}


//==============================================================
// ウィンドウ枠の表示
//--------------------------------------------------------------

static void DrawWindow(char *title,int x,int y,int wx,int wy)
{
	BoxFill( x, y, wx-1, FONTY+3, CORFL1, CORFL1 );
	HLine( x +2       , y +2 , wx-4   , CORFL3 );		// 上
	VLine( x +2       , y +2 , FONTY+2, CORFL3 );		// 左
	HLine( x +2       , y +14, wx-4   , CORFL2 );		// 下
	VLine( x +2 +wx -4, y +2 , FONTY+2, CORFL2 );		// 右
	mh_print( x +5, y +2, title, CORFCHR );				// タイトル
	HLine( x       , y    , wx, CORFL2 );				// 上
	VLine( x       , y    , wy, CORFL2 );				// 左
	HLine( x       , y +wy, wx, CORFL3 );				// 下
	VLine( x +wx -1, y    , wy, CORFL3 );				// 右
}


//==============================================================
// ウィンドウ背景の待避
//--------------------------------------------------------------

static void getBack(void)
{
	switch (gKey){
	case 0:
		gBackBuf[0] = KBPOSX1;
		gBackBuf[1] = KBPOSY1;
		BoxCopy( &gBackBuf[2],KBPOSX1,KBPOSY1,210,98 +15 );
		break;
	case 1:
		gBackBuf[0] = KBPOSX3;
		gBackBuf[1] = KBPOSY3;
		BoxCopy( &gBackBuf[2],KBPOSX3,KBPOSY3,175,166 );
		break;
	}
}


//==============================================================
// ウィンドウ背景の復元
//--------------------------------------------------------------

static void putBack(void)
{
	if (!gBDFlag){
		BoxPaste( &gBackBuf[2],gBackBuf[0],gBackBuf[1] );
	}
}


//==============================================================
// ソフトウェアキーボードの表示（５０音配列アシストカーソル）
//--------------------------------------------------------------
// x[],y[]   カーソルの到達目標位置
// xx[],yy[] カーソルの元位置
// step      カーソルの到達係数（0:目標位置）
//--------------------------------------------------------------
// xかyがNULLだった場合はカーソルは表示しません。
//--------------------------------------------------------------

static void DrawKey1(int *x,int *y,int t,int *xx,int *yy,int step)
{
	char	key[3] = "  ",km[4][3] = { "●","▲","■","×" };
	int		i,j,s,mx,my;
	long	cor[4] = { CORCUR1,CORCUR2,CORCUR3,CORCUR4 };

	DrawWindow( gKeyName1[t],KBPOSX1,KBPOSY1,210,97 +15 );						// ウィンドウフレーム
	for (i=0; i<4 ;i++)
		mh_print( KBPOSX1 +210-2-4*13 +i*13, KBPOSY1 +2, km[i], cor[i] );
	BoxFill( KBPOSX1 +1, KBPOSY1 +15, 210-2, 97, CORFL1, CORIN );				// 内側
	if (x!=NULL && y!=NULL){
		for (i=0; i<4 ;i++){													// カーソルの表示
			mx = (x[i] - xx[i]) *16 * step / CURSTEP;
			my = (y[i] - yy[i]) *16 * step / CURSTEP;
			BoxFill( KBPOSX1 +1 + x[i]*16 -mx, KBPOSY1 +15 + y[i]*16 -my, 16, 16, CORFL1, cor[i] );
		}
	}
	if (t>3){																	// 半角文字の位置補正
		s = 3;
	}else{
		s = 0;
	}
	for (i=0; i<6 ;i++)
		for (j=0; j<13 ;j++){
			key[0] = gKeyTable1[t][(j*6+i)*2];
			key[1] = gKeyTable1[t][(j*6+i)*2+1];					// キーマップから一文字取り出す
			mh_print( KBPOSX1 +1 +2 + j*16 +s, KBPOSY1 +15 +2 + i*16, key, CORWCHR );
		}
	mh_print( KBPOSX1 +1 +2 + 12*16 , KBPOSY1 +15 +2 + 5*16, "SP", CORSCHR );
}


//==============================================================
// ソフトウェアキーボードの表示（×５かな変換）
//--------------------------------------------------------------
// xかyがマイナスだった場合はカーソルは表示しません。
//--------------------------------------------------------------

static void DrawKey3(int x,int y,int t)
{
	char key[3] = "  ";
	int i,j,k,s;
	int map[5][2] = {{1,0} , {0,1} , {1,1} , {2,1} , {1,2} };

	DrawWindow( gKeyName1[t],KBPOSX3,KBPOSY3,175,164 );						// ウィンドウフレーム
	BoxFill(KBPOSX3 +1, KBPOSY3 +15, 175 -2, 149, CORFL1, CORIN);			// 内側
	for (i=0; i<4 ;i++){													// 罫線
		HLine(KBPOSX3 +1       , KBPOSY3 +15 + i*37, 149 +24, CORFL1);
		VLine(KBPOSX3 +1 + i*43, KBPOSY3 +15       , 149, CORFL1);
	}
	if (x>=0 && y>=0)
		BoxFill(KBPOSX3 +1 + x*43, KBPOSY3 +15 + y*37, 44, 38, CORFL1, CORCUR);

	if (t>3){																// 半角文字の位置補正
		s = 3;
	}else{
		s = 0;
	}
	mh_print(KBPOSX3 +1 +3*43 +2 +6, KBPOSY3 +15 +3*37+2*12+1, "space", CORSCHR);

	for (i=0; i<4 ;i++)															// ソフトウェアキーボード
		for (j=0; j<4 ;j++)
			for (k=0; k<5 ;k++){
				key[0] = gKeyTable3[t][(i*20+j*5+k)*2];
				key[1] = gKeyTable3[t][(i*20+j*5+k)*2+1];						// キーマップから一文字取り出す
				mh_print(KBPOSX3 +1 + j*43 + map[k][0]*13+3 +s, KBPOSY3 +15 + i*37 + map[k][1]*12+1, key, CORWCHR);
			}
}


//==============================================================
// 変換ラインの位置
//--------------------------------------------------------------
// gCx,gCyで指定されているカーソル位置に変換ラインを展開した場合の実際の座標とサイズを計算。
// 画面端やソフトキーとぶつかるなら開始位置を修正する。
//--------------------------------------------------------------

static void InLinePos(char *str,int *x,int *wx)
{
	int		cx,cy;

	if (gKey){
		cx = KBPOSX3;
		cy = KBPOSY3;
	} else {
		cx = KBPOSX1;
		cy = KBPOSY1;
	}

	*wx = strlen(str) * FONTX;					// 文字列のドット長
	*x = *gCx;
	if (*x+*wx>480) *x = 480 - *wx;				// 画面からはみ出すなら開始位置をずらす
	if (*gCy+FONTY>cy && gMode==1)				// ソフトキーと重なるなら開始位置をずらす
		if (*x+*wx>cx) *x = cx - *wx -gCxw;
}


//==============================================================
// 変換ラインの表示
//--------------------------------------------------------------
// blk 変換対象の文字数
//--------------------------------------------------------------
// gCx,gCyで指定されているカーソル位置に変換ライン文字列の表示を行う。
// ソフトキーに掛からないように位置補正されます。
// blkで指定された文字数分だけ先頭部分の背景色を変えます。
//--------------------------------------------------------------

static void InLine(char *str,int blk)
{
	int	x,wx,wx1,wx2;

	InLinePos( str,&x,&wx );									// 変換ライン位置
	if (wx){
		if (blk*FONTX >= wx){
			wx1 = wx;
			wx2 = 0;
		} else {
			wx1 = blk * FONTX;
			wx2 = wx - wx1;
		}
		if (wx1)
			BoxFill( x,*gCy,wx1,FONTY,CORCHCU,CORCHCU );		// 文字背景（変換中部位）
		if (wx2)
			BoxFill( x+wx1,*gCy,wx2,FONTY,CORCHBK,CORCHBK );	// 文字背景（未変換部位）
		mh_print( x,*gCy,str,CORCHCR );							// 変換ライン文字列
	}
}


//==============================================================
// 変換ラインの背景の待避
//--------------------------------------------------------------

static void getInLine(char *str)
{
	int	x,wx;

	InLinePos( str,&x,&wx );					// 変換ライン位置
	gInLnBuf[0] = x;
	gInLnBuf[1] = *gCy;
	BoxCopy( &gInLnBuf[2],x,*gCy,wx,FONTY );
}


//==============================================================
// 変換ラインの背景の復元
//--------------------------------------------------------------

static void putInLine(void)
{
	if (!gBDFlag){
		if (gInLnBuf[2]){						// X幅が0でないなら
			BoxPaste( &gInLnBuf[2],gInLnBuf[0],gInLnBuf[1] );
		}
	}
}


//==============================================================
// 移動モード時の処理
//--------------------------------------------------------------

static void move(char *str,SceCtrlData pad1)
{
	int		ch;

	ch = SIMEgetchar(pad1);
	DrawCursor(ch);								// カーソルの作画
	if (ch==0x02){								// △ （ソフトキーモードへ）
		ch = 0;
		gMode = 3;								// ソフトキーモード遷移準備
	} else {
		str[0] = ch;
		str[1] = 0;
	}
}


//==============================================================
// シフトJISの第１文字チェック
//--------------------------------------------------------------
// 戻り値  0:第１文字ではない
//        -1:第１文字である
//--------------------------------------------------------------

int chkSJIS(unsigned char cr)
{
	if (cr<0x80U || (cr>=0xA0U && cr<0xE0U) || cr>=0xFDU){
		return (0);
	} else {
		return (-1);
	}
}


//==============================================================
// 次の入力文字候補を調べる
//--------------------------------------------------------------
// 戻り値           文字候補の数
// str[4][3]        次に使用されている頻度が高い上位２個の文字と場合によっては「゛」
// wordList[50][33] 「よみ」候補リスト
// count            「よみ」候補リストの個数（最大50）
//--------------------------------------------------------------
// これまでに変換行に入力された文字を元に辞書を調べて入力中と思われる語句の候補
// を取得し文字入力のアシストを行う。
// ひらがなの入力中の場合にのみ動作します。
// 次のよみに使われる頻度の高い上位２個の「かな」を選択し、また直前に入力されて
// いる「かな」に濁音等が付加可能なら更に「゛」を選択します。
//--------------------------------------------------------------

static int nextChr(char str[][3],char wordList[][33],int *count)
{
	char	sTbl1[] = "かきくけこさしすせそたちつてとはひふへほ",
			sTbl2[] = "がぎぐげござじずぜぞだぢづでどばびぶべぼ",
			sTbl3[] = "ぱぴぷぺぽ";
	unsigned int	cr,p[2];
	int		i,hp,sp,sp2,tmp,max[2],len,crCnt,cc[85];
	long	pos;

	cr = 0;
	*count = 0;

	if (chkSJIS(gSt[0])){						// 先頭が全角文字なら
		cr = fcode(&gSt[0]);					// 先頭文字
		if (cr<0x829FU){						// 記号、数字、英語なら
			return (0);
		} else if (cr<0x82F2U){					// ひらがななら
			hp = cr - 0x829FU +1;
		} else if (cr==0x8394U){				// 「ヴ」
			return (0);
		} else {
			return (0);							// 漢字検索を行わない
		}
	} else {									// 先頭が半角なら
		return (0);
	}

	pos = gHEDtbl[hp];							// 頭文字に対応する辞書レコードの開始位置
	if (pos==0xFFFFFFFF) return (0);			// 辞書に登録なし

	pos += 2;
	sp2 = 0;
	len = strlen(gSt);
	for (i=0; i<85 ;i++)
		cc[i] = 0;
	while(1){
		sp = 0;
		while (gDic[pos+sp]!=0){				// よみに対応する単語を辞書より検索
			if (gDic[pos+sp]!=gSt[sp]) break;
			sp++;
			if (sp==len) break;
		}
		if (sp==len){
			if (*count<50){						// 「よみ」候補リストの取得
				strcpy( wordList[*count],&gDic[pos] );
				(*count)++;
			}
			cr = fcode(&gDic[pos+sp]);
			if (cr==0x815B){					// 「ー」
				cr = 84;
			} else {
				cr -= 0x829F;
			}
			if (cr<85) cc[cr]++;				// 次の文字候補の該当数をカウント
		}
		if (sp<sp2)								// 一致する文字数が前回より短くなったら探査終了
			break;
		sp2 = sp;
		tmp = getInt(&gDic[pos-2]);				// 次の単語候補への相対距離（int）
		if (tmp==0xFFFF) break;					// 辞書が終了したら探査終了
		pos += tmp;
	}

	max[0] = max[1] = 0;
	for (i=0; i<85 ;i++)						// 出現頻度の高い２個を探索
		if (cc[i]>=max[0]){
			max[1] = max[0];
			p[1] = p[0];
			max[0] = cc[i];
			p[0] = i;
		}
	crCnt = 0;
	for (i=0; i<2 ;i++)
		if (max[i]){
			if (p[i]<84){
				p[i] += 0x829F;
			} else {
				p[i] = 0x815B;					// 「ー」
			}
			str[i][0] = p[i] / 0x100;
			str[i][1] = p[i] & 0xFF;
			str[i][2] = '\0';
			if (strstr( sTbl2,str[i] )!=NULL)	// 「が」→「か」変換
				str[i][1]--;
			if (strstr( sTbl3,str[i] )!=NULL)	// 「ぱ」→「は」変換
				str[i][1] -= 2;
			crCnt++;
		}

	if (len>=2){
		len -= 2;								// 変換行最後の文字
		if (strstr( sTbl1,&gSt[len] )!=NULL){	// 前回濁音とかが付く文字が入力されているなら
			str[crCnt][0] = 0x81;
			str[crCnt][1] = 0x4A;				// 「゛」
			str[crCnt][2] = '\0';
			crCnt++;
		}
	}

	return (crCnt);
}


//==============================================================
// カーソルを指定文字位置に移動させる
//--------------------------------------------------------------

static void setCur(int *x,int *y,int bt,int *xx,int *yy,int reset,char wordList[][33],int *count)
{
	static int lock = 0;
	char	nchr[4][3];
	int		i,j,pos,flag,crCnt,zx,zy;
	int		dx[4] = {0,3,9,6},dy[4] = {0,3,4,2};

	for (i=0; i<4 ;i++){						// 移動元を保管
		xx[i] = x[i];
		yy[i] = y[i];
	}
	if (lock || reset){							// カーソルフォーメーションのリセット
		zx = x[bt];
		zy = y[bt];
		for (i=0; i<4 ;i++){
			x[bt] = zx + dx[i];
			if (x[bt]>12) x[bt] -= 13;
			y[bt] = zy + dy[i];
			if (y[bt]>5) y[bt] -= 6;
			bt++;
			if (bt>3) bt = 0;
		}
		lock = 0;
	}
	if (reset) return;

	crCnt = nextChr( nchr,wordList,count );		// 次の入力文字候補を調べる
	if (crCnt){									// 文字候補があるなら
		for (j=0; j<crCnt ;j++){
			pos = -1;
			for (i=0; i<78 ;i++)
				if (gKeyTable1[0][i*2]==nchr[j][0] && gKeyTable1[0][i*2+1]==nchr[j][1]){
					pos = i;					// 文字の位置を特定
					break;
				}
			if (pos!=-1){						// 文字位置を対応する座標に変換
				zx = pos / 6;
				zy = pos % 6;
				flag = 1;
				for (i=0; i<4 ;i++)				// 重複チェック
					if (x[i]==zx && y[i]==zy) flag = 0;
				if (flag){						// 重複していないなら
					x[bt] = zx;
					y[bt] = zy;
				}
				bt++;
				if (bt>3) bt = 0;
			}
		}
		lock = 1;
	}
}


//==============================================================
// 変換行に文字を追加する
//--------------------------------------------------------------
// ただ追加するだけではなく、濁音等の付加処理などもしています。
//--------------------------------------------------------------

static void addInline(char *str,char *si,int t)
{
	char	sdt[] = "かきくけこさしすせそたちつてとはひふへほカキクケコサシスセソ\タチツテトハヒフヘホ",	// 「゛」が付く文字
			sht[] = "はひふへほハヒフヘホ";																	// 「゜」が付く文字
	char	cr[3] = "  ";
	int		pos,len;

	if (!chkSJIS(si[0])) si[1] = '\0';
	if (gSt[0]!=0){								// ２文字目以降
		putInLine();							// 変換ライン背景を復元
		pos = strlen(gSt) - 2;
		cr[0] = gSt[pos];
		cr[1] = gSt[pos+1];
		if (strcmp( si,"゛" )==0){
			if ((unsigned char)cr[0]==0x83 &&
			    (unsigned char)cr[1]==0x45){	//「ウ」→「ヴ」変換
				gSt[pos+1] = 0x94;
				si[0] = 0;
				si[1] = 0;
			} else {
				if (strstr( sdt,cr )!=NULL){	// 「゛」の付く文字なら前の文字を修正
					gSt[pos+1]++;
					si[0] = 0;
					si[1] = 0;
				}
			}
		}else if (strcmp( si,"゜" )==0){
			if (strstr( sht,cr )!=NULL){		// 「゜」の付く文字なら前の文字を修正
				gSt[pos+1] += 2;
				si[0] = 0;
				si[1] = 0;
			}
		}
	} else {									// １文字目
		if (t>3){								// 半角の場合
			str[0] = si[0];						// そのまま出力
			str[1] = 0;
			si[0] = 0;
		}
	}

	if (si[1]){									// 全角文字を追加する場合
		len = CHLEN -2;
	} else {									// 半角文字を追加する場合
		len = CHLEN -1;
	}
	if (strlen(gSt)<=len){						// 変換ライン文字数制限
		strcat( gSt,si );						// 文字を追加
		getInLine(gSt);							// 変換ライン背景の取得
	}
}


//==============================================================
// ソフトキーモード時の処理（５０音配列アシストカーソル）
//--------------------------------------------------------------

static void softkey1(char *str,SceCtrlData pad1)
{
	static char	wordList[50][33];
	static int	getFlag = 0,t = 0,bt = 0,pc = 0,index = 0,count = 0,
				x[4] = {0,3,9,6},y[4] = {1,4,5,3},xx[4],yy[4];
	int		i,p,s,ch,fp,pos;
	char	si[3];

	fp = 0;												// 画面に変更があったか
	si[2] = 0;

	ch = SIMEgetchar(pad1);
	if (!strlen(gSt))
		if (ch==0 || ch>=32 || ch==0x02 || ch==0x08 || ch==0x0D || ch==0x1B)
			DrawCursor(ch);								// カーソル表示
	switch (ch){
	case 0x03:											// L
		break;
	case 0x04:											// R
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			t++;										// 文字種変更
			if (t>5) t = 0;
		} else {
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// カーソルフォーメーションをデフォルトに
			pc = CURSTEP;
		}
		break;
	case 0x05:											// [START]
		t++;											// 文字種変更
		if (t>5) t = 0;
		break;
	case 0x06:											// [SELECT]
		if (gSt[0]!=0){
			putInLine();								// 変換ラインを一旦消す
			if (count)
				putkList();
		}
		count = 0;
		gMode = 5;										// 設定画面へ
		break;
	case 0x02:											// △
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// カーソルフォーメーションをデフォルトに
			pc = CURSTEP;
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// 変換ラインを一旦消す
				gMode = 2;								// 漢字変換
			}
		} else {
			si[0] = gKeyTable1[t][(x[1]*6+y[1]) *2];
			si[1] = gKeyTable1[t][(x[1]*6+y[1]) *2+1];
			bt = 1;
			fp = 1;
		}
		break;
	case 0x08:											// □
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]!=0){
				putInLine();							// 変換ライン背景を復元
				if (count)
					putkList();
				pos = strlen(gSt);
				pos--;
				p = 0;
				while (1){								// 全角文字補正
					s = 1 -chkSJIS(gSt[p]);
					if (p+s>pos) break;
					p += s;
				}
				gSt[p] = 0;
				if (s==2) gSt[p+1] = 0;
				getInLine(gSt);							// 変換ライン背景を取得
				if (t==0){
					setCur( x,y,bt,xx,yy,0,wordList,&count );	// カーソルを次の文字候補位置へ
				} else {
					setCur( x,y,bt,xx,yy,1,wordList,&count );	// カーソルを次の文字候補位置へ
				}
				index = 0;
				getkList( wordList,count );
			} else {
				str[0] = ch;							// バックスペースを出力
				DrawCursor(ch);
			}
		} else {
			si[0] = gKeyTable1[t][(x[2]*6+y[2]) *2];
			si[1] = gKeyTable1[t][(x[2]*6+y[2]) *2+1];
			bt = 2;
			fp = 1;
		}
		break;
	case 0x0D:											// ○
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// カーソルフォーメーションをデフォルトに
			pc = CURSTEP;
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				str[0] = ch;							// [Enter]を出力
				DrawCursor(ch);
			} else {
				putInLine();							// 変換ライン背景を復元
				for (i=0; i<CHLEN ;i++){
					str[i] = gSt[i];
					gSt[i] = 0;
				}
			}
		} else {
			si[0] = gKeyTable1[t][(x[0]*6+y[0]) *2];
			si[1] = gKeyTable1[t][(x[0]*6+y[0]) *2+1];
			bt = 0;
			fp = 1;
		}
		break;
	case 0x1B:											// ×
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// カーソルフォーメーションをデフォルトに
			pc = CURSTEP;
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// 変換ライン背景を復元
				for (i=0; i<CHLEN ;i++){
					gSt[i] = 0;
				}
			}
		} else {
			si[0] = gKeyTable1[t][(x[3]*6+y[3]) *2];
			si[1] = gKeyTable1[t][(x[3]*6+y[3]) *2+1];
			bt = 3;
			fp = 1;
		}
		break;
	case 0x1C:											// →
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count && count<=YOMILMAX){
					putInLine();						// 変換ライン背景を復元
					putkList();
					strcpy( gSt,wordList[index] );
					getInLine(gSt);						// 変換ライン背景を取得
					count = 0;
				}
			}
		} else {
			for (i=0; i<4 ;i++){
				x[i]++;
				if (x[i]>12) x[i] = 0;
			}
		}
		break;
	case 0x1D:											// ←
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			}
		} else {
			for (i=0; i<4 ;i++){
				x[i]--;
				if (x[i]<0) x[i] = 12;
			}
		}
		break;
	case 0x1E:											// ↑
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count){
					index--;
					if (index<0) index = count -1;
				}
			}
		} else {
			for (i=0; i<4 ;i++){
				y[i]--;
				if (y[i]<0) y[i] = 5;
			}
		}
		break;
	case 0x1F:											// ↓
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count){
					index++;
					if (index>count-1) index = 0;
				}
			}
		} else {
			for (i=0; i<4 ;i++){
				y[i]++;
				if (y[i]>5) y[i] = 0;
			}
		}
		break;
	default:											// その他（文字の直接入力？）
		if (ch>=32){
			si[0] = ch;
			fp = 1;
		}
		break;
	}

	if (fp){											// 文字を変換行に追加
		if (count)
			putkList();
		addInline( str,si,t );
		setCur( x,y,bt,xx,yy,0,wordList,&count );		// カーソルを次の文字候補位置へ
		getkList( wordList,count );
		pc = CURSTEP;
		index = 0;
	}

	if (str[0]){										// 文字を出力する場合はウィンドウを一時的に消去
		putBack();
		if (count)
			putkList();
		getFlag = 1;
	} else {
		if (getFlag){									// 前回ウィンドウを消去していた（メイン側で作画してるかも）なら背景を再取得
			getBack();
			getkList( wordList,count );
			getFlag = 0;
		}
		if (gMode==1){									// 他モードに遷移するときは実行しない
			InLine( gSt,0 );							// 変換ライン表示
			if (pad1.Buttons & PSP_CTRL_LTRIGGER){
				if (count<=YOMILMAX)
					kList( wordList,count,index );		// 「かな」候補リスト表示
				DrawKey1( NULL,NULL,t,xx,yy,pc );		// ソフトキー表示
			} else {
				if (count<=YOMILMAX)
					kList( wordList,count,-1 );			// 「かな」候補リスト表示
				DrawKey1( x,y,t,xx,yy,pc );				// ソフトキー表示
			}
			pc--;
			if (pc<0) pc = 0;
		} else {
			putBack();									// ソフトキーを消去
			if (count)
				putkList();
		}
	}
}


//==============================================================
// ソフトキーモード時の処理（×５かな配列）
//--------------------------------------------------------------

static void softkey3(char *str,SceCtrlData pad1)
{
	static char	wordList[50][33];
	static int	getFlag = 0,x = 0,y = 0,t = 0,index = 0,count = 0,
				xd[4],yd[4],xx[4],yy[4];
	int		i,p,s,ch,fp,pos;
	char	si[3];

	fp = 0;												// 画面に変更があったか
	si[2] = 0;

	ch = SIMEgetchar(pad1);
	if (!strlen(gSt))
		if (ch==0 || ch>=32 || ch==0x02 || ch==0x08 || ch==0x0D || ch==0x1B)
			DrawCursor(ch);								// カーソル表示
	switch (ch){										// カーソルキー入力
	case 0x02:											// △
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// 変換ラインを一旦消す
				gMode = 2;
			}
		} else {
			si[0] = gKeyTable3[t][(y*20+x*5+0) *2];
			si[1] = gKeyTable3[t][(y*20+x*5+0) *2+1];
			fp = 1;
		}
		break;
	case 0x04:											// R
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			t++;										// 文字種変更
			if (t>5) t = 0;
		} else {
			si[0] = gKeyTable3[t][(y*20+x*5+2) *2];
			si[1] = gKeyTable3[t][(y*20+x*5+2) *2+1];
			fp = 1;
		}
		break;
	case 0x05:											// [START]
		t++;											// 文字種変更
		if (t>5) t = 0;
		break;
	case 0x06:											// [SELECT]
		if (gSt[0]!=0){
			putInLine();								// 変換ラインを一旦消す
			if (count)
				putkList();
		}
		count = 0;
		gMode = 5;										// 設定画面へ
		break;
	case 0x08:											// □
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]!=0){
				putInLine();							// 変換ライン背景を復元
				if (count)
					putkList();
				pos = strlen(gSt);
				pos--;
				p = 0;
				while (1){								// 全角文字補正
					s = 1 -chkSJIS(gSt[p]);
					if (p+s>pos) break;
					p += s;
				}
				gSt[p] = 0;
				if (s==2) gSt[p+1] = 0;
				getInLine(gSt);							// 変換ライン背景を取得
				if (t==0){
					setCur( xd,yd,0,xx,yy,0,wordList,&count );	// カーソルを次の文字候補位置へ
				} else {
					setCur( xd,yd,0,xx,yy,1,wordList,&count );	// カーソルを次の文字候補位置へ
				}
				index = 0;
				getkList( wordList,count );
			} else {
				str[0] = ch;							// バックスペースを出力
				DrawCursor(ch);
			}
		} else {
			si[0] = gKeyTable3[t][(y*20+x*5+1) *2];
			si[1] = gKeyTable3[t][(y*20+x*5+1) *2+1];
			fp = 1;
		}
		break;
	case 0x0D:											// ○
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				str[0] = ch;							// [Enter]を出力
				DrawCursor(ch);
			} else {
				putInLine();							// 変換ライン背景を復元
				for (i=0; i<CHLEN ;i++){
					str[i] = gSt[i];
					gSt[i] = 0;
				}
			}
		} else {
			si[0] = gKeyTable3[t][(y*20+x*5+3) *2];
			si[1] = gKeyTable3[t][(y*20+x*5+3) *2+1];
			fp = 1;
		}
		break;
	case 0x1B:											// ×
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// 変換ライン背景を復元
				for (i=0; i<CHLEN ;i++){
					gSt[i] = 0;
				}
			}
		} else {
			si[0] = gKeyTable3[t][(y*20+x*5+4) *2];
			si[1] = gKeyTable3[t][(y*20+x*5+4) *2+1];
			fp = 1;
		}
		break;
	case 0x1C:											// →
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count && count<=YOMILMAX){
					putInLine();						// 変換ライン背景を復元
					putkList();
					strcpy( gSt,wordList[index] );
					getInLine(gSt);						// 変換ライン背景を取得
					count = 0;
				}
			}
		} else {
			x++;
			if (x>3) x = 0;
		}
		break;
	case 0x1D:											// ←
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			}
		} else {
			x--;
			if (x<0) x = 3;
		}
		break;
	case 0x1E:											// ↑
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count){
					index--;
					if (index<0) index = count -1;
				}
			}
		} else {
			y--;
			if (y<0) y = 3;
		}
		break;
	case 0x1F:											// ↓
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count){
					index++;
					if (index>count-1) index = 0;
				}
			}
		} else {
			y++;
			if (y>3) y = 0;
		}
		break;
	default:											// その他（文字の直接入力？）
		if (ch>=32){
			si[0] = ch;
			fp = 1;
		}
		break;
	}

	if (fp){											// 文字を変換行に追加
		if (count)
			putkList();
		addInline( str,si,t );
		setCur( xd,yd,0,xx,yy,0,wordList,&count );		// 「よみ」候補リストの取得のみ使用
		getkList( wordList,count );
		index = 0;
	}

	if (str[0]){										// 文字を出力する場合はウィンドウを一時的に消去
		putBack();
		if (count)
			putkList();
		getFlag = 1;
	} else {
		if (getFlag){									// 前回ウィンドウを消去していたなら背景を再取得
			getBack();
			getkList( wordList,count );
			getFlag = 0;
		}
		if (gMode==1){									// 他モードに遷移するときは実行しない
			InLine( gSt,0 );							// 変換ライン表示
			if (pad1.Buttons & PSP_CTRL_LTRIGGER){
				if (count<=YOMILMAX)
					kList( wordList,count,index );		// 「かな」候補リスト表示
				DrawKey3( -1,-1,t );					// ソフトキー表示（カーソルなし）
			} else {
				if (count<=YOMILMAX)
					kList( wordList,count,-1 );			// 「かな」候補リスト表示
				DrawKey3( x,y,t );						// ソフトキー表示
			}
		} else {
			putBack();									// ソフトキーを消去
			if (count)
				putkList();
		}
	}
}


//==============================================================
// ソフトキーモード時の処理
//--------------------------------------------------------------
// セレクトされている各配列の処理先に分岐
//--------------------------------------------------------------

static void keyboard(char *str,SceCtrlData pad1)
{
	switch (gKey){
	case 0:
		softkey1( str,pad1 );
		break;
	case 1:
		softkey3( str,pad1 );
		break;
	default:
		gKey = 0;
		break;
	}
}


//==============================================================
// char列から全角文字コードを取得
//--------------------------------------------------------------

static unsigned int fcode(char *str)
{
	if (str[0]=='\0'){
		return (0);
	} else {
		return ((unsigned char)str[0] * 0x100 + (unsigned char)str[1]);
	}
}


//==============================================================
// char列からintデータを取得
//--------------------------------------------------------------

static unsigned int getInt(char *str)
{
	return ((unsigned char)str[0] + (unsigned char)str[1] * 0x100);
}


//==============================================================
// 全角文字種別の切り分け
//--------------------------------------------------------------

static int codeChk(char *str,unsigned int ar1,unsigned int ar2)
{
	unsigned int cr;
	int pos;

	pos = 0;
	while (pos<strlen(str)){
		if (chkSJIS(str[pos])){
			cr = fcode(&str[pos]);
			pos += 2;
		} else {								// 半角を見つけたら即終了
			break;
		}
		if (cr<ar1 || cr>ar2)
			break;								// 範囲外の文字を見つけたら終了
	}
	return (pos);
}


//==============================================================
// 漢字変換準備
//--------------------------------------------------------------
// len      変換対象となる文節長（バイト）
// adjust      0:文節長は指定値に強制固定
//          以外:文節長は指定値より短く変更される場合あり
// count    単語候補の数
// wordList 単語リスト（char wordList[50][33]の配列を指定すること）
//
// 戻り値   探査を行った「よみ」の長さ（バイト）
//          count   :単語リストの件数（１～）
//          wordList:単語リスト
//--------------------------------------------------------------
// gStの読みを元に辞書を検索し漢字候補を取得する。
// wordListの最後には探査を行った文節長の「よみ」そのものが入っている。
// 辞書に一致するものが無かった場合でも最低でも「よみ」だけは返されます。
//--------------------------------------------------------------

static int setkanji(int len,int adjust,int *count,char wordList[][33])
{
	unsigned int	cr;
	const char		kazu[21] = {"〇一二三四五六七八九"};
	int		i,j,hp,sp,sp2,sp3,tmp,sjis;
	long	pos,pos2;

	*count = 0;
	cr = 0;
	gDicWordPos = -1;

	if (chkSJIS(gSt[0])){						// 先頭が全角文字なら
		cr = fcode(&gSt[0]);					// 先頭文字
		if (cr<0x8200U){						// 記号なら
			hp = 0;
		} else if (cr<0x829FU){					// 数字とアルファベット
			hp = -3;
		} else if (cr<0x82F2U){					// ひらがななら
			hp = cr - 0x829FU +1;
		} else if (cr==0x8394U){				// 「ヴ」
			hp = 84;
		} else {
			hp = -1;							// 漢字検索を行わない
		}
	} else {									// 先頭が半角なら
		hp = -2;
	}

	if (hp<0){											// 漢字検索を行わない場合（辞書で対応していない先頭文字）
		if (adjust){
			if (hp==-2){								// 半角の部分を選択
				for (sp=0; sp<strlen(gSt) ;sp++)
					if (chkSJIS(gSt[sp])) break;
			} else {									// 同種の全角文字タイプ毎にまとめる
				if (cr<0x8259U){						// 数字
					sp = codeChk( gSt,0x824F,0x8258 );
				} else if (cr<0x829BU){					// アルファベット
					sp = codeChk( gSt,0x8260,0x829A );
				} else if (cr<0x8397U){					// カタカナ
					sp = codeChk( gSt,0x8340,0x8396 );
				} else if (cr<0x83D7U){					// ギリシャ
					sp = codeChk( gSt,0x839F,0x83D6 );
				} else if (cr<0x8492U){					// ロシア
					sp = codeChk( gSt,0x8440,0x8491 );
				} else {								// 記号
					sp = codeChk( gSt,0x84A0,0x879F );
				}
			}
		} else {
			sp = len;									// 文字タイプは無視
		}

	} else {												// 漢字変換を行う場合
		pos = gHEDtbl[hp];									// 頭文字に対応する辞書レコードの開始位置
		if (pos==0xFFFFFFFF){								// 辞書に登録なし
			sp = 2;
		} else {											// 辞書に先頭文字が登録されている場合
			pos += 2;
			sp2 = sp3 = 0;
			pos2 = -1;
			while(1){
				sp = 0;
				while (gDic[pos+sp]!=0){					// よみに対応する単語を辞書より検索
					if (gDic[pos+sp]!=gSt[sp]) break;
					sp++;
					if (sp==len) break;
				}
				if (gDic[pos+sp]==0){						// 単語候補発見（ただしもっと長く一致するものがあるかもしれないので探査は終わらない）
					pos2 = pos;
					sp3 = sp;
				}
				if (hp==0){									// 記号の場合の終了条件
					if (fcode(&gDic[pos])>0x829E)
						break;
				} else {									// ひらがなの場合の終了条件
					if (sp<sp2)								// 一致する文字数が前回より短くなったら探査終了
						break;
				}
				sp2 = sp;
				tmp = getInt(&gDic[pos-2]);					// 次の単語候補への相対距離（int）
				if (tmp==0xFFFF) break;						// 辞書が終了したら探査終了
				pos += tmp;
			}

			if (pos2==-1){						// 辞書に載っていない（記号を変換しようとした場合に起こりえる）
				sp = 2;
			} else if (adjust==0 && sp3!=len){	// 指定された文節長に足りない
				sp = len;
			} else {							// 単語リストを取り込む
				sp = sp3;
				pos = pos2 + strlen(&gDic[pos2]) +1;
				gDicWordPos = pos;				// 後で辞書記載順を入れ替えるために
				*count = gDic[pos++];			// 単語数（byte）
				if (*count>49) *count = 49;		// 単語数制限（バッファオーバーフロー対策）
				for (i=0; i<*count ;i++){		// 単語取り込み
					tmp = strlen(&gDic[pos]);
					if (tmp>32) tmp = 32;		// 単語の長さ制限（バッファオーバーフロー対策）
					sjis = 0;
					for (j=0; j<tmp ;j++){
						wordList[i][j] = gDic[pos+j];
						if (sjis){				// 長さ制限に引っかかった場合にゴミが残る恐れがあるので
							sjis = 0;
						} else {
							sjis = chkSJIS(gDic[pos+j]);
						}
					}
					wordList[i][j+sjis] = '\0';
					pos += strlen(&gDic[pos])+1;
				}
			}
		}
	}

	if ((hp>0 || hp==-3) && !*count){			// 候補が無い場合
		tmp = 0;
		for (i=0; i<sp ;i+=2){
			cr = fcode(&gSt[i]);
			if (cr>=0x824F && cr<0x8259){		// 数字を漢数字へ
				wordList[*count][i  ] = kazu[(cr-0x824F)*2  ];
				wordList[*count][i+1] = kazu[(cr-0x824F)*2+1];
				tmp = 1;
			} else if (cr>=0x829F && cr<0x82F2){
				if (cr>=0x82DE) cr++;			// sJISコードの「ミ」と「ム」の間にある空欄は何だろう？
				cr += 161;						// ひらがな→カタカナ
				wordList[*count][i  ] = cr >> 8;
				wordList[*count][i+1] = cr & 0x00FF;
				tmp = 1;
			}
		}
		if (tmp){								// コード変換を行っていた場合は
			wordList[*count][i] = '\0';
			(*count)++;
		}
	}

	for (i=0; i<sp ;i++)						// 最後に「よみ」そのものを単語リストに追加
		wordList[*count][i] = gSt[i];
	wordList[*count][i] = '\0';
	(*count)++;

	return (sp);
}


//==============================================================
// 漢字候補リストの位置
//--------------------------------------------------------------
// 変換ラインの下か上に漢字候補リストの位置を設定する。
// 画面端とぶつからない方向に設定します。
//--------------------------------------------------------------

static void kListPos(int *x,int *y,int *wx,int *wy,char wordList[][33],int count)
{
	int i,len,max,xl,yl,xx;

	InLinePos( gSt,x,wx );						// x座標取得
	xx = *x;

	max = 0;
	for (i=0; i<count ;i++){
		len = strlen(wordList[i]);
		if (len>max) max = len;
	}
	*wx = 2+ max * FONTX +3 +4 +1;
	if (count>KLIST){
		*wy = KLIST * (FONTY+1)+2 -1;
	} else {
		*wy = count * (FONTY+1)+2 -1;
	}

	if (*x+*wx>480) *x = 480 - *wx;				// 画面からはみ出すなら開始位置をずらす
	*y = *gCy + FONTY+1;
	if (*y+*wy>272) *y = *gCy -1 - *wy;

	if (gMode==1){								// ソフトキーモードならソフトキーを回避する
		if (gKey){
			xl = KBPOSX3;
			yl = KBPOSY3;
		} else {
			xl = KBPOSX1;
			yl = KBPOSY1;
		}
		if (*x+*wx>=xl && *y+*wy>=yl){			// ソフトキーに重なる
			if (*x<xl){
				*x = xl - *wx;
			} else {
				*y = *gCy -1 - *wy;
				if (*y<0){
					*x = xx - *wx;
					*y = yl -1 -*wy;
				}
			}
		}
	}
}


//==============================================================
// 漢字候補リストの表示
//--------------------------------------------------------------
// gCx,gCyで指定されているカーソル位置の下か上に漢字候補リストの表示を行う。
// index が -1 のときはカーソルを表示しない。
//--------------------------------------------------------------

static void kList(char wordList[][33],int count,int index)
{
	int	i,page,len,pos,n,x,y,wx,wy,df;

	if (!count) return;

	kListPos( &x, &y, &wx, &wy, wordList, count );		// 漢字候補リスト位置
	BoxFill( x, y, wx-5, wy, CORFL1, CORIN );

	BoxFill( x + wx-1-4-1, y, 5, wy, CORFL1, CORFR );
	if (index>=0){
		df = 1;
	} else {
		df = 0;
		index = 0;
	}
	page = index / KLIST;
	if (count<KLIST){
		pos = 0;
		len = wy -2;
	} else {
		pos = page * (wy-2) * KLIST / count;
		len = (wy-1) * KLIST / count;
	}
	if (len>wy-2) len = wy -2;							// スクロールバーのサイズ補正
	if (pos+len>wy-2) pos = wy-2 -len;					// スクロールバーの位置補正
	for (i=0; i<4 ;i++)									// スクロールバー作画
		VLine( x + wx-2-i, y+1 +pos, len, CORRBAR );

	pos = index % KLIST;
	y += 1;
	x += 1;
	if (df)
		BoxFill( x, y +pos *(FONTY+1), wx-2-4-1, FONTY, CORCUR, CORCUR );
	n = page * KLIST;
	for (i=0; i<KLIST ;i++){
		if (n>=count) break;
		mh_print( x+2,y,wordList[n],CORWCHR );			// 変換ライン文字列
		y += FONTY +1;
		n++;
	}
}


//==============================================================
// 漢字候補リストの背景の待避
//--------------------------------------------------------------

static void getkList(char wordList[][33],int count)
{
	int	x,y,wx,wy;

	if (!count){
		gkListBuf[0] = -1;
		return;
	}

	kListPos( &x, &y, &wx, &wy, wordList, count );		// 漢字候補リスト位置
	gkListBuf[0] = x;
	gkListBuf[1] = y;
	BoxCopy( &gkListBuf[2],x,y,wx,wy );
}


//==============================================================
// 漢字候補リストの背景の復元
//--------------------------------------------------------------

static void putkList(void)
{
	if (gkListBuf[0]==-1) return;

	if (!gBDFlag){
		if (gkListBuf[2])
			BoxPaste( &gkListBuf[2],gkListBuf[0],gkListBuf[1] );
	}
}


//==============================================================
// 辞書記載順の入れ替え
//--------------------------------------------------------------

static void dicWordEx(int index)
{
	long	pos,pos2;
	char	str[65];
	int		i,n,len;

	if (gDicWordPos==-1) return;				// 位置が指定されていない
	if (gDic[gDicWordPos]<(index+1)) return;	// 登録語数以上を指定している
	if (!index) return;							// 既に先頭位置にある

	gSaveFlag = -1;

	pos = gDicWordPos +1;
	pos2 = pos;
	n = 0;
	while (n<index){							// 目標語句の位置を探す
		pos += strlen(&gDic[pos]) +1;
		n++;
	}
	strcpy(str,&gDic[pos]);						// 先頭に移動させる語句を待避
	len = strlen(&gDic[pos]) +1;				// 移動させるサイズ
	n = pos - pos2;
	pos--;
	for (i=n; i>0 ;i--){						// その他の語句を移動
		gDic[pos+len] = gDic[pos];
		pos--;
	}
	strcpy(&gDic[pos2],str);					// 目標語句を先頭に書き戻す
}


//==============================================================
// 漢字選択モード
//--------------------------------------------------------------

static void change(char *str,SceCtrlData pad1)
{
	static char	wordList[50][33],ILstr[64];
	static int	mode = 0,count,len,index,getFlag = 0;
	int	l,p,s,ch,df;

	if (!mode){									// 変換開始直後の初期設定
		len = setkanji( strlen(gSt),1,&count,wordList );
		index = 0;
		mode = 1;
		strcpy( ILstr,wordList[index] );		// 変換対象
		strcat( ILstr,&gSt[len] );				// 未変換対象
		getInLine(ILstr);						// 初期状態の変換ライン背景を取得
		getkList( wordList,count );
	}

	df = 0;

	ch = SIMEgetchar(pad1);
	switch (mode){
	case 1:										// 漢字選択モード
		switch (ch){
		case 0x03:								// L
			putkList();							// 漢字候補リストを消す
			if (index==0){
				index = count -1;
			} else {
				index -= KLIST;
				if (index<0) index = 0;
			}
			df = 1;
			break;
		case 0x04:								// R
			putkList();							// 漢字候補リストを消す
			if (index==count-1){
				index = 0;
			} else {
				index += KLIST;
				if (index>count-1) index = count -1;
			}
			df = 1;
			break;
		case 0x0D:								// ○
			strcpy( str,wordList[index] );
			strcpy( gSt,&gSt[len] );
			if (strlen(gSt)==0){				// 「よみ」が全て無くなったらソフトキーモードへ
				gMode = 3;
			}
			if ((count-1)!=index)				// 候補リストの最後の語句は「よみ」なので入れ替え処理は行わない
				dicWordEx(index);
			mode = 0;
			break;
		case 0x08:								// □
		case 0x1B:								// ×
			putInLine();						// 変換ラインを消す
			putkList();							// 漢字候補リストを消す
			mode = 0;
			gMode = 3;							// ソフトキーモードへ
			break;
		case 0x1C:								// →
			putInLine();						// 変換ラインを消す
			putkList();							// 漢字候補リストを消す
			len += 1 -chkSJIS(gSt[len]);		// 全角文字補正
			l = strlen(gSt);
			if (len>l) len = l;
			strcpy( ILstr,gSt );
			getInLine(ILstr);
			getkList( wordList,count );
			mode = 2;
			break;
		case 0x1D:								// ←
			putInLine();						// 変換ラインを消す
			putkList();							// 漢字候補リストを消す
			len--;
			if (len<1) len = 1;
			p = 0;
			while (1){							// 全角文字補正
				s = 1 -chkSJIS(gSt[p]);
				if (p+s>len) break;
				p += s;
			}
			if (p==0) p = 2;
			len = p;
			strcpy( ILstr,gSt );
			getInLine(ILstr);
			getkList( wordList,count );
			mode = 2;
			break;
		case 0x1E:								// ↑
			putkList();							// 漢字候補リストを消す
			index--;
			if (index<0) index = count -1;
			df = 1;
			break;
		case 0x1F:								// ↓
		case 0x02:								// △
			putkList();							// 漢字候補リストを消す
			index++;
			if (index>count-1) index = 0;
			df = 1;
			break;
		}
		break;
	
	case 2:										// 文節長選択モード
		switch (ch){
		case 0x03:								// L
			break;
		case 0x04:								// R
			break;
		case 0x0D:								// ○
			strncpy( str,gSt,len );
			str[len] = '\0';
			strcpy( gSt,&gSt[len] );
			if (strlen(gSt)==0){				// 「よみ」が全て無くなったらソフトキーモードへ
				putInLine();					// 変換ラインを消す
				gMode = 3;
			}
			mode = 0;
			break;
		case 0x08:								// □
		case 0x1B:								// ×
			putInLine();						// 変換ラインを消す
			mode = 0;
			gMode = 3;							// ソフトキーモードへ
			break;
		case 0x1C:								// →
			len += 1 -chkSJIS(gSt[len]);		// 全角文字補正
			l = strlen(gSt);
			if (len>l) len = l;
			break;
		case 0x1D:								// ←
			len--;
			if (len<1) len = 1;
			p = 0;
			while (1){							// 全角文字補正
				s = 1 -chkSJIS(gSt[p]);
				if (p+s>len) break;
				p += s;
			}
			if (p==0) p = 2;
			len = p;
			break;
		case 0x1E:								// ↑
			len = setkanji( len,0,&count,wordList );
			index = count -1;
			mode = 1;
			df = 1;
			break;
		case 0x1F:								// ↓
		case 0x02:								// △
			len = setkanji( len,0,&count,wordList );
			index = 0;
			mode = 1;
			df = 1;
			break;
		}
		break;
	}

	if (df){									// 変換候補に変化があった
		putInLine();							// 変換ラインを消す
		strcpy( ILstr,wordList[index] );		// 変換対象
		strcat( ILstr,&gSt[len] );				// 未変換対象
		getInLine(ILstr);
		getkList( wordList,count );
	}

	if (str[0]){								// 文字を出力する場合はウィンドウを一時的に消去
		putInLine();							// 変換ラインを消す
		putkList();								// 漢字候補リストを消す
		getFlag = 1;
	} else {
		if (getFlag){							// 前回ウィンドウを消去していたなら背景を再取得
			getInLine(ILstr);
			getkList( wordList,count );
			getFlag = 0;
		}
		if (gMode==2){							// ソフトキーモードに遷移するときは実行しない
			switch (mode){						// 変換ラインを表示
			case 1:
				InLine( ILstr,strlen(wordList[index]) );
				kList( wordList,count,index );
				break;
			case 2:
				InLine(ILstr,len);
				break;
			}
		}
	}
}


//==============================================================
// 設定ウィンドウの作画
//--------------------------------------------------------------

static void DrawMenu(int pos,char *strKey,char *strSave)
{
	DrawWindow( SIMENAME,MEPOSX,MEPOSY,218,164 );							// ウィンドウフレーム
	BoxFill( MEPOSX +1, MEPOSY +15, 218-2, 149, CORFL1, CORFR );			// 内側

	mh_print( MEPOSX +4 +12 , MEPOSY +15 +12 , "使用している辞書", CORFCHR );
	DrawTextbox( gDicFile,MEPOSX +4 +30,MEPOSY +15 +28,162,CORFCHR,CORFL1 );

	if (pos==0)
		BoxFill( MEPOSX +4 +12 -1, MEPOSY +15 +60, 96 +2, 12, CORCUR, CORCUR );
	mh_print( MEPOSX +4 +12 , MEPOSY +15 +60 , "ソ\フトキーの種類", CORFCHR );
	DrawTextbox( strKey,MEPOSX +4 +84,MEPOSY +15 +78,108,CORWCHR,CORIN );

	if (pos==1)
		BoxFill( MEPOSX +4 +12 -1, MEPOSY +15 +108, 180 +2, 12, CORCUR, CORCUR );
	mh_print( MEPOSX +4 +12 , MEPOSY +15 +108 , "終了時に辞書の記載順を保存する", CORFCHR );
	DrawTextbox( strSave,MEPOSX +4 +144,MEPOSY +15 +126,48,CORWCHR,CORIN );
}


//==============================================================
// 設定ウィンドウ背景の待避
//--------------------------------------------------------------

static void getMenu()
{
	gBackBuf[0] = MEPOSX;
	gBackBuf[1] = MEPOSY;
	BoxCopy( &gBackBuf[2],MEPOSX,MEPOSY,218,166 );
}


//==============================================================
// 各種設定
//--------------------------------------------------------------

static void menu(SceCtrlData pad1)
{
	static int pos = 0,mode = 0,item[2];
	char	strKey[][21] = {"５０音配列AC","×５かな配置"},
			strSave[][7] = {"する","しない"};
	int ch;

	ch = SIMEgetchar(pad1);
	switch (mode){
	case 0:										// 初期設定
		item[0] = gKey;
		item[1] = gSave;
		pos = 0;
		mode = 1;
		break;

	case 1:										// 漢字選択モード
		switch (ch){
		case 0x03:								// L
			break;
		case 0x04:								// R
			break;
		case 0x0D:								// ○
			if (gKey!=item[0] || gSave!=item[1])
				gIni = 1;						// 環境データが変更された
			gKey = item[0];
			gSave = item[1];
			mode = 0;
			gMode = 3;							// ソフトキーモードへ
			break;
		case 0x08:								// □
			break;
		case 0x1B:								// ×
			mode = 0;
			gMode = 3;							// ソフトキーモードへ
			break;
		case 0x1C:								// →
			if (pos==0){
				item[pos]++;
				if (item[pos]>1) item[pos] = 1;
			} else {
				item[pos] = 1;
			}
			break;
		case 0x1D:								// ←
			item[pos]--;
			if (item[pos]<0) item[pos] = 0;
			break;
		case 0x1E:								// ↑
			pos = 0;
			break;
		case 0x1F:								// ↓
			pos = 1;
			break;
		case 0x02:								// △
			break;
		}
		break;
	}

	if (gMode==4){
		DrawMenu( pos, strKey[item[0]], strSave[item[1]] );
	} else {
		putBack();
	}
}


//==============================================================
// 文字列入力
//--------------------------------------------------------------
// *str   入力された文字（最大33バイト asciiz）
// pad1   パッド情報
// 戻り値 strそのもの
//--------------------------------------------------------------
// ソフトキーボードによる文字列の入力。
// かな英数およびIMEによる漢字変換処理も行う。
// 文字だけでなくカーソルキー等の各ボタンもコントロールコードとして返す。
// 入力文字列は（確定された文字がある場合は）strに入っています。
//--------------------------------------------------------------

char *SIMEgets(char *str,SceCtrlData pad1)
{
	int i;

	for (i=0; i<CHLEN ;i++)
		str[i] = 0;

	switch (gMode){
	case 0:										// 移動モード
		move( str,pad1 );
		break;

	case 1:										// キーボードモード
		keyboard( str,pad1 );
		break;

	case 2:										// 漢字選択モード
		change( str,pad1 );
		break;

	case 3:										// ソフトキーモードに遷移する
		gMode = 1;
		getInLine(gSt);
		getBack();
		keyboard( str,pad1 );
		break;

	case 4:										// 各種設定画面
		menu(pad1);
		break;

	case 5:										// 設定画面に遷移する
		gMode = 4;
		getMenu();
		menu(pad1);
		break;

	}
	return (str);
}




