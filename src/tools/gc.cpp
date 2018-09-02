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

#include <occa/tools/gc.hpp>
#include <iostream>

namespace occa {
  namespace gc {
    withRefs::withRefs() :
      refs(0) {}

    int withRefs::getRefs() const {
      return refs;
    }

    void withRefs::addRef() {
      if (refs >= 0) {
        ++refs;
      }
    }

    int withRefs::removeRef() {
      if (refs > 0) {
        return --refs;
      }
      return refs;
    }

    void withRefs::setRefs(const int refs_) {
      refs = refs_;
    }

    void withRefs::dontUseRefs() {
      refs = -1;
    }

    ringEntry_t::ringEntry_t() :
      leftRingEntry(this),
      rightRingEntry(this) {}

    void ringEntry_t::removeRef() {
      if (leftRingEntry != this) {
        leftRingEntry->rightRingEntry = rightRingEntry;
        rightRingEntry->leftRingEntry = leftRingEntry;
      }
      leftRingEntry = rightRingEntry = this;
    }

    bool ringEntry_t::isAlone() const {
      return (leftRingEntry == this);
    }
  }
}
