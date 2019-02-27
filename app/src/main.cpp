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
    if(avformat_open_input(&pInputFormatContext,":0.0+10.20",ifmt,&options)!=0){ //Grab at position 10,20 真正的打开文件,这个函数读取文件的头部并且把信息保存到我们给的AVFormatContext结构体中
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
	
    pInputCodecContex=pInputFormatContext->streams[videoindex]->codec;// 相当于虚基类,具体参数流中关于编解码器的信息就是被我们叫做"codec context"（编解码器上下文）的东西。
    pInputCodec=avcodec_find_decoder(pInputCodecContex->codec_id);//这里面包含了流中所使用的关于编解码器的所有信息，现在我们有了一个指向他的指针。但是我们必需要找到真正的编解码器并且打开
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	if(pInputCodecContex->codec_id == AV_CODEC_ID_H264)
	{
		printf("这是h264流\n");
	}
	
	printf("--------------------------------------------------------------------\n");
    if(pInputCodec==NULL)
    {
        printf("Codec not found.\n");
        return -1;
    }
    //打开解码器
	
    if(avcodec_open2(pInputCodecContex, pInputCodec,NULL)<0)
    {
        printf("Could not open codec.\n");
        return -1;
    }
    //---------------------------------------------------------------
    
    //为一帧图像分配内存
    AVFrame *pFrame;
    AVFrame *pFrameYUV;
    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();//为转换来申请一帧的内存(把原始帧->YUV)
 
    pFrameYUV->format=AV_PIX_FMT_YUV420P;
    pFrameYUV->width=pInputCodecContex->width;
    pFrameYUV->height=pInputCodecContex->height;
 
    //即使我们申请了一帧的内存，当转换的时候，我们仍然需要一个地方来放置原始的数据。我们使用avpicture_get_size来获得我们需要的大小，然后手工申请内存空间
     
    //获得帧图片大小
    
    unsigned char *out_buffer=(unsigned char *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pInputCodecContex->width, pInputCodecContex->height));
    //现在我们使用avpicture_fill来把帧和我们新申请的内存来结合
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pInputCodecContex->width, pInputCodecContex->height);
 
     

    FILE *fp_yuv=fopen("output.yuv","wb+");

//-----------------------------------------------------------------------------------------
    AVOutputFormat* fmt = av_guess_format(NULL, out_file , NULL);//根据out_file 猜测协议
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
 
    //Open output URL 打开输入文件 返回AVIOContext(pFormatCtx->pb)
    if (avio_open(&pOutputFormatContext->pb,out_file, AVIO_FLAG_READ_WRITE) < 0){
        printf("Failed to open output file! \n");
        return -1;
    }
		
    //创建输出流,AVStream  与  AVCodecContext一一对应
    pOutStream = avformat_new_stream(pOutputFormatContext, 0);
    if(pOutStream == NULL)
    {
        printf("Failed create pOutStream!\n");
        return -1;
    }
	
    //相当于虚基类,用于设置编码的具体参数
    pOutCodecContext = pOutStream->codec;
    pOutCodecContext->codec_id = AV_CODEC_ID_H264;
    //type
    pOutCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    //像素格式,
    pOutCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    //size
    pOutCodecContext->width = width;
    pOutCodecContext->height = height;
    //目标码率
    pOutCodecContext->bit_rate = 400000;
    //每250帧插入一个I帧,I帧越小视频越小
    pOutCodecContext->gop_size=250;
    //Optional Param B帧
    pOutCodecContext->max_b_frames=3;
    pOutCodecContext->time_base.num = 1;
    pOutCodecContext->time_base.den = 25;
    //pOutCodecContext->lmin=1;
    //pOutCodecContext->lmax=50;
    //最大和最小量化系数
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
   //通过读取包来读取整个视频流，然后把它解码成帧，最好后转换格式并且保存
   while((av_read_frame(pInputFormatContext, packet))>=0)
   {
       if(packet->stream_index==videoindex)
       {
           //真正解码,packet to pFrame
           avcodec_decode_video2(pInputCodecContex, pFrame, &got_picture, packet);
           if(got_picture)
           {
                
                   int y_size=pInputCodecContex->width*pInputCodecContex->height;
                   fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
                   fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
                   fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V

                   //写入pts，pts用于记录每帧输出的时序，只要按规则，从小到大排列即可(若果是网络传输如rtmp推流，必须填写
                   //此处保存为文件可以不写)
                   pFrameYUV->pts=frame_index;
                   frame_index++;
 
                   int picture;
                   //真正编码
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
