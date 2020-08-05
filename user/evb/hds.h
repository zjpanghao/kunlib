#ifndef INCLUDE_HDS_H
#define INCLUDE_HDS_H
namespace hdb {
typedef char*  Hds;
struct HdsBuf {
  unsigned int len;
  unsigned int alloc;
  unsigned char type;
  char buf[];
};

Hds hdsnewlen(int n);
Hds makeRoom(Hds s, int n);
int hdsLen(Hds s);
void hdsSetLen(Hds s, int len);
void hdsFree(Hds s);
}
#endif
