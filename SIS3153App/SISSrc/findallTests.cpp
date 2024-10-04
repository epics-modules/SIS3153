// Test FindAll_SIS3150USB_Devices...

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "sis3150usb_vme.h"

using namespace std;

class findAll : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(findAll);
  CPPUNIT_TEST(findOk);
  CPPUNIT_TEST(findTooSmall);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void findOk();
  void findTooSmall();
};

CPPUNIT_TEST_SUITE_REGISTRATION(findAll);

/*  Assuming we have only one interface.. test the case */
/*  When we find them all and have a large enough array of structs: */

void findAll::findOk()
{
  struct SIS3150USB_Device_Struct info[2];
  unsigned int                    found;
  int      status = FindAll_SIS3150USB_Devices(info,
					       &found,
					       2);
  EQMSG("status", 0, status);
  EQMSG("nfound", (unsigned int)1, found);
  EQMSG("vendor", (USHORT)0x1657,info[0].idVendor);
  EQMSG("model",  (USHORT)0x3150,info[0].idProduct);
  

}

/*  Give it no info structs to fill in: */
/* should still get status == 1 found = 1 */

void findAll::findTooSmall() 
{
  unsigned int found;
  int          status = FindAll_SIS3150USB_Devices((SIS3150USB_Device_Struct*)NULL,
						   &found,
						   0);
  EQMSG("status", 1, status);
  EQMSG("nfound", (unsigned int)1, found);
}
