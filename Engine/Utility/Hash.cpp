#include "Hash.hpp"

#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

std::string usagi::sha256(const std::string &string)
{
    using namespace CryptoPP;

    std::string digest;
    SHA256 hash;
    const StringSource src(string, true, new HashFilter(
        hash, new HexEncoder(new StringSink(digest))
    ));
    return digest;
}
