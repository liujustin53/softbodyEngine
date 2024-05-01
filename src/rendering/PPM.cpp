#include "rendering/PPM.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

// Constructor loads a filename with the .ppm extension
PPM::PPM(std::string fileName) { parsePPM(fileName); }

// Saves a PPM Image to a new file.
void PPM::savePPM(std::string outFileName) const {
  std::ofstream outFile(outFileName);
  if (!outFile.is_open()) {
    throw std::invalid_argument("Unable to open file: " + outFileName);
  }

  // Write header
  outFile << "P3\n";
  outFile << _width << " " << _height << "\n";
  outFile << _maxColorValue << "\n";
  for (int i = 0; i < _pixelData.size(); i++) {
    outFile << (int)_pixelData[i] << "\n";
  }

  outFile.close();
}

// Sets a pixel to a specific R,G,B value
// Note: You do not *have* to use setPixel in your implementation, but
//       it may be useful to implement.
void PPM::setPixel(int x, int y, uint8_t R, uint8_t G, uint8_t B) {
  int index = (y * _width + x) * 3;
  _pixelData.at(index) = R;
  _pixelData.at(index + 1) = G;
  _pixelData.at(index + 2) = B;
}

void PPM::flipHorizontal() {
  std::vector<uint8_t> flippedData(_pixelData.size());
  for (int y = 0; y < _height; y++) {
    for (int x = 0; x < _width; x++) {
      int index = (y * _width + x) * 3;
      int flippedIndex = (y * _width + (_width - x - 1)) * 3;
      flippedData[flippedIndex] = _pixelData[index];
      flippedData[flippedIndex + 1] = _pixelData[index + 1];
      flippedData[flippedIndex + 2] = _pixelData[index + 2];
    }
  }
  _pixelData = flippedData;
}

void PPM::flipVertical() {
  std::vector<uint8_t> flippedData(_pixelData.size());
  for (int y = 0; y < _height; y++) {
    for (int x = 0; x < _width; x++) {
      int index = (y * _width + x) * 3;
      int flippedIndex = ((_height - y - 1) * _width + x) * 3;
      flippedData[flippedIndex] = _pixelData[index];
      flippedData[flippedIndex + 1] = _pixelData[index + 1];
      flippedData[flippedIndex + 2] = _pixelData[index + 2];
    }
  }
  _pixelData = flippedData;
}

// Parses a ppm file and stores the pixel data in _pixelData
void PPM::parsePPM(std::string fileName) {
  std::vector<uint8_t> pixels;

  std::ifstream ppmFile(fileName);
  if (!ppmFile.is_open()) {
    throw std::invalid_argument("Unable to open file: " + fileName);
  }

  bool foundP3 = false;

  // Read lines
  std::string line;
  while (std::getline(ppmFile, line)) {
    // Ignore comments
    if (line[0] == '#') {
      continue;
    }

    // Get P3
    if (!foundP3) {
      foundP3 = line == "P3";
      continue;
    }

    std::stringstream lineStream(line);

    // Get width and height
    if (_width == -1 || _height == -1) {
      lineStream >> _width >> _height;
      continue;
    }

    // Get max color value
    if (_maxColorValue == -1) {
      lineStream >> _maxColorValue;
      continue;
    }

    // Get a single RGB value
    int value;
    while (lineStream >> value) {
      pixels.push_back(value);
    }
  }

  if (pixels.size() != _width * _height * 3) {
    throw std::invalid_argument("Invalid file format");
  }

  ppmFile.close();
  _pixelData = pixels;
}
