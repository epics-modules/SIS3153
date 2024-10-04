// Test register read/write functions.
// We are going to operate on the first SIS USB interface we find.


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "sis3150usb_vme.h"
#include <iostream>
using namespace std;

#include <usb.h>



class registerTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(registerTests);
  CPPUNIT_TEST(idRead);		// Single read
  CPPUNIT_TEST(leds);		// Single write.
  CPPUNIT_TEST(blockread);      // Read test block.
  CPPUNIT_TEST(blockwrite);	// test block write.
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
  void idRead();
  void leds();
  void blockread();
  void blockwrite();
};

CPPUNIT_TEST_SUITE_REGISTRATION(registerTests);


void registerTests::idRead()
BEGIN

  ULONG idRegister;
  int status = sis3150Usb_Register_Single_Read(m_device, 1, &idRegister);
  EQMSG("status", 0, status);
  EQMSG("id",  0x31500000UL,  idRegister & 0xffff0000);
}

// This tests single register write..

void registerTests::leds()
BEGIN

  // light up the user lights.

  int status= sis3150Usb_Register_Single_Write(m_device, 0, 3);
  EQMSG("status", 0, status);

  // Programmatically ensure they are lit up.

  ULONG csr;
  status = sis3150Usb_Register_Single_Read(m_device, 0, &csr);
  EQMSG("leds on", 3UL, csr & 3UL);

  // Unlight the lights .. this way we are ensured things have changed.
  // and that we leave the lights off for the next test run.

  sis3150Usb_Register_Single_Write(m_device, 0, 0x30000);
  sis3150Usb_Register_Single_Read(m_device, 0, &csr);
  EQMSG("leds off", 0UL, csr & 3UL);
   
}


void registerTests::blockread()
BEGIN
  ULONG data[100];		// Don't need to read too much.
  ULONG   count;


  int   status = sis3150Usb_Register_Dma_Read(m_device,
					      0x10000000, 
					      data, 
					      100, &count);
  EQMSG("status", status, 0);
  EQMSG("count",  100UL,    count);


  // Seems that this block is actually peformance counters and
  // not data = address hmmm.

  //  for (int i = 0; i < 100; i++) {
  //     EQMSG("data", (ULONG)(0x10000000+i), data[i]);
  // }
					      
}

// There's some evidence, the SIS module will accept writes
// to readonly registers.
//  We'll try to do a block write of the first 4 registers.
//  turning on the LEDS and not really caring about the rest of it for now
//  since registe DMA writes are a rather weird thing to attempt in any event.
//
void registerTests::blockwrite()
BEGIN
  ULONG data[4] = {3, 0, 0, 0};
  ULONG count;

  int status = sis3150Usb_Register_Dma_Write(m_device,
					     0,
					     data,
					     4,
					     &count);
  EQMSG("status", 0, status);
  EQMSG("count",  4UL, count);

  // See that we turned the light on too:

  ULONG csr;
  sis3150Usb_Register_Single_Read(m_device, 0, &csr);
  EQMSG("leds on", 3UL, csr & 3UL);

  // Turn the leds off for next time.

  sis3150Usb_Register_Single_Write(m_device, 0, 0x30000);

}
