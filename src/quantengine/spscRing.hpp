#pragma once

#include <cstdint>
#include <atomic>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <cstddef>

namespace daniel{

   struct Tick{
      uint64_t ts_ns;
      float price;
      float size;
      uint32_t symbol_id;
   };

   template <typename T>
   class SpscRing {
      public:
         explicit SpscRing(std::size_t _size)
         : buffer(_size),
         mask_(_size - 1)
         {
         if (_size == 0 || (_size & mask_) != 0) {
               throw std::invalid_argument("capacity must be a power of two (>0)");
            }
         }

         SpscRing(const SpscRing& other) = delete;
         SpscRing& operator = (const SpscRing&) = delete; // atommics cannot be moved or copied hence throw error
         SpscRing( SpscRing&& other) = delete;
         SpscRing& operator = ( SpscRing&&) = delete; 
         
         bool pop(T& val){
            uint64_t lcl_tail = tail_.load(std::memory_order_relaxed);
            uint64_t lcl_head = head_.load(std::memory_order_acquire);
            if(lcl_tail!= lcl_head){
               val =  buffer[lcl_tail & mask_];
               tail_.store(lcl_tail + 1, std::memory_order_release);
               return true;
            }
            else{
               return false;
            }
         }

         bool push(const T& val){
            uint64_t lcl_head = head_.load(std::memory_order_relaxed);
            uint64_t lcl_tail = tail_.load(std::memory_order_acquire);

            if((lcl_head - lcl_tail) <= mask_){
               buffer[lcl_head & mask_] = val;
               head_.store(lcl_head + 1, std::memory_order_release);
               return true;
            }
            else{
               return false;
            }
            
         }

         bool empty() const {
            return tail_.load(std::memory_order_acquire) ==
            head_.load(std::memory_order_acquire);
         }

         bool full() const {
            auto h = head_.load(std::memory_order_acquire);
            auto t = tail_.load(std::memory_order_acquire);
            return (h - t) > mask_;
         }

         std::size_t capacity() const { return mask_ + 1; }

         std::size_t size() const {
            auto h = head_.load(std::memory_order_acquire);
            auto t = tail_.load(std::memory_order_acquire);
            return static_cast<std::size_t>(h - t);
         }  

      private:
         std::atomic<uint64_t> head_{0};
         std::atomic<uint64_t> tail_{0};
         std::vector<T> buffer;
         std::size_t mask_;
   };
  
}



