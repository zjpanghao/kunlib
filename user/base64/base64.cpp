#include "base64.h"
#include <openssl/pem.h>                                                        
#include <openssl/bio.h>                                                        
#include <openssl/evp.h> 

void base64Encry(char *in, int len, std::string *out) {
  BIO *bmem = BIO_new(BIO_s_mem());  
  BIO *b64 = BIO_new(BIO_f_base64());
  b64 = BIO_push(b64, bmem); 
  BIO_write(b64, in, len);
  BIO_flush(b64);
  BUF_MEM *bptr = NULL;
  BIO_get_mem_ptr(b64, &bptr); 
  out->assign(bptr->data, bptr->length);
  BIO_free_all(b64);
}

void base64Decry(const std::string input, bool with_new_line, std::vector<unsigned char> *output) {
  BIO * b64 = NULL;
  BIO * bmem = NULL;
  std::vector<unsigned char> out(input.length(), 0);
  b64 = BIO_new(BIO_f_base64());
  if(!with_new_line) {
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
  }
  bmem = BIO_new_mem_buf(input.c_str(), input.length());
  bmem = BIO_push(b64, bmem);
  int len;
  len = BIO_read(bmem, &out[0], input.length());
  if (len > 0) {
    out.resize(len);
    output->swap(out);
  }
  BIO_free_all(bmem);
}
