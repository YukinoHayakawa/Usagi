#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <cassert>

const char * getChunkTypeString(uint32_t type)
{
    switch(type)
    {
        case 0x0001: return "Struct";
        case 0x0002: return "String";
        case 0x0003: return "Extension";
        case 0x0005: return "Camera";
        case 0x0006: return "Texture";
        case 0x0007: return "Material";
        case 0x0008: return "Material List";
        case 0x0009: return "Atomic Section";
        case 0x000A: return "Plane Section";
        case 0x000B: return "World";
        case 0x000C: return "Spline";
        case 0x000D: return "Matrix";
        case 0x000E: return "Frame List";
        case 0x000F: return "Geometry";
        case 0x0010: return "Clump";
        case 0x0012: return "Light";
        case 0x0013: return "Unicode String";
        case 0x0014: return "Atomic";
        case 0x0015: return "Texture Native";
        case 0x0016: return "Texture Dictionary";
        case 0x0017: return "Animation Database";
        case 0x0018: return "Image";
        case 0x0019: return "Skin Animation";
        case 0x001A: return "Geometry List";
        case 0x001B: return "Anim Animation";
        case 0x001C: return "Team";
        case 0x001D: return "Crowd";
        case 0x001E: return "Delta Morph Animation";
        case 0x001f: return "Right To Render";
        case 0x0020: return "MultiTexture Effect Native";
        case 0x0021: return "MultiTexture Effect Dictionary";
        case 0x0022: return "Team Dictionary";
        case 0x0023: return "Platform Independent Texture Dictionary";
        case 0x0024: return "Table of Contents";
        case 0x0025: return "Particle Standard Global Data";
        case 0x0026: return "AltPipe";
        case 0x0027: return "Platform Independent Peds";
        case 0x0028: return "Patch Mesh";
        case 0x0029: return "Chunk Group Start";
        case 0x002A: return "Chunk Group End";
        case 0x002B: return "UV Animation Dictionary";
        case 0x002C: return "Coll Tree";
        case 0x0101: return "Metrics PLG";
        case 0x0102: return "Spline PLG";
        case 0x0103: return "Stereo PLG";
        case 0x0104: return "VRML PLG";
        case 0x0105: return "Morph PLG";
        case 0x0106: return "PVS PLG";
        case 0x0107: return "Memory Leak PLG";
        case 0x0108: return "Animation PLG";
        case 0x0109: return "Gloss PLG";
        case 0x010a: return "Logo PLG";
        case 0x010b: return "Memory Info PLG";
        case 0x010c: return "Random PLG";
        case 0x010d: return "PNG Image PLG";
        case 0x010e: return "Bone PLG";
        case 0x010f: return "VRML Anim PLG";
        case 0x0110: return "Sky Mipmap Val";
        case 0x0111: return "MRM PLG";
        case 0x0112: return "LOD Atomic PLG";
        case 0x0113: return "ME PLG";
        case 0x0114: return "Lightmap PLG";
        case 0x0115: return "Refine PLG";
        case 0x0116: return "Skin PLG";
        case 0x0117: return "Label PLG";
        case 0x0118: return "Particles PLG";
        case 0x0119: return "GeomTX PLG";
        case 0x011a: return "Synth Core PLG";
        case 0x011b: return "STQPP PLG";
        case 0x011c: return "Part PP PLG";
        case 0x011d: return "Collision PLG";
        case 0x011e: return "HAnim PLG";
        case 0x011f: return "User Data PLG";
        case 0x0120: return "Material Effects PLG";
        case 0x0121: return "Particle System PLG";
        case 0x0122: return "Delta Morph PLG";
        case 0x0123: return "Patch PLG";
        case 0x0124: return "Team PLG";
        case 0x0125: return "Crowd PP PLG";
        case 0x0126: return "Mip Split PLG";
        case 0x0127: return "Anisotropy PLG";
        case 0x0129: return "GCN Material PLG";
        case 0x012a: return "Geometric PVS PLG";
        case 0x012b: return "XBOX Material PLG";
        case 0x012c: return "Multi Texture PLG";
        case 0x012d: return "Chain PLG";
        case 0x012e: return "Toon PLG";
        case 0x012f: return "PTank PLG";
        case 0x0130: return "Particle Standard PLG";
        case 0x0131: return "PDS PLG";
        case 0x0132: return "PrtAdv PLG";
        case 0x0133: return "Normal Map PLG";
        case 0x0134: return "ADC PLG";
        case 0x0135: return "UV Animation PLG";
        case 0x0180: return "Character Set PLG";
        case 0x0181: return "NOHS World PLG";
        case 0x0182: return "Import Util PLG";
        case 0x0183: return "Slerp PLG";
        case 0x0184: return "Optim PLG";
        case 0x0185: return "TL World PLG";
        case 0x0186: return "Database PLG";
        case 0x0187: return "Raytrace PLG";
        case 0x0188: return "Ray PLG";
        case 0x0189: return "Library PLG";
        case 0x0190: return "2D PLG";
        case 0x0191: return "Tile Render PLG";
        case 0x0192: return "JPEG Image PLG";
        case 0x0193: return "TGA Image PLG";
        case 0x0194: return "GIF Image PLG";
        case 0x0195: return "Quat PLG";
        case 0x0196: return "Spline PVS PLG";
        case 0x0197: return "Mipmap PLG";
        case 0x0198: return "MipmapK PLG";
        case 0x0199: return "2D Font";
        case 0x019a: return "Intersection PLG";
        case 0x019b: return "TIFF Image PLG";
        case 0x019c: return "Pick PLG";
        case 0x019d: return "BMP Image PLG";
        case 0x019e: return "RAS Image PLG";
        case 0x019f: return "Skin FX PLG";
        case 0x01a0: return "VCAT PLG";
        case 0x01a1: return "2D Path";
        case 0x01a2: return "2D Brush";
        case 0x01a3: return "2D Object";
        case 0x01a4: return "2D Shape";
        case 0x01a5: return "2D Scene";
        case 0x01a6: return "2D Pick Region";
        case 0x01a7: return "2D Object String";
        case 0x01a8: return "2D Animation PLG";
        case 0x01a9: return "2D Animation";
        case 0x01b0: return "2D Keyframe";
        case 0x01b1: return "2D Maestro";
        case 0x01b2: return "Barycentric";
        case 0x01b3: return "Platform Independent Texture Dictionary TK";
        case 0x01b4: return "TOC TK";
        case 0x01b5: return "TPL TK";
        case 0x01b6: return "AltPipe TK";
        case 0x01b7: return "Animation TK";
        case 0x01b8: return "Skin Split Tookit";
        case 0x01b9: return "Compressed Key TK";
        case 0x01ba: return "Geometry Conditioning PLG";
        case 0x01bb: return "Wing PLG";
        case 0x01bc: return "Generic Pipeline TK";
        case 0x01bd: return "Lightmap Conversion TK";
        case 0x01be: return "Filesystem PLG";
        case 0x01bf: return "Dictionary TK";
        case 0x01c0: return "UV Animation Linear";
        case 0x01c1: return "UV Animation Parameter";
        case 0x050E: return "Bin Mesh PLG";
        case 0x0510: return "Native Data PLG";
        case 0xF21E: return "ZModeler Lock";
        case 0x0253F200: return "Atomic Visibility Distance";
        case 0x0253F201: return "Clump Visibility Distance";
        case 0x0253F202: return "Frame Visibility Distance";
        case 0x0253F2F3: return "Pipeline Set";
        case 0x0253F2F4: return "Unused 5";
        case 0x0253F2F5: return "TexDictionary Link";
        case 0x0253F2F6: return "Specular Material";
        case 0x0253F2F7: return "Unused 8";
        case 0x0253F2F8: return "2d Effect";
        case 0x0253F2F9: return "Extra Vert Colour";
        case 0x0253F2FA: return "Collision Model";
        case 0x0253F2FB: return "GTA HAnim";
        case 0x0253F2FC: return "Reflection Material";
        case 0x0253F2FD: return "Breakable";
        case 0x0253F2FE: return "Frame";
        case 0x0253F2FF: return "Unused 16";
        default: return "Unknown";
    }
}

enum class RwStreamSectionType : uint32_t
{
    IGNORED = 0,
    RW_STRUCT = 0x0001,
    RW_STRING = 0x0002,
    RW_EXTENSION = 0x0003,
    RW_CAMERA = 0x0005,
    RW_TEXTURE = 0x0006,
    RW_MATERIAL = 0x0007,
    RW_MATERIAL_LIST = 0x0008,
    RW_ATOMIC_SECTION = 0x0009,
    RW_PLANE_SECTION = 0x000A,
    RW_WORLD = 0x000B,
    RW_SPLINE = 0x000C,
    RW_MATRIX = 0x000D,
    RW_FRAME_LIST = 0x000E,
    RW_GEOMETRY = 0x000F,
    RW_CLUMP = 0x0010,
    RW_LIGHT = 0x0012,
    RW_UNICODE_STRING = 0x0013,
    RW_ATOMIC = 0x0014,
    RW_TEXTURE_NATIVE = 0x0015,
    RW_TEXTURE_DICTIONARY = 0x0016,
    RW_ANIMATION_DATABASE = 0x0017,
    RW_IMAGE = 0x0018,
    RW_SKIN_ANIMATION = 0x0019,
    RW_GEOMETRY_LIST = 0x001A,
    RW_ANIM_ANIMATION = 0x001B,
    RW_TEAM = 0x001C,
    RW_CROWD = 0x001D,
    RW_DELTA_MORPH_ANIMATION = 0x001E,
    RW_RIGHT_TO_RENDER = 0x001F,
    RW_MULTITEXTURE_EFFECT_NATIVE = 0x0020,
    RW_MULTITEXTURE_EFFECT_DICTIONARY = 0x0021,
    RW_TEAM_DICTIONARY = 0x0022,
    RW_PLATFORM_INDEPENDENT_TEXTURE_DICTIONARY = 0x0023,
    RW_TABLE_OF_CONTENTS = 0x0024,
    RW_PARTICLE_STANDARD_GLOBAL_DATA = 0x0025,
    RW_ALTPIPE = 0x0026,
    RW_PLATFORM_INDEPENDENT_PEDS = 0x0027,
    RW_PATCH_MESH = 0x0028,
    RW_CHUNK_GROUP_START = 0x0029,
    RW_CHUNK_GROUP_END = 0x002A,
    RW_UV_ANIMATION_DICTIONARY = 0x002B,
    RW_COLL_TREE = 0x002C,
    RW_METRICS_PLG = 0x0101,
    RW_SPLINE_PLG = 0x0102,
    RW_STEREO_PLG = 0x0103,
    RW_VRML_PLG = 0x0104,
    RW_MORPH_PLG = 0x0105,
    RW_PVS_PLG = 0x0106,
    RW_MEMORY_LEAK_PLG = 0x0107,
    RW_ANIMATION_PLG = 0x0108,
    RW_GLOSS_PLG = 0x0109,
    RW_LOGO_PLG = 0x010A,
    RW_MEMORY_INFO_PLG = 0x010B,
    RW_RANDOM_PLG = 0x010C,
    RW_PNG_IMAGE_PLG = 0x010D,
    RW_BONE_PLG = 0x010E,
    RW_VRML_ANIM_PLG = 0x010F,
    RW_SKY_MIPMAP_VAL = 0x0110,
    RW_MRM_PLG = 0x0111,
    RW_LOD_ATOMIC_PLG = 0x0112,
    RW_ME_PLG = 0x0113,
    RW_LIGHTMAP_PLG = 0x0114,
    RW_REFINE_PLG = 0x0115,
    RW_SKIN_PLG = 0x0116,
    RW_LABEL_PLG = 0x0117,
    RW_PARTICLES_PLG = 0x0118,
    RW_GEOMTX_PLG = 0x0119,
    RW_SYNTH_CORE_PLG = 0x011A,
    RW_STQPP_PLG = 0x011B,
    RW_PART_PP_PLG = 0x011C,
    RW_COLLISION_PLG = 0x011D,
    RW_HANIM_PLG = 0x011E,
    RW_USER_DATA_PLG = 0x011F,
    RW_MATERIAL_EFFECTS_PLG = 0x0120,
    RW_PARTICLE_SYSTEM_PLG = 0x0121,
    RW_DELTA_MORPH_PLG = 0x0122,
    RW_PATCH_PLG = 0x0123,
    RW_TEAM_PLG = 0x0124,
    RW_CROWD_PP_PLG = 0x0125,
    RW_MIP_SPLIT_PLG = 0x0126,
    RW_ANISOTROPY_PLG = 0x0127,
    RW_GCN_MATERIAL_PLG = 0x0129,
    RW_GEOMETRIC_PVS_PLG = 0x012A,
    RW_XBOX_MATERIAL_PLG = 0x012B,
    RW_MULTI_TEXTURE_PLG = 0x012C,
    RW_CHAIN_PLG = 0x012D,
    RW_TOON_PLG = 0x012E,
    RW_PTANK_PLG = 0x012F,
    RW_PARTICLE_STANDARD_PLG = 0x0130,
    RW_PDS_PLG = 0x0131,
    RW_PRTADV_PLG = 0x0132,
    RW_NORMAL_MAP_PLG = 0x0133,
    RW_ADC_PLG = 0x0134,
    RW_UV_ANIMATION_PLG = 0x0135,
    RW_CHARACTER_SET_PLG = 0x0180,
    RW_NOHS_WORLD_PLG = 0x0181,
    RW_IMPORT_UTIL_PLG = 0x0182,
    RW_SLERP_PLG = 0x0183,
    RW_OPTIM_PLG = 0x0184,
    RW_TL_WORLD_PLG = 0x0185,
    RW_DATABASE_PLG = 0x0186,
    RW_RAYTRACE_PLG = 0x0187,
    RW_RAY_PLG = 0x0188,
    RW_LIBRARY_PLG = 0x0189,
    RW_2D_PLG = 0x0190,
    RW_TILE_RENDER_PLG = 0x0191,
    RW_JPEG_IMAGE_PLG = 0x0192,
    RW_TGA_IMAGE_PLG = 0x0193,
    RW_GIF_IMAGE_PLG = 0x0194,
    RW_QUAT_PLG = 0x0195,
    RW_SPLINE_PVS_PLG = 0x0196,
    RW_MIPMAP_PLG = 0x0197,
    RW_MIPMAPK_PLG = 0x0198,
    RW_2D_FONT = 0x0199,
    RW_INTERSECTION_PLG = 0x019A,
    RW_TIFF_IMAGE_PLG = 0x019B,
    RW_PICK_PLG = 0x019C,
    RW_BMP_IMAGE_PLG = 0x019D,
    RW_RAS_IMAGE_PLG = 0x019E,
    RW_SKIN_FX_PLG = 0x019F,
    RW_VCAT_PLG = 0x01A0,
    RW_2D_PATH = 0x01A1,
    RW_2D_BRUSH = 0x01A2,
    RW_2D_OBJECT = 0x01A3,
    RW_2D_SHAPE = 0x01A4,
    RW_2D_SCENE = 0x01A5,
    RW_2D_PICK_REGION = 0x01A6,
    RW_2D_OBJECT_STRING = 0x01A7,
    RW_2D_ANIMATION_PLG = 0x01A8,
    RW_2D_ANIMATION = 0x01A9,
    RW_2D_KEYFRAME = 0x01B0,
    RW_2D_MAESTRO = 0x01B1,
    RW_BARYCENTRIC = 0x01B2,
    RW_PLATFORM_INDEPENDENT_TEXTURE_DICTIONARY_TK = 0x01B3,
    RW_TOC_TK = 0x01B4,
    RW_TPL_TK = 0x01B5,
    RW_ALTPIPE_TK = 0x01B6,
    RW_ANIMATION_TK = 0x01B7,
    RW_SKIN_SPLIT_TOOKIT = 0x01B8,
    RW_COMPRESSED_KEY_TK = 0x01B9,
    RW_GEOMETRY_CONDITIONING_PLG = 0x01BA,
    RW_WING_PLG = 0x01BB,
    RW_GENERIC_PIPELINE_TK = 0x01BC,
    RW_LIGHTMAP_CONVERSION_TK = 0x01BD,
    RW_FILESYSTEM_PLG = 0x01BE,
    RW_DICTIONARY_TK = 0x01BF,
    RW_UV_ANIMATION_LINEAR = 0x01C0,
    RW_UV_ANIMATION_PARAMETER = 0x01C1,
    RW_BIN_MESH_PLG = 0x050E,
    RW_NATIVE_DATA_PLG = 0x0510,
    RW_ZMODELER_LOCK = 0xF21E,
    RW_ATOMIC_VISIBILITY_DISTANCE = 0x0253F200,
    RW_CLUMP_VISIBILITY_DISTANCE = 0x0253F201,
    RW_FRAME_VISIBILITY_DISTANCE = 0x0253F202,
    RW_PIPELINE_SET = 0x0253F2F3,
    RW_UNUSED_5 = 0x0253F2F4,
    RW_TEXDICTIONARY_LINK = 0x0253F2F5,
    RW_SPECULAR_MATERIAL = 0x0253F2F6,
    RW_UNUSED_8 = 0x0253F2F7,
    RW_2D_EFFECT = 0x0253F2F8,
    RW_EXTRA_VERT_COLOUR = 0x0253F2F9,
    RW_COLLISION_MODEL = 0x0253F2FA,
    RW_GTA_HANIM = 0x0253F2FB,
    RW_REFLECTION_MATERIAL = 0x0253F2FC,
    RW_BREAKABLE = 0x0253F2FD,
    RW_FRAME = 0x0253F2FE,
    RW_UNUSED_16 = 0x0253F2FF,
};

struct RwRGBA
{
    uint8_t r, g, b, a;
};

struct RwTexCoords
{
    float u, v;
};

struct RpTriangle
{
    uint16_t vertex2, vertex1, materialId, vertex3;
};

struct RwSphere
{
    float x, y, z, radius;
};

struct RwV3d
{
    float x, y, z;
};

struct RwSectionHeader
{
    uint32_t type;
    uint32_t size; // including child chunks and data
    struct RwVersion
    {
        uint32_t stamp;

        uint32_t getVersion() const
        {
            if(stamp & 0xFFFF0000)
                return (stamp >> 14 & 0x3FF00) + 0x30000 | (stamp >> 16 & 0x3F);
            return stamp << 8;
        }
    } library_id_stamp;
};

struct RwStreamSection
{
    virtual ~RwStreamSection() = default;

    virtual RwStreamSectionType getType() const = 0;

    void parse(std::istream &in)
    {
        RwSectionHeader header;
        in.read(reinterpret_cast<char*>(&header), sizeof(RwSectionHeader));

        if(static_cast<RwStreamSectionType>(header.type) != getType())
            throw std::runtime_error("type code not matched");

        auto ver = header.library_id_stamp.getVersion();
        std::cout
            << getChunkTypeString(header.type)
            << " size: " << header.size
            << " version: " << std::hex << ver << std::dec
            << std::endl;

        _parseBody(in, header, ver);
    }

    // extract a section from the input stream. IgnoredSecion is created if no according class is implemented.
    static std::shared_ptr<RwStreamSection> parseSection(std::istream &in);

protected:
    static void _skipSection(std::istream &in, size_t n)
    {
        RwSectionHeader header;
        for(size_t i = 0; i < n; ++i)
        {
            in.read(reinterpret_cast<char*>(&header), sizeof(RwSectionHeader));
            in.seekg(header.size, std::ios_base::cur);
        }
    }

    static void _skipHeader(std::istream &in)
    {
        in.ignore(sizeof(RwSectionHeader));
    }

private:
    virtual void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) = 0;
};

struct RwString : RwStreamSection
{
    std::string string;

    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::RW_STRING;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        std::getline(in, string, '\0');
        // == -(string.size() + 1) % 4?
        size_t padding = (4 - (string.size() + 1) % 4) % 4;
        in.ignore(padding);
    }
};

struct RwTexture : RwStreamSection
{
    int32_t data;
    RwString tex_name, alpha_name;

    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::RW_TEXTURE;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        _skipHeader(in);
        in.read(reinterpret_cast<char*>(&data), sizeof(data));
        tex_name.parse(in);
        alpha_name.parse(in);
        _skipSection(in, 1); // skip ext
    }
};

struct RpMaterial : RwStreamSection
{
    int32_t flags;
    RwRGBA color;
    int32_t unused;
    int32_t is_textured;
    float ambient, specular, diffuse;

    RwTexture texture;

    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::RW_MATERIAL;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        _skipHeader(in); // skip Struct header
        in.read(reinterpret_cast<char*>(&flags), sizeof(int32_t) * 3 + sizeof(color));
        if(version > 0x30400)
        {
            in.read(reinterpret_cast<char*>(&ambient), sizeof(float) * 3);
        }
        if(is_textured)
        {
            texture.parse(in);
        }
        _skipSection(in, 1); // skip extension
    }
};

struct RwMaterialList : RwStreamSection
{
    uint32_t num_materials;
    std::vector<int32_t> mat_indices;
    std::vector<RpMaterial> materials;

    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::RW_MATERIAL_LIST;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        _skipHeader(in); // skip Struct header
        in.read(reinterpret_cast<char*>(&num_materials), sizeof(num_materials));
        // mat_indices = decltype(mat_indices) { num_materials, 0 }; -> will be recognized as an initializer list so does not work
        mat_indices = decltype(mat_indices)(num_materials, 0);
        assert(mat_indices.size() == num_materials);
        in.read(reinterpret_cast<char*>(&mat_indices[0]), sizeof(decltype(mat_indices)::value_type) * mat_indices.size());

        materials = decltype(materials) { num_materials, RpMaterial { } };
        assert(materials.size() == num_materials);
        for(auto &&m : materials)
        {
            m.parse(in);
        }
    }
};

struct RpGeometry : RwStreamSection
{
    enum Format : uint32_t
    {
        rpGEOMETRYTRISTRIP = 0x00000001,
        rpGEOMETRYPOSITIONS = 0x00000002,
        rpGEOMETRYTEXTURED = 0x00000004,
        rpGEOMETRYPRELIT = 0x00000008,
        rpGEOMETRYNORMALS = 0x00000010,
        rpGEOMETRYLIGHT = 0x00000020,
        rpGEOMETRYMODULATEMATERIALCOLOR = 0x00000040,
        rpGEOMETRYTEXTURED2 = 0x00000080,
        rpGEOMETRYNATIVE = 0x01000000,
    } format;

    uint32_t num_triangles, num_vertices, num_morph_targets;
    float ambient, specular, diffuse;
    std::vector<RwRGBA> prelitcolor;
    uint32_t numTexSets;
    std::vector<std::vector<RwTexCoords>> texCoords;
    std::vector<RpTriangle> triangles;

    struct MorphTarget
    {
        RwSphere bounding_sphere;
        uint32_t has_vertices, has_normals;
        std::vector<RwV3d> vertices, normals;
    };

    std::vector<MorphTarget> morph_targets;
    RwMaterialList material_list;

    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::RW_GEOMETRY;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        _skipHeader(in); // skip Struct header
        in.read(reinterpret_cast<char*>(&format), sizeof(uint32_t) * 4);
        if((format & rpGEOMETRYNATIVE) == 0)
        {
            if(format & rpGEOMETRYPRELIT)
            {
                prelitcolor.clear();
                prelitcolor.insert(prelitcolor.end(), num_vertices, { });
                in.read(reinterpret_cast<char*>(&prelitcolor[0]), sizeof(RwRGBA) * num_vertices);
            }
            if(format & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2))
            {
                numTexSets = (format & 0x00FF0000) >> 16;
                texCoords = decltype(texCoords) { numTexSets, decltype(texCoords)::value_type { } };
                assert(texCoords.size() == numTexSets);
                for(auto &&t : texCoords)
                {
                    t = std::remove_reference_t<decltype(t)> { num_vertices, RwTexCoords { } };
                    assert(t.size() == num_vertices);
                    in.read(reinterpret_cast<char*>(&t[0]), sizeof(RwTexCoords) * num_vertices);
                }
            }
            triangles = decltype(triangles) { num_triangles, RpTriangle { } };
            assert(triangles.size() == num_triangles);
            in.read(reinterpret_cast<char*>(&triangles[0]), sizeof(RpTriangle) * num_triangles);
        }
        morph_targets = decltype(morph_targets) { num_morph_targets, MorphTarget { } };
        assert(morph_targets.size() == num_morph_targets);
        for(auto &&m : morph_targets)
        {
            in.read(reinterpret_cast<char*>(&m.bounding_sphere), sizeof(RwSphere) + 2 * sizeof(uint32_t));
            if(m.has_vertices)
            {
                m.vertices = std::vector<RwV3d> { num_vertices, RwV3d { } };
                assert(m.vertices.size() == num_vertices);
                in.read(reinterpret_cast<char*>(&m.vertices[0]), num_vertices * sizeof(RwV3d));
            }
            if(m.has_normals)
            {
                m.normals = std::vector<RwV3d> { num_vertices, RwV3d { } };
                assert(m.normals.size() == num_vertices);
                in.read(reinterpret_cast<char*>(&m.normals[0]), num_vertices * sizeof(RwV3d));
            }
        }
        material_list.parse(in);
        _skipSection(in, 1); // skip extension
    }
};

struct RwGeometryList : RwStreamSection
{
    uint32_t num_geometries;
    std::vector<RpGeometry> geometries;

    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::RW_GEOMETRY_LIST;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        _skipHeader(in); // skip Struct header
        in.read(reinterpret_cast<char*>(&num_geometries), sizeof(num_geometries));
        geometries.clear();
        geometries.reserve(num_geometries);
        for(decltype(num_geometries) i = 0; i < num_geometries; ++i)
        {
            geometries.emplace_back();
            geometries[i].parse(in);
        }
    }
};

struct IgnoredSection : RwStreamSection
{
    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::IGNORED;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        in.ignore(header.size);
    }
};

struct RpClump : RwStreamSection
{
    uint32_t num_atomics;
    uint32_t num_lights;
    uint32_t num_cameras;
    RwGeometryList geometry_list;

    RwStreamSectionType getType() const override
    {
        return RwStreamSectionType::RW_CLUMP;
    }

private:
    void _parseBody(std::istream &in, RwSectionHeader &header, uint32_t version) override
    {
        _skipHeader(in); // skip Struct header
        in.read(reinterpret_cast<char*>(&num_atomics), sizeof(num_atomics));
        if(version > 0x33000)
            in.read(reinterpret_cast<char*>(&num_lights), 2 * sizeof(num_lights));
        _skipSection(in, 1); // skip frame list
        geometry_list.parse(in);
        _skipSection(in, 6); // skip atomic, struct, light, struct, camera, and extension
    }
};

struct RwBinaryStreamFile
{
    std::vector<std::shared_ptr<RwStreamSection>> chunks;
};

std::shared_ptr<RwBinaryStreamFile> parseRwBinaryStreamFile(std::istream &in)
{
    auto bsobj = std::make_shared<RwBinaryStreamFile>();
    RwSectionHeader header;
    bool eof = false;
    while(in && !eof)
    {
        in.read(reinterpret_cast<char*>(&header), sizeof(RwSectionHeader));
        // rewind to the beginning of header to let RwStreamSection parse the header again.
        // here just use the section type to determine the type of instantiated object.
        in.seekg(-static_cast<int>(sizeof(RwSectionHeader)), std::ios_base::cur);

        switch(static_cast<RwStreamSectionType>(header.type))
        {
            case RwStreamSectionType::RW_CLUMP:
            {
                auto clump = std::make_shared<RpClump>();
                clump->parse(in);
                bsobj->chunks.push_back(clump);
                break;
            }
            case RwStreamSectionType::IGNORED:
            {
                eof = true;
                break;
            }
            default:
            {
                bsobj->chunks.push_back(std::make_shared<IgnoredSection>());
                break;
            }
        }
    }
    return bsobj;
}

int main(int argc, char *argv[])
{
    std::ifstream bin("L:\\gta3.img\\a51_crane.dff", std::ios::in | std::ios::binary);
    bin.exceptions(std::ios::failbit);
    auto result = parseRwBinaryStreamFile(bin);

    return 0;
}
