/*---------------------------------------------------------------------------
  This code converts between HSV and RGB space.  Building a color table in HSV
  space could result in a better table than RGB space because the human eye is
  more sensitive to H (luminosity) than color SV.
  
  gcc -g rgb2hav.c  -o rgb2hsv -lm -fsanitize=address -fsanitize=undefined

----------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "gifEncDec.h"

double max(double a, double b, double c) {
   return ((a > b)? (a > c ? a : c) : (b > c ? b : c));
}

double min(double a, double b, double c) {
   return ((a < b)? (a < c ? a : c) : (b < c ? b : c));
}

/*---------------------------------------------------------------------------
  This function converts a RGB pixel to a HSV pixel.  
  
  H should take the values 0..360
  S should take the values 0.100
  V should take the values 0..100
  
  HSV pixels are stored with the 9th bit of H in the 8th bit of S to keep the
  data structure 3 bytes wide
---------------------------------------------------------------------------*/
hsvPixel RGBtoHSV(pixel pix) {
   // R, G, B values are divided by 255
   // to change the range from 0..255 to 0..1:
   double h, s, v;
   double r, g, b;
   hsvPixel hsv;
   
   r = (double)pix.r / 255.0;
   g = (double)pix.g / 255.0;
   b = (double)pix.b / 255.0;
   
   double cmax = max(r, g, b); // maximum of r, g, b
   double cmin = min(r, g, b); // minimum of r, g, b
   double diff = cmax-cmin; // diff of cmax and cmin.
   if (cmax == cmin) {
      h = 0;
   }
   else if (cmax == r) {
      h = fmod((60 * ((g - b) / diff) + 360), 360.0);
   }
   else if (cmax == g) {
      h = fmod((60 * ((b - r) / diff) + 120), 360.0);
   }
   else if (cmax == b) {
      h = fmod((60 * ((r - g) / diff) + 240), 360.0);
   }
   // if cmax equal zero
      if (cmax == 0) {
         s = 0;
      }
      else {
         s = (diff / cmax) * 100;
      }
   // compute v
   v = cmax * 100;

   // Save the HSV values
   hsv.s = s+.5;
   hsv.v = v+.5;

   // Encode the extra H bit
   if (h > 255.0) {
      // Extra H bit, remove it and  
      hsv.h = h-255.0+.5;
      hsv.s |= S_MASK_HIGH;
   }
   else {   
      // Normal H bit`
      hsv.h = h+.5;
   }
   
   return(hsv);
}

/*---------------------------------------------------------------------------
  This function converts a HSV pixel back to a RGB pixel.  
  
  H should take the values 0..360
  S should take the values 0.100
  V should take the values 0..100
  HSV pixels are stored with the 9th bit of H in the 8th bit of S to keep the
  data structure 3 bytes wide
---------------------------------------------------------------------------*/
pixel HSVtoRGB(hsvPixel hsv){
    double H, S, V;
    pixel pix;
    H = hsv.h;
    S = hsv.s;
    V = hsv.v;
    
    // See if the H value needs a fix up
    if (hsv.s > 100) {
       // fixup needed
       S = (hsv.s & S_MASK_LOW);
       H += 255.0;
    }
    
    if(H>360.0 || H<0.0 ){
        printf("The given H %f value is not in valid range\n", H);
        exit(99);
    }
    
    if(S>100.0 || S<0.0){
        printf("The given S %f value is not in valid range\n", S);
        exit(99);
    }
    
    if(V>100.0 || V<0.0){
        printf("The given V %f value is not in valid range\n", V);
        exit(99);
    }
    double s = S/100;
    double v = V/100;
    double C = s*v;
    double X = C*(1-fabs(fmod(H/60.0, 2)-1));
    double m = v-C;
    double r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }
    pix.r = (r+m)*255.0+.5;
    pix.g = (g+m)*255.0+.5;
    pix.b = (b+m)*255.0+.5;
    return(pix);
}

#ifdef TESTRGB
//main function
int main(int argc, char const *argv[]) {
   pixel pix;
   hsvPixel hsvPix;

   pix.r = 0;
   pix.g = 0;
   pix.b = 0;
   printf("rgb = %3d %3d %3d\n", pix.r, pix.g, pix.b);
   hsvPix = RGBtoHSV(pix);
   printf("hsv = %3d %3d %3d\n", hsvPix.h, hsvPix.s, hsvPix.v );
   pix = HSVtoRGB(hsvPix);
   printf("rgb = %3d %3d %3d\n\n", pix.r, pix.g, pix.b);

   pix.r = 255;
   pix.g = 255;
   pix.b = 255;
   printf("rgb = %3d %3d %3d\n", pix.r, pix.g, pix.b);
   hsvPix = RGBtoHSV(pix);
   printf("hsv = %3d %3d %3d\n", hsvPix.h, hsvPix.s, hsvPix.v);
   pix = HSVtoRGB(hsvPix);
   printf("rgb = %3d %3d %3d\n\n", pix.r, pix.g, pix.b);
   
   pix.r = 45;
   pix.g = 215;
   pix.b = 0;
   printf("rgb = %3d %3d %3d\n", pix.r, pix.g, pix.b);
   hsvPix = RGBtoHSV(pix);  // bad
   printf("hsv = %3d %3d %3d\n", hsvPix.h, hsvPix.s, hsvPix.v);
   pix = HSVtoRGB(hsvPix);
   printf("rgb = %3d %3d %3d\n\n", pix.r, pix.g, pix.b);

   
   pix.r = 100;
   pix.g = 23;
   pix.b = 233;
   printf("rgb = %3d %3d %3d\n", pix.r, pix.g, pix.b);
   hsvPix = RGBtoHSV(pix);  // bad
   printf("hsv = %3d %3d %3d\n", hsvPix.h, hsvPix.s, hsvPix.v);
   pix = HSVtoRGB(hsvPix);
   printf("rgb = %3d %3d %3d\n\n", pix.r, pix.g, pix.b);


   return 0;
}
#endif