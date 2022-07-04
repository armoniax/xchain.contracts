#include <amax.xchain/amax.xchain.hpp>

#include<utils.hpp>
#include<string>
#include <eosio/transaction.hpp>

namespace amax {

inline int64_t get_precision(const symbol &s) {
    int64_t digit = s.precision();
    CHECK(digit >= 0 && digit <= 18, "precision digit " + std::to_string(digit) + " should be in range[0,18]");
    return calc_precision(digit);
}

inline int64_t get_precision(const asset &a) {
    return get_precision(a.symbol);
}

static constexpr eosio::name MT_BANK{"amax.mtoken"_n};

ACTION xchain::init( const name& admin, const name& maker, const name& checker, const name& fee_collector ) {
   require_auth( _self );
   
   _gstate.admin           = admin;
   _gstate.maker           = maker;
   _gstate.checker         = checker;
   _gstate.fee_collector   = fee_collector;
}

ACTION xchain::reqxintoaddr( const name& applicant, const name& applicant_account, const name& base_chain, const uint32_t& mulsign_wallet_id )
{
   require_auth( applicant );

   auto chain_info  = chain_t(base_chain); 
   CHECKC( _db.get(chain_info), err::RECORD_NOT_FOUND, "chain does not exist: " + base_chain.to_string() );

   account_xchain_address_t::idx_t xchaddrs( _self, _self.value);
   auto acctchain_index 			   = xchaddrs.get_index<"acctchain"_n>();
   const auto& itr 			         = acctchain_index.find( make128key( applicant_account.value, make64key( base_chain.value, mulsign_wallet_id ) ) );
   CHECKC( itr == acctchain_index.end(), err::RECORD_EXISTING, "the record already exists" );
   CHECKC( chain_info.chain == chain_info.base_chain, err::PARAM_INCORRECT, "base chain is incorrect" );
   CHECKC( mulsign_wallet_id < numeric_limits<uint32_t>::max() , err::PARAM_INCORRECT, "mulsign_wallet_id overflow" );

   auto acct_xchain_addr            = account_xchain_address_t( applicant_account, base_chain, mulsign_wallet_id);
   acct_xchain_addr.id              = xchaddrs.available_primary_key();
   acct_xchain_addr.created_at      = time_point_sec( current_time_point() );
   acct_xchain_addr.updated_at      = acct_xchain_addr.created_at;   

   if( chain_info.common_xin_account != "" ) { //for chain type like eos, amax
      acct_xchain_addr.status       = address_status::PROVISIONED;
      acct_xchain_addr.xin_to       = to_string(acct_xchain_addr.id);
   }
   _db.set( acct_xchain_addr );
}

ACTION xchain::setfeerate(const uint64_t& fee_rate) {
   require_auth( _self );

   _gstate.fee_rate = fee_rate;
}

ACTION xchain::setaddress( const name& applicant, const name& base_chain, const uint32_t& mulsign_wallet_id, const string& xin_to ) 
{
   require_auth( _gstate.maker );

   CHECKC( xin_to.length() < max_addr_len, err::ADDRESS_ILLEGAL, "illegal address" );

   auto chain_info =  chain_t( base_chain );
   CHECKC( _db.get(chain_info), err::RECORD_NOT_FOUND, "chain does not found: " + base_chain.to_string() );

   account_xchain_address_t::idx_t xchaddrs( _self, _self.value );
   auto acctchain_index 			   = xchaddrs.get_index<"acctchain"_n>();
   const auto& itr 			         = acctchain_index.find( make128key( applicant.value, make64key( base_chain.value, mulsign_wallet_id )) );
   CHECKC( itr != acctchain_index.end(), err::RECORD_EXISTING, "xchaddrs not found" );


   auto xinto_index 			         = xchaddrs.get_index<"xinto"_n>();
   const auto& xinto_itr			   = xinto_index.find( hash(xin_to) );

   CHECKC( xinto_itr == xinto_index.end(), err::RECORD_EXISTING, "xchaddrs: the record already exist, " + xin_to);



   xchaddrs.modify( *itr, _self, [&]( auto& row ) {
      row.status     = address_status::PROVISIONED;
      row.xin_to     = xin_to;
      row.updated_at = time_point_sec( current_time_point() );
   });
}

/**
 * maker create xin order
 * */
ACTION xchain::mkxinorder( const name& to, const name& chain_name, const symbol& coin_name, 
                           const string& txid, const string& xin_from, const string& xin_to,
                           const asset& quantity )
{
   require_auth( _gstate.maker );
   CHECKC( quantity.symbol == coin_name, err::SYMBOL_MISMATCH, "symbol mismatch" );
   CHECKC( is_account( to ), err::PARAM_INCORRECT, "to account does not exist");
   CHECKC( quantity.is_valid(), err::PARAM_INCORRECT,  "invalid quantity" );
   CHECKC( quantity.amount > 0, err::PARAM_INCORRECT, "must transfer positive quantity" );


   chain_coin_t::idx_t chain_coins (_self, _self.value);
   auto chain_coins_idx = chain_coins.get_index<"chaincoin"_n>();
   CHECKC( chain_coins_idx.find((uint128_t) chain_name.value << 64 | (uint128_t)coin_name.code().raw()) != chain_coins_idx.end(),
                   err::RECORD_NOT_FOUND, "chain_coin does not exist. ");

   CHECKC(!xin_from.empty(), err::ADDRESS_ILLEGAL, "xin_from addess is not null")

   uint32_t mulsign_wallet_id = 0;
   _check_xin_addr( to, chain_name, xin_to, mulsign_wallet_id );

   xin_order_t::idx_t xin_orders( _self, _self.value );
   auto txid_index 			   = xin_orders.get_index<"xintxids"_n>();
   CHECKC( txid_index.find( hash(txid) ) == txid_index.end(), err::RECORD_NOT_FOUND, "txid already existing!" );

   auto created_at = time_point_sec( current_time_point() );
   auto xin_order_id = xin_orders.available_primary_key();

   xin_orders.emplace( _self, [&]( auto& row ) {
      row.id 					= xin_order_id;
      row.txid 			   = txid;
      row.account      = to;
      row.mulsign_wallet_id = mulsign_wallet_id;
      row.xin_from         = xin_from;
      row.xin_to           = xin_to;
      row.chain 			   = chain_name;
      row.coin_name  	   = coin_name;
      row.quantity		   = quantity;
      row.status   			= xin_order_status::CREATED;
      row.maker			   = _gstate.maker;
      row.created_at       = created_at;
      row.updated_at       = created_at;
   });
}

void xchain::_check_xin_addr( const name& to, const name& chain_name, const string& xin_to , uint32_t& mulsign_wallet_id ) 
{
   auto chain_info = chain_t(chain_name);
   CHECKC( _db.get(chain_info), err::RECORD_NOT_FOUND, "chain does not exist: " + chain_name.to_string());
   auto base_chain = chain_info.base_chain;

   if( chain_info.common_xin_account != "" ) {
      CHECKC( chain_info.common_xin_account == xin_to, err::NOT_COMMON_XIN, "xin_to address is not common_xin_account: " + xin_to );
      return;
   }
   
   account_xchain_address_t::idx_t xchaddrs( _self, _self.value );
   auto xinto_index 			         = xchaddrs.get_index<"xinto"_n>();
   const auto& itr 			         = xinto_index.find( hash(xin_to) );

   CHECKC( itr != xinto_index.end(), err::RECORD_NOT_FOUND, "xchaddrs: the record does not exist, " + to.to_string() + ", " + chain_name.to_string() );
   CHECKC( itr->base_chain == base_chain, err::ADDRESS_MISMATCH, "incorrect base_chain used: " + itr->base_chain.to_string() + ", " + base_chain.to_string());
   if(itr->mulsign_wallet_id > 0)
      mulsign_wallet_id = itr->mulsign_wallet_id;
}

/**
 * checker to confirm xin order
 */
ACTION xchain::checkxinord( const uint64_t& order_id )
{
   require_auth( _gstate.checker );

   xin_order_t::idx_t xin_orders( _self, _self.value );
   auto xin_order_itr = xin_orders.find( order_id );
   CHECKC( xin_order_itr != xin_orders.end(), err::RECORD_NOT_FOUND, "xin order not found: " + to_string(order_id) );
   auto status = xin_order_itr->status;
   CHECKC( status == xin_order_status::CREATED, err::STATUS_INCORRECT, "xin order is not created: " + to_string(order_id) );

   auto txid = _get_tixd();
   
   xin_orders.modify( xin_order_itr, _self, [&]( auto& row ) {
      row.status         = xin_order_status::CHECKED;
      row.amc_txid       = txid;
      row.checker        = _gstate.checker;
      row.closed_at      = time_point_sec( current_time_point() );
      row.updated_at     = time_point_sec( current_time_point() );
   });

   auto memo = to_string(order_id);
   if(xin_order_itr->mulsign_wallet_id > 0) 
      memo = "lock:" + to_string(xin_order_itr->mulsign_wallet_id); //mulsign transfer

   TRANSFER( MT_BANK, xin_order_itr->account, xin_order_itr->quantity, memo );

   _reward_farm( xin_order_itr->quantity, xin_order_itr->account );
}

void xchain::_reward_farm( const asset& xin_quantity, const name& farmer ){
   auto symbol_code = xin_quantity.symbol_code().to_string();
   if (_gstate.apl_farm.xin_reward_conf.find(symbol_code) == _gstate.apl_farm.xin_reward_conf.end())
      return;

   auto unit_reward_quant = _gstate.apl_farm.xin_reward_conf[symbol_code];

   auto reward_quant = xin_quantity.amount / xin_quantity.get_precision(xin_quantity.symbol) * unit_reward_quant;
   GROW_APPLE( _gstate.apl_farm.contract, _gstate.apl_farm.land_id, farmer, reward_quant )
}

/**
 * checker cancel the xin order 
 * */
ACTION xchain::cancelxinord( const uint64_t& order_id, const string& cancel_reason )
{
   require_auth( _gstate.checker );

   xin_order_t::idx_t xin_orders( _self, _self.value );
   auto xin_order_itr = xin_orders.find( order_id );
   CHECKC( xin_order_itr != xin_orders.end(), err::RECORD_NOT_FOUND, "xin order not found: " + to_string(order_id) );
   auto status = xin_order_itr->status;
   CHECKC( status == xin_order_status::CREATED, err::STATUS_INCORRECT, "xin order already closed: " + to_string(order_id) );
   
   xin_orders.modify( xin_order_itr, _self, [&]( auto& row ) {
      row.status           = xin_order_status::CANCELED;
      row.close_reason     = cancel_reason;
      row.checker          = _gstate.checker;
      row.closed_at        = time_point_sec( current_time_point() );
      row.updated_at       = time_point_sec( current_time_point() );
   });

}

/**
 * tranfer amtoken(AMBTC/AMETH) event trigger
 * ontransfer, trigger by recipient of transfer()
 * @param quantity - mirrored asset on AMC
 * @param memo - memo format: $addr:$chain:coin_name:memo
 *                            "$eth_addr:eth:ETH,8:mulsign_wallet_id:xchain's memo
 *               
 */
[[eosio::on_notify("amax.mtoken::transfer")]] 
void xchain::ontransfer( name from, name to, asset quantity, string memo ) 
{
   CHECK(memo.size() <= 256, "memo has more than 256 bytes");

   if( _self == from ) return;
   if( to != _self ) return;
   if( get_first_receiver() != MT_BANK ) return;

   if ( memo == "refuel" ) return;

   auto parts = split( memo, ":" );
   CHECKC( parts.size() >= 5, err::PARAM_INCORRECT, "Expected format 'address:chain:coin_name:mulsign_wallet_id:memo'" );
   auto xout_to      = parts[0];
   CHECKC( !xout_to.empty(), err::PARAM_INCORRECT, "xin_out address must not be null" );

   auto chain_name   = name( parts[1] );
   auto coin_name    = to_symbol( (string)parts[2] );
   CHECKC( coin_name == quantity.symbol, err::SYMBOL_MISMATCH, "symbol mismatch" );


   chain_coin_t::idx_t chain_coins (_self, _self.value);
   auto chain_coins_idx = chain_coins.get_index<"chaincoin"_n>();
   auto chain_coin_ptr = chain_coins_idx.find((uint128_t) chain_name.value << 64 | (uint128_t)coin_name.code().raw());
   CHECKC( chain_coin_ptr!= chain_coins_idx.end(), err::RECORD_NOT_FOUND, "chain_coin does not exist. ");

   auto mulsign_wallet_id = stoi( string( parts[3] ));
   auto user_memo = parts[4];
   auto created_at = time_point_sec( current_time_point() );
   xout_order_t::idx_t xout_orders( _self, _self.value );

   auto id = xout_orders.available_primary_key();
   auto txid = _get_tixd();
   auto fee = _calc_fee(chain_coin_ptr->fee, quantity);
   xout_orders.emplace( _self, [&]( auto& row ) {
      row.id 					   = id;
      row.account             = from;
      row.amc_txid            = txid;
      row.mulsign_wallet_id   = mulsign_wallet_id;
      row.xout_to 			   = xout_to;
      row.chain               = chain_name;
      row.coin_name           = coin_name;
      row.apply_quantity		= quantity;
      row.quantity		      = quantity - fee;
      row.fee			         = fee;  
      row.status			      = xin_order_status::CREATED;
      row.created_at          = time_point_sec( current_time_point() );
      row.updated_at          = time_point_sec( current_time_point() );
      row.memo                = user_memo;
   });

}
/**
 * maker onpay the order
 * */
ACTION xchain::setxousent( const uint64_t& order_id, const string& txid, const string& xout_from )
{
   require_auth( _gstate.maker );

   xout_order_t::idx_t xout_orders( _self, _self.value );
   auto xout_order_itr = xout_orders.find( order_id );
   CHECKC( xout_order_itr != xout_orders.end(), err::RECORD_NOT_FOUND, "xout order not found: " + to_string(order_id) );
   auto status = xout_order_itr->status;
   CHECKC( status == xout_order_status::CREATED, err::STATUS_INCORRECT, "xout order status is not created: " + to_string(order_id));

   //check txid
   auto xout_orders_xintxids_itr    = xout_orders.get_index<"xouttxids"_n>();
   CHECKC( xout_orders_xintxids_itr.find( hash(txid) ) == xout_orders_xintxids_itr.end(), err::RECORD_EXISTING, "txid already exists" );

   xout_orders.modify( *xout_order_itr, _self, [&]( auto& row ) {
      row.status     = xout_order_status::SENT;
      row.txid       = txid;
      row.xout_from  = xout_from;
      row.maker      = _gstate.maker;
      row.updated_at = time_point_sec( current_time_point() );
   });
}

/**
 * maker onpay the order
 * */
ACTION xchain::setxouconfm( const uint64_t& order_id )
{
   require_auth( _gstate.maker );

   xout_order_t::idx_t xout_orders( _self, _self.value );
   auto xout_order_itr = xout_orders.find(order_id);
   CHECKC( xout_order_itr != xout_orders.end(), err::RECORD_NOT_FOUND, "xout order not found: " + to_string(order_id) );
   CHECKC( xout_order_itr->status == xout_order_status::SENT, err::STATUS_INCORRECT, "xout order status is not paying");

   //check status
   xout_orders.modify( *xout_order_itr, _self, [&]( auto& row ) {
      row.status     = xout_order_status::CONFIRMED;
      row.updated_at = time_point_sec( current_time_point() );
   });
}

/**
 * checker to confirm out order
 */
ACTION xchain::checkxouord( const uint64_t& order_id )
{
   require_auth( _gstate.checker );

   xout_order_t::idx_t xout_orders( _self, _self.value );
   auto xout_order_itr = xout_orders.find( order_id );
   CHECKC( xout_order_itr != xout_orders.end(), err::RECORD_NOT_FOUND, "xout order not found: " + to_string(order_id) );

   //check status
   CHECKC( xout_order_itr->status == xout_order_status::CONFIRMED, err::STATUS_INCORRECT, "xout order status is not paid" );

   xout_orders.modify( *xout_order_itr, _self, [&]( auto& row ) {
      row.status     = xout_order_status::CHECKED;
      row.closed_at  = time_point_sec( current_time_point() );
      row.updated_at = time_point_sec( current_time_point() );
      row.checker    = _gstate.checker;
   });

   TRANSFER( MT_BANK, _gstate.fee_collector, xout_order_itr->fee, to_string(order_id) );
}

/**
 * maker or checker can cancel xchain out order
 */
ACTION xchain::cancelxouord( const name& account, const uint64_t& order_id, const string& cancel_reason )
{
   require_auth( account );
   CHECKC( account == _gstate.checker || account == _gstate.maker, err::NO_AUTH, "account is not checker or taker" );

   xout_order_t::idx_t xout_orders( _self, _self.value );
   auto xout_order_itr = xout_orders.find( order_id );
   CHECKC( xout_order_itr != xout_orders.end(), err::RECORD_NOT_FOUND, "xout order not found: " + to_string(order_id) );
   CHECKC( xout_order_itr->status == xout_order_status::CONFIRMED||
               xout_order_itr->status == xout_order_status::SENT ||
                xout_order_itr->status == xout_order_status::CREATED  
               , err::STATUS_INCORRECT, "xout order status is not ready for cancel");

   xout_orders.modify( *xout_order_itr, _self, [&]( auto& row ) {
      row.status        = xout_order_status::CANCELED;
      row.closed_at     = time_point_sec( current_time_point() );
      row.updated_at    = time_point_sec( current_time_point() );  
      row.close_reason  = cancel_reason;
      row.checker       = account;   
   });

   auto memo = to_string(order_id);
   if(xout_order_itr->mulsign_wallet_id > 0) 
      memo = "lock:" + to_string(xout_order_itr->mulsign_wallet_id); //mulsign transfer

   TRANSFER( MT_BANK, xout_order_itr->account, xout_order_itr->apply_quantity, memo );

}

void xchain::addchain( const name& account, const name& chain, const name& base_chain, const string& common_xin_account ) {
   require_auth( account );
   CHECKC(account == _self || account == _gstate.admin , err::NO_AUTH, "no auth for operate");

   auto chain_info = chain_t(chain);
   CHECKC( !_db.get(chain_info), err::RECORD_EXISTING, "chain already exists: " + chain.to_string() );

   chain_info.base_chain         = base_chain;
   chain_info.common_xin_account = common_xin_account;
   _db.set( chain_info );
}

void xchain::delchain(const name& account, const name& chain ) {
   require_auth( account );
   CHECKC(account == _self || account == _gstate.admin , err::NO_AUTH, "no auth for operate");

   auto chain_info = chain_t(chain);
   CHECKC( _db.get(chain_info), err::RECORD_NOT_FOUND, "chain does not exists: " + chain.to_string() );

   _db.del( chain_info );
}

void xchain::addcoin(const name& account, const symbol& coin ) {
   require_auth( account );
   CHECKC(account == _self || account == _gstate.admin , err::NO_AUTH, "no auth for operate");

   auto coin_info = coin_t(coin);
   CHECKC( !_db.get(coin_info), err::RECORD_EXISTING, "coin already exists: " + coin.code().to_string() );

   coin_info.coin = coin;
   _db.set( coin_info );
}

void xchain::delcoin(const name& account, const symbol& coin ) {
   require_auth( account );
   CHECKC(account == _self || account == _gstate.admin , err::NO_AUTH, "no auth for operate");

   auto coin_info = coin_t(coin);
   CHECKC( _db.get(coin_info), err::RECORD_EXISTING, "coin not found: " + coin.code().to_string() );

   _db.del( coin_info );
}

void xchain::addchaincoin( const name& account, const name& chain, const symbol& coin, const asset& fee ) {
   require_auth( account );
   CHECKC(account == _self || account == _gstate.admin , err::NO_AUTH, "no auth for operate");

   CHECKC(coin == fee.symbol, err::SYMBOL_MISMATCH, "symbol mismatch");

   chain_coin_t::idx_t chain_coins (_self, _self.value);
   auto chain_coins_idx = chain_coins.get_index<"chaincoin"_n>();
   auto chain_coin_ptr = chain_coins_idx.find((uint128_t) chain.value << 64 | (uint128_t)coin.code().raw());
   CHECKC( chain_coin_ptr == chain_coins_idx.end(), err::RECORD_NOT_FOUND, "chain_coin already exists. ");

   auto chain_coin = chain_coin_t(chain, coin);
   chain_coin.id  = chain_coins.available_primary_key();
   chain_coin.fee = fee;
   _db.set( chain_coin );
}

void xchain::delchaincoin( const name& account, const name& chain, const symbol& coin ) {
   require_auth( account );
   CHECKC(account == _self || account == _gstate.admin , err::NO_AUTH, "no auth for operate");

   chain_coin_t::idx_t chain_coins (_self, _self.value);
   auto chain_coins_idx = chain_coins.get_index<"chaincoin"_n>();
   auto chain_coin_ptr = chain_coins_idx.find((uint128_t) chain.value << 64 | (uint128_t)coin.code().raw());
   CHECKC( chain_coin_ptr != chain_coins_idx.end(), err::RECORD_NOT_FOUND,  "chain_coin does not exists" );
   chain_coins.erase(*chain_coin_ptr);

}

checksum256 xchain::_get_tixd() {
   size_t tx_size = transaction_size();
   char* buffer = (char*)malloc( tx_size );
   read_transaction( buffer, tx_size );
   auto tx_id = sha256( buffer, tx_size );
   return tx_id;
}

asset xchain::_calc_fee( asset fee, asset quantity ) {
   return _calc_deal_amount(quantity) + fee;
}

asset xchain::_calc_deal_amount( const asset &quantity ) {
    int64_t amount = multiply_decimal64(quantity.amount, _gstate.fee_rate, percent_boost);
    amount = multiply_decimal64(amount, get_precision(quantity.symbol), get_precision(quantity));

    return asset(amount, quantity.symbol);
} 
}/// namespace xchain
