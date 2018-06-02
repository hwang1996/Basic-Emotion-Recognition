/*
 * ffmpeg.h
 *
 *  Created on: 2016-1-10
 *      Author: zhou
 */

#ifndef FFMPEG_H_
#define FFMPEG_H_


#ifndef __FFMpegVideoHander_Header__
#define __FFMpegVideoHander_Header__

// ʹ��C++���Ե���C��ؿ��ļ�?#ifdef __cplusplus
extern "C" {
#endif

#include "avcodec.h"
#include "avformat.h"
#include "avutil.h"
#include "swscale.h"
#include "pixfmt.h"
#include "dict.h"
//#include <string>
//#include "afx.h"
#define CODEC_TYPE_VIDEO AVMEDIA_TYPE_VIDEO
//#ifdef __FFMPEG_0_6__
//   if(av_open_input_file(&ffmpeg_fields.pFormatCtx,_filePath,NULL,0,NULL)!=0)
//#else
//   if(avformat_open_input(&ffmpeg_fields.pFormatCtx,_filePath,NULL,NULL)!=0)
//#endif

		   //using namespace std;

#ifdef __cplusplus
}
#endif

#include "cv.h"
#include "highgui.h"

//#pragma comment(lib,"cv.lib")
//#pragma comment(lib,"highgui.lib")
//#pragma comment(lib,"cxcore.lib")
//
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")

typedef unsigned char BYTE;

#include <windows.h> // for use BITMAPFILEHEADER / BITMAPINFOHEADER
#include <string.h>
#include <sys/types.h>
//#include <unistd.h>

//#define FF_API_AVCODEC_OPEN 1

class FFMpegVideoHander
{
public:
	FFMpegVideoHander();

	//�����Ĺ��캯��
	FFMpegVideoHander(int StartTime, int EndTime);

	//����Ϊ��ʼ��֡��
	FFMpegVideoHander(int StartCnt);

	~FFMpegVideoHander();

	// ������Ƶ
	bool Load(const char* filename, const char* filepath);

	// ж����Ƶ
	bool UnLoad();
	bool release_img();  //add by zhouyq for memory-leak

	// ��ȡ��һ֡
	bool GetNextFrame();

	// ���浱ǰ֡Ϊbmp�ļ�
	bool SaveAsBmp(char* filename);

	//���ص�ǰ֡��ΪIPlImage��ʽ
	IplImage *GetCurrFrame();

	// ��Ƶ֡����
	int  Sample(char *dstDir,int nCnt = 10000,int interval = 0);

	// ��ȡ��ǰ֡���?
	int   GetFrmWidth(){return m_nFrmWidth;}
	int   GetFrmHeight(){return m_nFrmHeight;}
	int   GetFrmIdx(){return m_nFrmIdx;}
	int   GetFrmCnt(){return m_nFrmCnt;}
	int   GetSecCnt(){return m_nSecCnt;}
	int   GetFps(){return m_nFps;}
	int   GetChannel(){return 3;}
	int   GetStartFrm(){return m_nStartFrm;}

	int64_t  last_pts;
	int64_t  last_dts;
private:
	int m_nFrmIdx;
	int m_nFrmWidth;
	int m_nFrmHeight;
	BYTE *m_pCurrFrame; // �������ռ�
	int m_nFrmCnt;
	int m_nSecCnt;
	int m_nFps;
	IplImage *m_pImg ;
	IplImage *m_pImg8 ;
	int64_t seek_start;
	int64_t seek_end;
	int m_nConvert;
	//unsigned __int64 target_start;
	//unsigned __int64 target_end;
	unsigned long long target_start;
	unsigned long long target_end;

	bool m_nFirstKey;

	void SetStartFrm();   //������֡��֡��
	int m_nStartFrm;

	//unsigned int frame_index;      //add by zhouyuqi
    //char *pic_buff=NULL;           //add by zhouyuqi

private:
	AVCodecContext *pCodecCtx;
	AVFormatContext *pFormatCtx;
	AVCodecContext  *out_pCodecCtx; //add by zhouyuqi for saving stream
	AVFormatContext *out_FormatCtx; //add by zhouyuqi for saving stream
	AVOutputFormat  *fmt;
	AVCodec *pCodec;
	AVCodec *out_pCodec;            //add by zhouyuqi for saving stream
	AVFrame *pFrame;
	AVFrame *pFrameRGB;
	AVFrame *out_pFrame;
	SwsContext *pSwsCtx;
	AVPacket packet;
	int videoStream;

	bool rtsp;
	bool video;
    AVStream *i_video_stream;
    AVStream *o_video_stream;

};

#endif



