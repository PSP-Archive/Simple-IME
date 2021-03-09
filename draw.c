//==============================================================
// ��ʍ��֘A
// STEAR 2009
//--------------------------------------------------------------

#include "draw.h"

//==============================================================
// �����̍��
//--------------------------------------------------------------

void HLine(int x,int y,int wx,long cor)
{
	int i;

	for (i=0; i<wx ;i++)
		VRAM[y][x+i] = cor;
}

//==============================================================
// �c���̍��
//--------------------------------------------------------------

void VLine(int x,int y,int wy,long cor)
{
	int i;

	for (i=0; i<wy ;i++)
		VRAM[y+i][x] = cor;
}

//==============================================================
// BOX�̍��
//--------------------------------------------------------------

void Box(int x,int y,int wx,int wy,long corFrame)
{
	HLine(x       ,y       ,wx,corFrame);
	VLine(x       ,y       ,wy,corFrame);
	VLine(x + wx-1,y       ,wy,corFrame);
	HLine(x       ,y + wy-1,wx,corFrame);
}

//==============================================================
// �h��Ԃ��ꂽBOX�̍��
//--------------------------------------------------------------

void BoxFill(int x,int y,int wx,int wy,long corFrame,long corIn)
{
	int	i;

	for (i=0; i<wy ;i++)										// ���g��h��Ԃ�
		HLine(x,y + i,wx,corIn);
	Box(x,y,wx,wy,corFrame);
}

//==============================================================
// �p�ۂ�BOX�̍��
//--------------------------------------------------------------

void CurveBox(int x,int y,int wx,int wy,long corIn)
{
	const int	rx[4] = {4,2,1,1};
	int	i;

	for (i=0; i<wy ;i++){
		if (i<4){
			HLine(x+rx[i],y + i,wx-rx[i]*2,corIn);
		} else if (i>=wy-4){
			HLine(x+rx[wy-i-1],y + i,wx-rx[wy-i-1]*2,corIn);
		} else {
			HLine(x,y + i,wx,corIn);
		}
	}
}

//==============================================================
// �w��̈�̃N���A
//--------------------------------------------------------------

void BoxClr(int x,int y,int wx,int wy)
{
	int i;

	for (i=0; i<wy ;i++)
		HLine(x,y + i,wx,0);
}

//==============================================================
// �w��̈�̃R�s�[
//--------------------------------------------------------------
// �w�肳�ꂽ�̈��buf�Ɏ�荞��
// buf�͗\�ߕK�v��+2���m�ۂ��Ă����Ă��������B
//--------------------------------------------------------------

void BoxCopy(long *buf,int x,int y,int wx,int wy)
{
	int i,j;

	buf[0] = wx;
	buf[1] = wy;
	for (i=0; i<wy ;i++)
		for (j=0; j<wx ;j++)
			buf[2+i*wx+j] = VRAM[y+i][x+j];
}

//==============================================================
// �w��̈�̃y�[�X�g
//--------------------------------------------------------------
// �w�肳�ꂽ�ʒu��buf��W�J����B
//--------------------------------------------------------------

void BoxPaste(long *buf,int x,int y)
{
	int i,j;

	for (i=0; i<buf[1] ;i++)
		for (j=0; j<buf[0] ;j++)
			VRAM[y+i][x+j] = buf[2+i*buf[0]+j];
}
