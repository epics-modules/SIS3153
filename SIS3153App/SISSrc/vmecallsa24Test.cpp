// Check the high level VME calls.. those that I have
// the hardware to check in any event.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <stdio.h>
#include <sis3150usb_vme_calls.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

static const ULONG RAM=0x500000;


class vmea24Calls : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(vmea24Calls);
  CPPUNIT_TEST(a24d8rw);
  CPPUNIT_TEST(a24d16rw);
  CPPUNIT_TEST(a24d32rw);
  CPPUNIT_TEST(a24d32dmarw);
  CPPUNIT_TEST(a24d32fifow);
  CPPUNIT_TEST(a24d32fifor);
  CPPUNIT_TEST_SUITE_END();


private:
  HANDLE m_device;

public:
  void setUp() {
    struct SIS3150USB_Device_Struct info[10];
    unsigned int found;
    int status;
    
    status = FindAll_SIS3150USB_Devices(info, &found, 1);
    ASSERT(found > 0);
    status = Sis3150usb_OpenDriver((PCHAR)info[0].cDName, &m_device);
    ASSERT(status == 0);
  }
  void tearDown() {
    Sis3150usb_CloseDriver(m_device);
  }
protected:
  void a24d8rw();
  void a24d16rw();
  void a24d32rw();
  void a24d32dmarw();
  void a24d32fifow();
  void a24d32fifor();
};

CPPUNIT_TEST_SUITE_REGISTRATION(vmea24Calls);


// Do d8 r/w to the RAM: counting pattern.

void vmea24Calls::a24d8rw()
BEGIN
  u_int8_t pattern[256];
  for(int i =0; i < 256; i++) {
    pattern[i] = i;
    vme_A24D8_write(m_device, RAM+i, i);
  }
  for (int i=0; i < 256; i++) {
    u_int8_t vme_data;
    vme_A24D8_read(m_device, RAM+i, &vme_data);
    EQMSG("comparing pattern", pattern[i], vme_data);
  }
}
// Do d16 r/w to the ram with a rotating bit.
void vmea24Calls::a24d16rw()
BEGIN
  u_int16_t pattern[16];
  u_int16_t bit = 1;
  for (int i = 0; i < 16; i++) {
    pattern [i] = bit;
    bit = bit << 1;
    vme_A24D16_write(m_device, RAM+i*sizeof(u_int16_t), pattern[i]);
  }
  for (int i=0; i < 16; i ++) {
    u_int16_t vme_data;
    vme_A24D16_read(m_device, RAM+i*sizeof(u_int16_t), &vme_data);
    EQ(pattern[i], vme_data);
  }
}

// Do d32 r/w to ram with alternating bits patterns.

void vmea24Calls::a24d32rw()
BEGIN
  u_int32_t pattern[256];
  for (int i =0; i < 256; i++) {
    pattern[i] = i % 2 ? 0x55555555 : 0xaaaaaaaa;
    vme_A24D32_write(m_device, RAM+i*sizeof(u_int32_t), pattern[i]);
  }
  for (int i =0; i < 256; i++) {
    u_int32_t data;
    vme_A24D32_read(m_device, RAM+i*sizeof(u_int32_t), &data);
    EQ(pattern[i], data);
  }
}
// DO d32 dma write of random dat.

void vmea24Calls::a24d32dmarw()
BEGIN
  u_int32_t pattern[1024*1024/sizeof(u_int32_t)]; // 1Mbyte should do it.
  u_int32_t readData[1024*1024/sizeof(u_int32_t)];
  u_int32_t transferCount;
  
  for (int i =0; i < sizeof(pattern)/sizeof(u_int32_t); i++) {
    pattern[i] =  random();
  }
  vme_A24DMA_D32_write(m_device, RAM, pattern, sizeof(pattern)/sizeof(u_int32_t),
		       &transferCount);
  EQMSG("dma write transfercount", sizeof(pattern)/sizeof(u_int32_t), transferCount);

  vme_A24DMA_D32_read(m_device, RAM, readData, sizeof(pattern)/sizeof(u_int32_t),
		      &transferCount);
  EQMSG("read size", sizeof(pattern)/sizeof(u_int32_t), transferCount);

  for(int i=0; i < 1024*1024/sizeof(u_int32_t); i++) {
    EQMSG("data compare", pattern[i], readData[i]);
  }
		      
		       
}
// If I write to memory in fifo mode, reading the address of the 
// write should give the last item in the pattern.
//
void vmea24Calls::a24d32fifow()
BEGIN
  u_int32_t pattern[1024];
  u_int32_t transferCount;
  u_int32_t data;
  for (int i =0; i < 1024; i++) {
    pattern[i] = i;
  }
  vme_A24DMA_D32FIFO_write(m_device, RAM, pattern,
			   1024, &transferCount);
  EQMSG("fifo write count", (u_int32_t)1024, transferCount);

  vme_A24D32_read(m_device, RAM, &data);
  EQMSG("fifo read compare", pattern[1023], data);
}
// If I write a pattern to memory, and then 
// fifo read at the same base address, I should just
// Get endless repetitions of the first pattern element.
//
void vmea24Calls::a24d32fifor()
BEGIN
  u_int32_t pattern[1024];
  u_int32_t readData[1024];
  u_int32_t transferCount;

  for (int i=0; i < 1024; i ++) {
    pattern[i] = 1024-i;
  }
  vme_A24DMA_D32_write(m_device, RAM, pattern, 1024,
		       &transferCount);

  vme_A24DMA_D32FIFO_read(m_device, RAM, readData,
			  1024, &transferCount);
  EQMSG("fifo read xfer count", (u_int32_t)1024, transferCount);
  for (int i =0; i < 1024; i++) {
    EQMSG("data compare", pattern[0], readData[i]);
  }
 
}
