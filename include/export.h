#ifndef USCAN_INCLUDE_EXPORT_H_
#define USCAN_INCLUDE_EXPORT_H_

#if !defined(ME_EXPORT)

#if defined(ME_SHARED_LIBRARY)
#if defined(_WIN32)

#if defined(ME_COMPILE_LIBRARY)
#define ME_EXPORT __declspec(dllexport)
#else
#define ME_EXPORT __declspec(dllimport)
#endif  // defined(ME_COMPILE_LIBRARY)

#else  // defined(_WIN32)
#if defined(ME_COMPILE_LIBRARY)
#define ME_EXPORT __attribute__((visibility("default")))
#else
#define ME_EXPORT
#endif
#endif  // defined(_WIN32)

#else  // defined(ME_SHARED_LIBRARY)
#define ME_EXPORT
#endif

#endif

#endif /* USCAN_INCLUDE_EXPORT_H_ */
