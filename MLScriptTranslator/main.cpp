#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <stack>
#include <set>
#include <map>
#include <boost/algorithm/string.hpp>
#include <strstream>

#include <utfcpp/utf8.h>

#ifdef _WIN32
#include <Usagi/Engine/Extension/Win32/Win32Helper.hpp>
#endif

using namespace boost;
using namespace std;
namespace fs = std::filesystem;

namespace moeloop
{
enum ErrorCodes
{
    INVALID_USAGE = 1,
    SYNTAX_ERROR,
};

enum class TokenType
{
    END,
    NEWLINE,
    HEADING_MARK,
    // HEADING_TAG_BEGIN,
    HEADING_TAG,
    // HEADING_TAG_END,
    HEADING_NAME,
    // CODE_BEGIN,
    CODE_TAG,
    // CODE_COLON,
    CODE_COMMENT,
    COMMAND_NAME,
    COMMAND_PARAM,
    // CODE_END,
    // CHAR_BEGIN,
    CHAR_PARAM,
    // CHAR_END,
    MESSAGE,
};

const char * tokenName(const TokenType t)
{
    switch(t)
    {
        case TokenType::END: return "END";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::HEADING_MARK: return "HEADING_MARK";
            // case TokenType::HEADING_TAG_BEGIN: return "HEADING_TAG_BEGIN";
        case TokenType::HEADING_TAG: return "HEADING_TAG";
            // case TokenType::HEADING_TAG_END: return "HEADING_TAG_END";
        case TokenType::HEADING_NAME: return "HEADING_NAME";
            // case TokenType::CODE_BEGIN: return "CODE_BEGIN";
        case TokenType::CODE_TAG: return "CODE_TAG";
            // case TokenType::CODE_COLON: return "CODE_COLON";
        case TokenType::CODE_COMMENT: return "CODE_COMMENT";
        case TokenType::COMMAND_NAME: return "COMMAND_NAME";
        case TokenType::COMMAND_PARAM: return "COMMAND_PARAM";
            // case TokenType::CODE_END: return "CODE_END";
            // case TokenType::CHAR_BEGIN: return "CHAR_BEGIN";
        case TokenType::CHAR_PARAM: return "CHAR_PARAM";
            // case TokenType::CHAR_END: return "CHAR_END";
        case TokenType::MESSAGE: return "MESSAGE";
        default: return "";
    }
}

struct Token
{
    TokenType type;
    string data;
};

bool is_number(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool isSpace(const uint32_t current)
{
    return current == ' ' ||
        current == '\t' ||
        current == '\n' ||
        current == '\v' ||
        current == '\f' ||
        current == '\r';
}

struct Tokenizer
{
    using utf32string = basic_string<uint32_t>;
    vector<Token> tokens;
    utf32string line;
    utf32string::iterator pos;

    void skipSpace() { while(pos != line.end() && isSpace(*pos)) ++pos; }

    void expect(const uint32_t c)
    {
        skipSpace();
        if(*pos != c)
        {
            cerr << "Expected a " << c << endl;
            exit(SYNTAX_ERROR);
        }
        ++pos;
    }

    string tillCharTrimmed(const uint32_t c)
    {
        const auto begin = pos;
        pos = find(begin, line.end(), c);
        if(pos == line.end())
        {
            cerr << "Expected a " << c << endl;
            exit(SYNTAX_ERROR);
        }
        string r;
        utf8::utf32to8(begin, pos, back_inserter(r));
        trim(r);
        return r;
    }

    string tillAnyCharTrimmed(const uint32_t c, const uint32_t c1)
    {
        const auto begin = pos;
        pos = std::min(find(begin, line.end(), c), find(begin, line.end(), c1));
        if(pos == line.end())
        {
            cerr << "Expected a " << c << endl;
            exit(SYNTAX_ERROR);
        }
        string r;
        utf8::utf32to8(begin, pos, back_inserter(r));
        trim(r);
        return r;
    }

    string tillEndTrimmed()
    {
        string r;
        utf8::utf32to8(pos, line.end(), back_inserter(r));
        pos = line.end();
        trim(r);
        if(r.empty())
        {
            cerr << "Expected a token" << endl;
            exit(SYNTAX_ERROR);
        }
        return r;
    }

    char peek() const
    {
        if(pos == line.end()) return 0;
        return *pos;
    }

    void params(const TokenType param, const uint32_t end)
    {
        auto fetch = true;
        while(fetch)
        {
            tokens.push_back({ param, tillAnyCharTrimmed(',', end) });
            skipSpace();
            switch(peek())
            {
                case ',': expect(','); break;
                default: expect(end); fetch = false; break;
            }
        }
    }

    bool appearEarlyThan(const uint32_t a, const uint32_t b)
    {
        return find(pos, line.end(), a) < find(pos, line.end(), b);
    }

    explicit Tokenizer(istream &in)
    {
        string utf8line;
        while(getline(in, utf8line))
        {
            line.clear();
            utf8::utf8to32(utf8line.begin(), utf8line.end(), back_inserter(line));
            if(line.empty()) continue;

            pos = line.begin();
            skipSpace();
            // line has only spaces
            if(pos == line.end()) continue;

            switch(*pos)
            {
                // heading
                case '#':
                {
                    const auto sharp_end = find_if(
                        pos, line.end(),
                        [](const char c) { return c != '#'; }
                    );
                    tokens.push_back({ TokenType::HEADING_MARK, { pos, sharp_end } });
                    pos = sharp_end;
                    expect('[');
                    tokens.push_back({ TokenType::HEADING_TAG, tillCharTrimmed(']') });
                    expect(']');
                    tokens.push_back({ TokenType::HEADING_NAME, tillEndTrimmed() });
                    break;
                }
                // code
                case '{':
                {
                    expect('{');
                    const auto has_params = appearEarlyThan(':', '}');
                    auto tag = tillAnyCharTrimmed(':', '}');
                    if(is_number(tag))
                    {
                        tokens.push_back({ TokenType::CODE_TAG, std::move(tag) });
                        expect(':');
                        tokens.push_back({ TokenType::CODE_COMMENT, tillCharTrimmed('}') });
                    }
                    else
                    {
                        tokens.push_back({ TokenType::COMMAND_NAME, std::move(tag) });
                        if(has_params)
                        {
                            expect(':');
                            params(TokenType::COMMAND_PARAM, '}');
                        }
                        else
                        {
                            expect('}');
                        }
                    }
                    break;
                }
                // message with expr
                case '[':
                {
                    expect('[');
                    params(TokenType::CHAR_PARAM, ']');
                    tokens.push_back({ TokenType::MESSAGE, tillEndTrimmed() });
                    break;
                }
                // narrator message
                default:
                {
                    tokens.push_back({ TokenType::MESSAGE, tillEndTrimmed() });
                    break;
                }
            }
            tokens.push_back({ TokenType::NEWLINE, "" });
        }
        tokens.push_back({ TokenType::END, "" });
    }
};


struct Scene
{
    static constexpr int INVALID_BLOCK_ID = -1;

    string name;
    fs::path path;
    vector<Token>::const_iterator token_begin;
    vector<Token>::const_iterator token_end;
    set<string> preload_resources;
    set<string> characters;
    map<int, vector<string>> code_blocks;

    Scene(
        string name,
        fs::path pathses,
        const vector<Token>::const_iterator token_begin,
        const vector<Token>::const_iterator token_end)
        : name(std::move(name))
        , path(std::move(pathses))
        , token_begin(token_begin)
        , token_end(token_end)
    {
    }

    void parseCodeBlocks()
    {
        code_blocks.clear();

        ifstream in(path);
        in.exceptions(ios::badbit);
        string line, token;
        auto block_id = INVALID_BLOCK_ID;
        vector<string> *block = nullptr;

        while(getline(in, line))
        {
            strstream s;
            s << line;
            s >> token;
            if(block_id == INVALID_BLOCK_ID)
            {
                if(token == "#!code-begin")
                {
                    s >> block_id;
                    block = &code_blocks[block_id];
                }
            }
            else
            {
                if(token == "#!code-end")
                {
                    block_id = INVALID_BLOCK_ID;
                    block = nullptr;
                }
                else
                {
                    block->push_back(std::move(line));
                }
            }
        }
    }

    void writeFile()
    {
        if(fs::exists(path))
        {
            parseCodeBlocks();
        }
    }
};

struct Translator
{
    vector<Token> tokens;
    vector<Token>::iterator pos;
    fs::path output_dir;
    vector<string> heading_stack;

    Translator(vector<Token> tokens, fs::path output_dir)
        : tokens(std::move(tokens))
        , output_dir(std::move(output_dir))
    {
        pos = tokens.begin();
        translate();
    }

    TokenType peekType() const
    {
        return pos == tokens.end() ? TokenType::END : pos->type;
    }

    void translate()
    {
        switch(peekType())
        {
            case TokenType::HEADING_MARK:
            {

                break;
            }
            case TokenType::HEADING_TAG: break;
            case TokenType::HEADING_NAME: break;
            case TokenType::CODE_TAG: break;
            case TokenType::CODE_COMMENT: break;
            case TokenType::COMMAND_NAME: break;
            case TokenType::COMMAND_PARAM: break;
            case TokenType::CHAR_PARAM: break;
            case TokenType::MESSAGE: break;
            default: ;
        }
    }
};
}

void exitHandler()
{
    return;
}

int main(const int argc, char *argv[])
{
    using namespace moeloop;

#ifdef _WIN32
    usagi::win32::patchConsole();
#endif

    atexit(&exitHandler);

    if(argc < 3)
    {
        cerr << "Usage: <input file> <output directory>" << endl;
        exit(INVALID_USAGE);
    }

    const fs::path input_file = argv[1];
    const fs::path output_dir = argv[2];

    ifstream script(input_file);
    script.exceptions(std::ios::badbit);
    fs::create_directories(output_dir);

    Tokenizer tokenizer(script);
    for(auto &&t : tokenizer.tokens)
    {
        cout << setw(20) << tokenName(t.type) << ": " << t.data << endl;
    }
}
