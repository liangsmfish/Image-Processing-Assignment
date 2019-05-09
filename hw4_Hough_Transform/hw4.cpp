#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <math.h>


using namespace std;
using namespace cv;

int main()
{
	//把圖片以灰階讀入
	Mat img = imread("sea.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	
	//先去雜訊，等下做Canny效果才會比較好	
	GaussianBlur(img, img, Size(3, 3), 0, 0);
	
	Mat img_canny, img_threshold;
	
	int edgeThresh = 50;
	
	//Canny，找出邊緣並變白色

	Canny(img, img_canny, edgeThresh, edgeThresh*3, 3);
	
	//反轉影像，讓邊緣變黑
	threshold(img_canny, img_threshold, 128, 255, THRESH_BINARY_INV);  

	imwrite("canny.jpg ", img_canny);
	imwrite("img_threshold.jpg ", img_threshold);	

	//建立hough圖，三角座標系 
	
	int size =  640;// 影像高寬 : 384*384+512*512 再開根號 	= 640
	
	// 3600 = 360 / 0.1，以0.1度為一單位 ，Scalar(0) 代表初始全部的點都為 黑色	
	Mat hough(size, 3600, CV_8U, Scalar(0));
	
	float PI = 3.1415;
	
	int height= img_canny.rows; //高
	int width= img_canny.cols; //寬

	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			//找出原圖座標系的白點
			if (img_canny.at<uchar>(y,x) == 255) 
			{

			//進入 theta，rho 三角坐標系

				for (int theta = 0; theta < 3600; theta ++) 
				{
					for (int rho = 0; rho < size; rho ++) 
					{
						// x cos + y sin 接近 0
						if (
						    ((x*cos((double)(theta/10)* PI / 180.0) + y * sin((double)(theta/10)* PI / 180.0))
								-rho/10	> 0)
									&&
							((x*cos((double)(theta/10)* PI / 180.0) + y * sin((double)(theta/10)* PI / 180.0)) 
								-rho/10 < 1)
						   )
						{
							//改成白色
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

