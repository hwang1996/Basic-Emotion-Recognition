/*
 * ffmpeg.cpp
 *
 *  Created on: 2015-1-22
 *      Author: zhou
 */
//#include "ShellDetect.h"
#include "ffmpeg.h"
#include <string>
//#include "bitmap.h"
#include "avformat.h"
#include "avio.h"
using namespace std;

#define BI_RGB    0L
#define BI_RLE8   1L
#define BI_RLE4   2L
#define BI_BITFIELDS 3L
#define BI_JPEG   4L
#define BI_PNG    5L

FFMpegVideoHander::FFMpegVideoHander()
{
	m_nFrmIdx = 0;
	m_nFrmCnt = 0;
	m_nFrmWidth = 0;
	m_nFrmHeight = 0;
	m_pCurrFrame = NULL;
    m_pImg = NULL;
	m_pImg8 = NULL;
	m_nFirstKey = true;

	m_nConvert = true;
    seek_start = 0;
    seek_end = 0;
	unsigned long long start = 0; //��ʼ
	unsigned long long end = 0;         //����
    target_start = start*AV_TIME_BASE;
    target_end = end*AV_TIME_BASE;

	m_nStartFrm = 0;
	pFormatCtx  = NULL;
}

FFMpegVideoHander::~FFMpegVideoHander()
{
	// nothing
}

/*****************************************************************************
���ܣ����в���Ĺ��캯��?����StartTime    -    ��ʼ������Ƶ��ʱ�䣬
EndTime      -    �������Ƶ��ʱ�䣬Ĭ��ֵ�?, �����ŵ���Ƶ��ĩβ
���أ���
ע�⣺������StartTime��EndTime���ʱ��ε���Ƶ
******************************************************************************/
FFMpegVideoHander::FFMpegVideoHander(int StartTime, int EndTime)
{
	m_nFrmIdx = 0;
	m_nFrmCnt = 0;
	m_nFrmWidth = 0;
	m_nFrmHeight = 0;
	m_pCurrFrame = NULL;
    m_pImg = NULL;
	m_pImg8 = NULL;
	m_nConvert = true;
    seek_start = 0;
    seek_end = 0;
	unsigned long long start = StartTime; //��ʼ
	unsigned long long end = EndTime;         //����
    target_start = start*AV_TIME_BASE;
    target_end = end*AV_TIME_BASE;
	m_nFirstKey = true;

	m_nStartFrm = 0;

}

FFMpegVideoHander::FFMpegVideoHander(int StartCnt)
{
	m_nFrmIdx = 0;
	m_nFrmCnt = 0;
	m_nFrmWidth = 0;
	m_nFrmHeight = 0;
	m_pCurrFrame = NULL;
    m_pImg = NULL;
	m_pImg8 = NULL;
	m_nConvert = true;
    seek_start = StartCnt;
    seek_end = 0;
	unsigned long long start = 0; //��ʼ
	unsigned long long end = 0;         //����
    target_start = start*AV_TIME_BASE;
    target_end = end*AV_TIME_BASE;
	m_nFirstKey = true;

	m_nStartFrm = 0;
}

/*****************************************************************************
  ���ܣ�������Ƶ������ʼ��������?  ����filename    -    ��Ƶ�ļ������磺d:\abc\asedas.avi
  ���أ�true��ʾ�ɹ���false��ʾʧ��
  ע�⣺�û�������øýӿ������Ƶ��������
******************************************************************************/
bool FFMpegVideoHander::Load(const char* filename, const char* filepath)
{
	int PictureSize;
	uint8_t *buf = NULL;
    rtsp = false;
    video= false;     //----*********!!!!!!!!!!!!----
	av_register_all();
	avcodec_register_all();
    //frame_index=0;
	//Add by zyb
	//force to open the .h64 file by  the file format .h264
	string str;
	string ss = "rtsp";
	string s;
	str = filename;
	string::iterator iter = str.begin();
	for(int i=0;i<4;i++)
	{
		s+=*iter;
		iter++;
	}

	if (s == ss)
	{
		rtsp = true;

		//AVInputFormat *fmt = new AVInputFormat;
		//fmt = av_find_input_format("mkv");

		AVDictionary* avdic=NULL;
		char option_key[]="rtsp_transport";
		char option_value[]="tcp";
		av_dict_set(&avdic,option_key,option_value,0);
		char option_key2[]="max_delay";
		char option_value2[]="100000";
		av_dict_set(&avdic,option_key2,option_value2,0);
		//AVDictionary **test = &avdic;
        const char *out_filename = filepath;
		avformat_network_init();

		if ( avformat_open_input(&pFormatCtx, filename, NULL, &avdic)  != 0 )
		{
			//AfxMessageBox ("av open input file failed!\n");
			return false;
		}

		if ( avformat_find_stream_info(pFormatCtx, NULL) < 0 )
		{
				//AfxMessageBox ("av find stream info failed!\n");
				return false;
		}

        for(unsigned i=0;i<pFormatCtx->nb_streams;i++)
        {
        	if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        	{
        		i_video_stream = pFormatCtx->streams[i];
        		break;
        	}

        }
		avformat_alloc_output_context2(&out_FormatCtx, NULL, NULL, out_filename);
		av_dict_free(&avdic);

		o_video_stream = avformat_new_stream(out_FormatCtx, NULL);

			//AVCodecContext *codec;
			out_pCodecCtx=o_video_stream->codec;
			out_pCodecCtx->extradata=i_video_stream->codec->extradata;
			out_pCodecCtx->extradata_size = i_video_stream->codec->extradata_size;
			out_pCodecCtx->bit_rate =i_video_stream->codec->bit_rate;
			out_pCodecCtx->codec_id  =i_video_stream->codec->codec_id;
			out_pCodecCtx->codec_type=i_video_stream->codec->codec_type;
			out_pCodecCtx->time_base.den =i_video_stream->time_base.den;
			out_pCodecCtx->time_base.num =i_video_stream->time_base.num;
			out_pCodecCtx->width = i_video_stream->codec->width;
			out_pCodecCtx->height= i_video_stream->codec->height;
			out_pCodecCtx->pix_fmt = i_video_stream->codec->pix_fmt;
			out_pCodecCtx->flags = i_video_stream->codec->flags;
			out_pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
			out_pCodecCtx->me_range = i_video_stream->codec->me_range;
			out_pCodecCtx->max_qdiff = i_video_stream->codec->max_qdiff;
			out_pCodecCtx->qmin = i_video_stream->codec->qmin;
			out_pCodecCtx->qmax = i_video_stream->codec->qmax;
			out_pCodecCtx->qcompress = i_video_stream->codec->qcompress;
			out_pCodecCtx->gop_size =i_video_stream->codec->gop_size;
			o_video_stream->r_frame_rate=i_video_stream->r_frame_rate;
            //add new part by zhouyuqi for saving stream
			/*out_pCodec = avcodec_find_encoder (out_pCodecCtx->codec_id);
			if(out_pCodec==NULL)
			{
				return false;
			}

			if ( avcodec_open2(out_pCodecCtx, out_pCodec,NULL)<0 )
			{
				//AfxMessageBox ("avcode open failed!\n");
				return false;
			}*/
		//add by zhouyuqi for saving stream
		av_dump_format(out_FormatCtx, 0, out_filename, 1);
		avio_open(&out_FormatCtx->pb, out_filename, AVIO_FLAG_WRITE);
		avformat_write_header(out_FormatCtx, NULL);
	}
	else if(video==true)
	{
		  const char *out_filename = filepath;
		  if ( avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0 )
		  {
					//AfxMessageBox ("av open input file failed!\n");
					return false;
		  }

		  if ( avformat_find_stream_info(pFormatCtx, NULL) < 0 )
		  {
					//AfxMessageBox ("av find stream info failed!\n");
					return false;
		  }

		  for(unsigned i=0;i<pFormatCtx->nb_streams;i++)
		  {
		       if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		       {
		        	 i_video_stream = pFormatCtx->streams[i];
		        	 break;
		       }

		   }
		   avformat_alloc_output_context2(&out_FormatCtx, NULL, NULL, out_filename);

		   fmt=av_guess_format(NULL,out_filename,NULL);
		   out_FormatCtx->oformat=fmt;

		   avio_open(&out_FormatCtx->pb, out_filename, AVIO_FLAG_WRITE);
		   o_video_stream = avformat_new_stream(out_FormatCtx, NULL);

		   			//AVCodecContext *codec;
		   	out_pCodecCtx=o_video_stream->codec;
		   	out_pCodecCtx->extradata=i_video_stream->codec->extradata;
		   	out_pCodecCtx->extradata_size = i_video_stream->codec->extradata_size;
		   	out_pCodecCtx->bit_rate =i_video_stream->codec->bit_rate;
		   	//out_pCodecCtx->codec_id  =i_video_stream->codec->codec_id;
		   	out_pCodecCtx->codec_id  =fmt->video_codec;
		   	out_pCodecCtx->codec_type=i_video_stream->codec->codec_type;
		   	out_pCodecCtx->time_base.den =i_video_stream->time_base.den;
		   	out_pCodecCtx->time_base.num =i_video_stream->time_base.num;
		   	out_pCodecCtx->width = i_video_stream->codec->width;
		   	out_pCodecCtx->height= i_video_stream->codec->height;
		   	out_pCodecCtx->pix_fmt = i_video_stream->codec->pix_fmt;
		   	out_pCodecCtx->flags = i_video_stream->codec->flags;
		   	out_pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
		   	out_pCodecCtx->me_range = i_video_stream->codec->me_range;
		   	out_pCodecCtx->max_qdiff = i_video_stream->codec->max_qdiff;
		   	out_pCodecCtx->qmin = i_video_stream->codec->qmin;
		   	out_pCodecCtx->qmax = i_video_stream->codec->qmax;
		   	out_pCodecCtx->qcompress = i_video_stream->codec->qcompress;
		   	out_pCodecCtx->gop_size =i_video_stream->codec->gop_size;
		   	o_video_stream->r_frame_rate=i_video_stream->r_frame_rate;

		   	/*if(av_set_parameters(out_FormatCtx,NULL)<0)
		   	{

		   	}*/
		   	av_dump_format(out_FormatCtx, 0, out_filename, 1);
		   	out_pCodec = avcodec_find_encoder (out_pCodecCtx->codec_id);
		   	//out_pCodec = avcodec_find_encoder (AV_CODEC_ID_MPEG4);
		   	if(out_pCodec==NULL)
		   	{
		   		return false;
		   	}

            avcodec_open2(out_pCodecCtx, out_pCodec,NULL);
		   	/*if(ret<0)
		   	{
		   				//AfxMessageBox ("avcode open failed!\n");
		   		return false;
		   	}*/
		   					//add by zhouyuqi for saving stream

		    //avio_open(&out_FormatCtx->pb, out_filename, AVIO_FLAG_WRITE);
		    avformat_write_header(out_FormatCtx, NULL);
	}
	else
	{
		if ( avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0 )
		{
			//AfxMessageBox ("av open input file failed!\n");
			return false;
		}

		if ( avformat_find_stream_info(pFormatCtx, NULL) < 0 )
		{
			//AfxMessageBox ("av find stream info failed!\n");
			return false;
		}
	}



	videoStream = -1;
	for ( int i=0; i < int(pFormatCtx->nb_streams); i++ )
	{
		if ( pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO )
		{
			videoStream = i;
			break;
		}
	}

	if (videoStream == -1)
	{
		//AfxMessageBox ("find video stream failed!\n");
		return false;
	}

	pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	// get total frame count
	m_nFrmCnt = int(pFormatCtx->streams[videoStream]->duration);
	m_nFps    = pFormatCtx->streams[videoStream]->r_frame_rate.num  / pFormatCtx->streams[videoStream]->r_frame_rate.den;
	m_nSecCnt = int(pFormatCtx->duration / AV_TIME_BASE);

	pCodec = avcodec_find_decoder (pCodecCtx->codec_id);

	if (pCodec == NULL)
	{
		//AfxMessageBox ("avcode find decoder failed!\n");
		return false;
	}

	if ( avcodec_open2(pCodecCtx, pCodec,NULL)<0 )
	{
		//AfxMessageBox ("avcode open failed!\n");
		return false;
	}

	pFrame = avcodec_alloc_frame();
	pFrameRGB = avcodec_alloc_frame();

	if ( (pFrame==NULL) || (pFrameRGB==NULL) )
	{
		//AfxMessageBox("avcodec alloc frame failed!\n");
		return false;
	}

	PictureSize = avpicture_get_size (PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

	buf = (uint8_t*)av_malloc(PictureSize);

	if ( buf == NULL )
	{
		//AfxMessageBox( "av malloc failed!\n");
		// exit(1);
		return false;
	}

	avpicture_fill ( (AVPicture *)pFrameRGB, buf, PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height );
	pSwsCtx = sws_getContext (pCodecCtx->width,pCodecCtx->height,AV_PIX_FMT_YUV420P,
		pCodecCtx->width,pCodecCtx->height,PIX_FMT_BGR24,SWS_BICUBIC,NULL, NULL, NULL);

	m_nFrmWidth = pCodecCtx->width;
	m_nFrmHeight = pCodecCtx->height;

	SetStartFrm();


	return true;
}

/*****************************************************************************
  ���ܣ��ͷ���Ƶ����������?  ������
  ���أ���
  ע�⣺�û�������øýӿ������Դ�ͷţ�������ܵ����޷�Ԥ�ϵĺ��
******************************************************************************/
bool FFMpegVideoHander::UnLoad()
{
	m_nConvert = false;
	m_pCurrFrame = NULL;
	if (pFrame != NULL)
	{
		av_free (pFrame);
	}
	if (pFrameRGB != NULL)
	{
		av_free (pFrameRGB);
	}

	if (pSwsCtx != NULL)
	{
		sws_freeContext (pSwsCtx);
	}

	if (pCodecCtx != NULL)
	{
		avcodec_close (pCodecCtx);
	}

	if (pFormatCtx != NULL)
	{
		avformat_close_input(&pFormatCtx);
	}

    if((rtsp ==true || video==true)&&(out_pCodecCtx != NULL))
     {
         avcodec_close (out_pCodecCtx);
     }

	 if((rtsp ==true || video==true)&&(out_FormatCtx != NULL))
     {
	     av_write_trailer(out_FormatCtx);
	     avcodec_close(out_FormatCtx->streams[0]->codec);

	     av_freep(&out_FormatCtx->streams[0]->codec);
	     av_freep(&out_FormatCtx->streams[0]);

	     avio_close(out_FormatCtx->pb);
	     av_free(out_FormatCtx);
	 }



	// add by zhouyuqi


	if (m_pImg != NULL)
	{
		cvReleaseImage(&m_pImg);
		m_pImg = NULL;
	}
	if (m_pImg8 != NULL)
	{
		cvReleaseImage(&m_pImg8);
		m_pImg8 = NULL;
	}
	return true;
}

bool FFMpegVideoHander::release_img()
{
	if (m_pImg != NULL)
		{
			cvReleaseImage(&m_pImg);
			m_pImg = NULL;
		}
		if (m_pImg8 != NULL)
		{
			cvReleaseImage(&m_pImg8);
			m_pImg8 = NULL;
		}
		return true;
}

/*****************************************************************************
  ���ܣ�����Ƶ���ж�ȡ��һ֡���?  ������
  ���أ�true��ʾ��һ֡���ڣ�false��ʾ��һ֡������
  ע�⣺֡��ݴ����m_pCurrFrame�У�m_nFrmIdx����
******************************************************************************/
bool FFMpegVideoHander::GetNextFrame()
{
	m_pCurrFrame = NULL;

	int frameFinished = 0;
	int skipped_frame = 0;//统计丢失的帧数，while完之后找�?
	//last_dts = 0;
	//last_pts = 0;

	if (target_end != 0)
	{
		if ( seek_start <= seek_end)
		{
			while( av_read_frame(pFormatCtx, &packet) >= 0 )
			{
				if( packet.stream_index == videoStream )
				{
					avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
					if(frameFinished)
					{
						sws_scale (pSwsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

						m_pCurrFrame = pFrameRGB->data[0];// m_pCurrFrame只是一个指针，没有分配资源

						seek_start += 1;

						//add by zhouyuqi for saving stream
						if(rtsp ==true ||video==true )
						{
						  //pFrame->pts=av_rescale(m_nFrmIdx,AV_TIME_BASE*(int64_t)out_pCodecCtx->time_base.num,out_pCodecCtx->time_base.den);
						  //pFrame->pict_type=0;
                          //int sign=0;
						  int len=1;//avcodec_encode_video2(out_pCodecCtx,&packet,pFrame,&sign);
						  if(len==0)
						  {
						   packet.flags |= AV_PKT_FLAG_KEY;
						  //packet.pts = av_rescale_q(packet.pts, i_video_stream->codec->time_base, i_video_stream->time_base);
						  //packet.dts = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
						   packet.pts = last_pts;
						   packet.dts = last_dts;
						   last_pts +=18000;
						   last_dts +=18000;
						   packet.duration = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
						   av_interleaved_write_frame(out_FormatCtx, &packet);
						  }
						}
						++m_nFrmIdx;
						av_free_packet(&packet);

						return true;
					}
					else                          //add by qiji
						{skipped_frame++;}

				}// end of if(packet.stream_index == videoStream)

				av_free_packet(&packet);
			}

			for (int i=skipped_frame;i>0;i--)     //add by qiji
			{
				if( packet.stream_index == videoStream )
				{
					avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
					if(frameFinished)
					{
						sws_scale (pSwsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

						m_pCurrFrame = pFrameRGB->data[0];// m_pCurrFrame只是一个指针，没有分配资源

						seek_start += 1;

						//add by zhouyuqi for saving stream
						if(rtsp ==true || video==true)
						{
						   //int sign=0;
			               int len=1;//avcodec_encode_video2(out_pCodecCtx,&packet,pFrame,&sign);
			               if(len==0)
			               {
						    packet.flags |= AV_PKT_FLAG_KEY;
						    //packet.pts = av_rescale_q(packet.pts, i_video_stream->codec->time_base, i_video_stream->time_base);
						    //packet.dts = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
						    packet.pts = last_pts;
						    packet.dts = last_dts;
						    last_pts +=18000;
						    last_dts +=18000;
						    packet.duration = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
						    av_interleaved_write_frame(out_FormatCtx, &packet);
			               }
						}
						++m_nFrmIdx;
						av_free_packet(&packet);

						return true;
					}
				}
				av_free_packet(&packet);
			}

		}

	}

	else
	{

		while( av_read_frame(pFormatCtx, &packet) >= 0 )
		{
			if( packet.stream_index == videoStream )
			{
				//int m = packet.pts;

				int kkk;
				kkk= avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
				//int keyFrame = pFrame->key_frame;
				if(frameFinished)
				{
// 					pFrame->data[0] += pFrame->linesize[0] * (pCodecCtx->height - 1);
// 					pFrame->linesize[0] *= -1;
// 					pFrame->data[1] += pFrame->linesize[1] * (pCodecCtx->height / 2 - 1);
// 					pFrame->linesize[1] *= -1;
// 					pFrame->data[2] += pFrame->linesize[2] * (pCodecCtx->height / 2 - 1);
// 					pFrame->linesize[2] *= -1;

					sws_scale (pSwsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

					m_pCurrFrame = pFrameRGB->data[0];// m_pCurrFrame只是一个指针，没有分配资源

					//add by zhouyuqi for saving stream
					if(rtsp ==true ||video==true )
					{
					  //int sign=0;
				      int len=1;//avcodec_encode_video2(out_pCodecCtx,&packet,pFrame,&sign);
				      if(len==0)
				      {
					   packet.flags |= AV_PKT_FLAG_KEY;
					   packet.pts = last_pts;
					   packet.dts = last_dts;
					   last_pts +=18000;
					   last_dts +=18000;
					   //packet.pts = av_rescale_q(packet.pts, i_video_stream->codec->time_base, i_video_stream->time_base);
					   //packet.dts = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
					   packet.duration = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
					   av_interleaved_write_frame(out_FormatCtx, &packet);
				      }

					}
					++m_nFrmIdx;
					av_free_packet(&packet);

					return true;
				}
				else                          //add by qiji
				{skipped_frame++;}

			}// end of if(packet.stream_index == videoStream)
			av_free_packet(&packet);
		}

		for (int i=skipped_frame;i>0;i--)     //add by qiji
		{
			if( packet.stream_index == videoStream )
			{
				//int m = packet.pts;
				int kkk;
				kkk= avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
				//int keyFrame = pFrame->key_frame;
				if(frameFinished)
				{

					sws_scale (pSwsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

					m_pCurrFrame = pFrameRGB->data[0];// m_pCurrFrame只是一个指针，没有分配资源

					//add by zhouyuqi for saving stream
					if(rtsp ==true ||video ==true)
					{
						//int sign=0;
				        int len=1;//avcodec_encode_video2(out_pCodecCtx,&packet,pFrame,&sign);
				        if(len==0)
				        {
						 packet.flags |= AV_PKT_FLAG_KEY;
						 packet.pts = last_pts;
						 packet.dts = last_dts;
						 last_pts +=18000;
						 last_dts +=18000;
						 //packet.pts = av_rescale_q(packet.pts, i_video_stream->codec->time_base, i_video_stream->time_base);
						 //packet.dts = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
						 packet.duration = av_rescale_q(packet.dts, i_video_stream->codec->time_base, i_video_stream->time_base);
					     av_interleaved_write_frame(out_FormatCtx, &packet);
				        }
					}
					++m_nFrmIdx;
					av_free_packet(&packet);

					return true;
				}
			}// end of if(packet.stream_index == videoStream)
			av_free_packet(&packet);
		}

	}

	av_free_packet(&packet);
	return false;
}

/*****************************************************************************
  ���ܣ����浱ǰ֡��ݵ�bmp�ļ���
  ����filename    -    �ļ�ȫ·�������磺d:\abc\frame_1.bmp
  ���أ�true��ʾ����ɹ���false��ʾʧ��
  ע�⣺֡��ݴ����m_pCurrFrame��ָ���pFrameRGB�У�m_pCurrFrame����û�з���ռ�?******************************************************************************/
bool FFMpegVideoHander::SaveAsBmp(char* filename)
{
	if ( m_pCurrFrame == NULL )
	{
		return false;
	}

	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	FILE *fp;

	if ( (fp=fopen(filename,"wb+")) == NULL )
	{
		printf ("open file %s failed!\n",filename);
		return false;
	}

	int width = m_nFrmWidth;
	int height = m_nFrmHeight;
	int bpp = 24;

	bmpheader.bfType = 0x4d42;
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;

	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biWidth = width;
	bmpinfo.biHeight = height;
	bmpinfo.biPlanes = 1;
	bmpinfo.biBitCount = bpp;
	bmpinfo.biCompression = BI_RGB;
	bmpinfo.biSizeImage = (width*bpp+31)/32*4*height;
	bmpinfo.biXPelsPerMeter = 100;
	bmpinfo.biYPelsPerMeter = 100;
	bmpinfo.biClrUsed = 0;
	bmpinfo.biClrImportant = 0;

	fwrite (&bmpheader, sizeof(bmpheader), 1, fp);
	fwrite (&bmpinfo, sizeof(bmpinfo), 1, fp);
	fwrite (m_pCurrFrame, width*height*bpp/8, 1, fp);

	fclose(fp);

	return true;
}

/*****************************************************************************
  ���ܣ�����Ƶ�ж�ȡһ����ͼ��֡����bmp�ļ���ʽ������dstDir�ļ�����
  ����dstDir    -    ������֡�ļ���ŵ�λ�ã����磺d:\abc
        nCnt      -    ����֡��
        interval  -    ���������Ǹ�����0����������1����֡����...
  ���أ�ʵ�ʲ����֡��?  ע�⣺nCntĬ��Ϊ10000��intervalĬ��Ϊ0
******************************************************************************/
int  FFMpegVideoHander::Sample(char *dstDir,int nCnt,int interval)
{
	if ( interval < 0 )
	{
		return 0;
	}

	int cnt = 0;
	char filename[512];
	while( GetNextFrame() && cnt < nCnt )
	{
		if ( (m_nFrmIdx-1) % (interval+1) == 0 )
		{
			sprintf(filename,"%s\\frame_%d.bmp",dstDir,m_nFrmIdx);
			SaveAsBmp(filename);
			++cnt;
		}
	}
	return cnt;
}

IplImage *FFMpegVideoHander::GetCurrFrame()
{
	if (m_pImg8 != NULL)
	{
		cvReleaseImage(&m_pImg8);
	}
	if (m_pImg != NULL)
	{
		cvReleaseImage(&m_pImg);
	}
    m_pImg = cvCreateImage(cvSize(m_nFrmWidth,m_nFrmHeight),8,3);
	m_pImg8 = cvCreateImage(cvSize(m_nFrmWidth,m_nFrmHeight), 8, 1);

	if (m_pCurrFrame != NULL && m_pImg8 != NULL && m_pImg != NULL)
	{
		memcpy(m_pImg->imageData,m_pCurrFrame,m_nFrmWidth*m_nFrmHeight*3);
	}

	cvConvertImage(m_pImg,m_pImg8);

	//cvSaveImage("C:\\Users\\Administrator\\Desktop\\�������ƽ�?.0\\VehecleDetect\\2.bmp",m_pImg);

	//return m_pImg8;          //return Gray
	return m_pImg;          // retrurn RGB
}

void FFMpegVideoHander::SetStartFrm()
{
	int frameFinished = 0;
	AVRational timebase;
	timebase.num = 1;
	timebase.den = AV_TIME_BASE;
	m_nConvert = false;
	//     int64_t seek_start;
	// 	int64_t seek_end;

	if(videoStream>=0)
	{
		if (seek_start != 0)
		{
	//		seek_start = seek_start / m_nFps;
			target_start = ((double)seek_start / m_nFps)*AV_TIME_BASE;
            target_end = 0*AV_TIME_BASE;
		}


		seek_start = av_rescale_q(target_start, timebase, pFormatCtx->streams[videoStream]->time_base);
		seek_end = av_rescale_q(target_end, timebase,pFormatCtx->streams[videoStream]->time_base);

		if (seek_start < 0)
		{
			seek_start = 0;
		}
		//av_seek_frame(pFormatCtx, videoStream, seek_start, AVSEEK_FLAG_BACKWARD);
		av_seek_frame(pFormatCtx, videoStream, seek_start, 1);
		while( av_read_frame(pFormatCtx, &packet) >= 0 )
		{
			if( packet.stream_index == videoStream )
			{
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
				if (pFrame->key_frame == 0 && m_nFirstKey )
				{
					seek_start++;
					continue;
				}
			}
			break;
		}
	}
	m_nStartFrm = (double)packet.pts / (pFormatCtx->streams[videoStream]->time_base.den / m_nFps) + 0.5;
}








