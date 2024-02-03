#ifndef _CreoMemory_h_
#define __CreoMemory_h__h_

#include <ProElement.h>

class pro_element {
    public:
        ProElement element;
        pro_element() {}
        pro_element(pro_element&&) = default;
        pro_element& operator=(pro_element&&) = default;
        pro_element(const pro_element&) = default;
        pro_element& operator=(const pro_element&) = default;
        ~pro_element() {
            ProElementFree(&element);
        }
};

#endif