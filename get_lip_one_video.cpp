#include <stdio.h>
#include <iostream>
#include <fstream>

//#include <io.h>
#include <stdlib.h>

#include "VIPLFaceRecognizer.h"
#include "VIPLPointDetector.h"
#include "VIPLFaceDetector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	//printf("%s",argv[1]);return 0;
	string video_name = string(argv[1]);	
	int save_face_flag = 0; // 0---no save face; 1 -- save face
	int frameId_flag = 200; // -1 full face; other: part face	
	
	//string ori_path = "/home/wch/桌面/相关程序1/get_lip_pro/build/all";
	string ori_path = "/home/wch/桌面/video";	
	string dst_path = "/home/wch/桌面/相关程序1/get_lip_pro/build/k";	
	string video_format = ".mp4";
	string video_fnm = ori_path + "/" + video_name + video_format;
	fstream _file;
	_file.open(video_fnm,ios::in);
	if (!_file){
		video_fnm = ori_path + "/" + video_name + ".MP4";
		_file.open(video_fnm,ios::in);
		if (!_file){
			video_fnm = ori_path + "/" + video_name + ".MOV";
			_file.open(video_fnm,ios::in);				
			if (!_file){
				video_fnm = ori_path + "/" + video_name + ".3gp";
				_file.open(video_fnm,ios::in);
			}	
		}
	}
	// for face detector	
	int32_t minFaceSize = 10;
	VIPLFaceDetector faceDetector("model/VIPLFaceDetector5.0.0.dat");	
	faceDetector.SetMinFaceSize(minFaceSize);
	faceDetector.SetScoreThresh(0.25, 0.25, 0.5);
	faceDetector.SetImagePyramidScaleFactor(1.414);// face detector
	
	VIPLPointDetector pointDetector("../model/VIPLPointDetector4.0.3.dat");// face point detector
	VIPLFaceRecognizer faceRecognizer;
	bool flag = faceRecognizer.LoadModel("../model/VIPLFaceRecognizer5.0.RN2.ext.dat");//face recognizer
	//if(!flag){
	//	system("pause");	
	//}
		
	Mat srcImg, imgGray, crop_face_gray_img, tmpImg, tmpImg2, lip_img, res_img, res_img1;
	Mat trans_srcImg, trans_2nd_srcImg, flip_srcImg;
	cv::VideoCapture cap(video_fnm.c_str());	
	
	if(!cap.isOpened()){
		std::cout << "Can't open video..." << endl;
		//system("pause");		
	}
	
	
	int frame_id = 0;
	int w, h;
	while(1)
	{
		if(cap.read(srcImg)){
			frame_id = frame_id + 1;
			if (frame_id == 1){
				imshow("srcImg", srcImg);	
				//waitKey();	
			}
			if (frame_id % 2 == 0)
				continue;
			if (frame_id > 300)
				break;
			cout<< frame_id << ": " << frame_id << ";ori_ w:" << w << "; ori_h:" << h << endl;
			
			w = srcImg.rows;
			h = srcImg.cols;
			
			/*double angle = 90;
			cv::Point2f center = cv::Point2f(h/2, w/2);
			tmpImg = cv::getRotationMatrix2D(center, angle, 1.0);
			cv::warpAffine(srcImg, flip_srcImg, tmpImg, srcImg.size());
			*/
			
			// rotate method 2
			srcImg.copyTo(flip_srcImg);			
						
			transpose(srcImg, trans_srcImg);
			//trans_srcImg.copyTo(flip_srcImg);
			
			flip(trans_srcImg, flip_srcImg, 0);
			
			//imshow("srcImg", srcImg);
			//waitKey();
			
			int save_frame_id = 188, save_flag = 0;//1;
			
			/*if (frame_id < save_frame_id - 3)
				continue;			
			
			if (frame_id > save_frame_id - 3 and frame_id < save_frame_id + 3 ){				
				imwrite(dst_path + '/' + std::to_string(frame_id) + "_frame.jpg", flip_srcImg);
				//continue;
			}
			
			if (frame_id > save_frame_id + 3)
				break;*/
			
			//imwrite(dst_path + '/' + std::to_string(frame_id) + "_frame.jpg", flip_srcImg);
			cout << "save_frame: "<< frame_id << endl;	
			string dst_img_name = dst_path + "/" + video_name + "_" + std::to_string(frame_id) + ".jpg";	
			
			//imwrite(dst_img_name, trans_srcImg);			
			//imshow("ori_img", srcImg);
			//imshow("trans_img", trans_srcImg);
			//imshow("flip_trans_img", flip_srcImg);			
			//waitKey();			
			/*dst_img_name = dst_path + "/trans_" + std::to_string(frame_id) + ".jpg";	
			transpose(srcImg, trans_tmp_srcImg);
			imwrite(dst_img_name, srcImg);*/
			
			
			if(flip_srcImg.channels()!=1)
				cvtColor(flip_srcImg, imgGray, COLOR_BGR2GRAY);
			else
				imgGray = flip_srcImg;
				
			// Mat ---> VIPLImageData
			VIPLImageData imgData(flip_srcImg.cols, flip_srcImg.rows, flip_srcImg.channels());
			imgData.data = flip_srcImg.data;
	
			VIPLImageData imgGrayData(imgGray.cols, imgGray.rows);
			imgGrayData.data = imgGray.data;
			imgGrayData.channels = 1;
	
			//face detect on srcImg
			vector<VIPLFaceInfo> faces = faceDetector.Detect(imgData);
			int32_t faceNum = faces.size();
			if(faceNum == 0){
				cout << "No Face..." << endl;
				
				continue;//return 0;
			}
	
			flip_srcImg.copyTo(tmpImg);
			// face point detection on gray data
			VIPLPoint5 points1, points2;
			pointDetector.PointDetectLandmarks(imgGrayData, faces[0], points1);
	
			// crop face on srcImg by face points
			cv::Mat dstImg(faceRecognizer.GetCropHeight(), faceRecognizer.GetCropWidth(), CV_8UC(faceRecognizer.GetCropChannels()));
			VIPLImageData dstImgData(dstImg.cols, dstImg.rows, dstImg.channels());
			dstImgData.data = dstImg.data;
			VIPLImageData srcImgData(flip_srcImg.cols, flip_srcImg.rows, flip_srcImg.channels());
			srcImgData.data = flip_srcImg.data;
			faceRecognizer.CropFace(srcImgData, points1, dstImgData);// crop face src--->dst
			
			// after crop: perform re_point detection for dstImgData
			// 1. crop_face_img ----> gray data
			cv::Mat crop_face_img(dstImgData.width, dstImgData.height, CV_8UC(dstImgData.channels));						
			crop_face_img.data = dstImgData.data;
			
			/*if (save_face_flag == 1 ){
				if (frameId_flag > 0 and frame_id > frameId_flag - 5 and frame_id < frameId_flag + 5){
					imwrite(dst_path + '/' + video_name + "_" + std::to_string(frame_id) + "_crop_frame.jpg", crop_face_img);
				}
				else // every frame
					imwrite(dst_path + '/' + video_name + "_" + std::to_string(frame_id) + "_crop_frame.jpg", crop_face_img);
				
			}*/
										
			cvtColor(crop_face_img, crop_face_gray_img, COLOR_BGR2GRAY);
	
			VIPLImageData crop_gray_data(crop_face_gray_img.cols, crop_face_gray_img.rows);
			crop_gray_data.data = crop_face_gray_img.data;
			crop_gray_data.channels = 1;
	
			// 2. face detection
			vector<VIPLFaceInfo> faces3;
			vector<VIPLFaceInfo> faces2 = faceDetector.Detect(dstImgData);
			int num_face = faces2.size();
			if(num_face==0){
				cout<<"No Face2..."<<endl;
				imwrite("no_face.jpg", crop_face_img);
		
				Mat tmp_img = imread("no_face.jpg");														
				VIPLImageData tmp_img_data(tmp_img.cols, tmp_img.rows, tmp_img.channels());
				tmp_img_data.data = tmp_img.data;
				faces3 = faceDetector.Detect(tmp_img_data);
				if(faces3.size()==0){
					cout<<"NO Face3..."<<endl;					
					continue;//return 1;		
				}		
				faces2=faces3;					
			}

			// 3. points detection
			crop_face_img.copyTo(tmpImg2);
			pointDetector.PointDetectLandmarks(crop_gray_data, faces2[0], points2);

			int st_pt_x = max(0, (int)(points2[0].x - 10));//
			int ed_pt_x = min(256,(int)(points2[1].x + 10));//
			int st_pt_y = max(0,(int)(points2[2].y));//
			int ed_pt_y = min(256, (int)(points2[3].y + points2[4].y - st_pt_y));//
			//int max_width = 
			//rectangle(tmpImg2, cv::Rect(st_pt_x, st_pt_y, ed_pt_x-st_pt_x, ed_pt_y-st_pt_y),cv::Scalar(255,0,0), 2, 8, 0);
			cv::Rect rect2(st_pt_x, st_pt_y, ed_pt_x-st_pt_x, ed_pt_y-st_pt_y);
			lip_img = tmpImg2(rect2);			
			
			lip_img.copyTo(res_img); // no resize
			//cv::resize(lip_img, res_img, cv::Size(64, 64),(0,0),(0,0));
		
			imwrite(dst_img_name, res_img);	
			cout<<"save:"<<dst_img_name<<endl;	
		}	
	}
		
	std::cout << "Ending..." << std::endl;
	return 0;
}
