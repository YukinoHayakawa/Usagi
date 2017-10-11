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
struct FileException : virtual RuntimeException { };

// - File Exceptions

struct CorruptedDataFileException : virtual FileException { };
struct CannotOpenFileException : virtual FileException { };

// - Graphics Exceptions

struct GraphicsSwapChainNotAvailableException : virtual GraphicsException { };

// - Incompatible Environment Exception Family

struct IncompatibleEnvironmentException : virtual RuntimeException { };
typedef boost::error_info<struct tagFeatureDescription, std::string> FeatureDescriptionInfo;
struct UnsupportedFeatureException : virtual IncompatibleEnvironmentException { };
typedef boost::error_info<struct tagPlatformDescription, std::string> PlatformDescriptionInfo;
struct UnsupportedPlatformException : virtual IncompatibleEnvironmentException { };

// Logic Exceptions

typedef boost::error_info<struct tagEnum, std::string> EnumInfo;
struct EnumTranslationException : virtual LogicException { };
typedef boost::error_info<struct tagSubsystem, std::string> SubsystemInfo;
typedef boost::error_info<struct tagComponent, std::string> ComponentInfo;
struct MismatchedSubsystemComponentException : virtual LogicException { };

// Combined Exception Types

// - OS API Exception Family

struct OSAPIException : virtual APIException, virtual OSException { };
struct OSAPIUnsupportedPlatformException : virtual OSAPIException, virtual UnsupportedPlatformException { };

// - Graphics API Exception Family

struct GraphicsAPIException : virtual APIException, virtual GraphicsException { };
struct GraphicsAPIEnumTranslationException : virtual GraphicsAPIException, virtual EnumTranslationException { };
struct GraphicsAPIUnsupportedFeatureException : virtual GraphicsAPIException, virtual UnsupportedFeatureException { };

}
