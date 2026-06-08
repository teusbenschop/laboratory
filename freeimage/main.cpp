
int main(int argc, char *argv[])
{

  // Initialize freeimage library first
  ...


  // Allocate an 8-bit bitmap canvas.
  FIBITMAP* bitmap = FreeImage_Allocate(width, height, bpp);
  if (!bitmap)
    throw std::runtime_error("Failed to allocate bitmap memory");

  //        // Build a linear grayscale palette (0 = black, 255 = white).
  //        RGBQUAD* palette = FreeImage_GetPalette(bitmap);
  //        for (int i = 0; i < 256; ++i) {
  //            palette[i].rgbRed   = static_cast<BYTE>(i);
  //            palette[i].rgbGreen = static_cast<BYTE>(i);
  //            palette[i].rgbBlue  = static_cast<BYTE>(i);
  //        }

  // Populate pixel data: Generate a linear horizontal gradient.
  for (int y = 0; y < height; ++y) {
    // Get pointer to the start of row 'y'
    BYTE* bits = FreeImage_GetBits(bitmap) + y * FreeImage_GetPitch(bitmap);
    for (int x = 0; x < width; x++) {
      // Map the x-coordinate to a 0-255 palette index value.
      //                BYTE pixel_value = static_cast<BYTE>((static_cast<float>(x) / width) * 255);
      BYTE pixel_value = x < width/2 ? 0 : 255;
      bits[x] = pixel_value;
    }
  }

  // Save to disk as an 8-bit BMP file.
  const char* filename = "/tmp/bitmap1.bmp";
  if (!FreeImage_Save(FIF_BMP, bitmap, filename, BMP_DEFAULT))
    throw std::runtime_error("Failed to save the bitmap to file");

  // Clean up memory resources.
  FreeImage_Unload(bitmap);

  // Deinitialize freeimage library.

  return 0;
}
  
/*
 {
 #include <Magick++.h>
 #include <iostream>

 int main(int argc, char **argv) {
 // 1. Initialize the GraphicsMagick library resources (Required)
 Magick::InitializeMagick(*argv);

 try {
 // 2. Define the resolution (Width x Height)
 Magick::Geometry canvasSize(400, 300);

 // 3. Define the canvas color (supports Hex strings, "white", "transparent", etc.)
 Magick::Color backgroundColor("#FFFFFF"); // White background

 // 4. Instantiate the Image object (this allocates the underlying pixel array)
 Magick::Image bitmap(canvasSize, backgroundColor);

 // 5. Specify the output compression/format context explicitly as BMP
 bitmap.magick("BMP");

 //                 OPTIONAL: Manipulate individual pixels or draw shapes
 //                 Let's paint a single red pixel at coordinate (x: 100, y: 150)
 //                Magick::Color redColor("red");
 bitmap.pixelColor(100, 150, redColor);

 // 6. Write the image payload to the local file system
 bitmap.write("output_canvas.bmp");

 std::cout << "Bitmap canvas created and saved successfully!" << std::endl;
 }
 catch (Magick::Exception &error) {
 // Catch and handle runtime exceptions thrown by GraphicsMagick
 std::cerr << "GraphicsMagick Error: " << error.what() << std::endl;
 return 1;
 }

 return 0;
 }

 g++ main.cpp -o create_bitmap `pkg-config --cflags --libs GraphicsMagick++`
 ./create_bitmap

 }
 */
