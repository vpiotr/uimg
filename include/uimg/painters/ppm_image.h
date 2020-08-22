#ifndef __UIMG_PPM_IMAGE_H__
#define __UIMG_PPM_IMAGE_H__

#include <ostream>
#include <fstream>
#include <string>

#include "uimg/painters/base.h"
#include "uimg/painters/rgb_image.h"

// class which writes RGB image as PPM file (Netpbm / P6)
class PpmWriter : public PixelImageWriter {
public:
    PpmWriter(std::basic_ostream<char> &output) : output_(output) {}

    virtual void writeImage(PixelImageBase &image) {
        writeHeader(image);
        writePixelMap(image);
    }

protected:
    void writePixelMap(PixelImageBase &image) {
        RgbColor color;
        char colorData[3];

        for (int y = 0, eposy = image.height(); y < eposy; ++y) {
            for (int x = 0, eposx = image.width(); x < eposx; ++x) {
                color = image.getPixel(Point(x, y));
                colorData[0] = color.red;
                colorData[1] = color.green;
                colorData[2] = color.blue;
                output_.write(static_cast<char *>(colorData), sizeof(colorData));
            }
        }
    }

    size_t writeHeader(PixelImageBase &image) {
        char ppmhead[40];

        sprintf(ppmhead, "P6\n%u %u\n255\n", image.width(), image.height());

        size_t len = std::char_traits<char>::length(ppmhead);

        output_.write(ppmhead, len);
        return len;
    }

    std::basic_ostream<char> &getOutput() { return output_; }

private:
    std::basic_ostream<char> &output_;
};

class PpmWriterForRgbImage : public PpmWriter {
public:
    PpmWriterForRgbImage(std::basic_ostream<char> &output) : PpmWriter(output) {}

    virtual void writeImage(PixelImageBase &image) {
        writeHeader(image);
        writePixelMap(dynamic_cast<RgbImage &>(image));
    }

protected:

    void writePixelMap(RgbImage &image) {
        getOutput().write(static_cast<char *>(image.data()), image.dataSize());
    }
};

class PpmImageLoader : public PixelImageLoader {
public:
    enum ErrorCodes {
        HEADER_INCORRECT
    };

    PpmImageLoader(std::basic_istream<char> &input) : input_(input) {}

    virtual ~PpmImageLoader() {}

    virtual PixelImageMetaInfo *loadImageMeta() {
        std::unique_ptr<PixelImageMetaInfoBase> meta(new PixelImageMetaInfoBase);
        if (loadHeader(*meta))
            return meta.release();
        else
            return nullptr;
    }

    virtual PixelImageBase *loadImage() {
        std::unique_ptr<PixelImageMetaInfo> meta(loadImageMeta());
        if (!meta.get())
            return nullptr;
        std::unique_ptr<PixelImageBase> outImage(newImage(*meta));

        if (outImage.get()) {
            if (loadPixelDataNew(*outImage))
                return outImage.release();
        }

        return nullptr;
    }

    virtual PixelImageBase *loadImagePart(const Rect &srcPart, const Point &targetOffset) {
        std::unique_ptr<PixelImageMetaInfo> meta(loadImageMeta());
        if (!meta.get())
            return nullptr;
        std::unique_ptr<PixelImageBase> outImage(newImage(*meta));

        if (outImage.get()) {
            if (loadPixelDataNewPart(*outImage, srcPart, targetOffset))
                return outImage.release();
        }

        return nullptr;
    }

    virtual bool loadImageInto(PixelImageBase &outputImage) {
        std::unique_ptr<PixelImageMetaInfo> meta(loadImageMeta());
        if (!meta.get())
            return false;
        Point srcSize = meta->getSize();
        Rect srcPart;
        Point topLeftOffset(0, 0);
        srcPart.topLeft(topLeftOffset).size(srcSize);

        return loadPixelDataInto(outputImage, srcSize, srcPart, topLeftOffset);
    }

    virtual bool loadImagePartInto(PixelImageBase &outputImage, const Rect &srcPart, const Point &targetOffset) {
        std::unique_ptr<PixelImageMetaInfo> meta(loadImageMeta());
        if (!meta.get())
            return false;
        Point srcSize = meta->getSize();

        return loadPixelDataInto(outputImage, srcSize, srcPart, targetOffset);
    }

protected:
    virtual PixelImageBase *newImage(const PixelImageMetaInfo &meta) { return nullptr; }

    virtual bool loadPixelDataNew(PixelImageBase &outputImage) {
        Point srcSize = outputImage.getSize();
        Rect srcFragment;
        Point offset(0, 0);
        srcFragment.topLeft(offset).size(srcSize);
        return loadPixelDataInto(outputImage, srcSize, srcFragment, offset);
    }

    virtual bool loadPixelDataNewPart(PixelImageBase &outputImage, const Rect &srcPart, const Point &targetOffset) {
        Point srcSize = outputImage.getSize();
        return loadPixelDataInto(outputImage, srcSize, srcPart, targetOffset);
    }

    virtual bool loadPixelDataInto(PixelImageBase &outputImage, const Point &srcSize, const Rect &srcFragment,
                                   const Point &destOffset) {
        Point destSize = outputImage.getSize();
        int xd, yd;
        RgbColor color;
        char colorBytes[3];
        bool result = true;

        for (int y = 0, eposy = srcSize.y; y < eposy; ++y) {
            for (int x = 0, eposx = srcSize.x; x < eposx; ++x) {
                if (!input_.read(colorBytes, sizeof(colorBytes))) {
                    result = false;
                    break;
                }

                if (x >= srcFragment.x1 && x <= srcFragment.x2 && y >= srcFragment.y1 && y <= srcFragment.y2) {
                    color.red = colorBytes[0];
                    color.green = colorBytes[1];
                    color.blue = colorBytes[2];

                    xd = x - srcFragment.x1 + destOffset.x;
                    yd = x - srcFragment.y1 + destOffset.y;

                    if (xd >= 0 && xd < destSize.x && yd >= 0 && yd < destSize.y) {
                        outputImage.setPixel(Point(xd, yd), color);
                    }
                }
            }
        }

        return result;
    }

    bool loadHeader(PixelImageMetaInfoBase &output) {
        std::string s;
        unsigned int u1, u2;

        bool correct = false;
        if (std::getline(input_, s)) {
            if (strncmp(s.c_str(), "P6", 2) == 0) {
                correct = true;
            }
        }

        if (correct) {
            correct = false;

            if (std::getline(input_, s)) {
                if (sscanf(s.c_str(), "%u %u", &u1, &u2) == 2) {
                    correct = true;
                }
            }
        }

        if (correct) {
            correct = false;

            if (std::getline(input_, s)) {
                if (strncmp(s.c_str(), "255", 3) == 0) {
                    output.setSize(Point(u1, u2));
                    return true;
                }
            }
        }

        handleError(HEADER_INCORRECT);
        return false;
    }


    virtual void handleError(unsigned int errorCode) {
        // does nothing
    }

    std::istream &getInput() { return input_; }

private:
    std::istream &input_;
};

// optimized version of PPM image loader for RgbImage target
class PpmImageLoaderForRgbImage : public PpmImageLoader {
    typedef PpmImageLoader inherited;
protected:
    virtual PixelImageBase *newImage(const PixelImageMetaInfo &meta) {
        return new RgbImage(meta.getSize().x, meta.getSize().y);
    }

    virtual bool loadPixelDataNew(PixelImageBase &outputImage) {
        Point srcSize = outputImage.getSize();
        bool result = true;
        std::istream &input = getInput();
        char *data = static_cast<char *>(static_cast<RgbImage &>(outputImage).data());

        for (int y = 0, eposy = srcSize.y; y < eposy; ++y) {
            for (int x = 0, eposx = srcSize.x; x < eposx; ++x) {
                if (!input.read(data, 3)) {
                    result = false;
                    break;
                }

                data += 3;
            }
        }

        return result;
    }
};


#endif
