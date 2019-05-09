#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <math.h>


using namespace std;
using namespace cv;

int main()
{
	//��Ϥ��H�Ƕ�Ū�J
	Mat img = imread("sea.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	
	//���h���T�A���U��Canny�ĪG�~�|����n	
	GaussianBlur(img, img, Size(3, 3), 0, 0);
	
	Mat img_canny, img_threshold;
	
	int edgeThresh = 50;
	
	//Canny�A��X��t���ܥզ�

	Canny(img, img_canny, edgeThresh, edgeThresh*3, 3);
	
	//����v���A����t�ܶ�
	threshold(img_canny, img_threshold, 128, 255, THRESH_BINARY_INV);  

	imwrite("canny.jpg ", img_canny);
	imwrite("img_threshold.jpg ", img_threshold);	

	//�إ�hough�ϡA�T���y�Шt 
	
	int size =  640;// �v�����e : 384*384+512*512 �A�}�ڸ� 	= 640
	
	// 3600 = 360 / 0.1�A�H0.1�׬��@��� �AScalar(0) �N���l�������I���� �¦�	
	Mat hough(size, 3600, CV_8U, Scalar(0));
	
	float PI = 3.1415;
	
	int height= img_canny.rows; //��
	int width= img_canny.cols; //�e

	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			//��X��Ϯy�Шt�����I
			if (img_canny.at<uchar>(y,x) == 255) 
			{

			//�i�J theta�Arho �T�����Шt

				for (int theta = 0; theta < 3600; theta ++) 
				{
					for (int rho = 0; rho < size; rho ++) 
					{
						// x cos + y sin ���� 0
						if (
						    ((x*cos((double)(theta/10)* PI / 180.0) + y * sin((double)(theta/10)* PI / 180.0))
								-rho/10	> 0)
									&&
							((x*cos((double)(theta/10)* PI / 180.0) + y * sin((double)(theta/10)* PI / 180.0)) 
								-rho/10 < 1)
						   )
						{
							//�令�զ�
							hough.at<uchar>(rho, theta) = 255;
						}
					}
				}
			}
		}
	}
	
	imwrite("hough.jpg ", hough);
	waitKey(0);
	return 0;

}

