#ifndef HASH_H_
#define HASH_H_

#ifdef HAS_OPENSSL
#include <openssl/evp.h>
#endif

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "../Logger.h"

namespace pluma {

namespace utils {

std::string fileHash(const std::string &filename);

bool saveHashToFile(const std::string &filename, const std::string &hash);

bool loadHashFromFile(const std::string &filename, std::string &hash);

}  // namespace utils

}  // namespace pluma
#endif