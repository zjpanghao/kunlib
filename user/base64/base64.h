#ifndef INCLUDE_BASE64_H
#define INCLUDE_BASE64_H
#include <string>
#include <vector>
void base64Encry(char *in, int len, std::string *out);
void base64Decry(const std::string input, bool with_new_line, std::vector<unsigned char> *output); 

#endif
