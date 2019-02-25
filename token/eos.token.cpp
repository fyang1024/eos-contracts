#include <eosio.token/eosio.token.hpp>

namespace eosio {
    void token::create(name issuer, asset max_supply) {
        require_auth(_self);
        auto sym = max_supply.symbol;
        eosio_assert(sym.is_valid(), "invalid symbol name");
        eosio_assert(max_supply.is_valid(), "invalid supply");
        eosio_assert(max_supply.amount > 0, "max supply must be positive");

        stats statstable(_self, sym.code().raw());
        auto existing = statstable.find(sym.code().raw());
        eosio_assert( existing == statstable.end(), "token with symbol already exists");

        statstable.emplace(_self, [&](auto& s) {
            s.supply.symbol = max_supply.symbol;
            s.max_supply = max_supply;
            s.issuer = issuer;
        });
    }

    void token::issue(name to, asset quantity, string memo) {
        auto sym = quantity.symbol;
        eosio_assert(sym.is_valid(), "invalid symbol name");
        eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

        stats statstable(_self, sym.code.raw());
        auto existing = statstable.find(sym.code().raw());
        eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
        const auto& st = *existing;

        require_auth(st.issuer);
        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must issue positive quantity");

        eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
        eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

        statstable.modify(st, same_payer, [&]( auto& s)) {
            s.supply += quantity;
        });

        add_balance(st.issuer, quantity, st.issuer);

        if (to != st.issuer) {
            SEND_INLINE_ACTION(*this, transfer, {{st.issuer, "active"_n}}, 
            {st.issuer, to, quantity, memo});
        }
    }

    void token::retire(asset quantity, string memo) {
        auto sym = quantity.symbol;
        eosio_assert(sym.is_valid(), "invalid symbol name");
        eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

        stats statstable(_self, sym.code.raw());
        auto existing = statstable.find(sym.code().raw());
        eosio_assert(existing != statstable.end(), "token with symbol does not exist");
        const auto& st = *existing;

        require_auth(st.issuer);
        eosio_assert(quanity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must retire positive quantity");
        eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

        statstable.modify( st, same_payer, [&](auto& s){
            s.supply -= quanity;
        });

        sub_balance(st.issuer, quantity);
    }
}