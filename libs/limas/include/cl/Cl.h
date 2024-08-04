#pragma once
#include "system/Logger.h"

namespace compute = boost::compute;

namespace limas {
namespace cl {

using compute::dim;
using double2 = compute::double2_;
using double4 = compute::double4_;
using double8 = compute::double8_;
using float2 = compute::float2_;
using float4 = compute::float4_;
using float8 = compute::float8_;
using int2 = compute::int2_;
using int4 = compute::int4_;
using int8 = compute::int8_;
using uint = compute::uint_;

static std::vector<compute::device> getAllDevices() {
  std::vector<compute::device> result;

  std::vector<compute::platform> platforms = compute::system::platforms();
  for (size_t i = 0; i < platforms.size(); i++) {
    const compute::platform &platform = platforms[i];
    std::vector<compute::device> devices = platform.devices();
    result.insert(result.end(), devices.begin(), devices.end());
  }

  return result;
}

static compute::program loadAndBuild(const compute::context &context,
                                     const std::string &path,
                                     const std::vector<string> &options = {}) {
  std::ifstream ifs(path);
  std::string source((std::istreambuf_iterator<char>(ifs)),
                     (std::istreambuf_iterator<char>()));

  std::stringstream ss;
  for (auto &o : options) ss << " " << o;

  compute::program program =
      compute::program::create_with_source(source, context);
  try {
    program.build(ss.str());
  } catch (compute::opencl_error &) {
    log::error("CL") << program.build_log() << log::end();
  }

  return program;
}

static void printDeviceInfo(const compute::device &device) {
  std::stringstream ss;

  ss << "device: " << device.name() << "\n";

  ss << "  type: ";
  if (device.type() & compute::device::gpu)
    ss << "GPU Device"
       << "\n";
  else if (device.type() & compute::device::cpu)
    ss << "CPU Device"
       << "\n";
  else if (device.type() & compute::device::accelerator)
    ss << "Accelerator Device"
       << "\n";
  else
    ss << "Unknown Device"
       << "\n";

  ss << "  global memory size: "
     << device.get_info<cl_ulong>(CL_DEVICE_GLOBAL_MEM_SIZE) / 1024 / 1024
     << " MB"
     << "\n";
  ss << "  local memory size: "
     << device.get_info<cl_ulong>(CL_DEVICE_LOCAL_MEM_SIZE) / 1024 << " KB"
     << "\n";
  ss << "  constant memory size: "
     << device.get_info<cl_ulong>(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE) / 1024
     << " KB"
     << "\n";

  log::info("CL") << ss.str() << log::end();
}

template <typename T>
static void printDeviceVector(const compute::command_queue &queue,
                              const compute::vector<T> &vector) {
  std::cout << "vector: [ ";
  boost::compute::copy(vector.begin(), vector.end(),
                       std::ostream_iterator<T>(std::cout, ", "), queue);
  std::cout << "]" << std::endl;
}

}  // namespace cl
}  // namespace limas
