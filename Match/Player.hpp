#pragma once

#include <Usagi/PropertySheet/CascadingPropertySheet.hpp>

namespace yuki
{
namespace match
{

class PropertyNotFoundException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class Player
{
private:
    property_sheet::CascadingPropertySheet m_properties { };

public:
    template <typename T>
    T getProperty(const std::string &key) const
    {
        try
        {
            return m_properties.find<T>(key);
        }
        catch(property_sheet::KeyNotFoundException &)
        {
            throw PropertyNotFoundException("player does not have such property");
        }
    }

    template <typename T>
    void setProperty(const std::string &key, T &&value)
    {
        m_properties.set(key, value);
    }
};

}
}
