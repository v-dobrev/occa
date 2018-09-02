/* The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 David Medina and Tim Warburton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 */

#include <occa/mode/serial/memory.hpp>
#include <occa/tools/sys.hpp>
#include <occa/device.hpp>

namespace occa {
  namespace serial {
    memory::memory(modeDevice_t *modeDevice_,
                   udim_t size_,
                   const occa::properties &properties_) :
      occa::modeMemory_t(modeDevice_, size_, properties_) {}

    memory::~memory() {}

    kernelArg memory::makeKernelArg() const {
      kernelArgData arg;

      arg.modeDevice = modeDevice;
      arg.modeMemory = const_cast<memory*>(this);

      arg.data.void_ = ptr;
      arg.size       = sizeof(void*);
      arg.info       = kArgInfo::usePointer;

      return kernelArg(arg);
    }

    modeMemory_t* memory::addOffset(const dim_t offset) {
      memory *m = new memory(modeDevice,
                             size - offset,
                             properties);
      m->ptr = ptr + offset;
      m->canBeFreed = false;
      return m;
    }

    void memory::copyTo(void *dest,
                        const udim_t bytes,
                        const udim_t offset,
                        const occa::properties &props) const {
      const void *srcPtr = ptr + offset;

      ::memcpy(dest, srcPtr, bytes);
    }

    void memory::copyFrom(const void *src,
                          const udim_t bytes,
                          const udim_t offset,
                          const occa::properties &props) {

      void *destPtr      = ptr + offset;
      const void *srcPtr = src;

      ::memcpy(destPtr, srcPtr, bytes);
    }

    void memory::copyFrom(const modeMemory_t *src,
                          const udim_t bytes,
                          const udim_t destOffset,
                          const udim_t srcOffset,
                          const occa::properties &props) {

      void *destPtr      = ptr + destOffset;
      const void *srcPtr = src->ptr + srcOffset;

      ::memcpy(destPtr, srcPtr, bytes);
    }

    void memory::free() {
      if (ptr) {
        sys::free(ptr);
        ptr = NULL;
        size = 0;
      }
    }

    void memory::detach() {
      ptr = NULL;
      size = 0;
    }
  }
}
