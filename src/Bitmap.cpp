#include <Bitmap.hpp>

#include <algorithm>

Bitmap::Bitmap(int width, int height) {
    this->width = width;
    this->height = height;
    this->colorData = std::vector<Color>(width * height);
}

void Bitmap::clear(const Color& clearColor) {
    for (auto& color : colorData)
        color = clearColor;
}

Color Bitmap::getPixel(const int x, const int y) const {
    return colorData[width * y + x];
}

void Bitmap::putPixel(const int x, const int y, const Color& color) {
    if (x < 0 || y < 0 || x >= width || y >= height)
        return;

    colorData[width * y + x] = color;
}

void write32(std::ofstream& s, unsigned int data) {
    s.write(reinterpret_cast<const char*>(&data), sizeof data);
}

void write16(std::ofstream& s, unsigned short data) {
    s.write(reinterpret_cast<const char*>(&data), sizeof data);
}

void write8(std::ofstream& s, unsigned char data) {
    s.write(reinterpret_cast<const char*>(&data), sizeof data);
}

void writeBitmap(std::ofstream& file, const Bitmap& bitmap) {
    // File Header
    file << "BM";
    write32(file, 54+(bitmap.width * bitmap.height * 4));  // file size
    write16(file, 0); // reserved
    write16(file, 0); // reserved
    write32(file, 54); // offset to image data

    // Image Header
    write32(file, 40); // header size
    write32(file, bitmap.width); // width
    write32(file, bitmap.height); // height
    write16(file, 1); // plane
    write16(file, 32); // bpp
    write32(file, 0); // compression type
    write32(file, 0); // image size (zero for uncompressed)
    write32(file, 0); // DPI X
    write32(file, 0); // DPI Y
    write32(file, 0); // Number Color Map entries that are actually used
    write32(file, 0); // Number of significant colors

    for (int y = bitmap.height-1; y >= 0; y--)
        for (int x = 0; x < bitmap.width; x++) {
            auto color = bitmap.getPixel(x, y);
            write8(file, color.b); // B
            write8(file, color.g); // G
            write8(file, color.r); // R
            write8(file, color.a);
        }
}

void writeBitmap(const std::string& filename, const Bitmap& bitmap) {
    auto file = std::ofstream(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    writeBitmap(file, bitmap);
    file.close();
}

void drawLine(Bitmap& bitmap, int x1, int y1, int x2, int y2, const Color& color) {
    int width = x2 - x1;
    int height = y2 - y1;

    if (std::abs(width) >= std::abs(height)) {
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
            width = -width;
            height = -height;
        }

        for (int i=0; i<=width; i++) {
            float t = (float)i / width;

            bitmap.putPixel(x1 + i, y1 + (t * height), color);
        }
    } else {
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
            width = -width;
            height = -height;
        }

        for (int i=0; i<=height; i++) {
            float t = (float)i / height;

            bitmap.putPixel(x1 + (t * width), y1 + (i), color);
        }
    }
}

void fillBottomFlatTriangle(Bitmap& bitmap, int x1, int y1, int x2, int y2, int x3, int y3, const Color& color)
{
    float invslope1 = float(x2 - x1) / float(y2 - y1);
    float invslope2 = float(x3 - x1) / float(y3 - y1);

    float curx1 = x1;
    float curx2 = x1;

    for (int scanlineY = y1; scanlineY <= y2; scanlineY++)
    {
        drawLine(bitmap, (int)curx1, scanlineY, (int)curx2, scanlineY, color);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void fillTopFlatTriangle(Bitmap& bitmap, int x1, int y1, int x2, int y2, int x3, int y3, const Color& color)
{
  float invslope1 = float(x3 - x1) / float(y3 - y1);
  float invslope2 = float(x3 - x2) / float(y3 - y2);

  float curx1 = x3;
  float curx2 = x3;

  for (int scanlineY = y3; scanlineY > y1; scanlineY--)
  {
    drawLine(bitmap, (int)curx1, scanlineY, (int)curx2, scanlineY, color);
    curx1 -= invslope1;
    curx2 -= invslope2;
  }
}

void fillTriangle(Bitmap& bitmap, int x1, int y1, int x2, int y2, int x3, int y3, const Color& color) {
    // sort vertexes by y value
    if (y2 < y1) { std::swap(x1, x2); std::swap(y1, y2); }
    if (y3 < y2) { std::swap(x2, x3); std::swap(y2, y3); }
    if (y2 < y1) { std::swap(x1, x2); std::swap(y1, y2); }

    if (y2 == y3)
    {
        fillBottomFlatTriangle(bitmap, x1, y1, x2, y2, x3, y3, color);
    }
    else if (y1 == y2)
    {
        fillTopFlatTriangle(bitmap,  x1, y1, x2, y2, x3, y3, color);
    }
    else
    {
        auto x4 = (int)(x1 + (float(y2 - y1) / float(y3 - y1)) * float(x3 - x1));
        auto y4 = y2;

        fillBottomFlatTriangle(bitmap, x1, y1, x2, y2, x4, y4, color);
        fillTopFlatTriangle(bitmap,  x2, y2, x4, y4, x3, y3, color);

    }
}
