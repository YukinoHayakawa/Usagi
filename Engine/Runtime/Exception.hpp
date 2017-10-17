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
struct MemoryException : virtual RuntimeException { };
struct TimeoutException : virtual RuntimeException { };

// - File Exceptions

struct CorruptedDataFileException : virtual FileException { };
struct CannotOpenFileException : virtual FileException { };

// - Memory Exceptions

typedef boost::error_info<struct tagSize, uint64_t> SizeInfo;
struct MemoryAllocationException : virtual MemoryException { };

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
struct OutOfBoundException : virtual LogicException { };
typedef boost::error_info<struct tagSubsystem, std::string> SubsystemInfo;
typedef boost::error_info<struct tagComponent, std::string> ComponentInfo;
struct MismatchedSubsystemComponentException : virtual LogicException { };
struct InvalidArgumentException : virtual LogicException { };
struct NotExpectedToBeReachedException : virtual LogicException { };
typedef boost::error_info<struct tagConfigure, std::string> ConfigureInfo;
struct InappropriatelyConfiguredException : virtual LogicException { };

// Combined Exception Types

// - OS Exception Family

struct OSAPIException : virtual APIException, virtual OSException { };
struct OSAPIUnsupportedPlatformException : virtual OSAPIException, virtual UnsupportedPlatformException { };

// - Graphics Exception Family

struct GraphicsAPIException : virtual APIException, virtual GraphicsException { };
struct GraphicsAPIEnumTranslationException : virtual GraphicsAPIException, virtual EnumTranslationException { };
struct GraphicsAPIUnsupportedFeatureException : virtual GraphicsAPIException, virtual UnsupportedFeatureException { };
typedef boost::error_info<struct tagMemoryUsage, std::string> MemoryUsageInfo;
struct GraphicsDeviceMemoryAllocationFailureException : virtual GraphicsException, virtual MemoryAllocationException { };

// - Memory Exception Family
struct MemoryAccessOutOfBoundException : virtual MemoryException, virtual OutOfBoundException { };
struct UnmatchedMemoryPoolException : virtual MemoryException, virtual LogicException { };

}
