struct GeometryData
{
        const char*  name;
        unsigned int fileLength = 0;
        unsigned int mem = 0;
};

constexpr unsigned int geometryDataLength = 1;
GeometryData           geometryData[geometryDataLength] = {{"NULL", 0, 0}};
