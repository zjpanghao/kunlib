#include "hds.h"
#include <stdlib.h>
namespace hdb {

Hds hdsnewlen(int n) {
  void *p = malloc(n + sizeof(struct HdsBuf));
  struct HdsBuf *hdf = (struct HdsBuf*)p;
  hdf->len = n;
  hdf->alloc = n;
  return (Hds)(p + sizeof(struct HdsBuf));
}

void hdsSetLen(Hds s, int len) {
  struct HdsBuf *hdf = (struct HdsBuf*)(s -
      sizeof(struct HdsBuf));
  hdf->len = len;
}

int hdsLen(Hds s) {
  struct HdsBuf *hdf = (struct HdsBuf*)(s -
      sizeof(struct HdsBuf));
  return hdf->len;
}

void hdsFree(Hds s) {
  char *p = s - sizeof(struct HdsBuf);
  free(p);
}

Hds makeRoom(Hds s, int n) {
  HdsBuf *hdf = 
    (HdsBuf*) (s - sizeof(struct HdsBuf));
  if (hdf->alloc - hdf->len >= n) {
    return s;
  }
  int nlen = n < 1024*1024 ? 
    2*(n + sizeof(struct HdsBuf) + hdf->len)
    : (2*n + sizeof(struct HdsBuf) + hdf->len);
  hdf->alloc = nlen - sizeof(struct HdsBuf);
  void *p = realloc((char*)hdf, nlen);
  return (Hds)p + sizeof(struct HdsBuf);
}

} // namespace hdb
