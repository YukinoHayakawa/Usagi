#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <stack>
#include <set>
#include <map>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <utfcpp/utf8.h>

#ifdef _WIN32
#include <Usagi/Extension/Win32/Win32Helper.hpp>
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
    LOGIC_ERROR,
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
    int line;
};

ostream & operator<<(ostream &s, const Token &t)
{
    s << setw(20) << tokenName(t.type) << ": " << t.data;
    return s;
}

const auto CODE_BEGIN_TAG = "--!code-begin";
const auto CODE_END_TAG = "--!code-end";
const auto COMMENT_TAG = "--!comment";
const auto SCENE_BEGIN_TAG = "--!scene-begin";
const auto SCENE_END_TAG = "--!scene-end";

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
    int line_num = 0;

    void skipSpace() { while(pos != line.end() && isSpace(*pos)) ++pos; }

    void expect(const uint32_t c)
    {
        skipSpace();
        if(*pos != c)
        {
            cerr << fmt::format("Line {}: Expected a {}", line_num, c) << endl;
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
            cerr << fmt::format("Line {}: Expected a {}", line_num, c) << endl;
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
            cerr << fmt::format("Line {}: Expected a {}", line_num, c) << endl;
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
            cerr << fmt::format("Line {}: Expected a string", line_num) << endl;
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
            tokens.push_back({ param, tillAnyCharTrimmed(',', end), line_num });
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
            ++line_num;
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
                    // ignore comment (begin with ######)
                    if(sharp_end - pos == 6) break;
                    tokens.push_back({ TokenType::HEADING_MARK, { pos, sharp_end }, line_num });
                    pos = sharp_end;
                    expect('[');
                    tokens.push_back({ TokenType::HEADING_TAG, tillCharTrimmed(']'), line_num });
                    expect(']');
                    tokens.push_back({ TokenType::HEADING_NAME, tillEndTrimmed(), line_num });
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
                        tokens.push_back({ TokenType::CODE_TAG, std::move(tag), line_num });
                        expect(':');
                        tokens.push_back({ TokenType::CODE_COMMENT, tillCharTrimmed('}'), line_num });
                    }
                    else
                    {
                        tokens.push_back({ TokenType::COMMAND_NAME, std::move(tag), line_num });
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
                    tokens.push_back({ TokenType::MESSAGE, tillEndTrimmed(), line_num });
                    break;
                }
                // narrator message
                default:
                {
                    tokens.push_back({ TokenType::MESSAGE, tillEndTrimmed(), line_num });
                    break;
                }
            }
            tokens.push_back({ TokenType::NEWLINE, "", line_num });
        }
        tokens.push_back({ TokenType::END, "", line_num });
    }
};

struct Scene
{
    static constexpr int INVALID_BLOCK_ID = -1;

    string name;
    string comment_name;
    fs::path path;
    vector<Token>::const_iterator begin;
    vector<Token>::const_iterator end;
    vector<Token>::const_iterator pos;
    struct Character
    {
        string obj;
        string last_expr;
        string last_pos;
        bool in_scene = false;
    };
    map<string, Character> characters;
    map<int, vector<string>> code_blocks;
    enum Section
    {
        BEFORE_SCENE,
        IN_SCENE,
        AFTER_SCENE
    } section = BEFORE_SCENE;
    vector<string> lines_before_scene;
    vector<string> lines_after_scene;
    set<int> code_ids;
    stringstream output;
    map<string, string> expressions;
    map<string, string> positions;

    Scene(fs::path _path, vector<Token>::const_iterator _begin)
        : path(std::move(_path))
        , begin(std::move(_begin))
    {
        pos = begin;
        if(fs::exists(path))
        {
            parseCodeBlocks();
        }
    }

    TokenType type() const
    {
        return pos == end ? TokenType::END : pos->type;
    }

    TokenType peekNextType() const
    {
        const auto next = pos + 1;
        return next == pos ? TokenType::END : next->type;
    }

    const string & data() const
    {
        return pos->data;
    }

    TokenType expectAny(std::initializer_list<TokenType> types)
    {
        ++pos;
        const auto i = find(types.begin(), types.end(), type());
        if(i == types.end())
        {
            cout << fmt::format("Line {}: Expected any of: ", pos->line);
            for(auto && t : types)
            {
                cout << tokenName(t) << " ";
            }
            cout << endl;
            throw std::runtime_error("");
        }
        return *i;
    }

    const string & expect(const TokenType type)
    {
        if((++pos)->type != type)
        {
            cout << fmt::format("Line {}: Expected a {}", pos->line, tokenName(type)) << endl;
            throw std::runtime_error("");
        }
        return data();
    }

    void insertCodeBlock(const int id)
    {
        const auto i = code_blocks.find(id);
        if(i == code_blocks.end())
        {
            output << fmt::format("unimplemented(\"Code block {}\");", id) << endl;
        }
        else
        {
            for(auto &&c : i->second)
            {
                output << c << endl;
            }
        }
    }

    void checkCharacterInScene(const string & cs)
    {
        const auto c = characters.find(cs);
        if(c == characters.end())
        {
            cout << fmt::format("Line {}: Character {} is undefined.", pos->line, cs) << endl;
            throw std::runtime_error("");
        }
        if(!c->second.in_scene)
        {
            cout << fmt::format("Line {}: Character {} has not entered the scene yet.", pos->line, cs) << endl;
            throw std::runtime_error("");
        }
    }

    Character & character(const string & cs)
    {
        const auto i = characters.find(cs);
        if(i == characters.end())
        {
            Character c;
            c.obj = "c" + to_string(characters.size());
            return characters.insert({ cs, std::move(c) }).first->second;
        }
        return i->second;
    }

    string expressionObj(const string & cs)
    {
        const auto i = expressions.find(cs);
        if(i == expressions.end())
        {
            auto hash = "e" + to_string(expressions.size());
            expressions.insert({ cs, hash });
            return hash;
        }
        return i->second;
    }

    string positionObj(const string & cs)
    {
        const auto i = positions.find(cs);
        if(i == positions.end())
        {
            auto hash = "p" + to_string(positions.size());
            positions.insert({ cs, hash });
            return hash;
        }
        return i->second;
    }

    void charChangeExpr(const string & cs, const string & expr)
    {
        auto &c = character(cs);
        if(expr == c.last_expr) return;
        output << fmt::format("-- {},{}\n{}:changeExpression({});",
            cs,
            expr,
            c.obj,
            expressionObj(expr)
        ) << endl;
        c.last_expr = expr;
    }

    void charMove(const string & cs, const string & pos)
    {
        auto &c = character(cs);
        if(pos == c.last_pos) return;
        output << fmt::format("-- {},{}\n{}:move({});",
            cs, pos,
            c.obj,
            positionObj(pos)
        ) << endl;
        c.last_pos = pos;
    }

    void ensureInScene(const string &c, const string &expr, const string &pos)
    {
        auto &cc = character(c);
        if(!cc.in_scene)
        {
            output << fmt::format("-- {},{},{}\n{}:enterScene({}, {});",
                c,
                expr, pos,
                cc.obj,
                expressionObj(cc.last_expr = expr),
                positionObj(cc.last_pos = pos)
            ) << endl;
            cc.in_scene = true;
        }
    }

    void charSay(const string &c, const string &expr, const string &pos, const string &msg_line)
    {
        auto &cc = character(c);
        ensureInScene(c, expr, pos);
        charChangeExpr(c, expr);
        charMove(c, pos);
        output << msg_line << endl;
    }

    void charMsg(const string &c, const string &msg_line)
    {
        auto &cc = character(c);
        output << msg_line << endl;
    }

    void exitScene(const std::string &cn, Character &c)
    {
        output << fmt::format("-- {}\n{}:exitScene();", cn, c.obj) << endl;
        c.in_scene = false;
    }

    void parse()
    {
        while(pos != end)
        {
            switch(type())
            {
                case TokenType::NEWLINE:
                {
                    break;
                }
                case TokenType::END:
                {
                    cout << "End of scene: " << name << endl;
                    return;
                }
                case TokenType::CODE_TAG:
                {
                    output << endl;
                    const auto id = data();
                    output << fmt::format("{} {}", CODE_BEGIN_TAG, id) << endl;
                    if(!expect(TokenType::CODE_COMMENT).empty())
                        output << fmt::format("{} {}", COMMENT_TAG, data()) << endl;
                    const auto int_id = stoi(id);
                    if(code_ids.find(int_id) != code_ids.end())
                    {
                        cout << fmt::format("Line {}: Code block id {} is already used.", pos->line, int_id) << endl;
                        throw std::runtime_error("");
                    }
                    code_ids.insert(int_id);
                    insertCodeBlock(int_id);
                    output << CODE_END_TAG << endl;
                    output << endl;
                    expect(TokenType::NEWLINE);
                    break;
                }
                case TokenType::COMMAND_NAME:
                {
                    if(data() == "expr")
                    {
                        checkCharacterInScene(expect(TokenType::COMMAND_PARAM));
                        charChangeExpr(data(), expect(TokenType::COMMAND_PARAM));
                    }
                    else if(data() == "move")
                    {
                        checkCharacterInScene(expect(TokenType::COMMAND_PARAM));
                        charMove(data(), expect(TokenType::COMMAND_PARAM));
                    }
                    else if(data() == "exitall")
                    {
                        for(auto &&c : characters)
                        {
                            if(c.second.in_scene)
                            {
                                exitScene(c.first, c.second);
                            }
                        }
                    }
                    else if(data() == "exit")
                    {
                        checkCharacterInScene(expect(TokenType::COMMAND_PARAM));
                        auto &c = characters[data()];
                        exitScene(data(), c);
                    }
                    else if(data() == "state")
                    {
                        const auto cn = expect(TokenType::COMMAND_PARAM);
                        const auto expr = expect(TokenType::COMMAND_PARAM);
                        const auto pos = expect(TokenType::COMMAND_PARAM);
                        ensureInScene(cn, expr, pos);
                        charChangeExpr(cn, expr);
                        charMove(cn, pos);
                    }
                    else
                    {
                        cout << fmt::format("Line {}: Invalid command {}.", pos->line, data()) << endl;
                        throw std::runtime_error("");
                    }
                    expect(TokenType::NEWLINE);
                    break;
                }
                case TokenType::CHAR_PARAM:
                {
                    const auto pretend = data().find('=');
                    if(pretend == string::npos)
                    {
                        if(peekNextType() == TokenType::CHAR_PARAM)
                        {
                            const auto cn = data();
                            auto &c = character(cn);
                            const auto expr = expect(TokenType::CHAR_PARAM);
                            const auto pos = expect(TokenType::CHAR_PARAM);
                            charSay(cn, expr, pos, fmt::format(
                                "-- {},{}\n{}:say(\"{}\"); w();",
                                cn, pos,
                                c.obj,
                                expect(TokenType::MESSAGE)
                            ));
                        }
                        else
                        {
                            const auto cn = data();
                            charMsg(cn, fmt::format(
                                "-- {}\n{}:say(\"{}\"); w();",
                                cn,
                                character(cn).obj,
                                expect(TokenType::MESSAGE)
                            ));
                        }
                    }
                    else
                    {
                        const auto pretend_name = data().substr(0, pretend);
                        const auto real_name = data().substr(pretend + 1, string::npos);
                        if(peekNextType() == TokenType::CHAR_PARAM)
                        {
                            auto &c = character(real_name);
                            const auto expr = expect(TokenType::CHAR_PARAM);
                            const auto pos = expect(TokenType::CHAR_PARAM);
                            charSay(real_name, expr, pos, fmt::format(
                                "-- {}\n{}:pretendSay(\"{}\", \"{}\"); w();",
                                real_name,
                                c.obj,
                                pretend_name,
                                expect(TokenType::MESSAGE)
                            ));
                        }
                        else
                        {
                            charMsg(real_name, fmt::format(
                                "-- {}\n{}:pretendSay(\"{}\", \"{}\"); w();",
                                real_name,
                                character(real_name).obj,
                                pretend_name,
                                expect(TokenType::MESSAGE)
                            ));
                        }
                    }
                    break;
                }
                case TokenType::MESSAGE:
                {
                    output << fmt::format("narrator:say(\"{}\"); w();", data()) << endl;
                    break;
                }
                default:
                {
                    cout << fmt::format("Line {}: Unexpected token.", pos->line) << endl;
                    throw std::runtime_error("");
                }
            }
            ++pos;
        }
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
            stringstream s;
            s << line;
            token.clear();
            s >> token;
            if(token == CODE_BEGIN_TAG)
            {
                assert(section == IN_SCENE);
                assert(block_id == INVALID_BLOCK_ID);

                s >> block_id;
                block = &code_blocks[block_id];
            }
            else if(token == CODE_END_TAG)
            {
                assert(section == IN_SCENE);
                assert(block_id != INVALID_BLOCK_ID);

                block_id = INVALID_BLOCK_ID;
                block = nullptr;
            }
            else if(token == SCENE_BEGIN_TAG)
            {
                assert(section == BEFORE_SCENE);

                section = IN_SCENE;
            }
            else if(token == SCENE_END_TAG)
            {
                assert(section == IN_SCENE);

                section = AFTER_SCENE;
            }
            else if(token == COMMENT_TAG)
            {
                // ignored. comment will be added back from the source.
            }
            else
            {
                switch(section)
                {
                    case BEFORE_SCENE:
                    {
                        lines_before_scene.push_back(std::move(line));
                        break;
                    }
                    case IN_SCENE:
                    {
                        if(block_id != INVALID_BLOCK_ID)
                            block->push_back(std::move(line));
                        break;
                    }
                    case AFTER_SCENE:
                    {
                        lines_after_scene.push_back(std::move(line));
                        break;
                    }
                    default: assert(false);
                }
            }
        }
    }

    void writeFile()
    {
        cout << "Writing scene script: " << path << endl;

        fs::create_directories(path.parent_path());
        ofstream out(path);
        out.exceptions(ios::badbit | ios::failbit);

        for(auto && l : lines_before_scene)
        {
            out << l << "\n";
        }
        out << SCENE_BEGIN_TAG << "\n";
        out << COMMENT_TAG << " " << name << " " << comment_name << "\n";
        out << "local scene = game:currentScene();\n\n";
        out << "local narrator = scene:loadCharacter(\"Narrator\");\n\n";
        if(!characters.empty())
        {
            for(auto &&c : characters)
            {
                out << fmt::format("local {} = scene:loadCharacter(\"{}\");\n", c.second.obj, c.first);
            }
            out << "\n";
        }
        if(!expressions.empty())
        {
            for(auto &&e : expressions)
            {
                out << fmt::format("local {} = scene:loadExpression(\"{}\");\n", e.second, e.first);
            }
            out << "\n";
        }
        if(!positions.empty())
        {
            for(auto &&e : positions)
            {
                out << fmt::format("local {} = scene:getPosition(\"{}\");\n", e.second, e.first);
            }
            out << "\n";
        }
        out << output.str();
        out << SCENE_END_TAG << "\n";
        for(auto && l : lines_after_scene)
        {
            out << l << "\n";
        }
    }
};

struct Translator
{
    vector<Token> tokens;
    vector<Token>::iterator pos;
    fs::path output_dir;
    vector<string> heading_stack; // size = heading depth
    unique_ptr<Scene> scene;
    bool reading_message = false;

    Translator(vector<Token> _tokens, fs::path output_dir)
        : tokens(std::move(_tokens))
        , output_dir(std::move(output_dir))
    {
        pos = tokens.begin();
        translate();
    }

    TokenType type()
    {
        return pos == tokens.end() ? TokenType::END : pos->type;
    }

    string & data()
    {
        return pos->data;
    }

    string peekData()
    {
        const auto next = pos + 1;
        return next == tokens.end() ? "<END>" : next->data;
    }

    string buildSceneName()
    {
        stringstream name;
        name << "scene_";
        for(auto &&h : heading_stack)
        {
            name << h << "_";
        }
        string s = name.str();
        if(!heading_stack.empty())
            s.pop_back();
        return s;
    }

    const string & expect(const TokenType type)
    {
        if((++pos)->type != type)
        {
            cout << fmt::format("Line {}: Expected a {}", pos->line, tokenName(type)) << endl;
            exit(SYNTAX_ERROR);
        }
        return data();
    }

    void endLastScene()
    {
        assert(!heading_stack.empty());
        assert(scene);
        if(reading_message)
        {
            scene->end = pos;
            try
            {
                scene->parse();
                scene->writeFile();
            }
            catch(...)
            {
                cout << "(In scene " << scene->comment_name << ")" << endl;
            }
        }
        scene.reset();
        heading_stack.pop_back();
        reading_message = false;
    }

    void nextLine()
    {
        while(pos != tokens.end())
        {
            if(pos->type == TokenType::NEWLINE)
            {
                ++pos;
                return;
            }
            ++pos;
        }
    }

    void translate()
    {
        while(pos != tokens.end())
        {
            switch(type())
            {
                case TokenType::HEADING_MARK:
                {
                    const auto depth = data().size(); // number of #s
                    if(depth <= heading_stack.size()) // popping, last scene ends
                    {
                         endLastScene();
                    }
                    else // pushing
                    {
                        // to push a heading, there must not be any message between
                        // current heading and parent heading
                        if(reading_message)
                        {
                            cout << fmt::format("Line {}: Scene cannot be nested.", pos->line) << endl;
                            exit(LOGIC_ERROR);
                        }
                    }
                    heading_stack.resize(depth - 1);
                    heading_stack.push_back(expect(TokenType::HEADING_TAG));
                    const auto comment_name = expect(TokenType::HEADING_NAME);
                    nextLine();
                    const auto name = buildSceneName();
                    auto path = output_dir / name;
                    path += ".lua";
                    scene.reset(new Scene(path, pos));
                    scene->name = name;
                    scene->comment_name = comment_name;
                    break;
                }
                case TokenType::END:
                {
                    if(scene) endLastScene();
                    nextLine();
                    break;
                }
                case TokenType::NEWLINE:
                {
                    nextLine();
                    break;
                }
                default:
                {
                    reading_message = true;
                    nextLine();
                }
            }

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
    fs::path output_dir = argv[2];

    ifstream script(input_file);
    script.exceptions(std::ios::badbit);
    fs::create_directories(output_dir);

    Tokenizer tokenizer(script);
    Translator translator(std::move(tokenizer.tokens), std::move(output_dir));
}
