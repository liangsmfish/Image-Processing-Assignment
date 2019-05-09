#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main()
{
    IplImage* img = cvLoadImage("2.jpg",CV_LOAD_IMAGE_COLOR);
    int height=cvGetDimSize(img,0);//讀取矩陣高度
    int width=cvGetDimSize(img,1);//讀取矩陣寬度

    
	//Laplacian  mask
	int mask_Laplacian[3][3] = { { -1,-1,-1 },{ -1,8,-1 },{ -1,-1,-1 } };
	
    //將laplacian mask ﹢原始圖 
	int mask_Laplacian_img[3][3] = { { -1,-1,-1 },{ -1,9,-1 },{ -1,-1,-1 } };

	//Sobel mask , 兩個 mask 對 x,y 方向分別做 sobel 邊緣偵測 處理
	int mask_sobel_x[3][3] = { { -1,-2,-1 },{ 0 ,0 ,0 },{ 1 ,2 ,1 } };
	int mask_sobel_y[3][3] = { { -1, 0, 1 },{-2 ,0 ,2 },{ -1,0 ,1 } };

	//Unsharp mask	
	float mask_Unsharp[3][3] = { {(float)1/9,(float)1/9,(float)1/9},{(float)1/9,(float)1/9,(float)1/9},{(float)1/9,(float)1/9,(float)1/9}};

	//每個pixel的值
	int img_pix, img_pix2;
	
    //存放RGB三通道的值
    int RGB[3];

	
    //做出一個Laplacian_Mask
    IplImage *img_Laplacian;
    CvSize img_Laplacian_size = cvSize(width, height);  //得到原圖SIZE
    img_Laplacian = cvCreateImage(img_Laplacian_size, IPL_DEPTH_8U,3);//創造img_Laplacian
	
    for(int y=1; y<height-1; y++)
    {
        for(int x=1; x<width-1; x++)
        {
            for(int i=0; i<3; i++)
            {
				//原圖*Laplacian_Mask
                img_pix = 
						  cvGet2D(img,y,x-1).val[i]   * mask_Laplacian[0][1]
						+ cvGet2D(img,y-1,x-1).val[i] * mask_Laplacian[0][0] 						
						+ cvGet2D(img,y+1,x-1).val[i] * mask_Laplacian[0][2]
						+ cvGet2D(img,y,x).val[i]     * mask_Laplacian[1][1] 
						+ cvGet2D(img,y-1,x).val[i]   * mask_Laplacian[1][0] 						
						+ cvGet2D(img,y,x+1).val[i]   * mask_Laplacian[1][2]
						+ cvGet2D(img,y+1,x).val[i]   * mask_Laplacian[2][1] 
						+ cvGet2D(img,y+1,x-1).val[i] * mask_Laplacian[2][0]
						+ cvGet2D(img,y+1,x+1).val[i] * mask_Laplacian[2][2];
                RGB[i] = img_pix;
            }
			    // BGR 順序相反
            cvSet2D(img_Laplacian,y,x,CV_RGB(RGB[2],RGB[1],RGB[0]));   
        }
    }

    cvSaveImage("Laplacian_Mask.jpg", img_Laplacian);
	
    //原始影像+Laplacian_Mask
    IplImage *img_Laplacian_ori;
    CvSize img_Laplacian_ori_size = cvSize(width, height);  //給影像SIZE
    img_Laplacian_ori = cvCreateImage(img_Laplacian_ori_size, IPL_DEPTH_8U,3);//創造img_Laplacian
	
     for(int y=1; y<height-1; y++)
    {
        for(int x=1; x<width-1; x++)
        {
            for(int i=0; i<3; i++) 
            {
				//原圖*Laplacian_Mask_img
                img_pix = 
						  cvGet2D(img,y,x-1).val[i]   * mask_Laplacian_img[0][1]
						+ cvGet2D(img,y-1,x-1).val[i] * mask_Laplacian_img[0][0] 						
						+ cvGet2D(img,y+1,x-1).val[i] * mask_Laplacian_img[0][2]
						+ cvGet2D(img,y,x).val[i]     * mask_Laplacian_img[1][1] 
						+ cvGet2D(img,y-1,x).val[i]   * mask_Laplacian_img[1][0] 						
						+ cvGet2D(img,y,x+1).val[i]   * mask_Laplacian_img[1][2]
						+ cvGet2D(img,y+1,x).val[i]   * mask_Laplacian_img[2][1] 
						+ cvGet2D(img,y+1,x-1).val[i] * mask_Laplacian_img[2][0]
						+ cvGet2D(img,y+1,x+1).val[i] * mask_Laplacian_img[2][2];
                RGB[i] = img_pix;
            }
            // BGR順序相反
            cvSet2D(img_Laplacian_ori,y,x,CV_RGB(RGB[2],RGB[1],RGB[0]));   
        }
    }
    cvSaveImage("Laplacian_Mask_img.jpg", img_Laplacian_ori);
   
    
    //sobel
    IplImage *img_sobel;
    CvSize img_sobelsize = cvSize(width, height); //給影像SIZE
    img_sobel = cvCreateImage(img_sobelsize, IPL_DEPTH_8U,3);//創造img_sobel
	//存放img_pix + img_pix2
	int sum_pix;
    for(int y=1; y<height-1; y++)
    {
        for(int x=1; x<width-1; x++)
        {
            for(int i=0; i<3; i++)
            {
				
				//對x方向做sobel
                img_pix = cvGet2D(img,y-1,x-1).val[i] * mask_sobel_x[0][0] 
				+ cvGet2D(img,y-1,x).val[i] 		  * mask_sobel_x[0][1]
				+ cvGet2D(img,y-1,x+1).val[i] 		  * mask_sobel_x[0][2]
                + cvGet2D(img,y,x-1).val[i]           * mask_sobel_x[1][0] 
				+ cvGet2D(img,y,x).val[i]             * mask_sobel_x[1][1]
				+ cvGet2D(img,y,x+1).val[i] 		  * mask_sobel_x[1][2]
                + cvGet2D(img,y+1,x-1).val[i] 		  * mask_sobel_x[2][0]
				+ cvGet2D(img,y+1,x).val[i] 		  * mask_sobel_x[2][1] 
				+ cvGet2D(img,y+1,x+1).val[i]		  * mask_sobel_x[2][2];
				
				
				//對y方向做sobel
                img_pix2 = cvGet2D(img,y-1,x-1).val[i] * mask_sobel_y[0][0]
				+ cvGet2D(img,y-1,x).val[i]    		   * mask_sobel_y[0][1]
				+ cvGet2D(img,y-1,x+1).val[i]		   * mask_sobel_y[0][2]
                + cvGet2D(img,y,x-1).val[i]			   * mask_sobel_y[1][0] 
				+ cvGet2D(img,y,x).val[i] 			   * mask_sobel_y[1][1] 
				+ cvGet2D(img,y,x+1).val[i] 		   * mask_sobel_y[1][2]
                + cvGet2D(img,y+1,x-1).val[i] 		   * mask_sobel_y[2][0] 
				+ cvGet2D(img,y+1,x).val[i] 		   * mask_sobel_y[2][1] 
				+ cvGet2D(img,y+1,x+1).val[i] 		   * mask_sobel_y[2][2];
				
				//絕對值 相加
                sum_pix = abs(img_pix) + abs(img_pix2);
				
				//若超過255以255計
				RGB[i]=sum_pix;
				if (sum_pix>255)
					RGB[i] = 255;
             
            }
            cvSet2D(img_sobel,y,x,CV_RGB(RGB[2],RGB[1],RGB[0]));
        }
    }
    cvSaveImage("Sobel.jpg", img_sobel);
    

    //sobel + 模糊處理
    IplImage *img_sobel_Unsharp;
    CvSize img_sobel_Unsharp_size = cvSize(width, height);  //給影像SIZE
    img_sobel_Unsharp = cvCreateImage(img_sobel_Unsharp_size, IPL_DEPTH_8U,3);//創造img_sobel_Unsharp
	
    for(int y=1; y<height-1; y++)
    {
        for(int x=1; x<width-1; x++)
        {
            for(int i=0; i<3; i++)
            {
				//這邊的點來源自前面的sobel
                img_pix = cvGet2D(img_sobel,y-1,x-1).val[i] 	* mask_Unsharp[0][0] 
				+ cvGet2D(img_sobel,y-1,x).val[i] 			    * mask_Unsharp[0][1]
				+ cvGet2D(img_sobel,y-1,x+1).val[i] 			* mask_Unsharp[0][2]
                + cvGet2D(img_sobel,y,x-1).val[i] 				* mask_Unsharp[1][0] 
				+ cvGet2D(img_sobel,y,x).val[i] 				* mask_Unsharp[1][1] 
				+ cvGet2D(img_sobel,y,x+1).val[i] 				* mask_Unsharp[1][2]
                + cvGet2D(img_sobel,y+1,x-1).val[i] 			* mask_Unsharp[2][0] 
				+ cvGet2D(img_sobel,y+1,x).val[i] 				* mask_Unsharp[2][1] 
				+ cvGet2D(img_sobel,y+1,x+1).val[i] 			* mask_Unsharp[2][2];
				
                RGB[i] = img_pix;
            }
            cvSet2D(img_sobel_Unsharp,y,x,CV_RGB(RGB[2],RGB[1],RGB[0]));
        }
    }
    cvSaveImage("Sobel_Unsharp.jpg", img_sobel_Unsharp);
   

    //(sobel+模糊+正規化)*img_Laplacian
	
	//正規化 先找出最大值的點 放至 find_RGB_max
	
	 float find_RGB_max[3] = {0} ;

    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            for(int i=0; i<3; i++)
            {
                if(cvGet2D(img_sobel_Unsharp,y,x).val[i] > find_RGB_max[i])
                    find_RGB_max[i] = cvGet2D(img_sobel_Unsharp,y,x).val[i];
            }
        }
    }
	
    IplImage *img_sobel_Laplacian_normal;
    CvSize img_sobel_Laplacian_normal_size = cvSize(width, height);  //給影像SIZE
    img_sobel_Laplacian_normal = cvCreateImage(img_sobel_Laplacian_normal_size, IPL_DEPTH_8U,3);//創造img_sobel_Laplacian
	
	float temp;	
	
    for(int y=0; y<height-1; y++)
    {
        for(int x=0; x<width-1; x++)
        {
            for(int i=0; i<3; i++)
            {
				//讀Laplacian
                temp = cvGet2D(img_Laplacian,y,x).val[i];
				//正規化 結果會介於0~1
                temp = temp * (float(cvGet2D(img_sobel_Unsharp,y,x).val[i]/ find_RGB_max[i]));
				
				RGB[i] = temp;             
            }
            cvSet2D(img_sobel_Laplacian_normal,y,x,CV_RGB(RGB[2],RGB[1],RGB[0]));
        }
    }
    cvSaveImage("img_sobel_Laplacian_normal*Laplacian.jpg", img_sobel_Laplacian_normal);    
	
	int final_pix;
	
	//最後加上原圖
	IplImage *img_final_output;
    CvSize img_final_output_size = cvSize(width, height);  //給影像SIZE
    img_final_output = cvCreateImage(img_final_output_size, IPL_DEPTH_8U,3);//創造img_sobel_Laplacian
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            for(int i=0; i<3; i++)            {
				
                img_pix = cvGet2D(img_sobel_Laplacian_normal,y,x).val[i];
				//原圖的pixel
                img_pix2 = cvGet2D(img,y,x).val[i];
				//原圖+正規化= 最終輸出
                final_pix = img_pix + img_pix2;

				//若超過255以255計
                RGB[i] = final_pix;
				if (final_pix>255)
					RGB[i] = 255;
            }
            cvSet2D(img_final_output,y,x,CV_RGB(RGB[2],RGB[1],RGB[0]));
        }
    }
    cvSaveImage("final_output.jpg", img_final_output);
    waitKey(0);
    return 0;
}
