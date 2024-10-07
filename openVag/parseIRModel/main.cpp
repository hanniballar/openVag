#include <iostream>
#include "IRXmlRep.h"

IRXmlRep parseIRModel(const std::string& fileName);

int main() {

    auto res = parseIRModel("D:/work/openVag/test/example.xml");
    return 0;
}