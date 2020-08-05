#include "hds.h"
#include <stdlib.h>
namespace hdb {

Hds hdsnewlen(int n) {
  void *p = malloc(n + sizeof(struct HdsBuf));
  struct HdsBuf *hdf = (struct HdsBuf*)p;
  hdf->len = n;
  hdf->alloc = n;
  return (Hds)p + sizeof(struct HdsBuf);
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
  int nlen = hdf->len + n;
  if (nlen < 1024*1024 ) {
    nlen *= 2;
  } else {
    nlen += 1024*1024;
  }
  hdf->alloc = nlen;
  void *p = realloc((char*)hdf, 
      sizeof(struct HdsBuf) + nlen);
  return (Hds)p + sizeof(struct HdsBuf);
}

} // namespace hdb
