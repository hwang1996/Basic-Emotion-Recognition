//lbp_hf.h��lbp_hf.cpp�������ļ�����������ʶ��������
#include "loadbmp.h"  
#include "ffmpeg.h"

#include <opencv.hpp>
#include "facedetect-dll.h"
#pragma comment(lib,"libfacedetect.lib")

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include "lbp_hf.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <algorithm>

#define  feature_num 1110 //����ʶ������ά�� 8x8��������ֻ�õ�30�� ÿ��������37ά �����ܵ�ά���ǣ�30*37
using namespace std;
using namespace cv;
IplImage *image1;

void face_detect(Mat& img, double scale);
void emotion_detect_LBP(IplImage* src, Rect select);
void emotion_detect_HOG(IplImage *image2, Rect select);
void emotion_detect_LBP_HOG(IplImage *image2, Rect select);

String cascadeName = "I:\\Software\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml";//������ѵ������ ���·�������Լ������ϵľ���������� ����opencv������Ķ��� 
String nestedCascadeName = "I:\\Software\\opencv\\sources\\data\\haarcascades\\haarcascade_eye.xml";//���۵�ѵ������ ���û�õ������

//�����ֻŪ��4�����������
//�ֱ��ǿ��ġ����ȡ�����������
CvSVM svm_happy;
CvSVM svm_surprise;
CvSVM svm_angry;
CvSVM svm_sad;
CvSVM svm_disgust;
CvSVM svm_fear;
CvSVM svm_neutral;
CvSVM svm_ns;

CvRTrees rtree_happy;
CvRTrees rtree_surprise;
CvRTrees rtree_angry;
CvRTrees rtree_sad;
CvRTrees rtree_disgust;
CvRTrees rtree_fear;
CvRTrees rtree_neutral;

int happy_result=0;
int surprise_result=0;
int angry_result=0;
int sad_result=0;
int disgust_result=0;
int fear_result=0;
int neutral_result=0;

int main( int argc, const char** argv )
{
	Mat image;
	CascadeClassifier cascade, nestedCascade;//������������������ ���������
	double scale = 1.3;  

	//���ط����� ����ѵ���õ�
	svm_happy.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Happy.xml");
	svm_surprise.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Surprise.xml");
	svm_angry.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Angry.xml");
	svm_sad.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Sad.xml");
	svm_fear.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Fear.xml");
	svm_disgust.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Disgust.xml");
	svm_neutral.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Neutral.xml");
	//svm_ns.load("I:\\train_code\\svm_hog\\svm\\train_1\\SVM_HOG_Ns.xml");

	/*svm_happy.load("video_happy.txt");
	svm_surprise.load("video_surprise.txt");
	svm_angry.load("video_angry.txt");
	svm_sad.load("video_sad.txt");
	svm_fear.load("video_fear.txt");
	svm_disgust.load("video_disgust.txt");
	svm_neutral.load("video_neutral.txt");*/

	/*rtree_happy.load("RandomForset_HOG_Happy.xml");
	rtree_surprise.load("RandomForset_HOG_Surprise.xml");
	rtree_angry.load("RandomForset_HOG_Angry.xml");
	rtree_sad.load("RandomForset_HOG_Sad.xml");
	rtree_fear.load("RandomForset_HOG_Fear.xml");
	rtree_disgust.load("RandomForset_HOG_Disgust.xml");
	rtree_neutral.load("RandomForset_HOG_Neutral.xml");*/

	/*svm_happy.load("SVM_LBP_HOG_Happy.txt");
	svm_surprise.load("SVM_LBP_HOG_Surprise.txt");
	svm_angry.load("SVM_LBP_HOG_Angry.txt");
	svm_sad.load("SVM_LBP_HOG_Sad.txt");
	svm_fear.load("SVM_LBP_HOG_Fear.txt");
	svm_disgust.load("SVM_LBP_HOG_Disgust.txt");
	svm_neutral.load("SVM_LBP_HOG_Neutral.txt");*/

	string a[600];              //���� string ���ͣ���100�е��ı�����Ҫ������ 
	int line_i=0; 
    ifstream infile("filelist_test.txt",ios::in);
 
    while(!infile.eof())            // ��δ���ļ�����һֱѭ�� 
    {  
        getline(infile, a[line_i], '\n');//��ȡһ�У��Ի��з����������� a[] ��
        line_i++;                    //��һ��
    }

	/*ifstream fin("SVM_HOG\\disgust.txt");
	int final_result_angry = 0;
	int final_result_disgust = 0;
	int final_result_fear = 0;
	int final_result_neutral = 0;
	int final_result_happy = 0;
	int final_result_sad = 0;
	int final_result_surprise = 0;
	string q, w, h, r, t, y, x;
	fin >> q >> final_result_disgust >> w >> final_result_surprise >> h >> final_result_fear >> r >> final_result_sad >> t >> final_result_happy >> y >> final_result_angry >> x >> final_result_neutral;*/

	for(int ii=560;ii<570;ii++)        // ��ʾ��ȡ��txt���� 
    {      
		FFMpegVideoHander ffmpeg; //ffmpeg ���������ý����Ƶ�ļ���

		string filePath = "I:\\test\\test_out\\";
		string filename = filePath + a[ii];
		char fileName[50];
		string txtType = ".txt";
		string txtName = filePath + a[ii].substr(0,9) + txtType;
		
		ofstream myfile(txtName,ios::out);

		int j;
		for(j=0;j<filename.length();j++)
		{
			fileName[j] = filename[j];
		}
		fileName[j] = '\0';
		cout << "����" << fileName << endl; 

		ffmpeg.Load(fileName,"");

		double t=0;
		int pic_num = 1;

		while (ffmpeg.GetNextFrame())
		{
			t = (double)cvGetTickCount()-t;	
		
			image1=ffmpeg.GetCurrFrame(); //��ȡ��ǰ֡
			image = image1;
			//printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) ); //��������������ʱ��� �ò�������ɾ��
		
			if( !image.empty() )//��ȡͼƬ���ݲ���Ϊ��
			{
				char filename2[50] = "I:\\face_sample\\test_video\\";
				char img_num[10];
				char file_type[] = ".jpg";
				sprintf(img_num, "%d", pic_num);
				strcat(img_num, file_type);
				strcat(filename2, img_num);
				IplImage *image_r = NULL;
				image_r = cvCreateImage(cvSize(672,378), image1->depth, 3);
				cvResize(image1, image_r, CV_INTER_LINEAR);
				cvSaveImage(filename2,image_r); //���м�ļ�����ݴ����� ������ȥ������
				pic_num++;
			}

			//cvNamedWindow("emotion",1);
			//cvShowImage("emotion",image1); //------------��ʾͼ��
			//char c=cvWaitKey(33);

			ffmpeg.release_img(); //�ͷ���Դ
			t=(double)cvGetTickCount();
		}
	
		happy_result=0;
		surprise_result=0;
		angry_result=0;
		sad_result=0;
		disgust_result=0;
		fear_result=0;
		neutral_result=0;

		for(int i=1; i<pic_num; i++)
		{
			char filename[50] = "I:\\face_sample\\test_video\\";
			char img_num[10];
			char file_type[] = ".jpg";
			sprintf(img_num, "%d", i);
			strcat(img_num, file_type);
			strcat(filename, img_num);
			image1=cvLoadImage(filename,1);
		
			image=image1;
			face_detect(image,scale);

			//cvNamedWindow("emotion1",1);
			//cvShowImage("emotion1",image1); //------------��ʾͼ��
			//char c=cvWaitKey(33);
		}

		cout<<endl;
		cout<<"happy  "<<happy_result<<endl;
		cout<<"surprise  "<<surprise_result<<endl;
		cout<<"sad  "<<sad_result<<endl;
		cout<<"fear  "<<fear_result<<endl;
		cout<<"disgust  "<<disgust_result<<endl;
		cout<<"angry  "<<angry_result<<endl;
		cout<<"neutral  "<<neutral_result<<endl;

		int a[7];
		a[0] = disgust_result;
		a[1] = surprise_result;
		a[2] = fear_result;
		a[3] = sad_result;
		a[4] = happy_result;
		a[5] = angry_result;
		a[6] = neutral_result;

		int temp = a[0];
		int num_temp = 0;
		for(int i = 0; i<6; i++)
		{
			if(a[i+1]>=temp){
				temp = a[i+1];
				num_temp = i+1;
			}
		}

		if(num_temp == 0){
			//final_result_disgust++;
			myfile<<"Disgust";
			myfile.close();
		}
		else if(num_temp == 1){
			//final_result_surprise++;
			myfile<<"Surprise";
			myfile.close();
		}
		else if(num_temp == 2){
			//final_result_fear++;
			myfile<<"Fear";
			myfile.close();
		}
		else if(num_temp == 3){
			//final_result_sad++;
			myfile<<"Sad";
			myfile.close();
		}
		else if(num_temp == 4){
			//final_result_happy++;
			myfile<<"Happy";
			myfile.close();
		}
		else if(num_temp == 5){
			//final_result_angry++;
			myfile<<"Angry";
			myfile.close();
		}
		else if(num_temp == 6){
			//final_result_neutral++;
			myfile<<"Neutral";
			myfile.close();
		}

		ffmpeg.UnLoad();
    }

	/*cout<<endl;
	cout << "disgust: " << final_result_disgust << endl;
	cout << "surprise: " << final_result_surprise << endl;
	cout << "fear: " << final_result_fear << endl;
	cout << "sad: " << final_result_sad << endl;
	cout << "happy: " << final_result_happy << endl;
	cout << "angry: " << final_result_angry << endl;
	cout << "neutral: " << final_result_neutral << endl;*/

	/*char filename[] = "SVM_HOG\\disgust.txt";
	ofstream fout(filename);
	fout << "disgust:  " << final_result_disgust << endl;
	fout << "surprise:  " << final_result_surprise << endl;
	fout << "fear:  " << final_result_fear << endl;
	fout << "sad:  " << final_result_sad << endl;
	fout << "happy:  " << final_result_happy << endl;
	fout << "angry:  " << final_result_angry << endl;
	fout << "neutral:  " << final_result_neutral << endl;*/

	return 0;
}

//int main( int argc, const char** argv )
//{
//	Mat image;
//	CascadeClassifier cascade, nestedCascade;//������������������
//	double scale = 1.3;
//
//	//svm_happy.load("SVM_HOG_Happy.xml");
//	//svm_surprise.load("SVM_HOG_Surprise.xml");
//	//svm_angry.load("SVM_HOG_Angry.xml");
//	//svm_sad.load("SVM_HOG_Sad.xml");
//	//svm_fear.load("SVM_HOG_Fear.xml");
//	//svm_disgust.load("SVM_HOG_Disgust.xml");
//	//svm_neutral.load("SVM_HOG_Neutral.xml");
//
//	rtree_happy.load("RandomForset_HOG_Happy.xml");
//	rtree_surprise.load("RandomForset_HOG_Surprise.xml");
//	rtree_angry.load("RandomForset_HOG_Angry.xml");
//	rtree_sad.load("RandomForset_HOG_Sad.xml");
//	rtree_fear.load("RandomForset_HOG_Fear.xml");
//	rtree_disgust.load("RandomForset_HOG_Disgust.xml");
//	rtree_neutral.load("RandomForset_HOG_Neutral.xml");
//
//	char filename[50] = "I:\\face_sample\\2.jpg";
//
//	image1=cvLoadImage(filename,1);
//	image=imread(filename,1);
//	if( !image.empty() )//��ȡͼƬ���ݲ���Ϊ��
//	{
//		face_detect(image,scale);
//		//waitKey(0);
//	}
//	cout<<"happy  "<<happy_result<<endl;
//	cout<<"surprise  "<<surprise_result<<endl;
//	cout<<"sad  "<<sad_result<<endl;
//	cout<<"fear  "<<fear_result<<endl;
//	cout<<"disgust  "<<disgust_result<<endl;
//	cout<<"angry  "<<angry_result<<endl;
//	cvNamedWindow("emotion",1);
//	cvShowImage("emotion",image1); //------------��ʾͼ��
//	char c=cvWaitKey(33);
//	system("pause");
//		
//	return 0;
//}


void face_detect( Mat& img, double scale)
{
	double t = 0;

	Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );//��ͼƬ��С���ӿ����ٶ�

	cvtColor( img, gray, CV_BGR2GRAY );//ת���ɻҶ�ͼ��
	resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );//���ߴ���С��1/scale,�����Բ�ֵ
	equalizeHist( smallImg, smallImg );//ֱ��ͼ����

	int * pResults = NULL;
	Rect select;
	pResults = facedetect_multiview_reinforce((unsigned char*)(smallImg.ptr(0)), smallImg.cols, smallImg.rows, smallImg.step,1.2f, 5, 24);
	//Mat eye_temp=imread("I:/eye.bmp", 0);
	
	for(int i = 0; i < (pResults ? *pResults : 0); i++)
	{
		short * p = ((short*)(pResults+1))+6*i;
		int x = p[0];
		int y = p[1];
		int w = p[2];
		int h = p[3];
		int neighbors = p[4];
		int angle = p[5];
		select.x=p[0]*scale;
		select.y=p[1]*scale;
		select.width=p[2]*scale;
		select.height=p[3]*scale;
		select&=Rect(0,0,gray.cols,gray.rows);//��֤��ѡ���ο�����Ƶ��ʾ����֮��
		if (select.width==0 || select.height==0)
			continue;
		cvSetImageROI(image1,select); // �Ѽ�����������Ȧ����
		IplImage *image2=cvCreateImage(cvSize(select.width,select.height),image1->depth,1); 
		cvCvtColor(image1,image2,CV_BGR2GRAY);

		cvResetImageROI(image1);
		/*��һ��*/
		IplImage *image_r = NULL;
		image_r = cvCreateImage(cvSize(128,128), image1->depth, 1);
		cvResize(image2, image_r, CV_INTER_LINEAR);
		/*��ʼ����ʶ��*/
		emotion_detect_HOG(image_r,select);

		//------------------------------����������ģ��ƥ���������۾�λ��--------------------------------
		//Rect select_eye=cvRect(0,0,select.width,select.height/2);
		//cvSetImageROI(image2,select_eye);
		//double eye_scale=image2->width*1.0/135; //��Ϊ�۾���ģ���Ǵ�135x135������ͼ���Ͽ۳����� ���԰���135�Ļ����Ͻ������� ��������Լ�����
		//IplImage *image3=cvCreateImage(cvSize(select.width/eye_scale,(select.height/2)/eye_scale),image1->depth,1); 
		//cvResize(image2,image3,CV_INTER_CUBIC);
		//Mat Image_temp=image3;
		//cv::Mat result;
		//result.create(Image_temp.rows-eye_temp.rows+1,Image_temp.cols-eye_temp.cols+1,CV_32FC1);
		//matchTemplate(Image_temp,eye_temp,result,CV_TM_CCORR_NORMED);
		//Point minloc;
		//double min;
		//double max;
		//minMaxLoc(result,&min,&max,NULL,&minloc,cv::Mat());
		//cvRectangle(image1,cvPoint(select.x+minloc.x*eye_scale,select.y+minloc.y*eye_scale),cvPoint(select.x+minloc.x*eye_scale+eye_temp.cols*eye_scale,select.y+minloc.y*eye_scale+eye_temp.rows*eye_scale),CV_RGB(0,0,255),2);
		cvRectangle(image1,cvPoint(select.x,select.y),cvPoint(select.x+select.width,select.y+select.height),CV_RGB(0,0,255),2);
		//cvReleaseImage(&image3);
		cvReleaseImage(&image2);
		cvReleaseImage(&image_r);
	}
}

void emotion_detect_LBP(IplImage *image2, Rect select)
{
	double final_vector[feature_num]={0.0}; //��ʼ��ʶ������
	int result=1;
	CvMat *testMat = cvCreateMat(1, feature_num, CV_32FC1);
	float temp=1.0f;

	get_vector_AU(image2,final_vector);
	for (int i = 0; i<feature_num; i++)
	{
		cvSetReal2D(testMat, 0, i, final_vector[i]); //����ȡ��ʶ��������ֵ��ȥ
	}
	
	float flag_happy = 0;
	flag_happy = rtree_happy.predict(testMat);
	cout << "happy:" << flag_happy << endl;
	if (flag_happy<temp)// && flag_happy<0)
	{
		temp=flag_happy;
		result=2;
	}

	float flag_surprise=0;
	flag_surprise = rtree_surprise.predict(testMat);
	cout << "surprise:" << flag_surprise << endl;
	if (flag_surprise<temp)// && flag_surprise<0)
	{
		temp=flag_surprise;
		result=3;
	}

	float flag_angry=0;
	flag_angry = rtree_angry.predict(testMat);
	cout << "angry:" << flag_angry << endl;
	if (flag_angry<temp)// && flag_angry<0)
	{
		temp=flag_angry;
		result=4;
	}

	float flag_disgust=0;
	flag_disgust = rtree_disgust.predict(testMat);
	cout << "disgust:" << flag_disgust << endl;
	if (flag_disgust<temp)// && flag_disgust<0)
	{
		temp=flag_disgust;
		result=5;
	}

	float flag_fear=0;
	flag_fear = rtree_fear.predict(testMat);
	cout << "fear:" << flag_fear << endl;
	if (flag_fear<temp)// && flag_fear<0)
	{
		temp=flag_fear;
		result=6;
	}

	float flag_sad=0;
	flag_sad = rtree_sad.predict(testMat);
	cout << "sad:" << flag_sad << endl;
	if (flag_sad<temp)// && flag_sad<0)
	{
		temp=flag_sad;
		result=7;
	}

	float flag_neutral=0;
	flag_neutral = rtree_neutral.predict(testMat);
	cout << "neutral:" << flag_neutral << endl;
	if (flag_neutral<temp)// && flag_sad<0)
	{
		temp=flag_neutral;
		result=8;
	}

	//�ѽ��д��ͼ����
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,1,0.5,0,2,8);
	if (result==2)
	{
		cvPutText(image1,"happy",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		happy_result++;
	}
	else if (result==3)
	{
		cvPutText(image1,"surprise",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		surprise_result++;
	}
	else if (result==4)
	{
		cvPutText(image1,"angry",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		angry_result++;
	}
	else if (result==5)
	{
		cvPutText(image1,"disgust",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		disgust_result++;
	}
	else if (result==6)
	{
		cvPutText(image1,"fear",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		fear_result++;
	}
	else if (result==7)
	{
		cvPutText(image1,"sad",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		sad_result++;
	}
	else if (result==8)
	{
		cvPutText(image1,"neutral",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		neutral_result++;
	}
}

void emotion_detect_HOG(IplImage *image2, Rect select)
{
	//Mat testImg;
	//testImg = image2;
	//int result=1;
	////float temp=1.0f;
	//float temp=0.0f;

	////��ⴰ��(64,128),��ߴ�(16,16),�鲽��(8,8),cell�ߴ�(8,8),ֱ��ͼbin����9
	//HOGDescriptor hog(Size(128,128),Size(16,16),Size(8,8),Size(8,8),9);//HOG���������������HOG�����ӵ�

	//vector<float> descriptor;
	//hog.compute(testImg,descriptor,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)
	//Mat testMat = Mat::zeros(1,descriptor.size(),CV_32FC1);//����������������������
	////������õ�HOG�����Ӹ��Ƶ�testFeatureMat������
	//for(int i=0; i<descriptor.size(); i++)
	//	testMat.at<float>(0,i) = descriptor[i];

	//cout << endl;
	//float flag_happy = 0;
	////flag_happy = svm_happy.predict(testMat,TRUE);
	//flag_happy = rtree_happy.predict_prob(testMat);
	//cout << "happy:" << flag_happy << endl;
	//if (flag_happy>temp)// && flag_happy<0)
	//{
	//	temp=flag_happy;
	//	result=2;
	//}

	//float flag_surprise=0;
	////flag_surprise = svm_surprise.predict(testMat,TRUE);
	//flag_surprise = rtree_surprise.predict_prob(testMat);
	//cout << "surprise:" << flag_surprise << endl;
	//if (flag_surprise>temp)// && flag_surprise<0)
	//{
	//	temp=flag_surprise;
	//	result=3;
	//}

	//float flag_angry=0;
	////flag_angry = svm_angry.predict(testMat,TRUE);
	//flag_angry = rtree_angry.predict_prob(testMat);
	//cout << "angry:" << flag_angry << endl;
	//if (flag_angry>temp)// && flag_angry<0)
	//{
	//	temp=flag_angry;
	//	result=4;
	//}

	//float flag_disgust=0;
	////flag_disgust = svm_disgust.predict(testMat,TRUE);
	//flag_disgust = rtree_disgust.predict_prob(testMat);
	//cout << "disgust:" << flag_disgust << endl;
	//if (flag_disgust>temp)// && flag_disgust<0)
	//{
	//	temp=flag_disgust;
	//	result=5;
	//}

	//float flag_fear=0;
	////flag_fear = svm_fear.predict(testMat,TRUE);
	//flag_fear = rtree_fear.predict_prob(testMat);
	//cout << "fear:" << flag_fear << endl;
	//if (flag_fear>temp)// && flag_fear<0)
	//{
	//	temp=flag_fear;
	//	result=6;
	//}

	//float flag_sad=0;
	////flag_sad = svm_sad.predict(testMat,TRUE);
	//flag_sad = rtree_sad.predict_prob(testMat);
	//cout << "sad:" << flag_sad << endl;
	//if (flag_sad>temp)// && flag_sad<0)
	//{
	//	temp=flag_sad;
	//	result=7;
	//}

	//float flag_neutral=0;
	////flag_neutral = svm_neutral.predict(testMat,TRUE);
	//flag_neutral = rtree_neutral.predict_prob(testMat);
	//cout << "neutral:" << flag_neutral << endl;
	//if (flag_neutral>temp)// && flag_sad<0)
	//{
	//	temp=flag_neutral;
	//	result=8;
	//}
	//cout << endl;

	////�ѽ��д��ͼ����
	//CvFont font;
	//cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,1,0.5,0,2,8);
	//if (result==2)
	//{
	//	cvPutText(image1,"happy",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
	//	happy_result++;
	//}
	//else if (result==3)
	//{
	//	cvPutText(image1,"surprise",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
	//	surprise_result++;
	//}
	//else if (result==4)
	//{
	//	cvPutText(image1,"angry",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
	//	angry_result++;
	//}
	//else if (result==5)
	//{
	//	cvPutText(image1,"disgust",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
	//	disgust_result++;
	//}
	//else if (result==6)
	//{
	//	cvPutText(image1,"fear",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
	//	fear_result++;
	//}
	//else if (result==7)
	//{
	//	cvPutText(image1,"sad",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
	//	sad_result++;
	//}
	//else if (result==8)
	//{
	//	cvPutText(image1,"neutral",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
	//	neutral_result++;
	//}

	Mat testImg = image2;
	vector<float> descriptor;
	HOGDescriptor hog(Size(128,128),Size(16,16),Size(8,8),Size(8,8),9);//HOG���������������HOG�����ӵ�
	hog.compute(testImg,descriptor,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)
	int DescriptorDim = descriptor.size();

	CvMat *testMat = cvCreateMat(1, DescriptorDim, CV_32FC1);

	for (int i = 0; i<DescriptorDim; i++)
	{
		cvSetReal2D(testMat, 0, i, descriptor[i]); //����ȡ��ʶ��������ֵ��ȥ
	}

	int result=1;
	float temp=1.0f;

	//cout << endl;
	float flag_happy = 0;
	flag_happy = svm_happy.predict(testMat,TRUE);
	//flag_happy = rtree_happy.predict_prob(testMat);
	//cout << "happy:" << flag_happy << endl;
	if (flag_happy<temp)// && flag_happy<0)
	{
		temp=flag_happy;
		result=2;
	}

	float flag_surprise=0;
	flag_surprise = svm_surprise.predict(testMat,TRUE);
	//flag_surprise = rtree_surprise.predict_prob(testMat);
	//cout << "surprise:" << flag_surprise << endl;
	if (flag_surprise<temp)// && flag_surprise<0)
	{
		temp=flag_surprise;
		result=3;
	}

	float flag_angry=0;
	flag_angry = svm_angry.predict(testMat,TRUE);
	//flag_angry = rtree_angry.predict_prob(testMat);
	//cout << "angry:" << flag_angry << endl;
	if (flag_angry<temp)// && flag_angry<0)
	{
		temp=flag_angry;
		result=4;
	}

	float flag_disgust=0;
	flag_disgust = svm_disgust.predict(testMat,TRUE);
	//flag_disgust = rtree_disgust.predict_prob(testMat);
	//cout << "disgust:" << flag_disgust << endl;
	if (flag_disgust<temp)// && flag_disgust<0)
	{
		temp=flag_disgust;
		result=5;
	}

	float flag_fear=0;
	flag_fear = svm_fear.predict(testMat,TRUE);
	//flag_fear = rtree_fear.predict_prob(testMat);
	//cout << "fear:" << flag_fear << endl;
	if (flag_fear<temp)// && flag_fear<0)
	{
		temp=flag_fear;
		result=6;
	}

	float flag_sad=0;
	flag_sad = svm_sad.predict(testMat,TRUE);
	//flag_sad = rtree_sad.predict_prob(testMat);
	//cout << "sad:" << flag_sad << endl;
	if (flag_sad<temp)// && flag_sad<0)
	{
		temp=flag_sad;
		result=7;
	}

	float flag_neutral=0;
	flag_neutral = svm_neutral.predict(testMat,TRUE);
	//flag_neutral = rtree_neutral.predict_prob(testMat);
	//cout << "neutral:" << flag_neutral << endl;
	if (flag_neutral<temp)// && flag_sad<0)
	{
		temp=flag_neutral;
		result=8;
	}

	//float flag_ns=0;
	//flag_ns = svm_ns.predict(testMat,TRUE);
	////cout << "ns:" << flag_ns << endl;
	//if (flag_ns<temp)
	//{
	//	temp=flag_ns;
	//	result=7;
	//}
	//cout << endl;

	//�ѽ��д��ͼ����
	/*CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,1,0.5,0,2,8);*/
	if (result==2)
	{
		//cvPutText(image1,"happy",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		happy_result++;
	}
	else if (result==3)
	{
		//cvPutText(image1,"surprise",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		surprise_result++;
	}
	else if (result==4)
	{
		//cvPutText(image1,"angry",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		angry_result++;
	}
	else if (result==5)
	{
		//cvPutText(image1,"disgust",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		disgust_result++;
	}
	else if (result==6)
	{
		//cvPutText(image1,"fear",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		fear_result++;
	}
	else if (result==7)
	{
		//cvPutText(image1,"sad",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		//if(svm_neutral.predict(testMat,TRUE)>svm_sad.predict(testMat,TRUE)){
			sad_result++;
		//}
		//else{
		//	neutral_result++;
		//}
	}
	else if (result==8)
	{
		//cvPutText(image1,"neutral",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		neutral_result++;
	}
}

void emotion_detect_LBP_HOG(IplImage *image2, Rect select)
{
	double final_vector[feature_num]={0.0}; //��ʼ��ʶ������
	get_vector_AU(image2,final_vector);

	Mat testImg = image2;
	vector<float> descriptor;
	HOGDescriptor hog(Size(128,128),Size(16,16),Size(8,8),Size(8,8),9);//HOG���������������HOG�����ӵ�
	hog.compute(testImg,descriptor,Size(8,8));//����HOG�����ӣ���ⴰ���ƶ�����(8,8)
	int DescriptorDim = descriptor.size();

	CvMat *testMat = cvCreateMat(1, feature_num+DescriptorDim, CV_32FC1);
		
	for (int i = 0; i<feature_num; i++)
	{
		cvSetReal2D(testMat, 0, i, final_vector[i]); //����ȡ��ʶ��������ֵ��ȥ
	}

	for (int i = feature_num; i<feature_num+DescriptorDim; i++)
	{
		cvSetReal2D(testMat, 0, i, descriptor[i-feature_num]); //����ȡ��ʶ��������ֵ��ȥ
	}

	int result=1;
	float temp=1.0f;

	cout << endl;
	float flag_happy = 0;
	flag_happy = svm_happy.predict(testMat,TRUE);
	//flag_happy = rtree_happy.predict_prob(testMat);
	cout << "happy:" << flag_happy << endl;
	if (flag_happy<temp)// && flag_happy<0)
	{
		temp=flag_happy;
		result=2;
	}

	float flag_surprise=0;
	flag_surprise = svm_surprise.predict(testMat,TRUE);
	//flag_surprise = rtree_surprise.predict_prob(testMat);
	cout << "surprise:" << flag_surprise << endl;
	if (flag_surprise<temp)// && flag_surprise<0)
	{
		temp=flag_surprise;
		result=3;
	}

	float flag_angry=0;
	flag_angry = svm_angry.predict(testMat,TRUE);
	//flag_angry = rtree_angry.predict_prob(testMat);
	cout << "angry:" << flag_angry << endl;
	if (flag_angry<temp)// && flag_angry<0)
	{
		temp=flag_angry;
		result=4;
	}

	float flag_disgust=0;
	flag_disgust = svm_disgust.predict(testMat,TRUE)*10;
	//flag_disgust = rtree_disgust.predict_prob(testMat);
	cout << "disgust:" << flag_disgust << endl;
	if (flag_disgust<temp)// && flag_disgust<0)
	{
		temp=flag_disgust;
		result=5;
	}

	float flag_fear=0;
	flag_fear = svm_fear.predict(testMat,TRUE);
	//flag_fear = rtree_fear.predict_prob(testMat);
	cout << "fear:" << flag_fear << endl;
	if (flag_fear<temp)// && flag_fear<0)
	{
		temp=flag_fear;
		result=6;
	}

	float flag_sad=0;
	flag_sad = svm_sad.predict(testMat,TRUE);
	//flag_sad = rtree_sad.predict_prob(testMat);
	cout << "sad:" << flag_sad << endl;
	if (flag_sad<temp)// && flag_sad<0)
	{
		temp=flag_sad;
		result=7;
	}

	float flag_neutral=0;
	flag_neutral = svm_neutral.predict(testMat,TRUE);
	//flag_neutral = rtree_neutral.predict_prob(testMat);
	cout << "neutral:" << flag_neutral << endl;
	if (flag_neutral<temp)// && flag_sad<0)
	{
		temp=flag_neutral;
		result=8;
	}
	cout << endl;

	//�ѽ��д��ͼ����
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX,1,0.5,0,2,8);
	if (result==2)
	{
		cvPutText(image1,"happy",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		happy_result++;
	}
	else if (result==3)
	{
		cvPutText(image1,"surprise",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		surprise_result++;
	}
	else if (result==4)
	{
		cvPutText(image1,"angry",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		angry_result++;
	}
	else if (result==5)
	{
		cvPutText(image1,"disgust",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		disgust_result++;
	}
	else if (result==6)
	{
		cvPutText(image1,"fear",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		fear_result++;
	}
	else if (result==7)
	{
		cvPutText(image1,"sad",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		sad_result++;
	}
	else if (result==8)
	{
		cvPutText(image1,"neutral",cvPoint(select.x,select.y),&font,CV_RGB(255,0,0));
		neutral_result++;
	}
}