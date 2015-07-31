#include "oni_to_sun_converter.h"

#include <iostream>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "sun_writer.h"

OniToSunConverter::OniToSunConverter()
{
}

OniToSunConverter::~OniToSunConverter()
{
    depth_.destroy();
    color_.destroy();
    openni::OpenNI::shutdown();
}

int OniToSunConverter::Init(int argc, char **argv)
{
    int status;
    std::string oni_file, out_path;
    status = VerifyParameters(argc, argv, oni_file, out_path);
    if (status != 0)
    {
        return status;
    }

    writer_ = boost::shared_ptr<SunWriter>(new SunWriter(out_path));

    status = InitOpenNI(oni_file);
    if (status != 0)
    {
        return status;
    }

    return 0;
}

int OniToSunConverter::Run()
{
    if (!writer_)
    {
        std::cout << "Converter wasn't initialize!" << std::endl;
        return 1;
    }

    int status = 0;
    openni::Status rc = openni::STATUS_OK;

    // Write intrinsics camera parameters.
    status = writer_->WriteIntrinsics();
    if (status)
    {
        return 1;
    }

    // Get number of frames.
    openni::PlaybackControl *pbc = device_.getPlaybackControl();
    pbc->setRepeatEnabled(false);
    int num_frames = pbc->getNumberOfFrames(depth_);
    if (num_frames != pbc->getNumberOfFrames(color_))
    {
        std::cout << "Invalid .oni file!" << std::endl;
        return 1;
    }

    // For each frame.
    openni::VideoFrameRef depth_frame;
    openni::VideoFrameRef color_frame;
    while (num_frames-- > 0)
    {
        rc = depth_.readFrame(&depth_frame);
        if (rc != openni::STATUS_OK)
        {
            std::cout << "Reading a depth frame:" << std::endl <<
                         openni::OpenNI::getExtendedError() << std::endl;
            return 1;
        }

        rc = color_.readFrame(&color_frame);
        if (rc != openni::STATUS_OK)
        {
            std::cout << "Reading a color frame:" << std::endl <<
                         openni::OpenNI::getExtendedError() << std::endl;
            return 1;
        }

        status = writer_->WriteImages(depth_frame, color_frame);
        if (status)
        {
            return 1;
        }
    }

    std::cout << "Finished!" << std::endl;
    std::cout << "Don't forget to specify 'intrinsics.txt', if needed." << std::endl;
    return 0;
}

int OniToSunConverter::VerifyParameters(int argc, char **argv,
        std::string &oni_file, std::string &out_path)
{
    // Parse the first parameter (.oni filename).
    if (argc < 2)
    {
        std::cout << "Specify input .oni file." << std::endl;
        return 1;
    }

    fs::path oni_file_path(argv[1]);
    if (!fs::exists(oni_file_path) || !fs::is_regular_file(oni_file_path) ||
            oni_file_path.extension().string().compare(fs::path(".oni").string()) != 0)
    {
        std::cout << "Specify correct .oni file." << std::endl;
        return 1;
    }
    oni_file = oni_file_path.string();

    // Parse the second parameter (output folder path).
    if (argc < 3)
    {
        out_path = oni_file_path.replace_extension("sun").string();
    }
    else
    {
        out_path = argv[2];
    }
    if (!fs::exists(out_path))
    {
        fs::create_directories(out_path);
    }
    else if (!fs::is_directory(out_path))
    {
        std::cout << "Specify correct out path." << std::endl;
        return 1;
    }
    return 0;
}

int OniToSunConverter::InitOpenNI(const std::string &oni_file)
{
    openni::Status rc = openni::STATUS_OK;

    rc = openni::OpenNI::initialize();

    if (rc != openni::STATUS_OK)
    {
        std::cout << "After initialization:" << std::endl <<
                     openni::OpenNI::getExtendedError() << std::endl;
    }

    rc = device_.open(oni_file.c_str());
    if (rc != openni::STATUS_OK)
    {
        std::cout << "Device open failed:" << std::endl <<
                     openni::OpenNI::getExtendedError() << std::endl;
        openni::OpenNI::shutdown();
        return 1;
    }
    std::cout << "Device opened:" <<  std::endl << oni_file << std::endl;

    rc = depth_.create(device_, openni::SENSOR_DEPTH);
    if (rc == openni::STATUS_OK)
    {
        rc = depth_.start();
        if (rc != openni::STATUS_OK)
        {
            std::cout << "Couldn't start depth stream:" << std::endl <<
                         openni::OpenNI::getExtendedError() << std::endl;
            depth_.destroy();
        }
    }
    else
    {
        std::cout << "Couldn't find depth stream:" << std::endl <<
                     openni::OpenNI::getExtendedError() << std::endl;
    }

    rc = color_.create(device_, openni::SENSOR_COLOR);
    if (rc == openni::STATUS_OK)
    {
        rc = color_.start();
        if (rc != openni::STATUS_OK)
        {
            std::cout << "Couldn't start color stream:" << std::endl <<
                         openni::OpenNI::getExtendedError() << std::endl;
            color_.destroy();
        }
    }
    else
    {
        std::cout << "Couldn't find color stream:" << std::endl <<
               openni::OpenNI::getExtendedError() << std::endl;
    }

    if (!depth_.isValid() || !color_.isValid())
    {
        std::cout << "No valid streams. Exiting" << std::endl;
        openni::OpenNI::shutdown();
        return 1;
    }
}
