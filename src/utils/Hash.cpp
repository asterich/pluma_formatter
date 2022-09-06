#include "Hash.h"

namespace pluma {

namespace utils {

#if HAS_OPENSSL

bool computeHash(const std::string &unhashed, std::string &hashed) {
    bool success = false;

    EVP_MD_CTX *context = EVP_MD_CTX_new();

    if (context != NULL) {
        if (EVP_DigestInit_ex(context, EVP_sha256(), NULL)) {
            if (EVP_DigestUpdate(context, unhashed.c_str(), unhashed.length())) {
                unsigned char hash[EVP_MAX_MD_SIZE];
                unsigned int lengthOfHash = 0;

                if (EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
                    std::stringstream ss;
                    for (unsigned int i = 0; i < lengthOfHash; ++i) {
                        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
                    }

                    hashed = ss.str();
                    success = true;
                }
            }
        }

        EVP_MD_CTX_free(context);
    }

    return success;
}

std::string readEntireFile(const std::string &filename) {
    constexpr auto readSize = std::size_t(4096);
    auto stream = std::ifstream(filename);
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string();
    auto buf = std::string(readSize, '\0');
    while (stream.read(&buf[0], readSize)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

std::string fileHash(const std::string &filename) {
    std::string fileStr = readEntireFile(filename);
    std::string hashed;
    if (computeHash(fileStr, hashed)) {
        return hashed;
    } else {
        logger << "Failed to hash a file.\n";
        return "";
    }
}

bool saveHashToFile(const std::string &filename, const std::string &hash) {
    auto file = std::ofstream(filename, std::ios::out);
    if (!file.is_open()) {
        logger << "Failed to open the file.\n";
        return false;
    }

    file << hash;

    file.close();

    return true;
}

bool loadHashFromFile(const std::string &filename, std::string &hash) {
    auto file = std::ifstream(filename);
    if (!file.is_open()) {
        logger << "Failed to open the file.\n";
        return false;
    }

    file >> hash;

    file.close();
    return true;
}

#else
// Do nothing.

std::string fileHash(const std::string &filename) { return ""; }

bool saveHashToFile(const std::string &filename, const std::string &hash) { return false; }

bool loadHashFromFile(const std::string &filename, std::string &hash) { return false; }

#endif

}  // namespace utils

}  // namespace pluma
