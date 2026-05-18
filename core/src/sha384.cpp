#include "sha384.h"
#include <cstring>
#include <cstdio>
#include <algorithm>

namespace pd::crypto {

static const uint64_t K[80] = {
    0x428a2f98d728ae22ULL,0x7137449123ef65cdULL,0xb5c0fbcfec4d3b2fULL,0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL,0x59f111f1b605d019ULL,0x923f82a4af194f9bULL,0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL,0x12835b0145706fbeULL,0x243185be4ee4b28cULL,0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL,0x80deb1fe3b1696b1ULL,0x9bdc06a725c71235ULL,0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL,0xefbe4786384f25e3ULL,0x0fc19dc68b8cd5b5ULL,0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL,0x4a7484aa6ea6e483ULL,0x5cb0a9dcbd41fbd4ULL,0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL,0xa831c66d2db43210ULL,0xb00327c898fb213fULL,0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL,0xd5a79147930aa725ULL,0x06ca6351e003826fULL,0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL,0x2e1b21385c26c926ULL,0x4d2c6dfc5ac42aedULL,0x53380d139d95b3dfULL,
    0x650a73548baf63deULL,0x766a0abb3c77b2a8ULL,0x81c2c92e47edaee6ULL,0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL,0xa81a664bbc423001ULL,0xc24b8b70d0f89791ULL,0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL,0xd69906245565a910ULL,0xf40e35855771202aULL,0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL,0x1e376c085141ab53ULL,0x2748774cdf8eeb99ULL,0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL,0x4ed8aa4ae3418acbULL,0x5b9cca4f7763e373ULL,0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL,0x78a5636f43172f60ULL,0x84c87814a1f0ab72ULL,0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL,0xa4506cebde82bde9ULL,0xbef9a3f7b2c67915ULL,0xc67178f2e372532bULL,
    0xca273eceea26619cULL,0xd186b8c721c0c207ULL,0xeada7dd6cde0eb1eULL,0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL,0x0a637dc5a2c898a6ULL,0x113f9804bef90daeULL,0x1b710b35131c471bULL,
    0x28db77f523047d84ULL,0x32caab7b40c72493ULL,0x3c9ebe0a15c9bebcULL,0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL,0x597f299cfc657e2aULL,0x5fcb6fab3ad6faecULL,0x6c44198c4a475817ULL
};

static inline uint64_t ROTR(uint64_t x, int n) { return (x >> n) | (x << (64 - n)); }

Sha384::Sha384() { 
    H[0]=0xcbbb9d5dc1059ed8ULL; H[1]=0x629a292a367cd507ULL;
    H[2]=0x9159015a3070dd17ULL; H[3]=0x152fecd8f70e5939ULL;
    H[4]=0x67332667ffc00b31ULL; H[5]=0x8eb44a8768581511ULL;
    H[6]=0xdb0c2e0d64f98fa7ULL; H[7]=0x47b5481dbefa4fa4ULL;
    bufLen = 0; totalBits = 0; finalized = false;
}

void Sha384::transform(const uint8_t block[128]) {
    uint64_t W[80];
    for (int i = 0; i < 16; ++i) {
        W[i] = 0;
        for (int j = 0; j < 8; ++j)
            W[i] = (W[i] << 8) | block[i*8 + j];
    }
    for (int i = 16; i < 80; ++i) {
        uint64_t s0 = ROTR(W[i-15],1) ^ ROTR(W[i-15],8) ^ (W[i-15] >> 7);
        uint64_t s1 = ROTR(W[i-2],19) ^ ROTR(W[i-2],61) ^ (W[i-2] >> 6);
        W[i] = W[i-16] + s0 + W[i-7] + s1;
    }
    uint64_t a=H[0],b=H[1],c=H[2],d=H[3],e=H[4],f=H[5],g=H[6],h=H[7];
    for (int i = 0; i < 80; ++i) {
        uint64_t S1 = ROTR(e,14) ^ ROTR(e,18) ^ ROTR(e,41);
        uint64_t ch = (e & f) ^ (~e & g);
        uint64_t t1 = h + S1 + ch + K[i] + W[i];
        uint64_t S0 = ROTR(a,28) ^ ROTR(a,34) ^ ROTR(a,39);
        uint64_t mj = (a & b) ^ (a & c) ^ (b & c);
        uint64_t t2 = S0 + mj;
        h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    H[0]+=a; H[1]+=b; H[2]+=c; H[3]+=d; H[4]+=e; H[5]+=f; H[6]+=g; H[7]+=h;
}

void Sha384::update(const void* data, size_t len) {
    const uint8_t* p = (const uint8_t*)data;
    totalBits += (uint64_t)len * 8;
    while (len > 0) {
        size_t take = std::min<size_t>(128 - bufLen, len);
        std::memcpy(buf + bufLen, p, take);
        bufLen += take; p += take; len -= take;
        if (bufLen == 128) { transform(buf); bufLen = 0; }
    }
}

std::array<uint8_t,48> Sha384::digest() {
    if (!finalized) {
        // Save the real message length BEFORE adding padding.
        uint64_t bitsLen = totalBits;
        uint8_t pad[256] = {0};
        pad[0] = 0x80;
        size_t padLen = (bufLen < 112) ? (112 - bufLen) : (240 - bufLen);
        update(pad, padLen); // this modifies totalBits, but we kept bitsLen
        uint8_t lenBlock[16] = {0};
        // 128-bit big-endian length; only the low 64 bits matter here.
        for (int i = 0; i < 8; ++i)
            lenBlock[15 - i] = (uint8_t)(bitsLen >> (i*8));
        // bufLen is now 112; manually append length to avoid mutating totalBits again.
        std::memcpy(buf + bufLen, lenBlock, 16);
        bufLen += 16;
        transform(buf);
        bufLen = 0;
        finalized = true;
    }
    std::array<uint8_t,48> out{};
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 8; ++j)
            out[i*8+j] = (uint8_t)(H[i] >> (56 - j*8));
    return out;
}

std::string Sha384::hex() {
    auto d = digest();
    std::string s; s.reserve(96);
    char b[3];
    for (auto v : d) { std::snprintf(b, sizeof b, "%02x", v); s.append(b, 2); }
    return s;
}

std::string Sha384::hash(const std::string& s) {
    Sha384 h; h.update(s); return h.hex();
}

} // namespace pd::crypto
