// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <iostream>
#include <string>
#include "Asserts.h"
#include <stdio.h>
#include <sis3150usb_vme_calls.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

class notifier {
public:
  notifier(std::string msg) {
    std::cout << msg << std::endl;
  }
};

static const unsigned long RAM=0x008000000; // A32 ram location.

static notifier needRam("vmecallsa32Test requires a few K of VME a32 memory at 0x08000000 else tests will fail");

class vmea32Calls : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(vmea32Calls);
  // CPPUNIT_TEST(a32d8rw);   // My A32 memory board only supports D16/D32.
  CPPUNIT_TEST(a32d16rw);
  CPPUNIT_TEST(a32d32rw);
  CPPUNIT_TEST(a32d32dmarw);
  CPPUNIT_TEST(a32d32fifow);
  CPPUNIT_TEST(a32d32fifor);
  CPPUNIT_TEST(a32d32bltw);	// My memory board support blt, but probably not
  CPPUNIT_TEST(a32d32bltr);     // mblt.
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
  void a32d8rw();
  void a32d16rw();
  void a32d32rw();
  void a32d32dmarw();
  void a32d32fifow();
  void a32d32fifor();
  void a32d32bltw();
  void a32d32bltr();
};

CPPUNIT_TEST_SUITE_REGISTRATION(vmea32Calls);


// Do d8 r/w to the RAM: counting pattern.

void vmea32Calls::a32d8rw()
BEGIN
  u_int8_t pattern[256];
  for(int i =0; i < 256; i++) {
    pattern[i] = i;
    vme_A32D8_write(m_device, RAM+i, i);
  }
  for (int i=0; i < 256; i++) {
    char message[100];
    sprintf(message, "data compare at offset: %d", i);
    u_int8_t vme_data;
    vme_A32D8_read(m_device, RAM+i, &vme_data);
    EQMSG(message, (u_int32_t)pattern[i], (u_int32_t)vme_data);
  }
}
// Do d16 r/w to the ram with a rotating bit.
void vmea32Calls::a32d16rw()
BEGIN
  u_int16_t pattern[16];
  u_int16_t bit = 1;
  for (int i = 0; i < 16; i++) {
    pattern [i] = bit;
    bit = bit << 1;
    vme_A32D16_write(m_device, RAM+i*sizeof(u_int16_t), pattern[i]);
  }
  for (int i=0; i < 16; i ++) {
    u_int16_t vme_data;
    vme_A32D16_read(m_device, RAM+i*sizeof(u_int16_t), &vme_data);
    EQ(pattern[i], vme_data);
  }
}

// Do d32 r/w to ram with alternating bits patterns.

void vmea32Calls::a32d32rw()
BEGIN
  u_int32_t pattern[256];
  for (int i =0; i < 256; i++) {
    pattern[i] = i % 2 ? 0x55555555 : 0xaaaaaaaa;
    vme_A32D32_write(m_device, RAM+i*sizeof(u_int32_t), pattern[i]);
  }
  for (int i =0; i < 256; i++) {
    u_int32_t data;
    vme_A32D32_read(m_device, RAM+i*sizeof(u_int32_t), &data);
    EQ(pattern[i], data);
  }
}
// DO d32 dma write of random dat.

void vmea32Calls::a32d32dmarw()
BEGIN
  u_int32_t pattern[1024/sizeof(u_int32_t)]; // 1Mbyte should do it.
  u_int32_t readData[1024/sizeof(u_int32_t)];
  u_int32_t transferCount;
  
  for (int i =0; i < sizeof(pattern)/sizeof(u_int32_t); i++) {
    pattern[i] =  random();
  }
  vme_A32DMA_D32_write(m_device, RAM, pattern, sizeof(pattern)/sizeof(u_int32_t),
		       &transferCount);
  EQMSG("dma write transfercount", sizeof(pattern)/sizeof(u_int32_t), transferCount);

  vme_A32DMA_D32_read(m_device, RAM, readData, sizeof(pattern)/sizeof(u_int32_t),
		      &transferCount);
  EQMSG("read size", sizeof(pattern)/sizeof(u_int32_t), transferCount);

  for(int i=0; i < 1024/sizeof(u_int32_t); i++) {
    char message[100];
    sprintf(message, "data compare offset %d", i);
    EQMSG(message, pattern[i], readData[i]);
  }
		      
		       
}
// If I write to memory in fifo mode, reading the address of the 
// write should give the last item in the pattern.
//
void vmea32Calls::a32d32fifow()
BEGIN
  u_int32_t pattern[1024];
  u_int32_t transferCount;
  u_int32_t data;
  for (int i =0; i < 1024; i++) {
    pattern[i] = i;
  }
  vme_A32DMA_D32FIFO_write(m_device, RAM, pattern,
			   1024, &transferCount);
  EQMSG("fifo write count", (u_int32_t)1024, transferCount);

  vme_A32D32_read(m_device, RAM, &data);
  EQMSG("fifo read compare", pattern[1023], data);
}
// If I write a pattern to memory, and then 
// fifo read at the same base address, I should just
// Get endless repetitions of the first pattern element.
//
void vmea32Calls::a32d32fifor()
BEGIN
  u_int32_t pattern[1024];
  u_int32_t readData[1024];
  u_int32_t transferCount;

  for (int i=0; i < 1024; i ++) {
    pattern[i] = 1024-i;
  }
  vme_A32DMA_D32_write(m_device, RAM, pattern, 1024,
		       &transferCount);

  vme_A32DMA_D32FIFO_read(m_device, RAM, readData,
			  1024, &transferCount);
  EQMSG("fifo read xfer count", (u_int32_t)1024, transferCount);
  for (int i =0; i < 1024; i++) {
    EQMSG("data compare", pattern[0], readData[i]);
  }
 
}

// Write a pattern in blt mode, read back in 
// DMA mode and ensure the two are equal.
//
void vmea32Calls::a32d32bltw()
BEGIN
  u_int32_t pattern[4096];
  u_int32_t readData[4096];
  u_int32_t transferCount;

  for (int i=0; i < 4096; i++) {
    pattern[i] = random();	// random data.
  }
  vme_A32BLT32_write(m_device, RAM, pattern, 4096, &transferCount);
  EQMSG("blt write count", (u_int32_t)4096, transferCount);

  vme_A32DMA_D32_read(m_device, RAM, readData, 4096, &transferCount);
  for (int i=0; i < 4096; i++) {
    char message[100];
    sprintf(message, "Data compare longword %d", i);
    EQMSG(message, pattern[i], readData[i]);
  }
}
// Write a pattern in dma mode and read back in blt.
//
void vmea32Calls::a32d32bltr()
BEGIN
  u_int32_t pattern[4096];
  u_int32_t readData[4096];
  u_int32_t transferCount;

  for (int i =0; i < 4096; i++) {
    pattern[i] = random();
  }
  vme_A32DMA_D32_write(m_device, RAM, pattern, 4096, &transferCount);

  vme_A32BLT32_read(m_device, RAM, readData, 4096, &transferCount);
  EQMSG("blt32 read transfer count", (u_int32_t)4096, transferCount);

  for (int i =0; i < 4096; i++) {
    char message[100];
    sprintf(message, "data compare longword %d", i);
    EQMSG(message, pattern[i], readData[i]);
  }
}
