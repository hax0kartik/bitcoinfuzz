#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "typewrapper.h"
#include "../bitcoin/blockencodings.h"
#include "../bitcoin/streams.h"

using namespace std::string_literals;

extern "C" char* rust_bitcoin_prefilledtransaction(const uint8_t *data, size_t len);

std::optional<uint16_t> PrefilledTransactionCore(Span<const uint8_t> buffer)
{
    DataStream ds{buffer};
    PrefilledTransaction tx;
    try {
        ds >> tx;
    } catch (const std::ios_base::failure&) {
        return std::nullopt;
    }
    return tx.index;
}

void PrefilledTransactionTarget(FuzzedDataProvider& provider) 
{
    std::vector<uint8_t> buffer{provider.ConsumeRemainingBytes<uint8_t>()};
    auto core{PrefilledTransactionCore(buffer)};

    PRETTY_TYPE_DEC(rust_bitcoin, std::string(rust_bitcoin_prefilledtransaction(buffer.data(), buffer.size())))

    if (*rust_bitcoin == "unsupported segwit version") return;
    if (core.has_value()) assert(rust_bitcoin != ""s);
    else assert(rust_bitcoin == ""s);
}
