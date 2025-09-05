// engine.hpp
#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include "spscRing.hpp"     // Tick, SpscRing
#include "rollingWindow.hpp"   // RollingWindow<Tick>
#include "stats.hpp"            // compute_stats


namespace daniel {

  class RollingEngine{
   
    public:
      explicit RollingEngine(std::size_t ringBufferSize,
      std::size_t windowSize, std::size_t recomputation_num = 64) :
      ring_(ringBufferSize), window_(windowSize),  recompute_every_(recomputation_num)
      {

      }
      ~RollingEngine(){ stop(); }
      void start(){
         if (running_.exchange(true)) return;
            cons_ = std::thread([this] { this->consumer_loop(); });
      }

      void stop(){
         if (!running_.exchange(false)) return;
         if(cons_.joinable()) cons_.join();
      }

      bool get_stats(Stats& out) const {
         return read_snapshots(snap_, out);
      }

      bool ingest_tick(const Tick& t) {
         return ring_.push(t); // producer-side call
      }

    private:


      void consumer_loop(){
         StatsScratch scratch;
         Stats current;
         std::size_t since = 0;
         Tick t;
         while (running_) {
            bool any = false;
            while (ring_.pop(t)) {           
               any = true;
               window_.push(t);
               if (++since >= recompute_every_) {
                  compute_stats(window_, current, scratch);
                  publish_snapshot(snap_, current);
                  since = 0;
               }
               }
               if (!any) {
               std::this_thread::sleep_for(std::chrono::microseconds(50));
               }
            }
         }


      SpscRing<Tick> ring_;
      RollingWindow<Tick> window_;
      const std::size_t  recompute_every_;
      std::thread cons_;
      Snapshot snap_;
      std::atomic<bool> running_{false};
  };
}
