//==============================================================
// Simple IME Ver1.00   (STPP.04)
//     for PSP CFW3.80 M33-5
// STEAR 2009
//--------------------------------------------------------------
// PSP�p�̊Ȉ�IME���쐬���Ă݂��B
// �\�t�g�E�F�A�L�[�{�[�h�Ən�ꊿ���ϊ��������B
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

//----- �}�N�� -----


//----- �萔�ݒ� -----

#define SIMENAME	"Simple IME Ver1.00"

#define DICFILE1	"SIME.DIC"					// �����t�@�C����
#define DICFILE2	"ms0:/PSP/COMMON/SIME.DIC"
#define INIFILE1	"SIME.INI"					// ���ݒ�t�@�C��
#define INIFILE2	"ms0:/PSP/COMMON/SIME.INI"

#define KBPOSX1		268							// �\�t�g�E�F�A�L�[�̈ʒuX�i�T�O���z��A�V�X�g�J�[�\���j
#define KBPOSY1		157							// �\�t�g�E�F�A�L�[�̈ʒuY�i�T�O���z��A�V�X�g�J�[�\���j
#define KBPOSX3		303							// �\�t�g�E�F�A�L�[�̈ʒuX�i�~�T���Ȕz�u�j
#define KBPOSY3		105							// �\�t�g�E�F�A�L�[�̈ʒuY�i�~�T���Ȕz�u�j
#define MEPOSX		131							// �ݒ��ʂ̈ʒu
#define MEPOSY		53							// �ݒ��ʂ̈ʒu
#define KEYREP1		20							// �L�[���s�[�g�J�n�܂ł̎���
#define KEYREP2		3							// �L�[���s�[�g�̊Ԋu
#define FONTX		6							// �t�H���g�̒���
#define FONTY		12							// �t�H���g�̍���
#define CHLEN		32							// �ϊ����C���̍ő啶����
#define KLIST		6							// ������⃊�X�g�̍s��
#define CBTIME		25							// �J�[�\���_�Ŏ���
#define CURSTEP		7							// �A�V�X�g�J�[�\���̈ړ����v����
#define YOMILMAX	24							// ���̐��ȉ��Ɂu��݁v��␔���Ȃ����烊�X�g��\��������

#define CORFL1		0xC0A0A0					// �g
#define CORFL2		0xF0D0D0					// ���邢�g
#define CORFL3		0x707070					// �Â��g
#define CORIN		0x600060					// �E�B���h�E���w�i
#define CORFR		0xA08080					// �E�B���h�E���O�i
#define CORCUR		0xFF8080					// �J�[�\��
#define CORCUR1		0x7070FF					// �J�[�\����
#define CORCUR2		0x60D060					// �J�[�\����
#define CORCUR3		0xA0A0E0					// �J�[�\����
#define CORCUR4		0xFF8080					// �J�[�\���~
#define CORRBAR		0x70E070					// �X�N���[���o�[
#define CORWCHR		0xFFFFFF					// �E�B���h�E������
#define CORSCHR		0xB0FFB0					// �E�B���h�E�����ꕶ��
#define CORFCHR		0x000000					// �E�B���h�E�g����
#define CORCHBK		0xFFFFFF					// �ϊ����C���w�i
#define CORCHCR		0x000000					// �ϊ����C������
#define CORCHCU		0xFF8080					// �ϊ����C���I��̈�

//----- �v���g�^�C�v�錾 -----

static unsigned int	fcode(char *str);
static unsigned int	getInt(char *str);
static void kList(char wordList[][33],int count,int index);
static void getkList(char wordList[][33],int count);
static void putkList(void);

//------ �O���[�o���ϐ� -----

static char			gKeyName1[7][25] = {
						"�P�F�S�p �Ђ炪��",
						"�Q�F�S�p �J�^�J�i",
						"�R�F�S�p �A���t�@�x�b�g",
						"�S�F�S�p �L��",
						"�T�F���p �A���t�@�x�b�g",
						"�U�F���p �L��",
						"�L�[�K�C�_���X"
					};
static char			gKeyTable1[6][161] = {
						"�O�����������P�����������Q�����������R�����ĂƂS�Ȃɂʂ˂̂T�͂ЂӂւقU�܂݂ނ߂��V����J�K�W������X�����A�B�I�����������H�������h�[�u�v�E�@",
						"�O�A�C�E�G�I�P�J�L�N�P�R�Q�T�V�X�Z�\\�R�^�`�c�e�g�S�i�j�k�l�m�T�n�q�t�w�z�U�}�~�������V�������J�K�W�����������X�������A�B�I�@�B�D�F�H�H���������b�h�[�u�v�E�@",
						"�O�`�a�b�c�d�P�e�f�g�h�i�Q�j�k�l�m�n�R�o�p�q�r�s�S�t�u�v�w�x�T�y���������U�����������V�����������W�����������X�����������I�����|�C�D�H�m�i���o�h�f�n�j���p�@",
						"�O�I�h���i���P�H�f���j���Q�����������R���{�|�}�~�S�^���o�C�G�T�_���p�D�F�U�m�Q�����O�V�n�b�`�E�c�W�������􁙂X�������~�������T�U�̓��́��������ӃքD�c�����@",
						"0 A B C D E 1 F G H I J 2 K L M N O 3 P Q R S T 4 U V W X Y 5 Z a b c d 6 e f g h i 7 j k l m n 8 o p q r s 9 t u v w x ! y z - , . ? [ ( < { : / ] ) > }   ",
						"7 4 1 0 + * 8 5 2 , - / 9 6 3 . = #             ( < [ { : ! ) > ] } ; ? \" $ % & @ \\ ' ^ _ | ~                                                               "
					};

static char			gKeyTable3[6][161] = {
						"�����������������������������������ĂƂȂɂʂ˂̂͂Ђӂւق܂݂ނ߂�����������������񂟂����������J�K�A�B�[�u�I�v�H�O�P�Q�R�S�T�U�V�W�X�E�@�@�@�@",
						"�A�C�E�G�I�J�L�N�P�R�T�V�X�Z�\\�^�`�c�e�g�i�j�k�l�m�n�q�t�w�z�}�~�������������������������������������@�B�D�F�H�b�J�K�A�B�[�u�I�v�H�O�P�Q�R�S�T�U�V�W�X�E�������@",	// �u�\\�v���ӁI
						"�O�P�Q�R�S�T�U�V�W�X�`�a�b�c�d�e�f�g�h�i�j�k�l�m�n�o�p�q�r�s�t�u�v�w�x�y�����������������������������������������������������f�C�|�I�i�D�j�H���m�h�n���F�o�^�p�@",
						"�O�P�Q�R�S�T�U�V�W�X�h���������C�i���j�D���{�}�|�~�F�^���_�G�I���Q���H�O�m���n�f�b�o�`�p�E�c�������􁪁����������������~�����T�U�������́����̃Ӄ��քD�c�����|�@",
						"0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z ' , - ! ( . ) ? < [ \" ] > : { / }   ",
						"0 1 2 3 4 5 6 7 8 9 ! \" # $ % & ( ' ) * + , - . / : < = > ; ? [ \\ ] @ ^ { _ } | ' ~                                                                             "
					};
static long			gHEDtbl[85],gDicWordPos,gDicSize;
static long			gBackBuf[(194+24)*(166) +4],							// �\�t�g�L�[�w�i�Ҕ��o�b�t�@
					gInLnBuf[480*FONTY +4],									// �ϊ����C���w�i�Ҕ��o�b�t�@
					gkListBuf[(CHLEN*FONTX+6+4+2) * ((FONTY+1)*KLIST + 4)];	// ������⃊�X�g�w�i�Ҕ��o�b�t�@
static char			*gDic,gDicFile[33],gIniFile[33],gSt[CHLEN +1];
static int			gMode,gBDFlag,gSaveFlag,gSave,gIni,gKey;
static int			*gCx,*gCy,gCxw;


//==============================================================
// �����t�@�C���̓ǂݍ���
//--------------------------------------------------------------
// �߂�l�F0 �����̓ǂݍ��ݐ���
//         1 �s���Ȏ����t�@�C��
//         2 ������������Ȃ�
//         3 �����t�@�C�����J���Ȃ��i���݂��Ȃ��H�j
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
			fa = 1;								// �W�����v�e�[�u��������Ȃ�
		} else {
			for (i=0; i<85 ;i++){
				if (gHEDtbl[i]!=0xFFFFFFFF && (gHEDtbl[i]>filesize-pos))
					fa = 1;						// �W�����v�悪�f�[�^�O�������Ă���
			}
			if (!fa){
				gDic = (char*) malloc( filesize - pos );
				if (gDic==NULL){
					fa = 2;						// ���������m�ۂł��Ȃ�
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

	if (fa)										// �����t�@�C�����J���Ȃ�����
		for (i=0; i<85 ;i++)
			gHEDtbl[i] = 0xFFFFFFFF;

	return (fa);
}


//==============================================================
// �����t�@�C���̓ǂݍ��݂Ɗe�평����
//--------------------------------------------------------------
// flag    0 �E�B���h�E�������ɔw�i�𕜌�����
//      �ȊO �E�B���h�E�������ɉ������Ȃ��i���C�����ō�悷��ׂ��j
// �߂�l�F0 �����̓ǂݍ��ݐ���
//         1 �s���Ȏ����t�@�C��
//         2 ������������Ȃ�
//         3 �����t�@�C�����J���Ȃ��i���݂��Ȃ��H�j
//--------------------------------------------------------------
// �����t�@�C����IME���ݒ��ǂݍ��݂܂��B
// ���ݒ�t�@�C���͎����t�@�C���Ɠ����t�H���_�ɔz�u����܂��B
//--------------------------------------------------------------

int InitSIME(int flag)
{
	char	str[256],*p;
	int		ret,type,val;
	FILE*	fp1;

	gMode = 0;
	gCxw = 2;									// �J�[�\���̑傫��
	gDicWordPos = -1;
	gDicSize = 0;
	gSaveFlag = 0;
	gSave = 0;									// �I�����Ɏ����̋L�ڏ���ۑ�����
	gIni = 0;
	gKey = 0;									// �\�t�g�L�[�̎��
	if (flag){
		gBDFlag = 1;							// �E�B���h�E�������ɉ������Ȃ�
	} else {
		gBDFlag = 0;							// �E�B���h�E�������ɔw�i�𕜌�����
	}

	ret = DicLoad( DICFILE1 );
	strcpy( gDicFile ,DICFILE1 );
	strcpy( gIniFile ,INIFILE1 );
	if (ret){
		ret = DicLoad( DICFILE2 );				// COMMON�t�H���_
		strcpy( gDicFile ,DICFILE2 );
		strcpy( gIniFile ,INIFILE2 );
		if (ret){
			gDicFile[0] = '\0';
			strcpy( gIniFile ,INIFILE1 );
		}
	}

	fp1 = fopen( gIniFile, "r" );				// ���ݒ�t�@�C���̓ǂݍ���
	if (fp1!=NULL){
		while (1){
			if (fgets( str,256,fp1 )==NULL) break;
			p = strtok( str," =\t" );
			type = 0;
			if (strstr( p,"KEYTYPE" )!=NULL) type = 1;
			if (strstr( p,"SAVEMODE" )!=NULL) type = 2;
			p = strtok( NULL," =\t" );
			val = (int)strtol( p,NULL,0 );		// �����񁨐��l�ϊ�
			if (val==0){						// �����ł͂Ȃ����ۂ��̂�
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
// �����t�@�C���֏�������
//--------------------------------------------------------------
// �������̕��בւ����ʂ������ɏ�������
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
// �I������
//--------------------------------------------------------------
// �������ɕێ����Ă����������t�@�C���ɏ����߂�����A�J������B
// ��������s�����ɓd����؂铙������Ɗ������̕��בւ�������ɔ��f����܂���B
// �܂��AIME���f�[�^��ݒ�t�@�C���ɕۑ����܂��B
// �ݒ�t�@�C���̈ʒu�͎����t�@�C���Ɠ����t�H���_�ɂȂ�܂����A
// ������������Ȃ������ꍇ�� EBOOT.PBP �Ɠ����t�H���_�ɂȂ�܂��B
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
// �����J�[�\���w��
//--------------------------------------------------------------
// xw  �J�[�\����X�����̕��i0�ȉ��F�J�[�\���\���Ȃ��j
// x,y �J�[�\���ʒu�ւ̃|�C���^
//--------------------------------------------------------------
// �J�[�\�����W���w�肷��ϐ��ƃJ�[�\���`����w�肷��B
// �J�[�\���\����Sgets()�����s���Ă���ԂɍX�V����܂��B
// �J�[�\����I�^�ŁAY�����̑傫���͎w��ł��܂���B
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
// �����J�[�\���̍��
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
	if (*gCy+FONTY>cy && gMode==1)				// �\�t�g�L�[�Əd�Ȃ�Ȃ�J�n�ʒu�����炷
		if (x+gCxw>cx) x = cx - gCxw;

	for (j=0; j<gCxw ;j++)
		for (i=0; i<12 ;i++)
			VRAM[*gCy+i][x+j] ^= 0xFFFFFF;
}


//==============================================================
// �����J�[�\����\��
//--------------------------------------------------------------
// cr    0�F�J�[�\����_�ł�����
//    �ȊO�F�J�[�\��������/�\�����Ȃ�
//--------------------------------------------------------------

static void DrawCursor(int ch)
{
	static int bk = 0,count = 0;

	if (ch){
		if (bk && !gBDFlag) DrawCursorSub();	// �L�[���͂���������J�[�\��������
		bk = 0;
		count = CBTIME;							// ����͒����\��������
	} else {
		count++;
		if (count>CBTIME){						// �J�[�\���_�Ŏ���
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
// �L�[����
//--------------------------------------------------------------
// �߂�l 0:���͂Ȃ�
//        ��:0x1C ��:0x1D ��:0x1E ��:0x1F
//        ��:0x0D�i[Enter]�j �~:0x1B�i[Esc]�j ��:0x08�i[BS]�j ��:0x02 L:0x03 R:0x04
//        [START]:0x05 [SELECT]:0x06
//--------------------------------------------------------------
// �e�{�^���̏�Ԃ��L�[�{�[�h���ȃL�[���s�[�g����Ŏ擾����B
// ���������̏ꍇ�͍Ō�ɉ����������{�^���ɑΉ�����R�[�h��Ԃ��B
//--------------------------------------------------------------

int SIMEgetchar(SceCtrlData pad1)
{
	static SceCtrlData	pad2;
	static int			rep = 0,kcode = 0,button = 0;

	if ((pad1.Buttons & PSP_CTRL_UP) && !(pad2.Buttons & PSP_CTRL_UP)){			// �����������u�Ԃ����o
		rep = 0;																// ���s�[�g�J�E���^���Z�b�g
		kcode = 0x1E;															// �o�̓R�[�h
		button = PSP_CTRL_UP;													// ���s�[�g���Ď�����{�^��
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
		if (rep>KEYREP1) rep -= KEYREP2;					// �L�[���s�[�g
		if (rep==1 || rep==KEYREP1) return (kcode);			// �����Ă���
	} else {
		rep = 0;
		kcode = 0;
		button = 0;
	}

	return (0);												// ���������Ă��Ȃ�
}


//==============================================================
// �e�L�X�g�g�̕\��
//--------------------------------------------------------------
// ���񂾃t���[���Ƀe�L�X�g��\�����܂��B
//--------------------------------------------------------------

static void DrawTextbox(char *str,int x,int y,int wx,long txcor,long bkcor)
{
	BoxFill( x, y, wx, FONTY+2, CORFL2, bkcor );
	HLine( x, y, wx-1, CORFL3 );				// ��
	VLine( x, y, FONTY+2-1, CORFL3 );			// ��
	mh_print( x+2, y+1, str, txcor );			// ����
}


//==============================================================
// �E�B���h�E�g�̕\��
//--------------------------------------------------------------

static void DrawWindow(char *title,int x,int y,int wx,int wy)
{
	BoxFill( x, y, wx-1, FONTY+3, CORFL1, CORFL1 );
	HLine( x +2       , y +2 , wx-4   , CORFL3 );		// ��
	VLine( x +2       , y +2 , FONTY+2, CORFL3 );		// ��
	HLine( x +2       , y +14, wx-4   , CORFL2 );		// ��
	VLine( x +2 +wx -4, y +2 , FONTY+2, CORFL2 );		// �E
	mh_print( x +5, y +2, title, CORFCHR );				// �^�C�g��
	HLine( x       , y    , wx, CORFL2 );				// ��
	VLine( x       , y    , wy, CORFL2 );				// ��
	HLine( x       , y +wy, wx, CORFL3 );				// ��
	VLine( x +wx -1, y    , wy, CORFL3 );				// �E
}


//==============================================================
// �E�B���h�E�w�i�̑Ҕ�
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
// �E�B���h�E�w�i�̕���
//--------------------------------------------------------------

static void putBack(void)
{
	if (!gBDFlag){
		BoxPaste( &gBackBuf[2],gBackBuf[0],gBackBuf[1] );
	}
}


//==============================================================
// �\�t�g�E�F�A�L�[�{�[�h�̕\���i�T�O���z��A�V�X�g�J�[�\���j
//--------------------------------------------------------------
// x[],y[]   �J�[�\���̓��B�ڕW�ʒu
// xx[],yy[] �J�[�\���̌��ʒu
// step      �J�[�\���̓��B�W���i0:�ڕW�ʒu�j
//--------------------------------------------------------------
// x��y��NULL�������ꍇ�̓J�[�\���͕\�����܂���B
//--------------------------------------------------------------

static void DrawKey1(int *x,int *y,int t,int *xx,int *yy,int step)
{
	char	key[3] = "  ",km[4][3] = { "��","��","��","�~" };
	int		i,j,s,mx,my;
	long	cor[4] = { CORCUR1,CORCUR2,CORCUR3,CORCUR4 };

	DrawWindow( gKeyName1[t],KBPOSX1,KBPOSY1,210,97 +15 );						// �E�B���h�E�t���[��
	for (i=0; i<4 ;i++)
		mh_print( KBPOSX1 +210-2-4*13 +i*13, KBPOSY1 +2, km[i], cor[i] );
	BoxFill( KBPOSX1 +1, KBPOSY1 +15, 210-2, 97, CORFL1, CORIN );				// ����
	if (x!=NULL && y!=NULL){
		for (i=0; i<4 ;i++){													// �J�[�\���̕\��
			mx = (x[i] - xx[i]) *16 * step / CURSTEP;
			my = (y[i] - yy[i]) *16 * step / CURSTEP;
			BoxFill( KBPOSX1 +1 + x[i]*16 -mx, KBPOSY1 +15 + y[i]*16 -my, 16, 16, CORFL1, cor[i] );
		}
	}
	if (t>3){																	// ���p�����̈ʒu�␳
		s = 3;
	}else{
		s = 0;
	}
	for (i=0; i<6 ;i++)
		for (j=0; j<13 ;j++){
			key[0] = gKeyTable1[t][(j*6+i)*2];
			key[1] = gKeyTable1[t][(j*6+i)*2+1];					// �L�[�}�b�v����ꕶ�����o��
			mh_print( KBPOSX1 +1 +2 + j*16 +s, KBPOSY1 +15 +2 + i*16, key, CORWCHR );
		}
	mh_print( KBPOSX1 +1 +2 + 12*16 , KBPOSY1 +15 +2 + 5*16, "SP", CORSCHR );
}


//==============================================================
// �\�t�g�E�F�A�L�[�{�[�h�̕\���i�~�T���ȕϊ��j
//--------------------------------------------------------------
// x��y���}�C�i�X�������ꍇ�̓J�[�\���͕\�����܂���B
//--------------------------------------------------------------

static void DrawKey3(int x,int y,int t)
{
	char key[3] = "  ";
	int i,j,k,s;
	int map[5][2] = {{1,0} , {0,1} , {1,1} , {2,1} , {1,2} };

	DrawWindow( gKeyName1[t],KBPOSX3,KBPOSY3,175,164 );						// �E�B���h�E�t���[��
	BoxFill(KBPOSX3 +1, KBPOSY3 +15, 175 -2, 149, CORFL1, CORIN);			// ����
	for (i=0; i<4 ;i++){													// �r��
		HLine(KBPOSX3 +1       , KBPOSY3 +15 + i*37, 149 +24, CORFL1);
		VLine(KBPOSX3 +1 + i*43, KBPOSY3 +15       , 149, CORFL1);
	}
	if (x>=0 && y>=0)
		BoxFill(KBPOSX3 +1 + x*43, KBPOSY3 +15 + y*37, 44, 38, CORFL1, CORCUR);

	if (t>3){																// ���p�����̈ʒu�␳
		s = 3;
	}else{
		s = 0;
	}
	mh_print(KBPOSX3 +1 +3*43 +2 +6, KBPOSY3 +15 +3*37+2*12+1, "space", CORSCHR);

	for (i=0; i<4 ;i++)															// �\�t�g�E�F�A�L�[�{�[�h
		for (j=0; j<4 ;j++)
			for (k=0; k<5 ;k++){
				key[0] = gKeyTable3[t][(i*20+j*5+k)*2];
				key[1] = gKeyTable3[t][(i*20+j*5+k)*2+1];						// �L�[�}�b�v����ꕶ�����o��
				mh_print(KBPOSX3 +1 + j*43 + map[k][0]*13+3 +s, KBPOSY3 +15 + i*37 + map[k][1]*12+1, key, CORWCHR);
			}
}


//==============================================================
// �ϊ����C���̈ʒu
//--------------------------------------------------------------
// gCx,gCy�Ŏw�肳��Ă���J�[�\���ʒu�ɕϊ����C����W�J�����ꍇ�̎��ۂ̍��W�ƃT�C�Y���v�Z�B
// ��ʒ[��\�t�g�L�[�ƂԂ���Ȃ�J�n�ʒu���C������B
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

	*wx = strlen(str) * FONTX;					// ������̃h�b�g��
	*x = *gCx;
	if (*x+*wx>480) *x = 480 - *wx;				// ��ʂ���͂ݏo���Ȃ�J�n�ʒu�����炷
	if (*gCy+FONTY>cy && gMode==1)				// �\�t�g�L�[�Əd�Ȃ�Ȃ�J�n�ʒu�����炷
		if (*x+*wx>cx) *x = cx - *wx -gCxw;
}


//==============================================================
// �ϊ����C���̕\��
//--------------------------------------------------------------
// blk �ϊ��Ώۂ̕�����
//--------------------------------------------------------------
// gCx,gCy�Ŏw�肳��Ă���J�[�\���ʒu�ɕϊ����C��������̕\�����s���B
// �\�t�g�L�[�Ɋ|����Ȃ��悤�Ɉʒu�␳����܂��B
// blk�Ŏw�肳�ꂽ�������������擪�����̔w�i�F��ς��܂��B
//--------------------------------------------------------------

static void InLine(char *str,int blk)
{
	int	x,wx,wx1,wx2;

	InLinePos( str,&x,&wx );									// �ϊ����C���ʒu
	if (wx){
		if (blk*FONTX >= wx){
			wx1 = wx;
			wx2 = 0;
		} else {
			wx1 = blk * FONTX;
			wx2 = wx - wx1;
		}
		if (wx1)
			BoxFill( x,*gCy,wx1,FONTY,CORCHCU,CORCHCU );		// �����w�i�i�ϊ������ʁj
		if (wx2)
			BoxFill( x+wx1,*gCy,wx2,FONTY,CORCHBK,CORCHBK );	// �����w�i�i���ϊ����ʁj
		mh_print( x,*gCy,str,CORCHCR );							// �ϊ����C��������
	}
}


//==============================================================
// �ϊ����C���̔w�i�̑Ҕ�
//--------------------------------------------------------------

static void getInLine(char *str)
{
	int	x,wx;

	InLinePos( str,&x,&wx );					// �ϊ����C���ʒu
	gInLnBuf[0] = x;
	gInLnBuf[1] = *gCy;
	BoxCopy( &gInLnBuf[2],x,*gCy,wx,FONTY );
}


//==============================================================
// �ϊ����C���̔w�i�̕���
//--------------------------------------------------------------

static void putInLine(void)
{
	if (!gBDFlag){
		if (gInLnBuf[2]){						// X����0�łȂ��Ȃ�
			BoxPaste( &gInLnBuf[2],gInLnBuf[0],gInLnBuf[1] );
		}
	}
}


//==============================================================
// �ړ����[�h���̏���
//--------------------------------------------------------------

static void move(char *str,SceCtrlData pad1)
{
	int		ch;

	ch = SIMEgetchar(pad1);
	DrawCursor(ch);								// �J�[�\���̍��
	if (ch==0x02){								// �� �i�\�t�g�L�[���[�h�ցj
		ch = 0;
		gMode = 3;								// �\�t�g�L�[���[�h�J�ڏ���
	} else {
		str[0] = ch;
		str[1] = 0;
	}
}


//==============================================================
// �V�t�gJIS�̑�P�����`�F�b�N
//--------------------------------------------------------------
// �߂�l  0:��P�����ł͂Ȃ�
//        -1:��P�����ł���
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
// ���̓��͕������𒲂ׂ�
//--------------------------------------------------------------
// �߂�l           �������̐�
// str[4][3]        ���Ɏg�p����Ă���p�x��������ʂQ�̕����Əꍇ�ɂ���Ắu�J�v
// wordList[50][33] �u��݁v��⃊�X�g
// count            �u��݁v��⃊�X�g�̌��i�ő�50�j
//--------------------------------------------------------------
// ����܂łɕϊ��s�ɓ��͂��ꂽ���������Ɏ����𒲂ׂē��͒��Ǝv������̌��
// ���擾���������͂̃A�V�X�g���s���B
// �Ђ炪�Ȃ̓��͒��̏ꍇ�ɂ̂ݓ��삵�܂��B
// ���̂�݂Ɏg����p�x�̍�����ʂQ�́u���ȁv��I�����A�܂����O�ɓ��͂����
// ����u���ȁv�ɑ��������t���\�Ȃ�X�Ɂu�J�v��I�����܂��B
//--------------------------------------------------------------

static int nextChr(char str[][3],char wordList[][33],int *count)
{
	char	sTbl1[] = "�������������������������ĂƂ͂Ђӂւ�",
			sTbl2[] = "�������������������������Âłǂ΂тԂׂ�",
			sTbl3[] = "�ς҂Ղ؂�";
	unsigned int	cr,p[2];
	int		i,hp,sp,sp2,tmp,max[2],len,crCnt,cc[85];
	long	pos;

	cr = 0;
	*count = 0;

	if (chkSJIS(gSt[0])){						// �擪���S�p�����Ȃ�
		cr = fcode(&gSt[0]);					// �擪����
		if (cr<0x829FU){						// �L���A�����A�p��Ȃ�
			return (0);
		} else if (cr<0x82F2U){					// �Ђ炪�ȂȂ�
			hp = cr - 0x829FU +1;
		} else if (cr==0x8394U){				// �u���v
			return (0);
		} else {
			return (0);							// �����������s��Ȃ�
		}
	} else {									// �擪�����p�Ȃ�
		return (0);
	}

	pos = gHEDtbl[hp];							// �������ɑΉ����鎫�����R�[�h�̊J�n�ʒu
	if (pos==0xFFFFFFFF) return (0);			// �����ɓo�^�Ȃ�

	pos += 2;
	sp2 = 0;
	len = strlen(gSt);
	for (i=0; i<85 ;i++)
		cc[i] = 0;
	while(1){
		sp = 0;
		while (gDic[pos+sp]!=0){				// ��݂ɑΉ�����P���������茟��
			if (gDic[pos+sp]!=gSt[sp]) break;
			sp++;
			if (sp==len) break;
		}
		if (sp==len){
			if (*count<50){						// �u��݁v��⃊�X�g�̎擾
				strcpy( wordList[*count],&gDic[pos] );
				(*count)++;
			}
			cr = fcode(&gDic[pos+sp]);
			if (cr==0x815B){					// �u�[�v
				cr = 84;
			} else {
				cr -= 0x829F;
			}
			if (cr<85) cc[cr]++;				// ���̕������̊Y�������J�E���g
		}
		if (sp<sp2)								// ��v���镶�������O����Z���Ȃ�����T���I��
			break;
		sp2 = sp;
		tmp = getInt(&gDic[pos-2]);				// ���̒P����ւ̑��΋����iint�j
		if (tmp==0xFFFF) break;					// �������I��������T���I��
		pos += tmp;
	}

	max[0] = max[1] = 0;
	for (i=0; i<85 ;i++)						// �o���p�x�̍����Q��T��
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
				p[i] = 0x815B;					// �u�[�v
			}
			str[i][0] = p[i] / 0x100;
			str[i][1] = p[i] & 0xFF;
			str[i][2] = '\0';
			if (strstr( sTbl2,str[i] )!=NULL)	// �u���v���u���v�ϊ�
				str[i][1]--;
			if (strstr( sTbl3,str[i] )!=NULL)	// �u�ρv���u�́v�ϊ�
				str[i][1] -= 2;
			crCnt++;
		}

	if (len>=2){
		len -= 2;								// �ϊ��s�Ō�̕���
		if (strstr( sTbl1,&gSt[len] )!=NULL){	// �O������Ƃ����t�����������͂���Ă���Ȃ�
			str[crCnt][0] = 0x81;
			str[crCnt][1] = 0x4A;				// �u�J�v
			str[crCnt][2] = '\0';
			crCnt++;
		}
	}

	return (crCnt);
}


//==============================================================
// �J�[�\�����w�蕶���ʒu�Ɉړ�������
//--------------------------------------------------------------

static void setCur(int *x,int *y,int bt,int *xx,int *yy,int reset,char wordList[][33],int *count)
{
	static int lock = 0;
	char	nchr[4][3];
	int		i,j,pos,flag,crCnt,zx,zy;
	int		dx[4] = {0,3,9,6},dy[4] = {0,3,4,2};

	for (i=0; i<4 ;i++){						// �ړ�����ۊ�
		xx[i] = x[i];
		yy[i] = y[i];
	}
	if (lock || reset){							// �J�[�\���t�H�[���[�V�����̃��Z�b�g
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

	crCnt = nextChr( nchr,wordList,count );		// ���̓��͕������𒲂ׂ�
	if (crCnt){									// ������₪����Ȃ�
		for (j=0; j<crCnt ;j++){
			pos = -1;
			for (i=0; i<78 ;i++)
				if (gKeyTable1[0][i*2]==nchr[j][0] && gKeyTable1[0][i*2+1]==nchr[j][1]){
					pos = i;					// �����̈ʒu�����
					break;
				}
			if (pos!=-1){						// �����ʒu��Ή�������W�ɕϊ�
				zx = pos / 6;
				zy = pos % 6;
				flag = 1;
				for (i=0; i<4 ;i++)				// �d���`�F�b�N
					if (x[i]==zx && y[i]==zy) flag = 0;
				if (flag){						// �d�����Ă��Ȃ��Ȃ�
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
// �ϊ��s�ɕ�����ǉ�����
//--------------------------------------------------------------
// �����ǉ����邾���ł͂Ȃ��A�������̕t�������Ȃǂ����Ă��܂��B
//--------------------------------------------------------------

static void addInline(char *str,char *si,int t)
{
	char	sdt[] = "�������������������������ĂƂ͂ЂӂւكJ�L�N�P�R�T�V�X�Z�\\�^�`�c�e�g�n�q�t�w�z",	// �u�J�v���t������
			sht[] = "�͂Ђӂւكn�q�t�w�z";																	// �u�K�v���t������
	char	cr[3] = "  ";
	int		pos,len;

	if (!chkSJIS(si[0])) si[1] = '\0';
	if (gSt[0]!=0){								// �Q�����ڈȍ~
		putInLine();							// �ϊ����C���w�i�𕜌�
		pos = strlen(gSt) - 2;
		cr[0] = gSt[pos];
		cr[1] = gSt[pos+1];
		if (strcmp( si,"�J" )==0){
			if ((unsigned char)cr[0]==0x83 &&
			    (unsigned char)cr[1]==0x45){	//�u�E�v���u���v�ϊ�
				gSt[pos+1] = 0x94;
				si[0] = 0;
				si[1] = 0;
			} else {
				if (strstr( sdt,cr )!=NULL){	// �u�J�v�̕t�������Ȃ�O�̕������C��
					gSt[pos+1]++;
					si[0] = 0;
					si[1] = 0;
				}
			}
		}else if (strcmp( si,"�K" )==0){
			if (strstr( sht,cr )!=NULL){		// �u�K�v�̕t�������Ȃ�O�̕������C��
				gSt[pos+1] += 2;
				si[0] = 0;
				si[1] = 0;
			}
		}
	} else {									// �P������
		if (t>3){								// ���p�̏ꍇ
			str[0] = si[0];						// ���̂܂܏o��
			str[1] = 0;
			si[0] = 0;
		}
	}

	if (si[1]){									// �S�p������ǉ�����ꍇ
		len = CHLEN -2;
	} else {									// ���p������ǉ�����ꍇ
		len = CHLEN -1;
	}
	if (strlen(gSt)<=len){						// �ϊ����C������������
		strcat( gSt,si );						// ������ǉ�
		getInLine(gSt);							// �ϊ����C���w�i�̎擾
	}
}


//==============================================================
// �\�t�g�L�[���[�h���̏����i�T�O���z��A�V�X�g�J�[�\���j
//--------------------------------------------------------------

static void softkey1(char *str,SceCtrlData pad1)
{
	static char	wordList[50][33];
	static int	getFlag = 0,t = 0,bt = 0,pc = 0,index = 0,count = 0,
				x[4] = {0,3,9,6},y[4] = {1,4,5,3},xx[4],yy[4];
	int		i,p,s,ch,fp,pos;
	char	si[3];

	fp = 0;												// ��ʂɕύX����������
	si[2] = 0;

	ch = SIMEgetchar(pad1);
	if (!strlen(gSt))
		if (ch==0 || ch>=32 || ch==0x02 || ch==0x08 || ch==0x0D || ch==0x1B)
			DrawCursor(ch);								// �J�[�\���\��
	switch (ch){
	case 0x03:											// L
		break;
	case 0x04:											// R
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			t++;										// ������ύX
			if (t>5) t = 0;
		} else {
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// �J�[�\���t�H�[���[�V�������f�t�H���g��
			pc = CURSTEP;
		}
		break;
	case 0x05:											// [START]
		t++;											// ������ύX
		if (t>5) t = 0;
		break;
	case 0x06:											// [SELECT]
		if (gSt[0]!=0){
			putInLine();								// �ϊ����C������U����
			if (count)
				putkList();
		}
		count = 0;
		gMode = 5;										// �ݒ��ʂ�
		break;
	case 0x02:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// �J�[�\���t�H�[���[�V�������f�t�H���g��
			pc = CURSTEP;
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// �ϊ����C������U����
				gMode = 2;								// �����ϊ�
			}
		} else {
			si[0] = gKeyTable1[t][(x[1]*6+y[1]) *2];
			si[1] = gKeyTable1[t][(x[1]*6+y[1]) *2+1];
			bt = 1;
			fp = 1;
		}
		break;
	case 0x08:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]!=0){
				putInLine();							// �ϊ����C���w�i�𕜌�
				if (count)
					putkList();
				pos = strlen(gSt);
				pos--;
				p = 0;
				while (1){								// �S�p�����␳
					s = 1 -chkSJIS(gSt[p]);
					if (p+s>pos) break;
					p += s;
				}
				gSt[p] = 0;
				if (s==2) gSt[p+1] = 0;
				getInLine(gSt);							// �ϊ����C���w�i���擾
				if (t==0){
					setCur( x,y,bt,xx,yy,0,wordList,&count );	// �J�[�\�������̕������ʒu��
				} else {
					setCur( x,y,bt,xx,yy,1,wordList,&count );	// �J�[�\�������̕������ʒu��
				}
				index = 0;
				getkList( wordList,count );
			} else {
				str[0] = ch;							// �o�b�N�X�y�[�X���o��
				DrawCursor(ch);
			}
		} else {
			si[0] = gKeyTable1[t][(x[2]*6+y[2]) *2];
			si[1] = gKeyTable1[t][(x[2]*6+y[2]) *2+1];
			bt = 2;
			fp = 1;
		}
		break;
	case 0x0D:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// �J�[�\���t�H�[���[�V�������f�t�H���g��
			pc = CURSTEP;
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				str[0] = ch;							// [Enter]���o��
				DrawCursor(ch);
			} else {
				putInLine();							// �ϊ����C���w�i�𕜌�
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
	case 0x1B:											// �~
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			setCur( x,y,bt,xx,yy,1,wordList,&count );	// �J�[�\���t�H�[���[�V�������f�t�H���g��
			pc = CURSTEP;
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// �ϊ����C���w�i�𕜌�
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
	case 0x1C:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count && count<=YOMILMAX){
					putInLine();						// �ϊ����C���w�i�𕜌�
					putkList();
					strcpy( gSt,wordList[index] );
					getInLine(gSt);						// �ϊ����C���w�i���擾
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
	case 0x1D:											// ��
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
	case 0x1E:											// ��
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
	case 0x1F:											// ��
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
	default:											// ���̑��i�����̒��ړ��́H�j
		if (ch>=32){
			si[0] = ch;
			fp = 1;
		}
		break;
	}

	if (fp){											// ������ϊ��s�ɒǉ�
		if (count)
			putkList();
		addInline( str,si,t );
		setCur( x,y,bt,xx,yy,0,wordList,&count );		// �J�[�\�������̕������ʒu��
		getkList( wordList,count );
		pc = CURSTEP;
		index = 0;
	}

	if (str[0]){										// �������o�͂���ꍇ�̓E�B���h�E���ꎞ�I�ɏ���
		putBack();
		if (count)
			putkList();
		getFlag = 1;
	} else {
		if (getFlag){									// �O��E�B���h�E���������Ă����i���C�����ō�悵�Ă邩���j�Ȃ�w�i���Ď擾
			getBack();
			getkList( wordList,count );
			getFlag = 0;
		}
		if (gMode==1){									// �����[�h�ɑJ�ڂ���Ƃ��͎��s���Ȃ�
			InLine( gSt,0 );							// �ϊ����C���\��
			if (pad1.Buttons & PSP_CTRL_LTRIGGER){
				if (count<=YOMILMAX)
					kList( wordList,count,index );		// �u���ȁv��⃊�X�g�\��
				DrawKey1( NULL,NULL,t,xx,yy,pc );		// �\�t�g�L�[�\��
			} else {
				if (count<=YOMILMAX)
					kList( wordList,count,-1 );			// �u���ȁv��⃊�X�g�\��
				DrawKey1( x,y,t,xx,yy,pc );				// �\�t�g�L�[�\��
			}
			pc--;
			if (pc<0) pc = 0;
		} else {
			putBack();									// �\�t�g�L�[������
			if (count)
				putkList();
		}
	}
}


//==============================================================
// �\�t�g�L�[���[�h���̏����i�~�T���Ȕz��j
//--------------------------------------------------------------

static void softkey3(char *str,SceCtrlData pad1)
{
	static char	wordList[50][33];
	static int	getFlag = 0,x = 0,y = 0,t = 0,index = 0,count = 0,
				xd[4],yd[4],xx[4],yy[4];
	int		i,p,s,ch,fp,pos;
	char	si[3];

	fp = 0;												// ��ʂɕύX����������
	si[2] = 0;

	ch = SIMEgetchar(pad1);
	if (!strlen(gSt))
		if (ch==0 || ch>=32 || ch==0x02 || ch==0x08 || ch==0x0D || ch==0x1B)
			DrawCursor(ch);								// �J�[�\���\��
	switch (ch){										// �J�[�\���L�[����
	case 0x02:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// �ϊ����C������U����
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
			t++;										// ������ύX
			if (t>5) t = 0;
		} else {
			si[0] = gKeyTable3[t][(y*20+x*5+2) *2];
			si[1] = gKeyTable3[t][(y*20+x*5+2) *2+1];
			fp = 1;
		}
		break;
	case 0x05:											// [START]
		t++;											// ������ύX
		if (t>5) t = 0;
		break;
	case 0x06:											// [SELECT]
		if (gSt[0]!=0){
			putInLine();								// �ϊ����C������U����
			if (count)
				putkList();
		}
		count = 0;
		gMode = 5;										// �ݒ��ʂ�
		break;
	case 0x08:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]!=0){
				putInLine();							// �ϊ����C���w�i�𕜌�
				if (count)
					putkList();
				pos = strlen(gSt);
				pos--;
				p = 0;
				while (1){								// �S�p�����␳
					s = 1 -chkSJIS(gSt[p]);
					if (p+s>pos) break;
					p += s;
				}
				gSt[p] = 0;
				if (s==2) gSt[p+1] = 0;
				getInLine(gSt);							// �ϊ����C���w�i���擾
				if (t==0){
					setCur( xd,yd,0,xx,yy,0,wordList,&count );	// �J�[�\�������̕������ʒu��
				} else {
					setCur( xd,yd,0,xx,yy,1,wordList,&count );	// �J�[�\�������̕������ʒu��
				}
				index = 0;
				getkList( wordList,count );
			} else {
				str[0] = ch;							// �o�b�N�X�y�[�X���o��
				DrawCursor(ch);
			}
		} else {
			si[0] = gKeyTable3[t][(y*20+x*5+1) *2];
			si[1] = gKeyTable3[t][(y*20+x*5+1) *2+1];
			fp = 1;
		}
		break;
	case 0x0D:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				str[0] = ch;							// [Enter]���o��
				DrawCursor(ch);
			} else {
				putInLine();							// �ϊ����C���w�i�𕜌�
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
	case 0x1B:											// �~
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (count)
				putkList();
			count = 0;
			if (gSt[0]==0){
				gMode = 0;
			} else {
				putInLine();							// �ϊ����C���w�i�𕜌�
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
	case 0x1C:											// ��
		if (pad1.Buttons & PSP_CTRL_LTRIGGER){
			if (gSt[0]==0){
				str[0] = ch;
				DrawCursor(ch);
			} else {
				if (count && count<=YOMILMAX){
					putInLine();						// �ϊ����C���w�i�𕜌�
					putkList();
					strcpy( gSt,wordList[index] );
					getInLine(gSt);						// �ϊ����C���w�i���擾
					count = 0;
				}
			}
		} else {
			x++;
			if (x>3) x = 0;
		}
		break;
	case 0x1D:											// ��
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
	case 0x1E:											// ��
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
	case 0x1F:											// ��
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
	default:											// ���̑��i�����̒��ړ��́H�j
		if (ch>=32){
			si[0] = ch;
			fp = 1;
		}
		break;
	}

	if (fp){											// ������ϊ��s�ɒǉ�
		if (count)
			putkList();
		addInline( str,si,t );
		setCur( xd,yd,0,xx,yy,0,wordList,&count );		// �u��݁v��⃊�X�g�̎擾�̂ݎg�p
		getkList( wordList,count );
		index = 0;
	}

	if (str[0]){										// �������o�͂���ꍇ�̓E�B���h�E���ꎞ�I�ɏ���
		putBack();
		if (count)
			putkList();
		getFlag = 1;
	} else {
		if (getFlag){									// �O��E�B���h�E���������Ă����Ȃ�w�i���Ď擾
			getBack();
			getkList( wordList,count );
			getFlag = 0;
		}
		if (gMode==1){									// �����[�h�ɑJ�ڂ���Ƃ��͎��s���Ȃ�
			InLine( gSt,0 );							// �ϊ����C���\��
			if (pad1.Buttons & PSP_CTRL_LTRIGGER){
				if (count<=YOMILMAX)
					kList( wordList,count,index );		// �u���ȁv��⃊�X�g�\��
				DrawKey3( -1,-1,t );					// �\�t�g�L�[�\���i�J�[�\���Ȃ��j
			} else {
				if (count<=YOMILMAX)
					kList( wordList,count,-1 );			// �u���ȁv��⃊�X�g�\��
				DrawKey3( x,y,t );						// �\�t�g�L�[�\��
			}
		} else {
			putBack();									// �\�t�g�L�[������
			if (count)
				putkList();
		}
	}
}


//==============================================================
// �\�t�g�L�[���[�h���̏���
//--------------------------------------------------------------
// �Z���N�g����Ă���e�z��̏�����ɕ���
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
// char�񂩂�S�p�����R�[�h���擾
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
// char�񂩂�int�f�[�^���擾
//--------------------------------------------------------------

static unsigned int getInt(char *str)
{
	return ((unsigned char)str[0] + (unsigned char)str[1] * 0x100);
}


//==============================================================
// �S�p������ʂ̐؂蕪��
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
		} else {								// ���p���������瑦�I��
			break;
		}
		if (cr<ar1 || cr>ar2)
			break;								// �͈͊O�̕�������������I��
	}
	return (pos);
}


//==============================================================
// �����ϊ�����
//--------------------------------------------------------------
// len      �ϊ��ΏۂƂȂ镶�ߒ��i�o�C�g�j
// adjust      0:���ߒ��͎w��l�ɋ����Œ�
//          �ȊO:���ߒ��͎w��l���Z���ύX�����ꍇ����
// count    �P����̐�
// wordList �P�ꃊ�X�g�ichar wordList[50][33]�̔z����w�肷�邱�Ɓj
//
// �߂�l   �T�����s�����u��݁v�̒����i�o�C�g�j
//          count   :�P�ꃊ�X�g�̌����i�P�`�j
//          wordList:�P�ꃊ�X�g
//--------------------------------------------------------------
// gSt�̓ǂ݂����Ɏ��������������������擾����B
// wordList�̍Ō�ɂ͒T�����s�������ߒ��́u��݁v���̂��̂������Ă���B
// �����Ɉ�v������̂����������ꍇ�ł��Œ�ł��u��݁v�����͕Ԃ���܂��B
//--------------------------------------------------------------

static int setkanji(int len,int adjust,int *count,char wordList[][33])
{
	unsigned int	cr;
	const char		kazu[21] = {"�Z���O�l�ܘZ������"};
	int		i,j,hp,sp,sp2,sp3,tmp,sjis;
	long	pos,pos2;

	*count = 0;
	cr = 0;
	gDicWordPos = -1;

	if (chkSJIS(gSt[0])){						// �擪���S�p�����Ȃ�
		cr = fcode(&gSt[0]);					// �擪����
		if (cr<0x8200U){						// �L���Ȃ�
			hp = 0;
		} else if (cr<0x829FU){					// �����ƃA���t�@�x�b�g
			hp = -3;
		} else if (cr<0x82F2U){					// �Ђ炪�ȂȂ�
			hp = cr - 0x829FU +1;
		} else if (cr==0x8394U){				// �u���v
			hp = 84;
		} else {
			hp = -1;							// �����������s��Ȃ�
		}
	} else {									// �擪�����p�Ȃ�
		hp = -2;
	}

	if (hp<0){											// �����������s��Ȃ��ꍇ�i�����őΉ����Ă��Ȃ��擪�����j
		if (adjust){
			if (hp==-2){								// ���p�̕�����I��
				for (sp=0; sp<strlen(gSt) ;sp++)
					if (chkSJIS(gSt[sp])) break;
			} else {									// ����̑S�p�����^�C�v���ɂ܂Ƃ߂�
				if (cr<0x8259U){						// ����
					sp = codeChk( gSt,0x824F,0x8258 );
				} else if (cr<0x829BU){					// �A���t�@�x�b�g
					sp = codeChk( gSt,0x8260,0x829A );
				} else if (cr<0x8397U){					// �J�^�J�i
					sp = codeChk( gSt,0x8340,0x8396 );
				} else if (cr<0x83D7U){					// �M���V��
					sp = codeChk( gSt,0x839F,0x83D6 );
				} else if (cr<0x8492U){					// ���V�A
					sp = codeChk( gSt,0x8440,0x8491 );
				} else {								// �L��
					sp = codeChk( gSt,0x84A0,0x879F );
				}
			}
		} else {
			sp = len;									// �����^�C�v�͖���
		}

	} else {												// �����ϊ����s���ꍇ
		pos = gHEDtbl[hp];									// �������ɑΉ����鎫�����R�[�h�̊J�n�ʒu
		if (pos==0xFFFFFFFF){								// �����ɓo�^�Ȃ�
			sp = 2;
		} else {											// �����ɐ擪�������o�^����Ă���ꍇ
			pos += 2;
			sp2 = sp3 = 0;
			pos2 = -1;
			while(1){
				sp = 0;
				while (gDic[pos+sp]!=0){					// ��݂ɑΉ�����P���������茟��
					if (gDic[pos+sp]!=gSt[sp]) break;
					sp++;
					if (sp==len) break;
				}
				if (gDic[pos+sp]==0){						// �P���┭���i�����������ƒ�����v������̂����邩������Ȃ��̂ŒT���͏I���Ȃ��j
					pos2 = pos;
					sp3 = sp;
				}
				if (hp==0){									// �L���̏ꍇ�̏I������
					if (fcode(&gDic[pos])>0x829E)
						break;
				} else {									// �Ђ炪�Ȃ̏ꍇ�̏I������
					if (sp<sp2)								// ��v���镶�������O����Z���Ȃ�����T���I��
						break;
				}
				sp2 = sp;
				tmp = getInt(&gDic[pos-2]);					// ���̒P����ւ̑��΋����iint�j
				if (tmp==0xFFFF) break;						// �������I��������T���I��
				pos += tmp;
			}

			if (pos2==-1){						// �����ɍڂ��Ă��Ȃ��i�L����ϊ����悤�Ƃ����ꍇ�ɋN���肦��j
				sp = 2;
			} else if (adjust==0 && sp3!=len){	// �w�肳�ꂽ���ߒ��ɑ���Ȃ�
				sp = len;
			} else {							// �P�ꃊ�X�g����荞��
				sp = sp3;
				pos = pos2 + strlen(&gDic[pos2]) +1;
				gDicWordPos = pos;				// ��Ŏ����L�ڏ������ւ��邽�߂�
				*count = gDic[pos++];			// �P�ꐔ�ibyte�j
				if (*count>49) *count = 49;		// �P�ꐔ�����i�o�b�t�@�I�[�o�[�t���[�΍�j
				for (i=0; i<*count ;i++){		// �P���荞��
					tmp = strlen(&gDic[pos]);
					if (tmp>32) tmp = 32;		// �P��̒��������i�o�b�t�@�I�[�o�[�t���[�΍�j
					sjis = 0;
					for (j=0; j<tmp ;j++){
						wordList[i][j] = gDic[pos+j];
						if (sjis){				// ���������Ɉ������������ꍇ�ɃS�~���c�鋰�ꂪ����̂�
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

	if ((hp>0 || hp==-3) && !*count){			// ��₪�����ꍇ
		tmp = 0;
		for (i=0; i<sp ;i+=2){
			cr = fcode(&gSt[i]);
			if (cr>=0x824F && cr<0x8259){		// ��������������
				wordList[*count][i  ] = kazu[(cr-0x824F)*2  ];
				wordList[*count][i+1] = kazu[(cr-0x824F)*2+1];
				tmp = 1;
			} else if (cr>=0x829F && cr<0x82F2){
				if (cr>=0x82DE) cr++;			// sJIS�R�[�h�́u�~�v�Ɓu���v�̊Ԃɂ���󗓂͉����낤�H
				cr += 161;						// �Ђ炪�ȁ��J�^�J�i
				wordList[*count][i  ] = cr >> 8;
				wordList[*count][i+1] = cr & 0x00FF;
				tmp = 1;
			}
		}
		if (tmp){								// �R�[�h�ϊ����s���Ă����ꍇ��
			wordList[*count][i] = '\0';
			(*count)++;
		}
	}

	for (i=0; i<sp ;i++)						// �Ō�Ɂu��݁v���̂��̂�P�ꃊ�X�g�ɒǉ�
		wordList[*count][i] = gSt[i];
	wordList[*count][i] = '\0';
	(*count)++;

	return (sp);
}


//==============================================================
// ������⃊�X�g�̈ʒu
//--------------------------------------------------------------
// �ϊ����C���̉�����Ɋ�����⃊�X�g�̈ʒu��ݒ肷��B
// ��ʒ[�ƂԂ���Ȃ������ɐݒ肵�܂��B
//--------------------------------------------------------------

static void kListPos(int *x,int *y,int *wx,int *wy,char wordList[][33],int count)
{
	int i,len,max,xl,yl,xx;

	InLinePos( gSt,x,wx );						// x���W�擾
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

	if (*x+*wx>480) *x = 480 - *wx;				// ��ʂ���͂ݏo���Ȃ�J�n�ʒu�����炷
	*y = *gCy + FONTY+1;
	if (*y+*wy>272) *y = *gCy -1 - *wy;

	if (gMode==1){								// �\�t�g�L�[���[�h�Ȃ�\�t�g�L�[���������
		if (gKey){
			xl = KBPOSX3;
			yl = KBPOSY3;
		} else {
			xl = KBPOSX1;
			yl = KBPOSY1;
		}
		if (*x+*wx>=xl && *y+*wy>=yl){			// �\�t�g�L�[�ɏd�Ȃ�
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
// ������⃊�X�g�̕\��
//--------------------------------------------------------------
// gCx,gCy�Ŏw�肳��Ă���J�[�\���ʒu�̉�����Ɋ�����⃊�X�g�̕\�����s���B
// index �� -1 �̂Ƃ��̓J�[�\����\�����Ȃ��B
//--------------------------------------------------------------

static void kList(char wordList[][33],int count,int index)
{
	int	i,page,len,pos,n,x,y,wx,wy,df;

	if (!count) return;

	kListPos( &x, &y, &wx, &wy, wordList, count );		// ������⃊�X�g�ʒu
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
	if (len>wy-2) len = wy -2;							// �X�N���[���o�[�̃T�C�Y�␳
	if (pos+len>wy-2) pos = wy-2 -len;					// �X�N���[���o�[�̈ʒu�␳
	for (i=0; i<4 ;i++)									// �X�N���[���o�[���
		VLine( x + wx-2-i, y+1 +pos, len, CORRBAR );

	pos = index % KLIST;
	y += 1;
	x += 1;
	if (df)
		BoxFill( x, y +pos *(FONTY+1), wx-2-4-1, FONTY, CORCUR, CORCUR );
	n = page * KLIST;
	for (i=0; i<KLIST ;i++){
		if (n>=count) break;
		mh_print( x+2,y,wordList[n],CORWCHR );			// �ϊ����C��������
		y += FONTY +1;
		n++;
	}
}


//==============================================================
// ������⃊�X�g�̔w�i�̑Ҕ�
//--------------------------------------------------------------

static void getkList(char wordList[][33],int count)
{
	int	x,y,wx,wy;

	if (!count){
		gkListBuf[0] = -1;
		return;
	}

	kListPos( &x, &y, &wx, &wy, wordList, count );		// ������⃊�X�g�ʒu
	gkListBuf[0] = x;
	gkListBuf[1] = y;
	BoxCopy( &gkListBuf[2],x,y,wx,wy );
}


//==============================================================
// ������⃊�X�g�̔w�i�̕���
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
// �����L�ڏ��̓���ւ�
//--------------------------------------------------------------

static void dicWordEx(int index)
{
	long	pos,pos2;
	char	str[65];
	int		i,n,len;

	if (gDicWordPos==-1) return;				// �ʒu���w�肳��Ă��Ȃ�
	if (gDic[gDicWordPos]<(index+1)) return;	// �o�^�ꐔ�ȏ���w�肵�Ă���
	if (!index) return;							// ���ɐ擪�ʒu�ɂ���

	gSaveFlag = -1;

	pos = gDicWordPos +1;
	pos2 = pos;
	n = 0;
	while (n<index){							// �ڕW���̈ʒu��T��
		pos += strlen(&gDic[pos]) +1;
		n++;
	}
	strcpy(str,&gDic[pos]);						// �擪�Ɉړ����������Ҕ�
	len = strlen(&gDic[pos]) +1;				// �ړ�������T�C�Y
	n = pos - pos2;
	pos--;
	for (i=n; i>0 ;i--){						// ���̑��̌����ړ�
		gDic[pos+len] = gDic[pos];
		pos--;
	}
	strcpy(&gDic[pos2],str);					// �ڕW����擪�ɏ����߂�
}


//==============================================================
// �����I�����[�h
//--------------------------------------------------------------

static void change(char *str,SceCtrlData pad1)
{
	static char	wordList[50][33],ILstr[64];
	static int	mode = 0,count,len,index,getFlag = 0;
	int	l,p,s,ch,df;

	if (!mode){									// �ϊ��J�n����̏����ݒ�
		len = setkanji( strlen(gSt),1,&count,wordList );
		index = 0;
		mode = 1;
		strcpy( ILstr,wordList[index] );		// �ϊ��Ώ�
		strcat( ILstr,&gSt[len] );				// ���ϊ��Ώ�
		getInLine(ILstr);						// ������Ԃ̕ϊ����C���w�i���擾
		getkList( wordList,count );
	}

	df = 0;

	ch = SIMEgetchar(pad1);
	switch (mode){
	case 1:										// �����I�����[�h
		switch (ch){
		case 0x03:								// L
			putkList();							// ������⃊�X�g������
			if (index==0){
				index = count -1;
			} else {
				index -= KLIST;
				if (index<0) index = 0;
			}
			df = 1;
			break;
		case 0x04:								// R
			putkList();							// ������⃊�X�g������
			if (index==count-1){
				index = 0;
			} else {
				index += KLIST;
				if (index>count-1) index = count -1;
			}
			df = 1;
			break;
		case 0x0D:								// ��
			strcpy( str,wordList[index] );
			strcpy( gSt,&gSt[len] );
			if (strlen(gSt)==0){				// �u��݁v���S�Ė����Ȃ�����\�t�g�L�[���[�h��
				gMode = 3;
			}
			if ((count-1)!=index)				// ��⃊�X�g�̍Ō�̌��́u��݁v�Ȃ̂œ���ւ������͍s��Ȃ�
				dicWordEx(index);
			mode = 0;
			break;
		case 0x08:								// ��
		case 0x1B:								// �~
			putInLine();						// �ϊ����C��������
			putkList();							// ������⃊�X�g������
			mode = 0;
			gMode = 3;							// �\�t�g�L�[���[�h��
			break;
		case 0x1C:								// ��
			putInLine();						// �ϊ����C��������
			putkList();							// ������⃊�X�g������
			len += 1 -chkSJIS(gSt[len]);		// �S�p�����␳
			l = strlen(gSt);
			if (len>l) len = l;
			strcpy( ILstr,gSt );
			getInLine(ILstr);
			getkList( wordList,count );
			mode = 2;
			break;
		case 0x1D:								// ��
			putInLine();						// �ϊ����C��������
			putkList();							// ������⃊�X�g������
			len--;
			if (len<1) len = 1;
			p = 0;
			while (1){							// �S�p�����␳
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
		case 0x1E:								// ��
			putkList();							// ������⃊�X�g������
			index--;
			if (index<0) index = count -1;
			df = 1;
			break;
		case 0x1F:								// ��
		case 0x02:								// ��
			putkList();							// ������⃊�X�g������
			index++;
			if (index>count-1) index = 0;
			df = 1;
			break;
		}
		break;
	
	case 2:										// ���ߒ��I�����[�h
		switch (ch){
		case 0x03:								// L
			break;
		case 0x04:								// R
			break;
		case 0x0D:								// ��
			strncpy( str,gSt,len );
			str[len] = '\0';
			strcpy( gSt,&gSt[len] );
			if (strlen(gSt)==0){				// �u��݁v���S�Ė����Ȃ�����\�t�g�L�[���[�h��
				putInLine();					// �ϊ����C��������
				gMode = 3;
			}
			mode = 0;
			break;
		case 0x08:								// ��
		case 0x1B:								// �~
			putInLine();						// �ϊ����C��������
			mode = 0;
			gMode = 3;							// �\�t�g�L�[���[�h��
			break;
		case 0x1C:								// ��
			len += 1 -chkSJIS(gSt[len]);		// �S�p�����␳
			l = strlen(gSt);
			if (len>l) len = l;
			break;
		case 0x1D:								// ��
			len--;
			if (len<1) len = 1;
			p = 0;
			while (1){							// �S�p�����␳
				s = 1 -chkSJIS(gSt[p]);
				if (p+s>len) break;
				p += s;
			}
			if (p==0) p = 2;
			len = p;
			break;
		case 0x1E:								// ��
			len = setkanji( len,0,&count,wordList );
			index = count -1;
			mode = 1;
			df = 1;
			break;
		case 0x1F:								// ��
		case 0x02:								// ��
			len = setkanji( len,0,&count,wordList );
			index = 0;
			mode = 1;
			df = 1;
			break;
		}
		break;
	}

	if (df){									// �ϊ����ɕω���������
		putInLine();							// �ϊ����C��������
		strcpy( ILstr,wordList[index] );		// �ϊ��Ώ�
		strcat( ILstr,&gSt[len] );				// ���ϊ��Ώ�
		getInLine(ILstr);
		getkList( wordList,count );
	}

	if (str[0]){								// �������o�͂���ꍇ�̓E�B���h�E���ꎞ�I�ɏ���
		putInLine();							// �ϊ����C��������
		putkList();								// ������⃊�X�g������
		getFlag = 1;
	} else {
		if (getFlag){							// �O��E�B���h�E���������Ă����Ȃ�w�i���Ď擾
			getInLine(ILstr);
			getkList( wordList,count );
			getFlag = 0;
		}
		if (gMode==2){							// �\�t�g�L�[���[�h�ɑJ�ڂ���Ƃ��͎��s���Ȃ�
			switch (mode){						// �ϊ����C����\��
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
// �ݒ�E�B���h�E�̍��
//--------------------------------------------------------------

static void DrawMenu(int pos,char *strKey,char *strSave)
{
	DrawWindow( SIMENAME,MEPOSX,MEPOSY,218,164 );							// �E�B���h�E�t���[��
	BoxFill( MEPOSX +1, MEPOSY +15, 218-2, 149, CORFL1, CORFR );			// ����

	mh_print( MEPOSX +4 +12 , MEPOSY +15 +12 , "�g�p���Ă��鎫��", CORFCHR );
	DrawTextbox( gDicFile,MEPOSX +4 +30,MEPOSY +15 +28,162,CORFCHR,CORFL1 );

	if (pos==0)
		BoxFill( MEPOSX +4 +12 -1, MEPOSY +15 +60, 96 +2, 12, CORCUR, CORCUR );
	mh_print( MEPOSX +4 +12 , MEPOSY +15 +60 , "�\\�t�g�L�[�̎��", CORFCHR );
	DrawTextbox( strKey,MEPOSX +4 +84,MEPOSY +15 +78,108,CORWCHR,CORIN );

	if (pos==1)
		BoxFill( MEPOSX +4 +12 -1, MEPOSY +15 +108, 180 +2, 12, CORCUR, CORCUR );
	mh_print( MEPOSX +4 +12 , MEPOSY +15 +108 , "�I�����Ɏ����̋L�ڏ���ۑ�����", CORFCHR );
	DrawTextbox( strSave,MEPOSX +4 +144,MEPOSY +15 +126,48,CORWCHR,CORIN );
}


//==============================================================
// �ݒ�E�B���h�E�w�i�̑Ҕ�
//--------------------------------------------------------------

static void getMenu()
{
	gBackBuf[0] = MEPOSX;
	gBackBuf[1] = MEPOSY;
	BoxCopy( &gBackBuf[2],MEPOSX,MEPOSY,218,166 );
}


//==============================================================
// �e��ݒ�
//--------------------------------------------------------------

static void menu(SceCtrlData pad1)
{
	static int pos = 0,mode = 0,item[2];
	char	strKey[][21] = {"�T�O���z��AC","�~�T���Ȕz�u"},
			strSave[][7] = {"����","���Ȃ�"};
	int ch;

	ch = SIMEgetchar(pad1);
	switch (mode){
	case 0:										// �����ݒ�
		item[0] = gKey;
		item[1] = gSave;
		pos = 0;
		mode = 1;
		break;

	case 1:										// �����I�����[�h
		switch (ch){
		case 0x03:								// L
			break;
		case 0x04:								// R
			break;
		case 0x0D:								// ��
			if (gKey!=item[0] || gSave!=item[1])
				gIni = 1;						// ���f�[�^���ύX���ꂽ
			gKey = item[0];
			gSave = item[1];
			mode = 0;
			gMode = 3;							// �\�t�g�L�[���[�h��
			break;
		case 0x08:								// ��
			break;
		case 0x1B:								// �~
			mode = 0;
			gMode = 3;							// �\�t�g�L�[���[�h��
			break;
		case 0x1C:								// ��
			if (pos==0){
				item[pos]++;
				if (item[pos]>1) item[pos] = 1;
			} else {
				item[pos] = 1;
			}
			break;
		case 0x1D:								// ��
			item[pos]--;
			if (item[pos]<0) item[pos] = 0;
			break;
		case 0x1E:								// ��
			pos = 0;
			break;
		case 0x1F:								// ��
			pos = 1;
			break;
		case 0x02:								// ��
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
// ���������
//--------------------------------------------------------------
// *str   ���͂��ꂽ�����i�ő�33�o�C�g asciiz�j
// pad1   �p�b�h���
// �߂�l str���̂���
//--------------------------------------------------------------
// �\�t�g�L�[�{�[�h�ɂ�镶����̓��́B
// ���ȉp�������IME�ɂ�銿���ϊ��������s���B
// ���������łȂ��J�[�\���L�[���̊e�{�^�����R���g���[���R�[�h�Ƃ��ĕԂ��B
// ���͕�����́i�m�肳�ꂽ����������ꍇ�́jstr�ɓ����Ă��܂��B
//--------------------------------------------------------------

char *SIMEgets(char *str,SceCtrlData pad1)
{
	int i;

	for (i=0; i<CHLEN ;i++)
		str[i] = 0;

	switch (gMode){
	case 0:										// �ړ����[�h
		move( str,pad1 );
		break;

	case 1:										// �L�[�{�[�h���[�h
		keyboard( str,pad1 );
		break;

	case 2:										// �����I�����[�h
		change( str,pad1 );
		break;

	case 3:										// �\�t�g�L�[���[�h�ɑJ�ڂ���
		gMode = 1;
		getInLine(gSt);
		getBack();
		keyboard( str,pad1 );
		break;

	case 4:										// �e��ݒ���
		menu(pad1);
		break;

	case 5:										// �ݒ��ʂɑJ�ڂ���
		gMode = 4;
		getMenu();
		menu(pad1);
		break;

	}
	return (str);
}




