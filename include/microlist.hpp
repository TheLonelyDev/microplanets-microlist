#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

CONTRACT microlist : public eosio::contract {
  public:
    using eosio::contract::contract;

    [[eosio::action]] void signup(eosio::name & scope, eosio::name & user, eosio::name & referer);
    [[eosio::action]] void decline(eosio::name & scope, eosio::name & user);
    
    [[eosio::action]] void setlist(eosio::name & scope, eosio::time_point & start_time, eosio::time_point & end_time, bool is_active);
    [[eosio::action]] void rmlist(eosio::name & scope, uint64_t limit);  

  private:
    struct [[eosio::table]] config {
      bool is_active = false;
      eosio::time_point start_time = eosio::current_time_point();
      eosio::time_point end_time = eosio::current_time_point();
    };
    typedef eosio::singleton<eosio::name("config"), config> config_table;

    struct [[eosio::table]] whitelist {
      eosio::name user;
      eosio::name referer;
      eosio::time_point time = eosio::current_time_point();

      uint64_t primary_key() const { return user.value; }
      uint64_t secondary_key_0() const { return referer.value; }
    };
    typedef eosio::multi_index<
      eosio::name("whitelist"), 
      whitelist,
      eosio::indexed_by<eosio::name("referer"), eosio::const_mem_fun<whitelist, uint64_t, &whitelist::secondary_key_0>>
    > whitelist_table;

    whitelist_table get_whitelist(eosio::name & scope)
    {
      return whitelist_table(get_self(), scope.value);
    }

    config_table get_config(eosio::name & scope)
    {
      return config_table(get_self(), scope.value);
    }
};

EOSIO_DISPATCH(microlist, 
  // User actions
  (signup)
  (decline)

  // Contract actions
  (setlist)
  (rmlist)
);
