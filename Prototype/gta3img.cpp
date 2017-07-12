#include <fstream>
#include <iostream>
#include <cstring>

int main(int argc, char const *argv[])
{
    // if(argc != 2) return 1;
    const char *img_file = "L:\\Rockstar Games\\GTA San Andreas\\models\\player.img"; // argv[1];

    std::ifstream img(img_file, std::ios::in | std::ios::binary);
    if(!img) return 2;
    img.exceptions(std::ios::badbit | std::ios::failbit);

    char ver[4];
    img.read(ver, 4);
    if(strncmp(ver, "VER2", 4) != 0) return 3;
    uint32_t num_entries = -1;
    img.read(reinterpret_cast<char*>(&num_entries), sizeof(num_entries));
    std::cout << img_file << " has " << num_entries << " entries" << std::endl;

    uint32_t offset = -1;
    uint16_t streaming_size_sectors = -1, size_sectors = -1;
    char filename[24] = { 0 };
    for(decltype(num_entries) i = 0; i < num_entries; ++i)
    {
        img.read(reinterpret_cast<char*>(&offset), sizeof(offset));
        img.read(reinterpret_cast<char*>(&streaming_size_sectors), sizeof(streaming_size_sectors));
        img.read(reinterpret_cast<char*>(&size_sectors), sizeof(size_sectors));
        img.read(filename, 24);
        filename[23] = 0;
        std::cout
            << "offset: " << offset
            << " ssize: " << streaming_size_sectors
            << " size: " << size_sectors
            << " filename: " << filename
            << std::endl;
    }

    return 0;
}
