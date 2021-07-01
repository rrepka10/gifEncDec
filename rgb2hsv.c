/*---------------------------------------------------------------------------
  This code converts between HSV and RGB space.  Building a color table in HSV
  space could result in a better table than RGB space because the human eye is
  more sensitive to H (luminosity) than color SV.
  
  gcc -g rgb2hav.c  -o rgb2hsv -lm -fsanitize=address -fsanitize=undefined

----------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>

double max(double a, double b, double c) {
   return ((a > b)? (a > c ? a : c) : (b > c ? b : c));
}

double min(double a, double b, double c) {
   return ((a < b)? (a < c ? a : c) : (b < c ? b : c));
}

int rgb_to_hsv(double r, double g, double b) {
   // R, G, B values are divided by 255
   // to change the range from 0..255 to 0..1:
   double h, s, v;
   r /= 255.0;
   g /= 255.0;
   b /= 255.0;
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
   printf("h s v=(%f, %f, %f)\n", h, s, v );
   return 0;
}


void HSVtoRGB(double H, double S,double V){
    if(H>360 || H<0 || S>100 || S<0 || V>100 || V<0){
        printf("The givem HSV values are not in valid range\n");
        return;
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
    int R = (r+m)*255;
    int G = (g+m)*255;
    int B = (b+m)*255;
    printf("rgb = %3d %3d %3d\n", R,G,B);
}


//main function
int main(int argc, char const *argv[]) {
   int r = 45, g = 215, b = 0;
   double h,s,v;
   
   printf("rgb = %3d %3d %3d\n", r,g,b);
   rgb_to_hsv(r, g, b);
   
   h = 107.441864;
   s = 100.000000;
   v = 84.313728;
   
   printf("hsv = %3f %3f %3f\n", h,s,v);
   HSVtoRGB(h,s,v);

   return 0;
}