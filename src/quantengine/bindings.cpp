#include <pybind11/pybind11.h>
#include "engine.hpp"
#include "stats.hpp"

namespace py = pybind11;
using namespace daniel;

PYBIND11_MODULE(_quantengine, m) {
  m.doc() = "High-frequency rolling stats engine (simulator-only MVP)";

  py::class_<Tick>(m, "Tick")
    .def(py::init<>())
    .def_readwrite("ts_ns", &Tick::ts_ns)
    .def_readwrite("price", &Tick::price)
    .def_readwrite("size", &Tick::size)
    .def_readwrite("symbol_id", &Tick::symbol_id);

  py::class_<Stats>(m, "Stats")
    .def_readonly("count", &Stats::count)
    .def_readonly("mean",  &Stats::mean)
    .def_readonly("std",   &Stats::std)
    .def_readonly("minp",  &Stats::minp)
    .def_readonly("maxp",  &Stats::maxp)
    .def_readonly("vwap",  &Stats::vwap);

  py::class_<RollingEngine>(m, "Engine")
    .def(py::init<std::size_t,std::size_t,std::size_t>(),
         py::arg("ringBufferSize"), py::arg("windowSize"), py::arg("recomputation_num") = 64)
    .def("start",
         [](RollingEngine& self){ py::gil_scoped_release r; self.start(); })
    .def("stop",
         [](RollingEngine& self){ py::gil_scoped_release r; self.stop(); })
    .def("ingest_tick", &RollingEngine::ingest_tick)
    .def("snapshot",
         [](const RollingEngine& self){
           Stats s{}; (void)self.get_stats(s); return s;
         })
    .def("count", [](const RollingEngine& self){ Stats s{}; self.get_stats(s); return s.count; })
    .def("mean",  [](const RollingEngine& self){ Stats s{}; self.get_stats(s); return s.mean;  })
    .def("std",   [](const RollingEngine& self){ Stats s{}; self.get_stats(s); return s.std;   })
    .def("vwap",  [](const RollingEngine& self){ Stats s{}; self.get_stats(s); return s.vwap;  })
    .def("min",   [](const RollingEngine& self){ Stats s{}; self.get_stats(s); return s.minp;  })
    .def("max",   [](const RollingEngine& self){ Stats s{}; self.get_stats(s); return s.maxp;  });
}