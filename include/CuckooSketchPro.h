#ifndef _CUCKOO_SKETCH_PRO_H
#define _CUCKOO_SKETCH_PRO_H

#include <string>
#include <stdint.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <limits.h>
#include <iostream>

#include "LossyStrategy.h"
#include "BaseSketch.h"
#include "BOBHASH64.h"


namespace sketch {
namespace cuckoosketchpro {

const int MAX_ROW = 2;
const int MAX_ENTRY = 4;
const int MAX_KICK_OUT = 1;
class Entry {
public:
    Entry(): ID(""), fingerprint(0), count(0) {}
    Entry(const std::string &id, uint8_t fp, uint32_t cnt): ID(id), fingerprint(fp), count(cnt) {}
    uint32_t Empty();
    bool Equalfp(uint8_t fp);
    bool Equall0fp(uint32_t fp);
    void Insert();
    void Insert(uint32_t fp, const std::string &id, uint32_t cnt = 1);
    void Lossy(std::function<void(uint32_t &)> &lossy_func);

    uint8_t get_fingerprint();
    uint32_t get_l0fp();
    uint32_t get_count();
    std::string get_ID();

    bool operator<(const Entry &e) const {
        return count > e.count;
    }
private:
    std::string ID;
    uint8_t fingerprint;
    uint32_t l0fp;
    uint32_t count;
};

class Bucket {
public:
    Bucket() {
        entries.resize(MAX_ENTRY);
    }
    void Clear();
    bool Empty(int index);
    bool Full(int index);
    void Insert(int index);
    void Insert(int index, const Entry &entry);
    void Insert(int index, uint32_t fp, const std::string &id);
    void Remove(int index);
    void Lossy(int index, std::function<void(uint32_t &)> &lossy_func);
    void BucketSort(int index);
    bool Equal(int index, uint32_t fp);

    uint8_t get_fp(int index);
    Entry get_entry(int index);
    int get_col_index();
    int get_entry_count(int index);
    void down_stairs(int index);
private:
    std::vector<Entry> entries;
    int col_index;
};

class CuckooSketchPro: public BaseSketch {
public:
    CuckooSketchPro(int threshold1, int K, int MEM);
    ~CuckooSketchPro();
    void clear() override;
    void Insert(const std::string &str) override;
    std::pair<std::string, int> Query(int k) override;
    void work() override;
    std::string get_name() override;

private:
    uint64_t Hash(const std::string &str);
    uint64_t Hash(uint8_t fp);
    int _bucket_num;
    int _threshold1;
    BOBHash64 *_bobhash;
    std::vector<std::vector<Bucket> > _buckets;
    Lossy::BaseStrategy *_lossy;
    std::function<void(uint32_t &)> _lossy_func;
    std::vector<Entry> _ret;
    int _K;
};
} // namespace cuckoosketchpro
} // namespace sketch

#endif