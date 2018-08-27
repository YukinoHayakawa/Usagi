#include "Hash.hpp"

#include <cryptopp/sha.h>
#include <cryptopp/crc.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

std::string usagi::sha256(const std::string &string)
{
    std::string digest;
    SHA256 hash;
    const StringSource src(string, true, new HashFilter(
        hash, new HexEncoder(new StringSink(digest), false)
    ));
    return digest;
}

std::string usagi::crc32(const std::string &string)
{
    std::string digest;
    CRC32 hash;
    const StringSource src(string, true, new HashFilter(
        hash, new HexEncoder(new StringSink(digest), false)
    ));
    return digest;
}
