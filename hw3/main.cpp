#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <filesystem>

#include "Transform.h"
#include "raytrace.h"
#include <FreeImage.h>

using namespace std; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" 


void saveScreenshot(std::vector<BYTE> pixel_data) {
  int w = globals->width, h = globals->height;
  BYTE *pixels = &pixel_data[0];
  FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
  string fname = globals->output_file;
  std::cout << "Saving screenshot: " << fname << std::endl;

  FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
}

int main(int argc, char* argv[]) {
  FreeImage_Initialise();
  readfile(argv[1]) ; 
  saveScreenshot(raytrace());
  FreeImage_DeInitialise();
  return 0;
}
