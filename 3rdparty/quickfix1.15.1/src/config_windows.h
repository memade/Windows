#pragma once

// Define if you are using stlport
// #define HAVE_STLPORT

// Define if you have sql library (libMySQL.lib)
// #define HAVE_MYSQL 1

// Define if you have sql library (libpq.lib)
// #define HAVE_POSTGRESQL 1

// Define if you have odbc library (Odbc32.lib)
// #define HAVE_ODBC 1

// Define if you have ftime
#define HAVE_FTIME 1

// Define if you have openssl
#define HAVE_SSL 1

#define HAVE_STD_SHARED_PTR  1

#define HAVE_STD_UNIQUE_PTR  1

#define HAVE_PACKEY_COMPRESS 1

#if HAVE_PACKEY_COMPRESS
const size_t GC_PACKET_PROCESS_SIZE = 1024;
#endif

#if _MSC_VER >= 1928
#pragma warning(disable :5040 4996)
#endif

