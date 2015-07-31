#include "sun_writer.h"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <OpenNI.h>

// Set static constants.
const std::string SunWriter::kIntrinsicsFilename("intrinsics_example.txt");

SunWriter::SunWriter(const std::string &out_path) :
    out_path_(out_path), frame_counter_(0u)
{
}

int SunWriter::WriteImages(const openni::VideoFrameRef &depth_frame,
                           const openni::VideoFrameRef &color_frame)
{
    int status = 0;

    // Generate names.
    std::string depth_file, color_file;
    status = GenerateNames(depth_file, color_file);
    if (status)
    {
        return status;
    }

    // Write depth.
    cv::Mat depth_image;
    const openni::DepthPixel *depth_data =
            static_cast<const openni::DepthPixel*>(depth_frame.getData());

    depth_image.create(depth_frame.getHeight(), depth_frame.getWidth(), CV_16U);
    memcpy(depth_image.data, depth_data, depth_frame.getDataSize());
    try
    {
        ShiftDepth(depth_image);
        cv::imwrite(depth_file, depth_image);
    }
    catch (std::exception &ex)
    {
        std::cout << "Writing depth:" << std::endl << ex.what() << std::endl;
        return 1;
    }

    // Write color.
    cv::Mat color_image;
    openni::RGB888Pixel *color_data;
    color_data = (openni::RGB888Pixel*)(color_frame.getData());

    color_image.create(color_frame.getHeight(), color_frame.getWidth(), CV_8UC3);
    memcpy(color_image.data, color_data, color_frame.getDataSize());
    try
    {
        cv::cvtColor(color_image, color_image, CV_BGR2RGB);
        cv::imwrite(color_file, color_image);
    }
    catch (std::exception &ex)
    {
        std::cout << "Writing depth:" << std::endl << ex.what() << std::endl;
        return 1;
    }

    // Print progress.
    std::cout << "Process frame #" << frame_counter_ - 1 << std::endl;

    return 0;
}

int SunWriter::WriteIntrinsics()
{
    std::string out_intrnisics = out_path_ + "/intrinsics.txt";
    if (!fs::exists(out_intrnisics))
    {
        fs::copy_file(kIntrinsicsFilename, out_intrnisics,
                      fs::copy_option::fail_if_exists);
    }
    return 0;
}

int SunWriter::GenerateNames(std::string &depth_file, std::string &color_file)
{
    // Init buffer.
    std::stringstream str_buffer;

    // Transform counter to string.
    str_buffer << std::setfill('0') << std::setw(7) << frame_counter_;
    std::string str_counter = str_buffer.str();
    str_buffer.str("");

    // Verify depth path.
    str_buffer << out_path_ << "/depth";
    fs::path depth_path(str_buffer.str());
    if (fs::exists(depth_path) && !fs::is_directory(depth_path))
    {
        std::cout << "Wrong path for depth:" << std::endl <<
                     depth_path << std::endl;
        return 1;
    }
    if (!fs::exists(depth_path))
    {
        fs::create_directories(depth_path);
    }
    // Construct depth filename.
    str_buffer << '/' << str_counter << '-' << str_counter << ".png";
    depth_file = str_buffer.str();

    str_buffer.str("");

    // Verify color path.
    str_buffer << out_path_ << "/image";
    fs::path color_path(str_buffer.str());
    if (fs::exists(color_path) && !fs::is_directory(color_path))
    {
        std::cout << "Wrong path for color:" << std::endl <<
                     color_path << std::endl;
        return 1;
    }
    if (!fs::exists(color_path))
    {
        fs::create_directories(color_path);
    }
    // Construct color filename.
    str_buffer << '/' << str_counter << '-' << str_counter << ".jpg";
    color_file = str_buffer.str();

    // Increse frame counter and return.
    ++frame_counter_;
    return 0;
}

int SunWriter::ShiftDepth(cv::Mat &depth_image)
{
    // Some magic to circulary shift 3 bit of each pixel of the depth image
    //   to the left so that the depth image look nice in a typical image viewer.
    depth_image = depth_image * (1 << 3) + depth_image / (1 << (16 - 3));
    return 0;
}
