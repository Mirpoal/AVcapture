#include <stdio.h>
#include <stdlib.h>
extern "C"
{
#include<unistd.h>
#include<fcntl.h>
	
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include "libavutil/imgutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/opt.h"
};
#define OUTPUT_YUV420P 1
int width;
int height;

int main(int argc, char* argv[])
{
    //input
    AVFormatContext* pInputFormatContext=NULL;
    AVCodec* pInputCodec=NULL;
    AVCodecContext* pInputCodecContex=NULL;
    //output
    AVFormatContext *pOutputFormatContext=NULL;
    AVCodecContext* pOutCodecContext=NULL;
    AVCodec* pOutCodec=NULL;
    AVStream* pOutStream=NULL;
 
    av_register_all();
 
    avdevice_register_all();
		avcodec_register_all();
	
 
    const char* out_file = "ds.h264";
	
		width = 600;
		height = 800;
	
    pInputFormatContext = avformat_alloc_context();
    AVDictionary* options = NULL;
    AVInputFormat *ifmt=av_find_input_format("x11grab");
    av_dict_set(&options,"framerate","25",0);
    av_dict_set(&options,"video_size","800*600",0);
    if(avformat_open_input(&pInputFormatContext,":0.0+10.20",ifmt,&options)!=0){ //Grab at position 10,20 �����Ĵ��ļ�,���������ȡ�ļ���ͷ�����Ұ���Ϣ���浽���Ǹ���AVFormatContext�ṹ����
        printf("Couldn't open input stream.\n");
        return -1;
    }
    //
    if(avformat_find_stream_info(pInputFormatContext,NULL)<0)
    {
        printf("Couldn't find stream information.\n");
        return -1;
    }
    //
	
    int i,videoindex=-1;
    for(i=0; /* i< */pInputFormatContext->nb_streams; i++)
        if(pInputFormatContext->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    if(videoindex==-1)
    {
        printf("Didn't find a video stream.\n");
        return -1;
    }
    //
	
    pInputCodecContex=pInputFormatContext->streams[videoindex]->codec;// �൱�������,����������й��ڱ����������Ϣ���Ǳ����ǽ���"codec context"��������������ģ��Ķ�����
    pInputCodec=avcodec_find_decoder(pInputCodecContex->codec_id);//�����������������ʹ�õĹ��ڱ��������������Ϣ��������������һ��ָ������ָ�롣�������Ǳ���Ҫ�ҵ������ı���������Ҵ�
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	if(pInputCodecContex->codec_id == AV_CODEC_ID_H264)
	{
		printf("����h264��\n");
	}
	
	printf("--------------------------------------------------------------------\n");
    if(pInputCodec==NULL)
    {
        printf("Codec not found.\n");
        return -1;
    }
    //�򿪽�����
	
    if(avcodec_open2(pInputCodecContex, pInputCodec,NULL)<0)
    {
        printf("Could not open codec.\n");
        return -1;
    }
    //---------------------------------------------------------------
    
    //Ϊһ֡ͼ������ڴ�
    AVFrame *pFrame;
    AVFrame *pFrameYUV;
    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();//Ϊת��������һ֡���ڴ�(��ԭʼ֡->YUV)
 
    pFrameYUV->format=AV_PIX_FMT_YUV420P;
    pFrameYUV->width=pInputCodecContex->width;
    pFrameYUV->height=pInputCodecContex->height;
 
    //��ʹ����������һ֡���ڴ棬��ת����ʱ��������Ȼ��Ҫһ���ط�������ԭʼ�����ݡ�����ʹ��avpicture_get_size�����������Ҫ�Ĵ�С��Ȼ���ֹ������ڴ�ռ�
     
    //���֡ͼƬ��С
    
    unsigned char *out_buffer=(unsigned char *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pInputCodecContex->width, pInputCodecContex->height));
    //��������ʹ��avpicture_fill����֡��������������ڴ������
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pInputCodecContex->width, pInputCodecContex->height);
 
     

    FILE *fp_yuv=fopen("output.yuv","wb+");

//-----------------------------------------------------------------------------------------
    AVOutputFormat* fmt = av_guess_format(NULL, out_file , NULL);//����out_file �²�Э��
    if(fmt == NULL)
    {
        printf("Faild out h264 file\n");
        return -1;
    }
 
    pOutputFormatContext = avformat_alloc_context();
    if(pOutputFormatContext == NULL)
    {        printf("pOutFormatContext create error!\n");
        return -1;
    }
    pOutputFormatContext->oformat = fmt;
 
    //Open output URL �������ļ� ����AVIOContext(pFormatCtx->pb)
    if (avio_open(&pOutputFormatContext->pb,out_file, AVIO_FLAG_READ_WRITE) < 0){
        printf("Failed to open output file! \n");
        return -1;
    }
		
    //���������,AVStream  ��  AVCodecContextһһ��Ӧ
    pOutStream = avformat_new_stream(pOutputFormatContext, 0);
    if(pOutStream == NULL)
    {
        printf("Failed create pOutStream!\n");
        return -1;
    }
	
    //�൱�������,�������ñ���ľ������
    pOutCodecContext = pOutStream->codec;
    pOutCodecContext->codec_id = AV_CODEC_ID_H264;
    //type
    pOutCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    //���ظ�ʽ,
    pOutCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    //size
    pOutCodecContext->width = width;
    pOutCodecContext->height = height;
    //Ŀ������
    pOutCodecContext->bit_rate = 400000;
    //ÿ250֡����һ��I֡,I֡ԽС��ƵԽС
    pOutCodecContext->gop_size=250;
    //Optional Param B֡
    pOutCodecContext->max_b_frames=3;
    pOutCodecContext->time_base.num = 1;
    pOutCodecContext->time_base.den = 25;
    //pOutCodecContext->lmin=1;
    //pOutCodecContext->lmax=50;
    //������С����ϵ��
    pOutCodecContext->qmin = 10;
    pOutCodecContext->qmax = 51;
    pOutCodecContext->qblur=0.0;
    
    //av_dump_format(pOutputFormatContext, 0, out_file, 1);
 
    AVDictionary *param = 0;
   //H.264
    if(pOutCodecContext->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    
   av_dump_format(pOutputFormatContext, 0, out_file, 1);
    
    pOutCodec = avcodec_find_encoder(pOutCodecContext->codec_id );
    if (!pOutCodec){
        printf("Can not find encoder! \n");
        return -1;
    }
	 
    if (avcodec_open2(pOutCodecContext, pOutCodec,&param) < 0)
    {
        printf("Failed to open encoder! \n");
        return -1;
    }
    //Write File Header
	
   int r = avformat_write_header(pOutputFormatContext,NULL);
   if(r<0)
   {
        printf("Failed write header!\n");
        return -1;
   }

   AVPacket *packet=(AVPacket *)av_malloc(sizeof(AVPacket));
   int got_picture;
 
   AVPacket pkt;
   int picture_size = avpicture_get_size(pOutCodecContext->pix_fmt, pOutCodecContext->width, pOutCodecContext->height);
   av_new_packet(&pkt,picture_size);
	
    int frame_index=0;
	
	
	FILE *pFileOutput =fopen("pFileOutput.h264", "wb+");
   //ͨ����ȡ������ȡ������Ƶ����Ȼ����������֡����ú�ת����ʽ���ұ���
   while((av_read_frame(pInputFormatContext, packet))>=0)
   {
       if(packet->stream_index==videoindex)
       {
           //��������,packet to pFrame
           avcodec_decode_video2(pInputCodecContex, pFrame, &got_picture, packet);
           if(got_picture)
           {
                
                   int y_size=pInputCodecContex->width*pInputCodecContex->height;
                   fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
                   fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
                   fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V

                   //д��pts��pts���ڼ�¼ÿ֡�����ʱ��ֻҪ�����򣬴�С�������м���(���������紫����rtmp������������д
                   //�˴�����Ϊ�ļ����Բ�д)
                   pFrameYUV->pts=frame_index;
                   frame_index++;
 
                   int picture;
                   //��������
                   int ret=avcodec_encode_video2(pOutCodecContext, &pkt,pFrameYUV, &picture);
                   if(ret < 0){
                       printf("Failed to encode! \n");
                       return -1;
                   }
                   if (picture==1){
                       printf("Succeed to encode frame: size:%5d\n",pkt.size);
                       pkt.stream_index = pOutStream->index;
                       ret = av_write_frame(pOutputFormatContext, &pkt);
					   printf(" ret==   %d\n",ret);
					   
					   fwrite(pkt.data, 1, pkt.size, pFileOutput);
                       //
                       av_free_packet(&pkt);
                   }
           }
       }
       av_free_packet(packet);
   }
 
    //Write file trailer
    av_write_trailer(pOutputFormatContext);
 
    //Clean
        fclose(fp_yuv);
        av_free(out_buffer);
        av_free(pFrameYUV);
        av_free(pFrame);
        avcodec_close(pInputCodecContex);
        avformat_close_input(&pInputFormatContext);
 
    avcodec_close(pOutStream->codec);
    av_free(pOutCodec);
    av_free(fmt);
    avcodec_close(pOutCodecContext);
 
    avformat_free_context(pOutputFormatContext);
 
    return 0;
}
