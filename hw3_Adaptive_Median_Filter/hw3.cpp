#include <opencv2/opencv.hpp>
#include <random>
#include <ctime>
#include <iostream> 
#include <algorithm>  // sort() �禡

using namespace std;
using namespace cv;

int main()
{
	//�H�Ƕ�Ū�J�Ϥ�
	IplImage *img = cvLoadImage("ori_img.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	int height = cvGetDimSize(img, 0);
	int width = cvGetDimSize(img, 1);
	int pixel;

	cvSaveImage("img_gray.jpg", img);

	//�s�@���T��
	IplImage *img_noise;
	CvSize img_noise_size = cvSize(width, height);
	img_noise = cvCreateImage(img_noise_size, IPL_DEPTH_8U, 1);

	// �üƲ��;� 
	default_random_engine generator(time(NULL));
	// �üƪ����v���� 
	uniform_real_distribution<float> unif(0.0, 1.0);

	//�}�l�����T
	for (int y = 0; y<height; y++)
	{
		for (int x = 0; x<width; x++)
		{
			// ���Ͷü� 
			float random_num = unif(generator);

			pixel = cvGet2D(img, y, x).val[0];

			if (random_num >0 && random_num <0.25) {
				//����0~0.25  	������ = 0
				pixel = 0;
				cvSet2D(img_noise, y, x, CV_RGB(0, 0, pixel));
			}
			else if (random_num>0.25 && random_num<0.5) {

				//����0.25~0.5  ������ = 255
				pixel = 255;
				cvSet2D(img_noise, y, x, CV_RGB(0, 0, pixel));
			}
			else 
				//����0.5~1   	�����ȫO������            
				cvSet2D(img_noise, y, x, CV_RGB(0, 0, pixel));
			
		}
	}

	cvSaveImage("img_noise.jpg", img_noise);

	//�h���T��(Median Filter)
	IplImage *img_denoise_med;
	CvSize img_denoise_med_size = cvSize(width, height);
	img_denoise_med = cvCreateImage(img_denoise_med_size, IPL_DEPTH_8U, 1);

	int mask[49] , med; //mask size is 7x7
	int temp = 0;
	int m, n;
	for (m = 3; m < height - 3; m++)	//�_�l�n�q (3,3) �}�l �A��� (m-3,n-3)�A �]�� mask = 7 x 7 �A�~���|�W�L�y�Х������d�� 
	{
		for (n = 3; n < width - 3; n++)
		{

			for (int a = -3; a <= 3; a++)      
			{									
				for (int b = -3; b <= 3; b++)	 
				{
					mask[temp] = cvGet2D(img_noise, m + a, n + b).val[0];  //�� img_noise �� 3x3 windoow Ū�J mask_3
					temp++;
					if (a == 3 && b == 3)					
						temp = 0;		
				}
			}

			sort(mask, mask + 49); //sort( array_begin , array_end ): �Ѥp��j�Ƨ�

			med = mask[25]; 	// �����

			cvSet2D(img_denoise_med, m, n, CV_RGB(0, 0, med));  //med ����m�� pixel
		}
	}

	cvSaveImage("Denoise_med.jpg", img_denoise_med);

	//�h���T��(Adaptive Median Filter)
	IplImage *img_denoise;
	CvSize img_denoise_size = cvSize(width, height);
	img_denoise = cvCreateImage(img_denoise_size, IPL_DEPTH_8U, 1);

	//Adaptive Median Filter 
	int mask_3[9], mask_5[25], mask_7[49]; // S_max = 7 x 7
	int z_med, z_min, z_max, z_xy;
	int A1, A2, B1, B2;
	int y, x;
	int temp1 = 0, temp2 = 0, temp3 = 0;

	for (y = 3; y < height - 3; y++)	//�_�l�n�q (3,3) �}�l �A��� (x-3,y-3)�A �]���p�G mask �ܦ� 7 x 7 �A�~���|�W�L�y�Х������d�� 
	{
		for (x = 3; x < width - 3; x++)
		{
			z_xy = cvGet2D(img_noise, y, x).val[0]; // z_xy = img_noise ��������

			for (int a = -1; a <= 1; a++)      // -1 -1  1
			{									// -1  0  1
				for (int b = -1; b <= 1; b++)	// -1  1  1 
				{
					mask_3[temp1] = cvGet2D(img_noise, y + a, x + b).val[0];  //�� img_noise �� 3x3 windoow Ū�J mask_3
					temp1++;

					if (a == 1 && b == 1)
					{
						temp1 = 0;
					}

				}
			}

			sort(mask_3, mask_3 + 9); //sort( array_begin , array_end ): �Ѥp��j�Ƨ�

			z_min = mask_3[0];  // �Ĥ@�ӳ̤p
			z_med = mask_3[5]; 	// �����
			z_max = mask_3[8];	// �̫�@�ӳ̤j

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
