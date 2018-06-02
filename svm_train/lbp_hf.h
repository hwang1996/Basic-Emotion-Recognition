#include "cv.h"
#include "ml.h"
#include "highgui.h"
#include "math.h"
#include "iostream"
#include <stdio.h>
#include <math.h>
#include <cxcore.h>


void FFT_Calculate_OneNode(int k);
void FFT_Calculate();
void re_initial();
void LBP_HF(IplImage* image, int num);
void get_vector(IplImage* image,double final_vector[]);
void get_vector_AU(IplImage* image,double final_vector[]);