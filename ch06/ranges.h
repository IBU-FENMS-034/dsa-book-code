#ifndef CH06_RANGES_H
#define CH06_RANGES_H

#include <string>
#include "Search.h"

// The lab's IP-to-location problem, reduced to the part this chapter is about.
// The file holds ranges rather than single keys, so the search has to find the
// range a key falls inside rather than a position holding the key itself.
namespace Ranges {

    struct Block {
        long start;             // first address in the range, as a number
        long end;               // last address in the range, inclusive
        std::string country;
    };

    // An IPv4 address written w.x.y.z as the number it stands for. Each octet
    // is a digit in base 256, most significant first.
    inline long to_number(const std::string& ip) {
        long value = 0, octet = 0;
        int seen = 0;
        for (char c : ip + ".") {
            if (c == '.') {
                value = value * 256 + octet;
                octet = 0;
                ++seen;
            } else {
                octet = octet * 10 + (c - '0');
            }
        }
        return seen == 4 ? value : -1;
    }

    // The blocks are sorted by start and do not overlap, so the block that can
    // contain the key is the last one that starts at or before it. Binary
    // search finds that block; then one comparison says whether the key is
    // actually inside it or in the gap after it.
    inline int find(const long* starts, const long* ends, int len, long key) {
        int i = Search::floor_index(starts, len, key);
        if (i >= 0 && key <= ends[i]) return i;
        return -1;
    }

}

#endif
