#include "vector"
#ifdef __CINT__ 
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;
#pragma link C++ class vector<* TNtuple>+;
#pragma link C++ class vector<* TNtuple>::*;
#ifdef G__VECTOR_HAS_CLASS_ITERATOR
#pragma link C++ operators vector<* TNtuple>::iterator;
#pragma link C++ operators vector<* TNtuple>::const_iterator;
#pragma link C++ operators vector<* TNtuple>::reverse_iterator;
#endif
#endif
