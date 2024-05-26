#include "Pair.h"

Pair::Pair() {

}

QVector<PairCL> Pair::getPairsCL(uint nElements) {
    QVector<PairCL> pairsCL;

    for(uint i = 0; i < nElements; i++) {
        for(uint j = i + 1; j < nElements; j++) {
            PairCL pairCL;

            pairCL.first = i;
            pairCL.second = j;
            pairCL.isNeighbor = true;

            pairsCL.push_back(pairCL);
        }
    }

    return pairsCL;
}

QVector<PairCL> Pair::getPairsCL(uint nElements1, uint nElements2) {
    QVector<PairCL> pairsCL;

    for(uint i = 0; i < nElements1; i++) {
        for(uint j = 0; j < nElements2; j++) {
            PairCL pairCL;

            pairCL.first = i;
            pairCL.second = j;
            pairCL.isNeighbor = true;

            pairsCL.push_back(pairCL);
        }
    }

    return pairsCL;
}

