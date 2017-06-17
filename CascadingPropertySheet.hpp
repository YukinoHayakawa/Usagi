#pragma once

#include "PropertySheet.hpp"
#include <map>

namespace yuki
{
namespace property_sheet
{

/**
 * \brief A hierarchical implementation of PropertySheet. A child sheet can
 * inherit or override values from its parent.
 */
class CascadingPropertySheet : public PropertySheet
{
private:
    CascadingPropertySheet *m_parent = nullptr;
    std::map<std::string, std::any> m_value_map;

public:
    CascadingPropertySheet(CascadingPropertySheet *parent = nullptr)
        : m_parent{ parent }
    {
    }

private:
    std::any & _find_value(const std::string &key) override
    {
        auto iter = m_value_map.find(key);
        if(iter == m_value_map.end())
        {
            if(m_parent)
                return m_parent->_find_value(key);
            throw KeyNotFoundException("The specified key cannot be found in this property sheet and its parent.");
        }
        return iter->second;
    }

    void _insert_value(std::string key, std::any value) override
    {
        auto result = m_value_map.insert(std::make_pair(std::move(key), std::move(value)));
        if(!result.second)
            throw DuplicateKeyException("The key is already used.");
    }

    bool _erase_value(const std::string &key) override
    {
        return m_value_map.erase(key) != 0;
    }
};

}
}
