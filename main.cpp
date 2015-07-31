#include "oni_to_sun_converter.h"

#include <iostream>

int main(int argc, char** argv)
{
    OniToSunConverter converter;

    int status = 0;
    status = converter.Init(argc, argv);
    if (status)
    {
        return status;
    }

    status = converter.Run();
    if (status)
    {
        return status;
    }

    return 0;
}
