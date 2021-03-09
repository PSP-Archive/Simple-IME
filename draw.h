#ifndef DRAW_H
#define DRAW_H

#define SCRBUF0		(char*)(0)

/*キャッシュなしVRAM  VRAM[y][x]としてアクセス*/
#define VRAM		((long(*)[512])(((char*)0x4000000)+0x40000000))

void	HLine(int x,int y,int wx,long cor);
void	VLine(int x,int y,int wy,long cor);
void	Box(int x,int y,int wx,int wy,long corFrame);
void	BoxFill(int x,int y,int wx,int wy,long corFrame,long corIn);
void	CurveBox(int x,int y,int wx,int wy,long corIn);
void	BoxClr(int x,int y,int xw,int yw);
void	BoxCopy(long *buf,int x,int y,int wx,int wy);
void	BoxPaste(long *buf,int x,int y);

#endif
