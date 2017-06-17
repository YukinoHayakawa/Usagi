#pragma once

#include <string>
#include <any>

namespace yuki
{
namespace property_sheet
{

class DuplicateKeyException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class KeyNotFoundException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class PropertySheet
{
public:
    virtual ~PropertySheet()
    {
    }

    template <typename ValueType>
    ValueType find(const std::string &key)
    {
        return std::any_cast<ValueType>(_find_value(key));
    }

    void insert(std::string key, std::any value)
    {
        _insert_value(std::move(key), std::move(value));
    }

    void insert(std::string key, const char *value)
    {
        _insert_value(std::move(key), std::make_any<std::string>(value));
    }

    bool erase(const std::string &key)
    {
        return _erase_value(key);
    }

private:
    virtual std::any & _find_value(const std::string &key) = 0;
    virtual bool _erase_value(const std::string &key) = 0;
    virtual void _insert_value(std::string key, std::any value) = 0;
};

}
}
