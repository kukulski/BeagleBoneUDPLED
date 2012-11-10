#include "TCLFast.hxx"
#include <math.h>

float sGammaR[256], sGammaG[256], sGammaB[256];

void DitherPixel::initGamma(double gamma_red, double gamma_green, double gamma_blue) {
   int i;
  
  for(i=0;i<256;i++) {
    sGammaR[i] = pow(i/255.0,gamma_red)*255.0;
    sGammaG[i] = pow(i/255.0,gamma_green)*255.0;
    sGammaB[i] = pow(i/255.0,gamma_blue)*255.0;
  }
