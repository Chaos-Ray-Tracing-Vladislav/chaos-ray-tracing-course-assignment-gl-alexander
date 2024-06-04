#include "PPMImageSaver.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))


class Shape {
public: 
    virtual void createShape(Image& image) const = 0;
};

class Circle : public Shape{
    int center_x; 
    int center_y;
    unsigned radius;
    Color color;
    bool isInCircle(int x, int y) const {
        int diffX = x - center_x;
        int diffY = y - center_y;
        return diffX * diffX + diffY * diffY <= radius * radius; 
    }
public:
    Circle(unsigned radius, const Color& color) : Circle(IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2, radius, color) {}
    Circle(int center_x, int center_y, unsigned radius, const Color& color) : center_x(center_x), center_y(center_y), radius(radius), color(color) {}
    
    void createShape(Image& image) const override {
        // This limits the number of pixels we consider by only checking the square around the circle
        for (int rowId = max(center_y - radius, 0); rowId <= min(center_y + radius, IMAGE_HEIGHT - 1); rowId++) { // so we look only in the bounds of the image
            for (int colId = max(center_x - radius, 0); colId <= min(center_x + radius, IMAGE_WIDTH - 1); colId++) {
                if (isInCircle(colId, rowId)) {
                    image[rowId][colId] = color;
                }
            }
        }
    }

};

class Rectangle : public Shape{
    int center_x;
    int center_y;
    unsigned width;
    unsigned height;
    Color color;
public:
    Rectangle(unsigned width, unsigned height, const Color& color) :
        Rectangle(IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2, width, height, color) {}

    Rectangle(int center_x, int center_y, unsigned width, unsigned height, const Color& color) : 
        center_x(center_x), center_y(center_y), width(width), height(height), color(color) {
    }

    virtual void createShape(Image& image) const override {
        for (int rowId = max(center_y - height / 2, 0); rowId <= min(center_y + height / 2, IMAGE_HEIGHT - 1); rowId++) { // so we look only in the bounds of the image
            for (int colId = max(center_x - width / 2, 0); colId <= min(center_x + width / 2, IMAGE_WIDTH - 1); colId++) {
                image[rowId][colId] = color;
            }
        }
    }
};

class Square : public Rectangle {
public:
    Square(unsigned side, const Color& color) : Rectangle(side, side, color) {}
    Square(int center_x, int center_y, unsigned side, const Color& color) : Rectangle(center_x, center_y, side, side, color) {}
    void createShape(Image& image) const override {
        Rectangle::createShape(image);
    }
};

int main()
{
    Color background{ 50, 50, 50 };

    Color circleColor{ 10, 200, 200 };
    unsigned radiusInPixels = 200;
    Circle circle(radiusInPixels, circleColor);

    Color squareColor{ 200, 20, 20 };
    unsigned side = 300;
    Square square(IMAGE_WIDTH / 2 - 300, IMAGE_HEIGHT / 2, side, squareColor);

    Image image;
    image.assign(IMAGE_HEIGHT, std::vector<Color>(IMAGE_WIDTH, background)); // assigns all the memory in one call to avoid resizes
    
    square.createShape(image);
    circle.createShape(image);


    PPMImageSaver::saveImage("crt_figures.ppm", image);
}

