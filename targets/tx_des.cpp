#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "typewrapper.h"
#include "tx_des.h"
#include "../bitcoin/consensus/tx_check.h"
#include "../bitcoin/consensus/validation.h"
#include "../bitcoin/primitives/block.h"
#include "../bitcoin/streams.h"

extern "C" char* go_btcd_des_tx(uint8_t *data, size_t len);

std::string TransactionDesCore(Span<const uint8_t> buffer)
{
    DataStream ds{buffer};
    CMutableTransaction tx;
    try {
        ds >> TX_WITH_WITNESS(tx);
    } catch (const std::ios_base::failure&) {
        return "";
    }

    TxValidationState state;
    CTransaction tx_def{tx};
    const bool res{CheckTransaction(tx_def, state)};
    if (res) return tx.GetHash().ToString();
    return "";
}

void TransactionDes(FuzzedDataProvider& provider) 
{
    std::vector<uint8_t> buffer{provider.ConsumeRemainingBytes<uint8_t>()};

    PRETTY_TYPE_DEC(core, TransactionDesCore(buffer))
    PRETTY_TYPE_DEC(btcd, std::string(go_btcd_des_tx(buffer.data(), buffer.size())))

    assert(core == btcd);
}