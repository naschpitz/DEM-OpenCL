#include "Pair.h"

Pair::Pair() {

}

QVector<PairCL> Pair::getPairsCL(uint nElements) {
    QVector<PairCL> pairsCL;

    for(ulong i = 0; i < nElements; i++) {
        for(ulong j = i + 1; j < nElements; j++) {
            PairCL pairCL;

            pairCL.first = i;
            pairCL.second = j;

            pairsCL.push_back(pairCL);
        }
    }

    return pairsCL;
}
