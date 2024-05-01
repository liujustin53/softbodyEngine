/** @file PPM.hpp
 *  @brief Class for working with PPM images
 *
 *  Class for working with P3 PPM images specifically.
 *
 *  @author your_name_here
 *  @bug No known bugs.
 */
#ifndef PPM_HPP
#define PPM_HPP

#include <string>
#include <vector>

class PPM {
public:
  // Constructor loads a filename with the .ppm extension
  PPM(std::string fileName);

  // NOTE: commented out as a destructor is not needed here
  // Destructor clears any memory that has been allocated
  // ~PPM();

  // Saves a PFPM Image to a new file.
  void savePPM(std::string outputFileName) const;

  // Sets a pixel to a specific R,G,B value
  // Note: You do not have to use this function in your implementation,
  //       but it is probably a useful helper function to have.
  void setPixel(int x, int y, uint8_t R, uint8_t G, uint8_t B);

  void flipHorizontal();

  void flipVertical();

  // Returns the raw pixel data in an array.
  // You may research what 'inline' does.
  // You may also want to research what 'const' does on a function.
  // In brief, 'const' gaureentees that we are not modifying
  // any member variables in a class, and this is useful if we are
  // returning private member variables.
  std::vector<uint8_t> pixelData() const { return _pixelData; }

  uint8_t *pixelDataPtr() { return _pixelData.data(); }

  // Returns image width
  int getWidth() const { return _width; }

  // Returns image height
  int getHeight() const { return _height; }

  // NOTE:    You may add any helper functions you like in the
  //          private section.
private:
  // Parses a ppm file and stores the pixel data in _pixelData
  void parsePPM(std::string fileName);

  // Store the raw pixel data here
  // Data is R,G,B format
  // Note: Yes, you are allowed to replace 'uint8_t* m_PixelDatal' with a
  // std::vector<uint8_t> m_PixelData.
  //       In fact, using a std::vector will likely make your life easier.
  std::vector<uint8_t> _pixelData;
  // Store width and height of image.
  int _width{-1};
  int _height{-1};
  int _maxColorValue{-1};
};

#endif
