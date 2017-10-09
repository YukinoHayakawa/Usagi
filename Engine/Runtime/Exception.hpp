#pragma once

#include <boost/exception/all.hpp>

namespace yuki
{

// Base Exception Types

struct Exception : virtual std::exception, virtual boost::exception { };

struct RuntimeException : virtual Exception { };
struct LogicException : virtual Exception { };

// Runtime Exceptions

struct APIException : virtual RuntimeException { };
struct OSException : virtual RuntimeException { };
struct GraphicsException : virtual RuntimeException { };

// - Incompatible Environment Exception Family

struct IncompatibleEnvironmentException : virtual RuntimeException { };
typedef boost::error_info<struct tagFeatureDescription, const char *> FeatureDescriptionInfo;
struct UnsupportedFeatureException : virtual IncompatibleEnvironmentException { };
typedef boost::error_info<struct tagPlatformDescription, const char *> PlatformDescriptionInfo;
struct UnsupportedPlatformException : virtual IncompatibleEnvironmentException { };

// Logic Exceptions

typedef boost::error_info<struct tagEnumName, const char *> EnumNameInfo;
struct InvalidEnumException : virtual LogicException { };

// Combined Exception Types

// - OS API Exception Family

struct OSAPIException : virtual APIException, virtual OSException { };
struct OSAPIUnsupportedPlatformException : virtual OSAPIException, virtual UnsupportedPlatformException { };

// - Graphics API Exception Family

struct GraphicsAPIException : virtual APIException, virtual GraphicsException { };
struct GraphicsAPIInvalidEnumException : virtual GraphicsAPIException, virtual InvalidEnumException { };
struct GraphicsAPIUnsupportedFeatureException : virtual GraphicsAPIException, virtual UnsupportedFeatureException { };

}
