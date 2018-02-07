#pragma once

#include <string>
#include <vector>
#include <fstream>

struct Color {
    unsigned char r, g, b, a;

    Color() : r(0), g(0), b(0), a(0) {}

    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a): r(r), g(g), b(b), a(a) {}
};

struct Bitmap {
    int width;
    int height;
    std::vector<Color> colorData;

    Bitmap(int width, int height);

    void clear(const Color& clearColor);

    Color getPixel(const int x, const int y) const;

    void putPixel(const int x, const int y, const Color& color);
};

void writeBitmap(std::ofstream& file, const Bitmap& bitmap);
void writeBitmap(const std::string& filename, const Bitmap& bitmap);
void drawLine(Bitmap& bitmap, int x1, int y1, int x2, int y2, const Color& color);
void fillTriangle(Bitmap& bitmap, int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);
