#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h> 

using namespace cv;

int main() {

	// 讀取影像檔案，combime為最後的成果圖
	Mat image1, image2, combine;

	image1 = imread("l.jpg", CV_LOAD_IMAGE_COLOR);
	image2 = imread("r.jpg", CV_LOAD_IMAGE_COLOR);
	combine.create(1152, 4096, CV_8UC3);
	Vec3b abc;
	for (int j = 0; j< 1152; j++)
	{
		for (int i = 0; i< 4096; i++)
		{
			if (i< 2048 && j<1152)
			{
				abc = image1.at<Vec3b>(Point(i, j));
				combine.at<Vec3b>(Point(i, j)) = abc;
			}
		}
	}
	double a, b, c, d, e, f;
	a = 1.1539;
	b = -1.3772;
	c = 372.5642;
	d = -0.0049;
	e = 0.9584;
	f = 26.3633;

	
	for (int jj = 0; jj< 1152; jj++) {
		for (int ii = 0; ii< 4096; ii++) {
			double xx = a*ii + b*jj + c;
			double yy = d*ii + e*jj + f;
			int xxx = int(xx);
			int yyy = int(yy);

			if (xx >= 0 && yy >= 0 && xx<2048 && yy<1152)
			{
				double aa = xx - xxx;
				double bb = yy - yyy;

				abc = (1 - aa)*(1 - bb)*image2.at<Vec3b>(Point(xxx, yyy))
					+ aa*(1 - bb)*image2.at<Vec3b>(Point(xxx + 1, yyy))
					+ (1 - aa)*bb*image2.at<Vec3b>(Point(xxx, yyy + 1))
					+ aa*bb*image2.at<Vec3b>(Point(xxx + 1, yyy + 1));
				combine.at<Vec3b>(Point(ii, jj)) = abc;
			}
		}
	}
	// 建立視窗
	namedWindow("Display Image", WINDOW_AUTOSIZE);
	// 設定視窗大小
	resizeWindow("Display Image", 1200, 900);
	// 用視窗顯示影像
	imshow("Display Image", combine);
	// 顯示視窗，直到任何鍵盤輸入後才離開
	waitKey(0);

	return 0;
}

