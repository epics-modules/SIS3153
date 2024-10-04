// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <iostream>
#include <string>

#include <sis3150usb_vme.h>

using namespace std;

class notifier {
public:
  notifier(std::string msg) {
    std::cout << msg << std::endl;
  }
};

static const ULONG AM=0x39;
static const ULONG RAM=0x500000;

static notifier needRam("vmeTest requires VME a24/d32 memory at 0x500000-0x5fffff else tests will fail");

class vmeTransfers : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(vmeTransfers);
  CPPUNIT_TEST(singlerwLong);	// Note I don't have a 64 bit slave device
  CPPUNIT_TEST(singlerwShort);  // but chances are if these 3 work
  CPPUNIT_TEST(singlerwByte);	// 8 byte I/O will too.
  CPPUNIT_TEST(dmarwLong);	// dma test for longs.
  CPPUNIT_TEST(dmarwShort);     // dma test for shorts. -- restricted sizes for now.
  // CPPUNIT_TEST(dmarwByte);      // dma test for bytes. // DMA 8 bit does not work (yet?).

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
  void singlerwLong();
  void singlerwShort();
  void singlerwByte();

  void dmarwLong();
  void dmarwShort();
  void dmarwByte();
};

CPPUNIT_TEST_SUITE_REGISTRATION(vmeTransfers);


// First write/read 0x55555555 
// The write/read   0xaaaaaaaa
//
void vmeTransfers::singlerwLong()
BEGIN
  ULONG data = 0;
  int status = sis3150Usb_Vme_Single_Write(m_device,
				       RAM,
				       AM,
				       sizeof(ULONG),
				       0x55555555UL);
  EQMSG("write status", 0, status);

  status = sis3150Usb_Vme_Single_Read(m_device,
				      RAM,
				      AM,
				      sizeof(ULONG),
				      &data);
  EQMSG("read status", 0, status);
  EQMSG("read data", 0x55555555UL, data);

  sis3150Usb_Vme_Single_Write(m_device,
			      RAM,
			      AM,
			      sizeof(ULONG),
			      0xaaaaaaaaUL);

  data = 0;
  sis3150Usb_Vme_Single_Read(m_device,
			     RAM,
			     AM,
			     sizeof(ULONG),
			     &data);
  EQMSG("read data 2", 0xaaaaaaaaUL, data);

}

// Now read/write 0xffff
// then read/write 0x0f0f.
void vmeTransfers::singlerwShort()
BEGIN
  ULONG data = 0;
  int status = sis3150Usb_Vme_Single_Write(m_device,
					   RAM,
					   AM,
					   sizeof(unsigned short),
					   0xffff);
  EQMSG("write status", 0, status);
  status    = sis3150Usb_Vme_Single_Read(m_device,
					 RAM,
					 AM,
					 sizeof(unsigned short),
					 &data);
  EQMSG("read status", 0, status);
  EQMSG("data 1", 0xffffUL, data & 0xffff);

  sis3150Usb_Vme_Single_Write(m_device,
			      RAM,
			      AM,
			      sizeof(unsigned short),
			      0xf0f);
  data = 0;
  sis3150Usb_Vme_Single_Read(m_device,
			     RAM,
			     AM,
			     sizeof(unsigned short),
			     &data);
  EQMSG("data 2", 0xf0fUL, data & 0xffff);
}
// We'll set first 0x12
// then            0x34.

void vmeTransfers::singlerwByte()
BEGIN
  ULONG data = 0;
  int status;
  status = sis3150Usb_Vme_Single_Write(m_device,
				       RAM,
				       AM,
				       1,
				       0x12);
  EQMSG("write status", 0, status);
  status = sis3150Usb_Vme_Single_Read(m_device,
				      RAM,
				      AM,
				      1,
				      &data);
  EQMSG("Read status", 0, status);
  EQMSG("data 1", 0x12UL, data & 0xff);

  sis3150Usb_Vme_Single_Write(m_device,
			      RAM,
			      AM,
			      1,
			      0x34);
  data = 0;
  sis3150Usb_Vme_Single_Read(m_device,
			     RAM,
			     AM,
			     1,
			     &data);
  EQMSG("data 2", 0x34UL, data & 0xff);
}

// 2 cases < 256 transfers and > 256 transfers.
void vmeTransfers::dmarwLong()
BEGIN
  ULONG pattern[10240];
  ULONG readData[10240];
  ULONG xferCount;
  // Make a counting pattern 50 longs:

  for (int i=0; i < 50; i++) {
    pattern[i] = i;
  }
  int status = sis3150Usb_Vme_Dma_Write(m_device,
					RAM,
					AM,
					sizeof(long), false,
					pattern, 50,
					&xferCount);
  EQMSG("small write status", 0, status);
  EQMSG("small write count", 50UL, xferCount);
  
  status = sis3150Usb_Vme_Dma_Read(m_device,
				   RAM, AM, sizeof(long), false,
				   readData, 50,
				   &xferCount);
  EQMSG("small read status", 0, status);
  EQMSG("small read count", 50UL, xferCount);
  for (int i =0; i < 50; i++ ) { // compare data
    EQMSG("small rw compare", pattern[i], readData[i]);
  }
  // This tests ability to chunk up the I/O as needed to fit in
  // usb packets:

  for (int i =0; i < 10240; i++) {
    pattern[i] = (i%2) ? 0xaaaaaaaaUL : 0x55555555UL; // alternating bits pattern.
  }
  status = sis3150Usb_Vme_Dma_Write(m_device,
				    RAM, AM, sizeof(long), false,
				    pattern, 10240, &xferCount);
  EQMSG("large write status", 0, status);
  EQMSG("large write count", 10240UL, xferCount);

  status = sis3150Usb_Vme_Dma_Read(m_device,
				   RAM, AM, sizeof(long), false,
				   readData, 10240, &xferCount);
  EQMSG("large read status", 0, status);
  EQMSG("large read count", 10240UL, xferCount);
  for (int i =0; i < 10240; i++ ) { // compare data
    EQMSG("small rw compare", pattern[i], readData[i]);
  }


  
}
static const unsigned long COMPAREFAIL=0x1b; // Write/read works but compare fails.
static const unsigned long OK=0x1a;          // write/read works and compares ok.
static const unsigned long READFAIL=0x1e; // write works, but reads fail completion.
void vmeTransfers::dmarwShort()
BEGIN
	
  unsigned short pattern[10240];
  unsigned short readData[10240];
  ULONG xferCount;
  ULONG trycount = OK;		

  // 50 element pattern alternate 0x0000 0xffff

  for (int i=0; i < 10240; i++) {
    pattern[i] = (i%2) ? 0x0000 : 0xffff;
  }
  int status = sis3150Usb_Vme_Dma_Write(m_device,
					RAM,
					AM,
					sizeof(short), false,
					(ULONG*)pattern, trycount,
					&xferCount);
  EQMSG("small write status", 0, status);
  EQMSG("small write count", trycount, xferCount);

  status = sis3150Usb_Vme_Dma_Read(m_device,
				   RAM, AM, sizeof(short), false,
				   (ULONG*)readData, trycount,
				   &xferCount);
  EQMSG("small read status", 0, status);
  EQMSG("small read count", trycount, xferCount);
  for (int i =0; i < trycount; i++ ) { // compare data
    EQMSG("small rw compare", pattern[i], readData[i]);
  }


  
}

void vmeTransfers::dmarwByte()
BEGIN
  unsigned char pattern[10240];
  unsigned char readData[10240];
  ULONG xferCount;
  ULONG trycount = READFAIL;		// First the short transfer.

  // Pattern of rolling bits:

  unsigned char bit = 1;
  for (int i =0; i < 10240; i++) {
    pattern[i] = bit;
    bit = bit << 1;
    if (bit == 0) bit = 1;
  }
  int status = sis3150Usb_Vme_Dma_Write(m_device,
					RAM,
					AM,
					1, false,
					(ULONG*)pattern, trycount,
					&xferCount);
  EQMSG("small write status", 0, status);
  EQMSG("small write count",  trycount, xferCount);

  status = sis3150Usb_Vme_Dma_Read(m_device,
				   RAM, AM, 1, false,
				   (ULONG*)readData, trycount,
				   &xferCount);
  EQMSG("small read status", 0, status);
  EQMSG("small read count", trycount, xferCount);

  for(int i =0; i < trycount; i++) {
    EQMSG("small rw compare", pattern[i], readData[i]);
  }
}
