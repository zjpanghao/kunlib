#include <iostream>
#include "base64.h"
Base64::Base64() {
  char tmp[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
  };
  memcpy(table, tmp, 64);
  for (int i = 0; i < 64; i++) {
    vtable[table[i]] = i;
  }
};

Base64 &Base64::getBase64() {
  static Base64 base;
  return base;
}

std::string Base64::encode(const std::vector<unsigned char> &data) {
  std::string result;
  result.reserve((data.size() + 2)/3*4);
  for (int i = 0; i < data.size(); i++) {
    int d = i % 3;
    switch (d) {
      case 0:
       result.append(1, table[data[i]  >> 2]);
       if (i + 1 == data.size()) {
         result.append(1, table[(data[i] & 0x3) << 4]);
         result.append("==");
       }
       break;
      case 1:
       result.append(1, table[(data[i - 1] & 0x3) << 4 |  data[i] >> 4]);
       if (i + 1 == data.size()) {
         result.append(1, table[(data[i] & 0xf) << 2]);
         result.append("=");
       }
       break;
      case 2:
       result.append(1, table[(data[i - 1] & 0xf) << 2 | data[i] >> 6]);
       result.append(1, table[data[i] & 0x3f]);
       break;
    }
  }
  return result;
}

void Base64::decode(std::string &inData, std::vector<unsigned char> &data) {
  if (data.size() % 4 != 0) {
    return;
  }
  data.reserve((data.size() + 3 / 4) * 3);
  for (int i = 0; i < inData.size(); i+= 4) {
    data.push_back(vtable[inData[i]] << 2 | vtable[inData[i + 1] >> 4]); 
    if (data[i + 2] != '=') {
      data.push_back((vtable[inData[i + 1]] & 0xf) << 4 | vtable[inData[i + 2] >> 2]); 
    } else {
      break;
    }
    if (data[i + 3] != '=') {
      data.push_back((vtable[inData[i + 2]] & 0x3) << 6 | (vtable[inData[i + 3] & 0x3f])); 
    } else {
      break;
    }
  }
}

