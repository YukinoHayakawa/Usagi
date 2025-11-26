#pragma once

namespace usagi
{
struct construct_by_member_refs_t
{
};

constexpr inline construct_by_member_refs_t construct_by_member_refs;

struct construct_by_ref_t
{
};

constexpr inline construct_by_ref_t construct_by_ref;

struct construct_in_place_t
{
};

constexpr inline construct_in_place_t construct_in_place;

struct construct_no_name_t
{
};

constexpr inline construct_no_name_t construct_no_name;

struct construct_default_name_t
{
};

constexpr inline construct_default_name_t construct_default_name;
} // namespace usagi
