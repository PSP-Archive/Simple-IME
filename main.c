//==============================================================
// Simple IME TEST
//     for PSP CFW3.80 M33-5
// STEAR 2009
//--------------------------------------------------------------
// Simple IME�e�X�g�v���O�����B
//--------------------------------------------------------------

#include <pspuser.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include "zenkaku.h"
#include <string.h>

#include "draw.h"
#include "sime.h"

//----- �}�N�� -----


//----- �萔�ݒ� -----


//----- �v���g�^�C�v�錾 -----


//- �V�X�e���֘A -

int		SetupCallbacks(void);
int		callbackThread(SceSize args, void *argp);
int		exitCallback(int arg1, int arg2, void *common);

//- ���̑� -


//------ �O���[�o���ϐ� -----

static unsigned int __attribute__((aligned(16))) gList[262144];
static volatile int	gExitRequest = 0;


//------ ���s���[�h�w�� -----

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

	//----- Gu������ -----

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

	//----- SimpleIME������ -----

	mh_print(0, 0, "�����t�@�C�����[�h��", 0xFFFFFF);
	ret = InitSIME(0);									// �����ǂݍ��݂Ɗe�평�����i�w�i�̎��ȕ������[�h�j
	switch(ret){
	case 1:
		mh_print(0, 13, "�s���Ȏ����t�@�C���ł��B", 0xFFFFFF);
		break;
	case 2:
		mh_print(0, 13, "������������Ȃ��Ď������ǂ߂܂���ł����B", 0xFFFFFF);
		break;
	case 3:
		mh_print(0, 13, "�����iSIME.DIC�j���J���܂���ł����B", 0xFFFFFF);
		break;
	}
	if (ret){											// �G���[���������Ă�����
		while(SIMEgetchar(pad1)==0 && !gExitRequest){	// �����������܂őҋ@
			sceCtrlReadBufferPositive(&pad1, 1);
			sceDisplayWaitVblankStart();
		}
		gExitRequest = 1;								// �I��
	}
	BoxClr(0, 0, 480, 24);

	//----- ���C�� -----

	for (i=0; i<480 ;i++)						// �w�i���
		VLine(i,0,272,(i/5)<<16 | ((479-i)/5)<<8);

	mh_print(0, 0, "Simple IME Ver1.00 TEST�v���O����  ���Ń\\�t�g�L�[�W�J", 0xFFFFFF);
	x = 0;
	y = 13;
	SIMEcursor(2,&x,&y);						// �J�[�\���̑傫���ƍ��W�ϐ��̎w��
	while(!gExitRequest){
		sceCtrlReadBufferPositive(&pad1, 1);
		SIMEgets(str,pad1);						// ���͕�����̎擾
		if (str[0]!=0){							// ���͂�������
			if (str[0]<32U){					// �R���g���[���R�[�h�������ꍇ
				switch(str[0]){
				case SIME_KEY_CIRCLE:			// ��
					x = 0;
					y += 13;
					if (y>260){
						y -= 13;
						for (i=13; i<272+13 ;i++)
							for (j=0; j<480 ;j++)
								VRAM[i-13][j] = VRAM[i][j];
					}
					break;
				case SIME_KEY_UP:				// ��
					y -= 13;
					if (y<0) y = 0;
					break;
				case SIME_KEY_DOWN:				// ��
					y += 13;
					if (y>260) y -= 13;
					break;
				case SIME_KEY_LEFT:				// ��
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
				case SIME_KEY_RIGHT:			// ��
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
			} else {							// �����������ꍇ
				i = strlen(str);				// ���������J�E���g
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

	//----- �I������ -----

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
