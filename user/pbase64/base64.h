#include <string.h>
#include <vector>
#include <string>
class Base64 {
 public:
  Base64(); 
  std::string encode(const std::vector<unsigned char> &data); 
  void decode(std::string &inData, std::vector<unsigned char> &data);
  static Base64 &getBase64();

 private:
  unsigned char vtable[128];
  char table[64];
};
