// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "sis3150usb_vme.h"

using namespace std;

class simpleOpencloseTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(simpleOpencloseTest);
  CPPUNIT_TEST(simpleOpen);
  CPPUNIT_TEST(openWithLoad);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void simpleOpen();
  void openWithLoad();

};

CPPUNIT_TEST_SUITE_REGISTRATION(simpleOpencloseTest);


void simpleOpencloseTest::simpleOpen()
{


  struct SIS3150USB_Device_Struct info;
  unsigned int                    found;
  int                             status;
  HANDLE                          devHandle;

  // Locate the first device:

  status = FindAll_SIS3150USB_Devices(&info,
				      &found,
				      1);
  EQMSG("findstatus", 0, status);
  ASSERT(found > 0);

  // Now open the first one:

  status = Sis3150usb_OpenDriver((PCHAR)info.cDName, &devHandle);

  EQMSG("openstatus", 0, status);

  // Close the device:

  status = Sis3150usb_CloseDriver(devHandle);
  EQMSG("closestatus", 0, status);
}
// Download firmware and then open.

void simpleOpencloseTest::openWithLoad()
{

  struct SIS3150USB_Device_Struct info[2];
  unsigned int                    found;
  int                             status;
  HANDLE                          devHandle;

  // Locate the first device:

  status = FindAll_SIS3150USB_Devices(info,
				      &found,
				      1);
  EQMSG("findstatus", 0, status);
  ASSERT(found > 0);

  // Now open the first one:

  status = Sis3150usb_OpenDriver_And_Download_FX2_Setup((PCHAR)info[0].cDName, &devHandle);

  EQMSG("fwopenstatus", 0, status);

  // Close the device:

  status = Sis3150usb_CloseDriver(devHandle);
  EQMSG("closestatus", 0, status);
}
