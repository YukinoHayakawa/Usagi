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

/**
 * \brief A key-value mapping from string to any type.
 */
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

    /**
     * \brief Insert new key-value pair. If the key is already used, throws
     * DuplicateKeyException.
     * \param key 
     * \param value 
     */
    void insert(std::string key, std::any value)
    {
        _insert_value(std::move(key), std::move(value));
    }

    /**
     * \brief An overload of insert(), automatically converting a string literal
     * to std::string.
     * \param key 
     * \param value 
     */
    void insert(std::string key, const char *value)
    {
        _insert_value(std::move(key), std::make_any<std::string>(value));
    }
    
    /**
     * \brief Remove a key associated with value from the sheet.
     * \param key 
     * \return If the key-value pair is removed, returns true; otherwise returns
     * false.
     */
    bool erase(const std::string &key)
    {
        return _erase_value(key);
    }

    void set(const std::string &key, std::any value)
    {
        _set_value(std::move(key), std::move(value));
    }
    
    void set(const std::string &key, const char *value)
    {
        _set_value(std::move(key), std::make_any<std::string>(value));
    }

private:
    virtual std::any & _find_value(const std::string &key) = 0;
    virtual bool _erase_value(const std::string &key) = 0;
    virtual void _insert_value(std::string key, std::any value) = 0;
    virtual void _set_value(const std::string &key, std::any value) = 0;
};

}
}
