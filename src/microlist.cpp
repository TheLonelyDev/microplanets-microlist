#include <microlist.hpp>

[[eosio::action]] void microlist::signup(
  eosio::name & scope, 
  eosio::name & user,
  eosio::name & referer
) {
  require_auth(user);

  auto whitelists = get_whitelist(scope);
  auto config = get_config(scope).get();

  eosio::check(config.is_active, "This signup is not active");

  eosio::check(user != referer, "Sorry, but you cannot be your own friend in this case... *hugs*");

  eosio::check(eosio::current_time_point() >= config.start_time, "The signup period has not yet started");

  eosio::check(config.end_time.sec_since_epoch() == 0 || eosio::current_time_point() <= config.end_time, "The signup period has ended");

  whitelists.emplace(user, [&](auto &row) {
    row.user = user;
    row.referer = referer;
  });
}

[[eosio::action]] void microlist::decline(
  eosio::name & scope, 
  eosio::name & user
) {
  require_auth(user);

  auto whitelists = get_whitelist(scope);
  auto config = get_config(scope).get();

  whitelists.erase(whitelists.require_find(user.value, "No whitelist entry found"));
}

[[eosio::action]] void microlist::setlist(
  eosio::name & scope, 
  eosio::time_point & start_time, 
  eosio::time_point & end_time, 
  bool is_active
) {
  require_auth(get_self());

  get_config(scope).set(
    config {
      is_active,
      start_time,
      end_time
    }, 
    get_self()
  );
}

[[eosio::action]] void microlist::rmlist(
  eosio::name & scope, 
  uint64_t limit
) {
  require_auth(get_self());

  auto config_obj = get_config(scope);

  if (config_obj.exists()) {
    config_obj.remove();
  }

  auto whitelists = get_whitelist(scope);
  auto whitelist_itr = whitelists.begin();
  uint64_t current = 0;

  while (whitelist_itr != whitelists.end()) {
    if (current >= limit) {
      break;
    }    

    whitelist_itr = whitelists.erase(whitelist_itr);

    current++;
  }
}
