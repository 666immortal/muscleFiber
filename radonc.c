/* Copyright 1993-2003 The MathWorks, Inc. */   
   
/* $Revision: 1.13.4.3 $  $Date: 2003/08/01 18:11:24 $ */   
   
/*  
 *   
 * RADONC.C .MEX file  
 * Implements Radon transform.  
 *  
 * Syntax  [P, r] = radon(I, theta)  
 *  
 * evaluates the Radon transform P, of I along the angles  
 * specified by the vector THETA.  THETA values measure   
 * angle counter-clockwise from the horizontal axis.    
 * THETA defaults to [0:179] if not specified.  
 *  
 * The output argument r is a vector giving the  
 * values of r corresponding to the rows of P.  
 *  
 * The origin of I is computed in the same way as origins  
 * are computed in filter2:  the origin is the image center  
 * rounded to the upper left.  
 *  
 * Algorithm  
 * The Radon transform of a point-mass is known, and the Radon  
 * transform is linear (although not shift-invariant).  So  
 * we use superposition（叠加） of the Radon transforms of each  
 * image pixel.  The dispersion（分布） of each pixel (point mass)  
 * along the r-axis is a nonlinear function of theta and the  
 * pixel location, so to improve the accuracy, we divide  
 * each pixel into 4 submasses located to the NE, NW, SE,   
 * and SW of the original location.  Spacing of the submasses  
 * is 0.5 in the x- and y-directions.  We also smooth   
 * the result in the r-direction by a triangular smoothing   
 * window of length 2.  
 *   
 * Reference  
 * Ronald N. Bracewell, Two-Dimensional Imaging, Prentice-Hall,  
 * 1995, pp. 518-525.  
 *  
 * S. Eddins 1-95  
 */   
   
#include <math.h>   
#include "mex.h"   
   
static void radon(double *pPtr, double *iPtr, double *thetaPtr, int M, int N,    
          int xOrigin, int yOrigin, int numAngles, int rFirst,    
          int rSize);   
   
static char rcs_id[] = "$Revision: 1.13.4.3 $";   
   
#define MAXX(x,y) ((x) > (y) ? (x) : (y))   
   
// 传入参数：不可修改
/* Input Arguments */   
#define I      (prhs[0])   // 传入参数：图像序列
#define THETA  (prhs[1])   // 传入参数：角度
#define OPTION (prhs[2])   
   
// 传出参数
/* Output Arguments */   
#define P      (plhs[0])   // 传出参数：P
#define R      (plhs[1])   // 传出参数：R
 
//nlhs是输出变量个数，plhs是输出变量的存储数组
//nrhs是输入变量个数，prhs是输入变量的存储数组
 
void    
mexFunction(int nlhs, mxArray  *plhs[], int nrhs, const mxArray  *prhs[])   
{   
	//这个函数其实不用怎么看的，这个主要是MATLAB与C语言混合编程时的一个接口函数，没有实现radon变换的核心功能
	//radon变换的核心功能在radon和incrementRadon函数中
	//由于radon函数的参数众多，所以可以参考这个函数，看那些参数是什么意思，怎么赋值
    int numAngles;          /* number of theta values */   
    double *thetaPtr;       /* pointer to theta values in radians */   
    double *pr1, *pr2;      /* double pointers used in loop */   
    double deg2rad;         /* conversion factor */   
    double temp;            /* temporary theta-value holder */   
    int k;                  /* loop counter */   
    int M, N;               /* input image size */   
    int xOrigin, yOrigin;   /* center of image */   
    int temp1, temp2;       /* used in output size computation */   
    int rFirst, rLast;      /* r-values for first and last row of output */   
    int rSize;              /* number of rows in output */   
     
    /* Check validity of arguments */   
    if (nrhs < 2)    
    {   
        mexErrMsgIdAndTxt("Images:radonc:tooFewInputs",   
                          "Too few input arguments");   
    }   
    if (nrhs > 2)   
    {   
        mexErrMsgIdAndTxt("Images:radonc:tooManyInputs",   
                          "Too many input arguments");   
    }   
    if (nlhs > 2)   
    {   
        mexErrMsgIdAndTxt("Images:radonc:tooMany Outputs",   
                          "Too many output arguments to RADON");   
    }   
   
    if (mxIsSparse(I) || mxIsSparse(THETA))   
    {   
        mexErrMsgIdAndTxt("Images:radonc:inputsMustBeNonsparse",   
                          "Sparse inputs not supported");   
    }   
   
    if (!mxIsDouble(I) || !mxIsDouble(THETA))   
    {   
        mexErrMsgIdAndTxt("Images:radonc:inputsIAndThetaMustBeDouble",   
                          "I and THETA must be double");   
    }   
       
    /* Get THETA values */   
    deg2rad = 3.14159265358979 / 180.0;   
    numAngles = mxGetM(THETA) * mxGetN(THETA);   
    thetaPtr = (double *) mxCalloc(numAngles, sizeof(double));   //MATLAB的内存申请函数，对应C语言可以换成calloc函数(calloc函数会初始化为0而malloc函数不会)
    pr1 = mxGetPr(THETA);   //获取矩阵的数组表达，返回数组的首地址
    pr2 = thetaPtr;   
    for (k = 0; k < numAngles; k++)   
        *(pr2++) = *(pr1++) * deg2rad; // 将角度转化为弧度  
     
    M = mxGetM(I);   //获得图像矩阵的行数
    N = mxGetN(I);   //获得图像矩阵的列数
   
    /* Where is the coordinate system's origin? */   
    xOrigin = MAXX(0, (N-1)/2);   // 选出最大值，找到图像的水平方向的中点，即坐标轴的横轴原点
    yOrigin = MAXX(0, (M-1)/2);   // 选出最大值，找到图像的垂直方向的中点，即坐标轴的纵轴原点
   
    /* How big will the output be? */   
    temp1 = M - 1 - yOrigin;   
    temp2 = N - 1 - xOrigin;   
    rLast = (int) ceil(sqrt((double) (temp1*temp1+temp2*temp2))) + 1;   //向上舍入为最接近的整数
    rFirst = -rLast;   // 关于原点对称
    rSize = rLast - rFirst + 1;   
   
    if (nlhs == 2) {   
        R = mxCreateDoubleMatrix(rSize, 1, mxREAL);   //创建一个rSize行1列的矩阵，其实也就是一个数组
        pr1 = mxGetPr(R);   
        for (k = rFirst; k <= rLast; k++)   
            *(pr1++) = (double) k;   
    }   
   
    /* Invoke main computation routines */   
    if (mxIsComplex(I)) {   
        P = mxCreateDoubleMatrix(rSize, numAngles, mxCOMPLEX);  //创建一个rSize行numAngles列的矩阵 
        radon(mxGetPr(P), mxGetPr(I), thetaPtr, M, N, xOrigin, yOrigin,    
              numAngles, rFirst, rSize);    //mxGetPr函数可以把刚才那个矩阵变成一个一维数组形式的存储方式，并返回一维数组的数组首地址
        radon(mxGetPi(P), mxGetPi(I), thetaPtr, M, N, xOrigin, yOrigin,    
              numAngles, rFirst, rSize);   
    } else {   
        P = mxCreateDoubleMatrix(rSize, numAngles, mxREAL);   
        radon(mxGetPr(P), mxGetPr(I), thetaPtr, M, N, xOrigin, yOrigin,    
              numAngles, rFirst, rSize);   
    }   
}   
/**
**pr：进行radon变换后输出矩阵的对于一个特定theta角的列的首地址
**pixel：要进行radon变换的像素值乘以0.25以后的值（由于每一个像素点取了相邻四个点提高精度，
**故在计算时pixel也要相应乘以0.25，类似于一个点占0.25的比例，然后四个点刚好凑足1的份额）
**r：进行radon变换的该点与初始的r值——rFirst之间的差
**/ 
void incrementRadon(double *pr, double pixel, double r)   
{   
    int r1;   
    double delta;   
   
    r1 = (int) r;   //对于每一个点，r值不同，所以，通过这种方式，可以把这一列中相应行的元素的值给赋上
    delta = r - r1;   
    pr[r1] += pixel * (1.0 - delta); //radon变换本来就是通过记录目标平面上某一点的被映射后点的积累厚度来反推原平面的直线的存在性的，故为+=  
    pr[r1+1] += pixel * delta;  //两个点互相配合，提高精度 
}   
/***
**参数解释：
**pPtr：经过radon变换后输出的一维数组，该一维数组是其实要还原成一个rSize*numAngles的矩阵
**iPtr：需要进行radon变换的矩阵的一维数组存储形势
**thetaPtr：指定进行radon变换的弧度的数组，该角度就是极坐标中偏离正方向的角度
**M、N：输入图像的行数和列数
**M：要进行radon变换的矩阵的行数
**N：要进行radon变换的矩阵的列数
**xOrigin：要进行radon变换的矩阵的的中心的横坐标
**yOrigin：要进行radon变换的矩阵的中心的纵坐标
**numAngles：thetaPtr数组中元素的个数
**rFist：极坐标中初始点与变换原点的距离
**rSize：整个radon变换中极坐标的点之间的最远距离
***/ 
static void radon(double *pPtr, double *iPtr, double *thetaPtr, int M, int N,    
      int xOrigin, int yOrigin, int numAngles, int rFirst, int rSize)   
{   
    int k, m, n;              /* loop counters */   
    double angle;             /* radian angle value */   
    double cosine, sine;      /* cosine and sine of current angle */   
    double *pr;               /* points inside output array */   
    double *pixelPtr;         /* points inside input array */   
    double pixel;             /* current pixel value */   
    double *ySinTable, *xCosTable;   
    /* tables for x*cos(angle) and y*sin(angle) */   
    double x,y;   
    double r, delta;   
    int r1;   
   
    /* Allocate space for the lookup tables */   
    xCosTable = (double *) mxCalloc(2*N, sizeof(double));  //MATLAB的内存申请函数，对应C语言可以换成calloc函数 
    ySinTable = (double *) mxCalloc(2*M, sizeof(double));   
   
    for (k = 0; k < numAngles; k++) 
	{   
		//每一个theta角，经过radon变化后，就会产生一列数据，这一列数据中，共有rSize个数据
        angle = thetaPtr[k];   
        pr = pPtr + k*rSize;  /* pointer to the top of the output column */   
        cosine = cos(angle);    
        sine = sin(angle);      
   
        /* Radon impulse response locus:  R = X*cos(angle) + Y*sin(angle) */   
        /* Fill the X*cos table and the Y*sin table.                      */   
        /* x- and y-coordinates are offset from pixel locations by 0.25 */   
        /* spaced by intervals of 0.5. */   
		/*
		**radon 变换中，极坐标下，沿r轴的theta角和每一个像素点的分布都是非线性的，而此处采用的是线性radon变换，
		**为了提高精度，把每一个像素点分成其四周四个相邻的像素点来进行计算！x、y坐标的误差是正负0.25
		*/
        for (n = 0; n < N; n++)   
        {   
            x = n - xOrigin;   
            xCosTable[2*n]   = (x - 0.25)*cosine;   //由极坐标的知识知道，相对于变换的原点，这个就是得到了该点的横坐标
            xCosTable[2*n+1] = (x + 0.25)*cosine;   
        }   
        for (m = 0; m < M; m++)   
        {   
            y = yOrigin - m;   
            ySinTable[2*m] = (y - 0.25)*sine;   //同理，相对于变换的原点，得到了纵坐标
            ySinTable[2*m+1] = (y + 0.25)*sine;   
        }   
   
        pixelPtr = iPtr;   
        for (n = 0; n < N; n++)   
        {   
            for (m = 0; m < M; m++)   //遍历原矩阵中的每一个像素点
            {   
                pixel = *pixelPtr++;   
                if (pixel != 0.0)   //如果该点像素值不为0，也即图像不连续
                {   
                    pixel *= 0.25;   
					
					//一个像素点分解成四个临近的像素点进行计算，提高精确度
                    r = xCosTable[2*n] + ySinTable[2*m] - rFirst;   
                    incrementRadon(pr, pixel, r);   
   
                    r = xCosTable[2*n+1] + ySinTable[2*m] - rFirst;   
                    incrementRadon(pr, pixel, r);   
   
                    r = xCosTable[2*n] + ySinTable[2*m+1] - rFirst;   
                    incrementRadon(pr, pixel, r);   
   
                    r = xCosTable[2*n+1] + ySinTable[2*m+1] - rFirst;   
                    incrementRadon(pr, pixel, r);   
                }   
            }   
        }   
    }   
                   
    mxFree((void *) xCosTable);   //MATLAB的内存释放函数，对应C语言可以换成free函数
    mxFree((void *) ySinTable);   
}  