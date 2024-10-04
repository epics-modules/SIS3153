#ifndef __ASSERTS_H
#define __ASSERTS_H
#include <iostream>
using namespace std;

// Abbreviations for assertions in cppunit.
//    - Add direct cerr output:
#define EQMSG(msg, a, b)   if ((a) != (b))  {    \
                              cerr << "\nEQMSG FAILED: " << __FILE__ << ":" <<  __LINE__ << msg << " expected " << (a) << " got " << (b) <<endl; \
                           } \
                           CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,a,b)

#define EQ(a,b)            if ((a) != (b)) {       \
                             cerr << "\nEQ Failed " << __FILE__  << ":" <<  __LINE__ << " expected " << (a) << " got " << (b) << endl; \
                           } \
                           CPPUNIT_ASSERT_EQUAL(a,b)
#define ASSERT(expr)       if (!(expr)) {   \
                             cerr << "Assertion failed " << __FILE__ << ":" <<  __LINE__ << "expr was: " << (expr) << endl; \
                           }  \
                           CPPUNIT_ASSERT(expr)
#define FAIL(msg)          CPPUNIT_FAIL(msg)

#define BEGIN { cerr << __PRETTY_FUNCTION__ << endl;

// Macro to test for exceptions:

#define EXCEPTION(operation, type) \
   {                               \
     bool ok = false;              \
     try {                         \
         operation;                 \
     }                             \
     catch (type e) {              \
       ok = true;                  \
     }                             \
     ASSERT(ok);                   \
   }


#endif
