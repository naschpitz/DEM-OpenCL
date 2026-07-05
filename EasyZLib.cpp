#include "EasyZLib.h"

#include "easyzlib/easyzlib.h"

EasyZLib::EasyZLib()
{

}

std::string EasyZLib::deflate(const QByteArray& data)
{
    ezbuffer bufferData(data.size());
    std::memcpy(bufferData.pBuf, data.data(), data.size());
    ezbuffer bufferCompressed;
    ezcompress(bufferCompressed, bufferData);

    std::string package(reinterpret_cast<const char*>(bufferCompressed.pBuf), bufferCompressed.nLen);

    return package;
}
