#include <eosiolib/eosio.hpp>

using namespace eosio;

class [[eosio::contract("hello")]] hello: public contract {
    public:
        using contract::contract;

        [[eosio::action]]
        void hi(name user) {
            print("Hello, ", name{user});
        }
};

EOSIO_DISPATCH( hello, (hi))