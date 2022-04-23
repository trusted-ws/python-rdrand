#ifndef __PYCDEF__
#define __PYCDEF__

#define rdrand__doc__                                                   \
    "Intel's RDRAND/RDSEED interface for Python 3.9"

#define range__doc__                                                    \
    "range(length, boundary=90)\n"                                      \
    "--\n\n"                                                            \
    "Generate a non-repeated random range with specified length."       \

#define randint__doc__                                                  \
    "randint(min, max)\n"                                               \
    "--\n\n"                                                            \
    "DEPRECATED. DO NOT USE!"                     \

#define randbits__doc__                                                  \
    "randbits(number_bits)\n"                                            \
    "--\n\n"                                                             \
    "Generate `number_bits` random bits."                                \

#define range_below__doc__                                             \
    "range_below(length, below)\n"                                    \
    "--\n\n"                                                            \
    "Generate a N positive integers below."                            \

#define n_range_below__doc__                                           \
    "n_range_below(length, below, amount)\n"                          \
    "--\n\n"                                                            \
    "Generate a N-sized array with 'length' positive integers below."  \

#define is_rdrand_supported__doc__                                      \
    "is_rdrand_supported()\n"                                           \
    "--\n\n"                                                            \
    "Check if rdrand instruction is supported by current CPU."          \

#define is_rdseed_supported__doc__                                      \
    "is_rdseed_supported()\n"                                           \
    "--\n\n"                                                            \
    "Check if rdseed instruction is supported by current CPU."          \

#define rdseed__doc__                                                   \
    "rdseed()\n"                                                        \
    "--\n\n"                                                            \
    "Return an int converted from uint64_t using  rdseed instruction."  \

#endif