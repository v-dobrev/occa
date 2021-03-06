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
#include <occa/tools/testing.hpp>

#include <occa.hpp>

void testDim();
void testBitfield();

int main(const int argc, const char **argv) {
  testDim();
  testBitfield();

  return 0;
}

void testDim() {
#define TEST_DIM(dims_, x_, y_, z_)             \
  ASSERT_EQ(dim.dims, dims_);                   \
  ASSERT_EQ(dim.x, (occa::udim_t) x_);          \
  ASSERT_EQ(dim.y, (occa::udim_t) y_);          \
  ASSERT_EQ(dim.z, (occa::udim_t) z_)

  // Constructors
  occa::dim dim;
  TEST_DIM(0, 1, 1, 1);

  dim = occa::dim(10);
  TEST_DIM(1, 10, 1, 1);

  dim = occa::dim(10, 20);
  TEST_DIM(2, 10, 20, 1);

  dim = occa::dim(10, 20, 30);
  TEST_DIM(3, 10, 20, 30);

  dim = occa::dim(1, 10, 20, 30);
  TEST_DIM(1, 10, 20, 30);

  dim = occa::dim(1, 10, 20, 30);
  TEST_DIM(1, 10, 20, 30);
  ASSERT_NEQ(dim,
             occa::dim(10, 20, 30));
  ASSERT_EQ(dim,
            occa::dim(1, 10, 20, 30));

  dim = dim - occa::dim(10, 10, 10);
  TEST_DIM(3, 0, 10, 20);

  dim = dim + occa::dim(10, 10, 10);
  TEST_DIM(3, 10, 20, 30);

  dim = dim * occa::dim(10, 10, 10);
  TEST_DIM(3, 100, 200, 300);

  dim = dim / occa::dim(10, 10, 10);
  TEST_DIM(3, 10, 20, 30);
  ASSERT_EQ(dim,
            occa::dim(10, 20, 30))

  // [] indexing
  ASSERT_EQ(dim[0],
            (occa::udim_t) 10);
  ASSERT_EQ(dim[1],
            (occa::udim_t) 20);
  ASSERT_EQ(dim[2],
            (occa::udim_t) 30);

  const occa::dim constDim(10, 20, 30);
  ASSERT_EQ(constDim[0],
            (occa::udim_t) 10);
  ASSERT_EQ(constDim[1],
            (occa::udim_t) 20);
  ASSERT_EQ(constDim[2],
            (occa::udim_t) 30);

  std::cout << constDim << '\n';

#undef TEST_DIM
}

void testBitfield() {
  occa::bitfield bf1(1 << 0);
  ASSERT_EQ_BINARY(bf1.b1, 0UL);
  ASSERT_EQ_BINARY(bf1.b2, 1UL);

  bf1 <<= (occa::bitfield::bits() / 2);
  ASSERT_EQ_BINARY(bf1.b1, 1UL);
  ASSERT_EQ_BINARY(bf1.b2, 0UL);

  bf1 >>= (occa::bitfield::bits() / 2);
  ASSERT_EQ_BINARY(bf1.b1, 0UL);
  ASSERT_EQ_BINARY(bf1.b2, 1UL);

  occa::bitfield bf2 = (
    occa::bitfield(1 << 0) |
    occa::bitfield(1 << 1)
  );

  ASSERT_TRUE(bf1 & bf2);
  bf2 <<= 1;
  ASSERT_FALSE(bf1 & bf2);

  const occa::bitfield a1(1L << 0, 1L << 0);
  const occa::bitfield a2(1L << 1, 1L << 1);
  const occa::bitfield b1(1L << 2, 1L << 2);
  const occa::bitfield b2(1L << 3, 1L << 3);
  const occa::bitfield c1(1L << 4, 1L << 4);
  const occa::bitfield c2(1L << 5, 1L << 5);

  const occa::bitfield a = (a1 | a2);
  const occa::bitfield b = (b1 | b2);
  const occa::bitfield c = (c1 | c2);

  const occa::bitfield start = (a1 | b1 | c1);
  const occa::bitfield end   = (a2 | b2 | c2);

  ASSERT_TRUE(a & a1);
  ASSERT_TRUE(a & a2);

  ASSERT_TRUE(start & a);
  ASSERT_TRUE(start & a1);
  ASSERT_TRUE(start & b1);
  ASSERT_TRUE(start & c1);

  ASSERT_TRUE(end & a);
  ASSERT_TRUE(end & a2);
  ASSERT_TRUE(end & b2);
  ASSERT_TRUE(end & c2);

  ASSERT_FALSE(a & b);
  ASSERT_FALSE(a & c);
  ASSERT_FALSE(b & c);

  ASSERT_FALSE(start & end);

  ASSERT_TRUE(a1 != a2);
  ASSERT_TRUE(a1 <  a2);
  ASSERT_TRUE(a2 <= a2);
  ASSERT_TRUE(a2 == a2);
  ASSERT_TRUE(a2 >= a2);
  ASSERT_TRUE(a2 >  a1);

  // Test bad shifts
  occa::bitfield e(1L << 0);
  occa::bitfield e1 = (e << 0);
  occa::bitfield e2 = (e << -1);

  ASSERT_EQ(e, e1);
  ASSERT_EQ(e, e2);
  ASSERT_EQ(e1, e2);

  e1 = (e >> 0);
  e2 = (e >> -1);

  ASSERT_EQ(e, e1);
  ASSERT_EQ(e, e2);
  ASSERT_EQ(e1, e2);

  // Test too much shift
  e1 = e << 1000;
  e2 = e >> 1000;
  ASSERT_FALSE(e1);
  ASSERT_FALSE(e2);
}
