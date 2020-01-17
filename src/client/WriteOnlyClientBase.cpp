#include <client/ClientError.hpp>
#include <client/ReadOnlyClientBase.hpp>
#include <client/WriteOnlyClientBase.hpp>
#include <client/odin/ReadWriteOdinClient.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <g3log/g3log.hpp>
#include <utilxx/Result.hpp>

using forge::client::WriteOnlyClientBase;
using forge::client::ReadWriteOdinClient;



auto WriteOnlyClientBase::writeTxToBlockchain(std::string txid_input,
                                              std::int64_t index,
                                              std::vector<std::byte> metadata,
                                              std::int64_t burn_value,
                                              std::vector<
                                                  std::pair<std::string,
                                                            std::int64_t>>
                                                  outputs) const
    -> utilxx::Result<std::string, ClientError>
{
    return generateRawTx(std::move(txid_input),
                         index,
                         std::move(metadata),
                         burn_value,
                         std::move(outputs))
        .flatMap([this](auto raw_tx) {
            return signRawTx(std::move(raw_tx));
        })
        .flatMap([this](auto signed_tx) {
            return sendRawTx(std::move(signed_tx));
        });
}


auto forge::client::make_writing_client(const std::string& host,
                                        const std::string& user,
                                        const std::string& password,
                                        std::int64_t port,
                                        core::Coin coin)
    -> std::unique_ptr<WriteOnlyClientBase>
{
    switch(coin) {
    case core::Coin::Odin:
    case core::Coin::tOdin:
        return std::make_unique<ReadWriteOdinClient>(host,
                                                     user,
                                                     password,
                                                     port,
                                                     coin);

    default:
        LOG(FATAL) << "entered default case which should never happen";
        return nullptr;
    }
}
