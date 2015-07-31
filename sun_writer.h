#ifndef SUN_WRITER_H
#define SUN_WRITER_H

#include <string>

// Forward declaration.
namespace openni {
    class VideoFrameRef;
}

namespace cv {
    class Mat;
}

// Class for writing results of conversion.
class SunWriter
{
public:
    SunWriter(const std::string &out_path);

    // Write depth and color images.
    int WriteImages(const openni::VideoFrameRef &depth_frame,
                    const openni::VideoFrameRef &color_frame);

    // Write intrinsics camera parameters.
    int WriteIntrinsics();
private:
    // Generates names for out files.
    int GenerateNames(std::string &depth_file, std::string &color_file);

    // Shift depth image's pixels.
    int ShiftDepth(cv::Mat &depth_image);

    // Path for resulting files.
    std::string out_path_;

    // Counter for frames.
    unsigned int frame_counter_;

    // Constant pointing to a file with intrinsics camera parameters examples.
    const static std::string kIntrinsicsFilename;
};

#endif // SUN_WRITER_H
