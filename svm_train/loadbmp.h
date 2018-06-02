#include <stdio.h>  
#include <math.h>  
#include <time.h>  
#include <string.h>  
#include <stdlib.h>  
#include <windows.h> // for use BITMAPFILEHEADER / BITMAPINFOHEADER

#define pi (double)3.14159265359  

//#define BI_RGB       0  
//#define BI_RLE8      1  
//#define BI_RLE4      2  
//#define BI_BITFIELDS 3  


typedef unsigned long       DWORD;  
typedef int                 BOOL;  
typedef unsigned char       BYTE;  
typedef unsigned short      WORD;  
typedef float               FLOAT;  
typedef int                 INT;  
typedef unsigned int        UINT;  
typedef char CHAR;  
typedef short SHORT;  
typedef long LONG;  


#pragma   pack(push)  
#pragma   pack(1)  
/*typedef struct tagBITMAPINFOHEADER{  
	DWORD      biSize;  
	LONG       biWidth;  
	LONG       biHeight;  
	WORD       biPlanes;  
	WORD       biBitCount;  
	DWORD      biCompression;  
	DWORD      biSizeImage;  
	LONG       biXPelsPerMeter;  
	LONG       biYPelsPerMeter;  
	DWORD      biClrUsed;  
	DWORD      biClrImportant;  
} BITMAPINFOHEADER;*/  
#pragma     pack(pop)  

#pragma   pack(push)  
#pragma   pack(1)  
/*typedef struct tagBITMAPFILEHEADER {  
	WORD    bfType;  
	DWORD   bfSize;  
	WORD    bfReserved1;  
	WORD    bfReserved2;  
	DWORD   bfOffBits;  
} BITMAPFILEHEADER; */ 
#pragma     pack(pop)  

#pragma   pack(push)  
#pragma   pack(1)  
/*typedef struct tagRGBQUAD {  
	BYTE    rgbBlue;   
	BYTE    rgbGreen;   
	BYTE    rgbRed;   
	BYTE    rgbReserved;   
} RGBQUAD;*/   
#pragma     pack(pop)  


int nWidth;  
int nHeight;  
int nLen;  
int nByteWidth;  
BYTE *lpBackup;  
BYTE *lpBitmap;  
BYTE *lpBits;  

/*复数定义*/  
typedef struct  
{  
	double re;  
	double im;  
}COMPLEX;  

/*复数加运算*/  
COMPLEX Add(COMPLEX c1, COMPLEX c2)  
{  
	COMPLEX c;  
	c.re=c1.re+c2.re;  
	c.im=c1.im+c2.im;  
	return c;  
}  

/*复数减运算*/  
COMPLEX Sub(COMPLEX c1, COMPLEX c2)  
{  
	COMPLEX c;  
	c.re=c1.re-c2.re;  
	c.im=c1.im-c2.im;  
	return c;  
}  

/*复数乘运算*/  
COMPLEX Mul(COMPLEX c1, COMPLEX c2)  
{  
	COMPLEX c;  
	c.re=c1.re*c2.re-c1.im*c2.im;  
	c.im=c1.re*c2.im+c2.re*c1.im;  
	return c;  
}  

void GetPoints(BYTE *lpPoints)  
{  
	int x,y,p;  
	for(y=0;y<nHeight;y++)  
	{  
		for(x=0;x<nWidth;x++)  
		{  
			p=x*3+y*nByteWidth;  
			lpPoints[x+y*nWidth]=(BYTE)(0.299*(float)lpBits[p+2]+0.587*(float)lpBits[p+1]+0.114*(float)lpBits[p]+0.1);  
		}  
	}  
}  

void PutPoints(BYTE *lpPoints)  
{  
	int x,y,p,p1;  
	for(y=0;y<nHeight;y++)  
	{  
		for(x=0;x<nWidth;x++)  
		{  
			p=x*3+y*nByteWidth;  
			p1=x+y*nWidth;  
			lpBits[p]=lpPoints[p1];  
			lpBits[p+1]=lpPoints[p1];  
			lpBits[p+2]=lpPoints[p1];  
		}  
	}  
}  


void LoadBitmap()  
{   int nBWidth;  
	BITMAPINFOHEADER *pInfo;  
	unsigned int PaletteSize;  
	RGBQUAD *pPalette;  

	BITMAPFILEHEADER bmh;  
	BITMAPINFOHEADER bmi;  
	BYTE *lpTemp=lpBitmap;  
	BYTE *lpBits2;  
	int x,y,p1,p2,Palette;  


	pInfo=(BITMAPINFOHEADER *)(lpBitmap+sizeof(BITMAPFILEHEADER));  
	nWidth=pInfo->biWidth;  
	nByteWidth=nWidth*3;  
	if (nByteWidth%4) nByteWidth+=4-(nByteWidth%4);  
	nHeight=pInfo->biHeight;  
	if (pInfo->biBitCount!=24)  
	{  
		if (pInfo->biBitCount!=8)  
		{  
			printf("Invalidation Bitmap");  
			free(lpBitmap);  
			lpBitmap=0;  
			return;  
		}  
		PaletteSize=1<<pInfo->biBitCount;  
		if (pInfo->biClrUsed!=0 && pInfo->biClrUsed<PaletteSize) PaletteSize=pInfo->biClrUsed;  
		lpBits=lpBitmap+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);  
		pPalette=(RGBQUAD *)lpBits;  
		lpBits+=sizeof(RGBQUAD)*PaletteSize;  
		nLen=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nByteWidth*nHeight;  
		lpTemp=lpBitmap;  
		lpBitmap=(BYTE*)malloc(sizeof(BYTE)*nLen);  

		bmh.bfType='B'+'M'*256;  
		bmh.bfSize=nLen;  
		bmh.bfReserved1=0;  
		bmh.bfReserved2=0;  
		bmh.bfOffBits=54;  
		bmi.biSize=sizeof(BITMAPINFOHEADER);  
		bmi.biWidth=nWidth;  
		bmi.biHeight=nHeight;  
		bmi.biPlanes=1;  
		bmi.biBitCount=24;  
		bmi.biCompression=BI_RGB;  
		bmi.biSizeImage=0;  
		bmi.biXPelsPerMeter=0;  
		bmi.biYPelsPerMeter=0;  
		bmi.biClrUsed=0;  
		bmi.biClrImportant=0;  

		nBWidth=pInfo->biWidth;  

		if (nBWidth%4) nBWidth+=4-(nBWidth%4);  
		memset(lpBitmap,0,nLen);                                //初始化
		memcpy(lpBitmap,&bmh,sizeof(BITMAPFILEHEADER));         //放入头文件数据
		memcpy(lpBitmap+sizeof(BITMAPFILEHEADER),&bmi,sizeof(BITMAPINFOHEADER));  //放入像素数据
		lpBits2=lpBitmap+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);  

		for(y=0;y<nHeight;y++)  
		{  
			for(x=0;x<nWidth;x++)  
			{  
				p1=y*nBWidth+x;  
				p2=y*nByteWidth+x*3;  
				if (lpBits[p1]<PaletteSize) Palette=lpBits[p1];  
				else Palette=0;  
				lpBits2[p2]=pPalette[Palette].rgbBlue;  
				lpBits2[p2+1]=pPalette[Palette].rgbGreen;  
				lpBits2[p2+2]=pPalette[Palette].rgbRed;  
			}  
		}  
		free(lpTemp);  
	}  

	lpBits=lpBitmap+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);  
	if (lpBackup) free(lpBackup);  
	lpBackup=(BYTE*)malloc(sizeof(BYTE)*nLen);  
	memcpy(lpBackup,lpBitmap,nLen);  

}  


void OpenFile(const char* filename)   
{  
	FILE* fp;  
	if ((fp=fopen(filename,"r+b")) == NULL)  
		return;  

	if (lpBitmap)   
		free(lpBitmap);  

	fseek(fp, 0L, SEEK_END);  
	nLen=ftell(fp);  
	fseek(fp, 0L, SEEK_SET);  

	lpBitmap=(BYTE*)malloc(sizeof(BYTE)*nLen);  
	fread(lpBitmap,nLen,1,fp);  

	LoadBitmap();   
}  




void SaveAs(const char* filename)  
{  
	BITMAPFILEHEADER bmh;  
	FILE* fp;  
	int nLen=nByteWidth*nHeight;  
	BYTE *pMem=(BYTE*)malloc(nLen+sizeof(BITMAPINFOHEADER));  
	BITMAPINFOHEADER *bmi=(BITMAPINFOHEADER *)pMem;  

	if (lpBitmap==0) return;  

	if ((fp=fopen(filename,"w+b")) == NULL)  
		return;  



	bmi->biSize=sizeof(BITMAPINFOHEADER);  
	bmi->biWidth=nWidth;  
	bmi->biHeight=nHeight;  
	bmi->biPlanes=1;  
	bmi->biBitCount=24;  
	bmi->biCompression=BI_RGB;  
	bmi->biSizeImage=0;  
	bmi->biXPelsPerMeter=0;  
	bmi->biYPelsPerMeter=0;  
	bmi->biClrUsed=0;  
	bmi->biClrImportant=0;  

	bmh.bfType='B'+'M'*256;  
	bmh.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nLen;  
	bmh.bfReserved1=0;  
	bmh.bfReserved2=0;  
	bmh.bfOffBits=54;  
	memcpy(pMem+sizeof(BITMAPINFOHEADER),lpBits,nLen);  
	fwrite(&bmh,sizeof(BITMAPFILEHEADER), 1, fp);  
	fwrite(pMem,nLen+sizeof(BITMAPINFOHEADER), 1, fp);  
	fclose(fp);  
} 