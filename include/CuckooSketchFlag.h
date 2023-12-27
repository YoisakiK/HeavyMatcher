#ifndef CUCKOOSKETCH_FLAG_H
#define CUCKOOSKETCH_FLAG_H
#include "BOBHASH64.h"
#include "BaseSketch.h"
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <unistd.h>
namespace sketch {
namespace cuckoo_flag {

#define DEGENERATE
const int MAX_ROW = 2;
const int MAX_ENTRY_SIZE = 8;
const int MAX_KICK_NUM = 1;
const int BUCKET_SIZE = MAX_ENTRY_SIZE * 32 + 8;
static int TOP_K_THRESHOLD = 50;

using BigFlow = struct {
  uint32_t fp;
  uint32_t count;
};

using SmallFlow = struct {
  uint16_t fp1;
  uint16_t count1;
  uint16_t fp2;
  uint16_t count2;
};

using EntryData = union {
  uint64_t all;
  BigFlow bf;
  SmallFlow sf;
};

// sf for small flow, bf for big flow
class Entry {
public:
  Entry();
  bool bf_empty();
  bool bf_equal(uint32_t fp);
  bool sf_empty(bool part);
  bool sf_equal(bool part, uint16_t fp);
  void sf_insert(bool part, uint16_t fp, uint16_t cnt);
  bool sf_plus(bool part);
  void bf_insert(uint32_t fp, uint32_t cnt, const std::string &ID);
  void bf_plus();
  std::pair<std::string, int> get_bf_flow();
  std::pair<uint16_t, uint16_t> get_sf_entry();
// private:
  EntryData data;
  std::string _ID;
};

class Bucket {
public:
  Bucket();
  bool bf_flag(int index);
  void update_flags(int index, bool fun);
  void sort(int index);
  uint8_t get_bf_cnt();
  void down_stairs();
  void up_stairs();
  std::vector<Entry> entries;
  void lossy(int index);
private:
  // swap with up one
  void swap_sf_entry(int index);
  void swap_sf_entry(int index1, int index2);
  uint8_t flags;
};
class CuckooSketchFlag : public BaseSketch {
public:
  CuckooSketchFlag(int threshold, int MEM);
  ~CuckooSketchFlag();
  void clear() override;
  void Insert(const std::string &str) override;
  std::pair<std::string, int> Query(int k) override;
  void work() override;
  std::string get_name() override;

private:
  uint64_t Hash(const std::string &str);
  uint64_t Hash(const uint16_t &fp);
  // void Expand(uint32_t fp, uint32_t count, const std::string &ID, int index0, int index1, bool cur_index);
  void Expand(uint32_t fp, uint32_t count, const std::string &ID, int cur_index, int opposite_index, int cur_row);
  // bool kickout();
  BOBHash64 *_bobhash;
  std::vector<std::vector<Bucket>> _buckets;
  int _bucket_num;
  std::vector<std::pair<std::string, int>> _ret;
};

}  // namespace cuckoo_flag
}  // namespace sketch
#endif
