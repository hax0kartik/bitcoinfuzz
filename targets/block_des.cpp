#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "typewrapper.h"
#include "block_des.h"
#include "../bitcoin/primitives/block.h"
#include "../bitcoin/streams.h"

extern "C" char* rust_bitcoin_des_block(const uint8_t *data, size_t len);
extern "C" char* go_btcd_des_block(uint8_t *data, size_t len);

std::string BlockDesCore(Span<const uint8_t> buffer)
{
    DataStream ds{buffer};
    CBlock block;
    try {
        ds >> TX_WITH_WITNESS(block);
        return block.GetHash().ToString();
    } catch (const std::ios_base::failure&) {
        return "";
    }
    return "";
}

// This target is expected to crash, needs some verification (e.g. segwit version).
void BlockDes(FuzzedDataProvider& provider) 
{
    std::vector<uint8_t> buffer{provider.ConsumeRemainingBytes<uint8_t>()};

    PRETTY_TYPE_DEC(core, BlockDesCore(buffer))
    PRETTY_TYPE_DEC(rust_bitcoin, std::string(rust_bitcoin_des_block(buffer.data(), buffer.size())))
    PRETTY_TYPE_DEC(btcd, std::string(go_btcd_des_block(buffer.data(), buffer.size())))

    if (*rust_bitcoin == "unsupported segwit version") return;
    assert(btcd == rust_bitcoin && rust_bitcoin == core);
}