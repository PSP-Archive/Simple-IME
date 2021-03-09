//==============================================================
// Simple IME TEST
//     for PSP CFW3.80 M33-5
// STEAR 2009
//--------------------------------------------------------------
// Simple IMEテストプログラム。
//--------------------------------------------------------------

#include <pspuser.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include "zenkaku.h"
#include <string.h>

#include "draw.h"
#include "sime.h"

//----- マクロ -----


//----- 定数設定 -----


//----- プロトタイプ宣言 -----


//- システム関連 -

int		SetupCallbacks(void);
int		callbackThread(SceSize args, void *argp);
int		exitCallback(int arg1, int arg2, void *common);

//- その他 -


//------ グローバル変数 -----

static unsigned int __attribute__((aligned(16))) gList[262144];
static volatile int	gExitRequest = 0;


//------ 実行モード指定 -----

#ifndef	PSP_MODULE_KERNEL
#define	PSP_MODULE_KERNEL	0x1000
#endif
#ifndef	PSP_MODULE_USER
#define	PSP_MODULE_USER		0
#endif

PSP_MODULE_INFO("SimpleIMEtest", PSP_MODULE_USER, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_STACK_SIZE_KB(32);


//==============================================================
// main
//--------------------------------------------------------------

int	main(int argc,char **argv)
{
	SceCtrlData		pad1,pad2;
	char			str[33];
	int				i,j,x,y,ret;

	SetupCallbacks();

	//----- Gu初期化 -----

	sceGuInit();
	sceGuStart(GU_DIRECT, gList);
	sceGuDrawBuffer(GU_PSM_8888, SCRBUF0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, SCRBUF0, BUF_WIDTH);
	sceGuFinish();
	sceGuSync(0, 0);
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	//----- SimpleIME初期化 -----

	mh_print(0, 0, "辞書ファイルロード中", 0xFFFFFF);
	ret = InitSIME(0);									// 辞書読み込みと各種初期化（背景の自己復元モード）
	switch(ret){
	case 1:
		mh_print(0, 13, "不正な辞書ファイルです。", 0xFFFFFF);
		break;
	case 2:
		mh_print(0, 13, "メモリが足りなくて辞書が読めませんでした。", 0xFFFFFF);
		break;
	case 3:
		mh_print(0, 13, "辞書（SIME.DIC）が開けませんでした。", 0xFFFFFF);
		break;
	}
	if (ret){											// エラーが発生していたら
		while(SIMEgetchar(pad1)==0 && !gExitRequest){	// 何か押されるまで待機
			sceCtrlReadBufferPositive(&pad1, 1);
			sceDisplayWaitVblankStart();
		}
		gExitRequest = 1;								// 終了
	}
	BoxClr(0, 0, 480, 24);

	//----- メイン -----

	for (i=0; i<480 ;i++)						// 背景作画
		VLine(i,0,272,(i/5)<<16 | ((479-i)/5)<<8);

	mh_print(0, 0, "Simple IME Ver1.00 TESTプログラム  △でソ\フトキー展開", 0xFFFFFF);
	x = 0;
	y = 13;
	SIMEcursor(2,&x,&y);						// カーソルの大きさと座標変数の指定
	while(!gExitRequest){
		sceCtrlReadBufferPositive(&pad1, 1);
		SIMEgets(str,pad1);						// 入力文字列の取得
		if (str[0]!=0){							// 入力があった
			if (str[0]<32U){					// コントロールコードだった場合
				switch(str[0]){
				case SIME_KEY_CIRCLE:			// ○
					x = 0;
					y += 13;
					if (y>260){
						y -= 13;
						for (i=13; i<272+13 ;i++)
							for (j=0; j<480 ;j++)
								VRAM[i-13][j] = VRAM[i][j];
					}
					break;
				case SIME_KEY_UP:				// ↑
					y -= 13;
					if (y<0) y = 0;
					break;
				case SIME_KEY_DOWN:				// ↓
					y += 13;
					if (y>260) y -= 13;
					break;
				case SIME_KEY_LEFT:				// ←
				case 0x08:						// [BS]
					x -= 6;
					if (x<0){
						y -= 13;
						if (y<0){
							y = 0;
							x = 0;
						} else{
							x += 480;
						}
					}
					break;
				case SIME_KEY_RIGHT:			// →
					x += 6;
					if (x>479){
						y += 13;
						if (y>260){
							y -= 13;
							x -= 6;
						} else {
							x = 0;
						}
					}
					break;
				}
			} else {							// 文字だった場合
				i = strlen(str);				// 文字数をカウント
				BoxClr(x, y, i*6 ,12);
				mh_print(x, y, str, 0xFFFFFF);
				x += i * 6;
				if (x>479){
					x = 0;
					y += 13;
					if (y>260) y -= 13;
				}
			}
		}
		pad2 = pad1;

		sceDisplayWaitVblankStart();
	}

	//----- 終了処理 -----

	EndSIME();
	sceGuTerm();
 	sceKernelExitGame();
	return 0;
}


//==============================================================
// Exit callback
//--------------------------------------------------------------

int exit_callback(int arg1, int arg2, void *common)
{
	gExitRequest = 1;
	return 0;
}

//==============================================================
// Callback thread
//--------------------------------------------------------------

int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

//==============================================================
// Sets up the callback thread and returns its thread id
//--------------------------------------------------------------

int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
	if(thid >= 0){
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}
