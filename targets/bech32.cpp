#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "typewrapper.h"
#include "bech32.h"
#include "../bitcoin/bech32.h"
#include "../bitcoin/streams.h"

extern "C" char* go_btcd_bech32(uint8_t *data, size_t len);

std::string Bech32Core(Span<const uint8_t> buffer)
{
    const std::string random_string(buffer.begin(), buffer.end());
    const auto r1 = bech32::Decode(random_string);
    if (r1.encoding == bech32::Encoding::INVALID) {
        return "";
    }
    return r1.hrp;
}


void Bech32(FuzzedDataProvider& provider) 
{
    std::vector<uint8_t> buffer{provider.ConsumeRemainingBytes<uint8_t>()};

    PRETTY_TYPE_DEC(core, Bech32Core(buffer))
    PRETTY_TYPE_DEC(btcd, std::string(go_btcd_bech32(buffer.data(), buffer.size())))

    assert(btcd == core);
}