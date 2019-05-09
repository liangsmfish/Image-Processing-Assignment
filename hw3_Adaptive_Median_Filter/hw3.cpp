#include <opencv2/opencv.hpp>
#include <random>
#include <ctime>
#include <iostream> 
#include <algorithm>  // sort() 函式

using namespace std;
using namespace cv;

int main()
{
	//以灰階讀入圖片
	IplImage *img = cvLoadImage("ori_img.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	int height = cvGetDimSize(img, 0);
	int width = cvGetDimSize(img, 1);
	int pixel;

	cvSaveImage("img_gray.jpg", img);

	//製作雜訊圖
	IplImage *img_noise;
	CvSize img_noise_size = cvSize(width, height);
	img_noise = cvCreateImage(img_noise_size, IPL_DEPTH_8U, 1);

	// 亂數產生器 
	default_random_engine generator(time(NULL));
	// 亂數的機率分布 
	uniform_real_distribution<float> unif(0.0, 1.0);

	//開始撒雜訊
	for (int y = 0; y<height; y++)
	{
		for (int x = 0; x<width; x++)
		{
			// 產生亂數 
			float random_num = unif(generator);

			pixel = cvGet2D(img, y, x).val[0];

			if (random_num >0 && random_num <0.25) {
				//介於0~0.25  	像素值 = 0
				pixel = 0;
				cvSet2D(img_noise, y, x, CV_RGB(0, 0, pixel));
			}
			else if (random_num>0.25 && random_num<0.5) {

				//介於0.25~0.5  像素值 = 255
				pixel = 255;
				cvSet2D(img_noise, y, x, CV_RGB(0, 0, pixel));
			}
			else 
				//介於0.5~1   	像素值保持不變            
				cvSet2D(img_noise, y, x, CV_RGB(0, 0, pixel));
			
		}
	}

	cvSaveImage("img_noise.jpg", img_noise);

	//去雜訊圖(Median Filter)
	IplImage *img_denoise_med;
	CvSize img_denoise_med_size = cvSize(width, height);
	img_denoise_med = cvCreateImage(img_denoise_med_size, IPL_DEPTH_8U, 1);

	int mask[49] , med; //mask size is 7x7
	int temp = 0;
	int m, n;
	for (m = 3; m < height - 3; m++)	//起始要從 (3,3) 開始 ，止於 (m-3,n-3)， 因為 mask = 7 x 7 ，才不會超過座標平面的範圍 
	{
		for (n = 3; n < width - 3; n++)
		{

			for (int a = -3; a <= 3; a++)      
			{									
				for (int b = -3; b <= 3; b++)	 
				{
					mask[temp] = cvGet2D(img_noise, m + a, n + b).val[0];  //把 img_noise 裡 3x3 windoow 讀入 mask_3
					temp++;
					if (a == 3 && b == 3)					
						temp = 0;		
				}
			}

			sort(mask, mask + 49); //sort( array_begin , array_end ): 由小到大排序

			med = mask[25]; 	// 中位數

			cvSet2D(img_denoise_med, m, n, CV_RGB(0, 0, med));  //med 當此位置的 pixel
		}
	}

	cvSaveImage("Denoise_med.jpg", img_denoise_med);

	//去雜訊圖(Adaptive Median Filter)
	IplImage *img_denoise;
	CvSize img_denoise_size = cvSize(width, height);
	img_denoise = cvCreateImage(img_denoise_size, IPL_DEPTH_8U, 1);

	//Adaptive Median Filter 
	int mask_3[9], mask_5[25], mask_7[49]; // S_max = 7 x 7
	int z_med, z_min, z_max, z_xy;
	int A1, A2, B1, B2;
	int y, x;
	int temp1 = 0, temp2 = 0, temp3 = 0;

	for (y = 3; y < height - 3; y++)	//起始要從 (3,3) 開始 ，止於 (x-3,y-3)， 因為如果 mask 變成 7 x 7 ，才不會超過座標平面的範圍 
	{
		for (x = 3; x < width - 3; x++)
		{
			z_xy = cvGet2D(img_noise, y, x).val[0]; // z_xy = img_noise 的像素值

			for (int a = -1; a <= 1; a++)      // -1 -1  1
			{									// -1  0  1
				for (int b = -1; b <= 1; b++)	// -1  1  1 
				{
					mask_3[temp1] = cvGet2D(img_noise, y + a, x + b).val[0];  //把 img_noise 裡 3x3 windoow 讀入 mask_3
					temp1++;

					if (a == 1 && b == 1)
					{
						temp1 = 0;
					}

				}
			}

			sort(mask_3, mask_3 + 9); //sort( array_begin , array_end ): 由小到大排序

			z_min = mask_3[0];  // 第一個最小
			z_med = mask_3[5]; 	// 中位數
			z_max = mask_3[8];	// 最後一個最大

			A1 = z_med - z_min;
			A2 = z_med - z_max;

			if (A1 > 0 && A2 < 0) {

				//level B		

				B1 = z_xy - z_min;
				B2 = z_xy - z_max;

				if (B1 > 0 && B2 < 0)
					cvSet2D(img_denoise, y, x, CV_RGB(0, 0, z_xy));  //z_xy is not noise
				else
					cvSet2D(img_denoise, y, x, CV_RGB(0, 0, z_med));	//z_xy is noise
			}
			else {
				//Increase the window size	5x5

				for (int a = -2; a <= 2; a++)
				{
					for (int b = -2; b <= 2; b++)
					{
						mask_5[temp2] = cvGet2D(img_noise, y + a, x + b).val[0];
						temp2++;

						if (a == 2 && b == 2)
						{
							temp2 = 0;
						}

					}
				}


				sort(mask_5, mask_5 + 25);

				z_min = mask_5[0];
				z_med = mask_5[13];
				z_max = mask_5[24];

				A1 = z_med - z_min;
				A2 = z_med - z_max;

				if (A1 > 0 && A2 < 0) {

					//level B

					B1 = z_xy - z_min;
					B2 = z_xy - z_max;

					if (B1 >0 && B2 < 0)
						cvSet2D(img_denoise, y, x, CV_RGB(0, 0, z_xy));
					else
						cvSet2D(img_denoise, y, x, CV_RGB(0, 0, z_med));
				}
				else {
					//Increase the window size	7x7

					for (int a = -3; a <= 3; a++)
					{
						for (int b = -3; b <= 3; b++)
						{
							mask_7[temp3] = cvGet2D(img_noise, y + a, x + b).val[0];
							temp3++;

							if (a == 3 && b == 3)
							{
								temp3 = 0;
							}
						}
					}

					sort(mask_7, mask_7 + 49);

					z_min = mask_7[0];
					z_med = mask_7[25];
					z_max = mask_7[48];

					A1 = z_med - z_min;
					A2 = z_med - z_max;

					if (A1 > 0 && A2 < 0) {

						//level B

						B1 = z_xy - z_min;
						B2 = z_xy - z_max;

						if (B1 > 0 && B2 < 0)
							cvSet2D(img_denoise, y, x, CV_RGB(0, 0, z_xy));
						else
							cvSet2D(img_denoise, y, x, CV_RGB(0, 0, z_med));

					}
					else {
						cvSet2D(img_denoise, y, x, CV_RGB(0, 0, z_med));
					}
				}
			}
		}
	}

	cvSaveImage("Denoise_adaptive.jpg", img_denoise);
	waitKey(0);
	return 0;

}
