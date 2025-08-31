#pragma once
#include <cmath>
#include <limits>
#include "rollingWindow.hpp"
#include "spscRing.hpp"

namespace daniel{

struct StatsScratch{
  double sum_p=0, sum_p2=0, sum_sz=0, sum_p_sz=0;
  double min_p= std::numeric_limits<double>::infinity();
  double max_p=-std::numeric_limits<double>::infinity();
  std::size_t n=0;
  void clear(){ sum_p=sum_p2=sum_sz=sum_p_sz=0; 
                min_p= std::numeric_limits<double>::infinity();
                max_p=-std::numeric_limits<double>::infinity(); n=0; }

};
struct Stats{
  std::size_t count {0};
  double std{0};
  double mean{0};
  double maxp{0};
  double minp{0};
  double vwap{0};
};

struct Snapshot{
  std::atomic<std::uint64_t> version{0};
  Stats data{};
};

inline void compute_stats(const RollingWindow<Tick>& win, Stats& out, StatsScratch& sc) {
   sc.clear();
   win.for_each([&](const Tick& t){
    const double p = t.price;
    const double z = t.size;
    sc.sum_p += p; sc.sum_p2 += p*p; sc.sum_sz += z; sc.sum_p_sz += p*z;
    if (p < sc.min_p) sc.min_p = p;
    if (p > sc.max_p) sc.max_p = p;
    ++sc.n;
  });
  out.count = sc.n;
  if (sc.n == 0) { out = {}; return; }
  const double n = static_cast<double>(sc.n);
  const double mean = sc.sum_p / n;
  const double var  = std::max(0.0, sc.sum_p2 / n - mean*mean);
  out.mean = mean; out.std = std::sqrt(var);
  out.minp = sc.min_p; out.maxp = sc.max_p;
  out.vwap = (sc.sum_sz > 0.0) ? (sc.sum_p_sz / sc.sum_sz) : 0.0;
}

inline void publish_snapshot(Snapshot& snap, const Stats& s){
   snap.version.fetch_add(1, std::memory_order_relaxed);
   snap.data = s;
   snap.version.fetch_add(1, std::memory_order_release);
}

inline bool read_snapshots(const Snapshot& snap, Stats& out){
   for(int i = 0; i < 4; ++i){
      auto value1 = snap.version.load(std::memory_order_acquire);
      if(value1 & 1u) 
         continue;
      Stats tmp = snap.data;
      auto value2 = snap.version.load(std::memory_order_acquire);
      if(value1 == value2)
      {
         out = tmp;
         return true;
      }

   }
   return false;
}

}

