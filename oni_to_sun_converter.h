#ifndef ONI_TO_SUN_CONVERTER_H
#define ONI_TO_SUN_CONVERTER_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <OpenNI.h>

// Forward declaration.
class SunWriter;

// Main class to convert .oni files to file sequences for SUN3Dsfm.
class OniToSunConverter
{
public:
    OniToSunConverter();
    ~OniToSunConverter();

    // Initialize the class context.
    int Init(int argc, char** argv);

    // Perform converting and save the result.
    int Run();

private:
    // Verify input parameters.
    int VerifyParameters(int argc, char** argv,
            std::string &oni_file, std::string &out_path);

    // Initialize OpenNI context.
    int InitOpenNI(const std::string &oni_file);

    // Device to get image.
    openni::Device device_;

    // Depth stream of that device.
    openni::VideoStream depth_;

    // Color stream of that device.
    openni::VideoStream color_;

    // Pointer to the result writer.
    boost::shared_ptr<SunWriter> writer_;
};

#endif // ONI_TO_SUN_CONVERTER_H
