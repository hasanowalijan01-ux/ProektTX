#include "vigenere.h"
#include <stdexcept>
#include <cstdint>
#include <vector>

namespace pd::crypto {

// Decode UTF-8 byte sequence -> codepoints.
static std::vector<uint32_t> utf8_decode(const std::string& s) {
    std::vector<uint32_t> r;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        uint32_t cp = 0; int n = 0;
        if (c < 0x80)        { cp = c; n = 1; }
        else if ((c>>5)==6)  { cp = c & 0x1F; n = 2; }
        else if ((c>>4)==14) { cp = c & 0x0F; n = 3; }
        else if ((c>>3)==30) { cp = c & 0x07; n = 4; }
        else { cp = c; n = 1; }
        for (int k = 1; k < n && i+k < s.size(); ++k)
            cp = (cp<<6) | (s[i+k] & 0x3F);
        r.push_back(cp);
        i += n;
    }
    return r;
}

static std::string utf8_encode(const std::vector<uint32_t>& cps) {
    std::string r;
    for (auto cp : cps) {
        if (cp < 0x80) r.push_back((char)cp);
        else if (cp < 0x800) {
            r.push_back((char)(0xC0 | (cp>>6)));
            r.push_back((char)(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            r.push_back((char)(0xE0 | (cp>>12)));
            r.push_back((char)(0x80 | ((cp>>6) & 0x3F)));
            r.push_back((char)(0x80 | (cp & 0x3F)));
        } else {
            r.push_back((char)(0xF0 | (cp>>18)));
            r.push_back((char)(0x80 | ((cp>>12) & 0x3F)));
            r.push_back((char)(0x80 | ((cp>>6) & 0x3F)));
            r.push_back((char)(0x80 | (cp & 0x3F)));
        }
    }
    return r;
}

// Returns (base, size, isUpper) for a letter or {0,0,false} for non-letter.
struct Info { uint32_t base; uint32_t size; bool letter; bool upper; };
static Info classify(uint32_t cp) {
    if (cp >= 'A' && cp <= 'Z') return {'A', 26, true, true};
    if (cp >= 'a' && cp <= 'z') return {'a', 26, true, false};
    if (cp >= 0x0410 && cp <= 0x042F) return {0x0410, 32, true, true};   // А-Я
    if (cp >= 0x0430 && cp <= 0x044F) return {0x0430, 32, true, false};  // а-я
    if (cp == 0x0401) return {0x0410, 32, true, true};   // Ё treat as Е
    if (cp == 0x0451) return {0x0430, 32, true, false};
    return {0, 0, false, false};
}

std::string Vigenere::transform(const std::string& text, const std::string& key, bool enc) {
    auto cps = utf8_decode(text);
    auto kps = utf8_decode(key);

    std::vector<uint32_t> kletters;
    for (auto cp : kps) {
        auto info = classify(cp);
        if (info.letter) {
            // normalize to lowercase index in its alphabet, but we'll store actual letter codepoint
            kletters.push_back(cp);
        }
    }
    if (kletters.empty())
        throw std::invalid_argument("Vigenere: key must contain alphabetic chars");

    std::vector<uint32_t> out;
    out.reserve(cps.size());
    size_t ki = 0;
    for (auto cp : cps) {
        auto info = classify(cp);
        if (!info.letter) { out.push_back(cp); continue; }
        // shift = position of key letter inside its own alphabet
        uint32_t kcp = kletters[ki % kletters.size()];
        auto kinfo = classify(kcp);
        uint32_t shift = kcp - kinfo.base;
        // map shift to current text alphabet size (use modulo with text alphabet)
        shift = shift % info.size;
        uint32_t idx = cp - info.base;
        uint32_t newIdx = enc ? (idx + shift) % info.size
                              : (idx + info.size - shift) % info.size;
        out.push_back(info.base + newIdx);
        ++ki;
    }
    return utf8_encode(out);
}

std::string Vigenere::encrypt(const std::string& p, const std::string& k) { return transform(p, k, true); }
std::string Vigenere::decrypt(const std::string& c, const std::string& k) { return transform(c, k, false); }

} // namespace pd::crypto
