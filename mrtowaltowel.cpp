#include <eosio/asset.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>


using namespace eosio;

class [[eosio::contract]] mrtowaltowel : public eosio::contract {
    
    using contract::contract;

    private:
        static constexpr symbol vex_symbol = symbol(symbol_code("VEX"), 4);
        static const uint32_t oneday = 86400;
        //static const uint32_t promountil = 1597708799;

        /*
        struct account
        {
            asset balance;
            uint64_t primary_key() const {return balance.symbol.code().raw();}
        };

        typedef eosio::multi_index< eosio::name("accounts"), account > accounts;
        */

        uint32_t get_param_maxmessages() {
            parameters _parameters(get_self(), get_self().value);

            auto itr = _parameters.find(1);

            return itr->maxmessages;
        }

        uint32_t get_param_promo() {
            parameters _parameters(get_self(), get_self().value);

            auto itr = _parameters.find(1);

            return itr->promo;
        }

        uint32_t get_param_promoamount() {
            parameters _parameters(get_self(), get_self().value);

            auto itr = _parameters.find(1);

            return itr->promoamount;
        }
        
        uint64_t get_param_recordnumber() {
           parameters _parameters(get_self(), get_self().value);

           auto itr = _parameters.find(1);

           return itr->recordnumber;
        }

        void inc_param_recordnumber() {
            parameters _parameters(get_self(), get_self().value);

            auto itr = _parameters.find(1);
            _parameters.modify( itr, get_self(), [&](auto &row) {
                row.recordnumber += 1;
            });

        }

        uint32_t get_msg_counter( name sender ) {

            msgcounters _msgcounters(get_self(), get_self().value);

            auto itr = _msgcounters.find( sender.value );


            uint32_t counter;
            if ( itr != _msgcounters.end() ) {
                counter = itr->counter;
            } else {
                counter = 0;
            }

            return counter;
        }

        void inc_msg_counter( name sender ) {

            msgcounters _msgcounters(get_self(), get_self().value);

            auto itr = _msgcounters.find( sender.value );

            if( itr != _msgcounters.end() ) {
                _msgcounters.modify( itr, get_self(), [&](auto &row) {
                    row.counter += 1;
                });
            } else {
                _msgcounters.emplace( get_self(), [&](auto &row){
                    row.sender = sender;
                    row.counter = 1;
                });
            }
        }

        void dec_msg_counter( name sender ) {

            msgcounters _msgcounters(get_self(), get_self().value);

            auto itr = _msgcounters.find( sender.value );

            if( itr != _msgcounters.end() ) {
                _msgcounters.modify( itr, get_self(), [&](auto &row) {
                    row.counter -= 1;
                });
            } 
        }

        uint32_t now() {
            return current_time_point().sec_since_epoch();
        }


    public:

        // @abi table parameter
        struct [[eosio::table]] parameter {
        //struct parameter {
            uint32_t id;
            uint64_t recordnumber;
            uint32_t maxmessages;
            uint32_t promo;
            uint32_t promoamount; 

            uint64_t primary_key() const { return id; }

            EOSLIB_SERIALIZE( parameter, (id)(recordnumber)(maxmessages)(promo)(promoamount) )
        };

        typedef eosio::multi_index< "parameters"_n, parameter > parameters;

        // @abi table msgcounter
        struct [[eosio::table]] msgcounter {
        //struct msgcounter {
            name sender;
            uint32_t counter;

            uint64_t primary_key() const { return sender.value; }

            EOSLIB_SERIALIZE( msgcounter, (sender)(counter) )
        };

        typedef eosio::multi_index< "msgcounters"_n, msgcounter > msgcounters; 
 

        // @abi table postit
        struct [[eosio::table]] postit {
            uint64_t id;
            name sender;
            asset quantity;
            std::string memo;
            uint32_t created_time;
            uint32_t expired_time;

            uint64_t primary_key()const { return id; }

            EOSLIB_SERIALIZE( postit, (id)(sender)(quantity)(memo)(created_time)(expired_time) )
        };

        typedef eosio::multi_index< "postits"_n, postit > postits;


        [[eosio::action]]
        void getversion() {
            print("MrTowalTowel SC v1.7 - proitidgovex - 20200812\t");
            print("Max Message ", get_param_maxmessages(), " Promo ", get_param_promo(), " Promo Amout ", get_param_promoamount(), " Record Number ", get_param_recordnumber());
        }

        [[eosio::action]]
        void cleartables() {
            require_auth(_self);

            parameters _parameters(get_self(), get_self().value);

            auto itr0 = _parameters.begin();
            while (itr0 != _parameters.end()) {
                itr0 = _parameters.erase(itr0);
            }

            msgcounters _msgcounters(get_self(), get_self().value);
            auto itr1 = _msgcounters.begin();
            while (itr1 != _msgcounters.end()) {
                itr1 = _msgcounters.erase(itr1);
            }

            postits _postits(get_self(), get_self().value);
            auto itr2 = _postits.begin();
            while (itr2 != _postits.end()) {
                itr2 = _postits.erase(itr2);
            }
            print("All records are deleted!");
        }

        [[eosio::action]]
        void clearexpired() {
            require_auth(_self);

            //msgcounters _msgcounters(get_self(), get_self().value);
            postits _postits(get_self(), get_self().value);

            auto itr = _postits.begin();
            
            for (auto itr = _postits.begin(); itr != _postits.end(); itr++) {
                if( itr->expired_time < now() ) {
                    itr = _postits.erase(itr);

                    dec_msg_counter( itr->sender );

                    /*
                    auto itr1 = _msgcounters.find( itr->sender );
                    if( itr1 != _msgcounters.end() ) {
                        _msgcounters.modify( itr1, get_self(), [&](auto &row) {
                            row.counter -= 1;
                        });
                    }
                    */
                }

            }
            print("All expired records are deleted!");
        }

        [[eosio::action]]
        void clearpostit( uint64_t id ) {
            require_auth(_self);

            msgcounters _msgcounters(get_self(), get_self().value);
            postits _postits(get_self(), get_self().value);

            auto itr = _postits.find( id );
            
            if( itr != _postits.end() ) {
                    itr = _postits.erase(itr);

                    dec_msg_counter( itr->sender );
            }
            print("Poist it id ", id, " is deleted!");
        }

        [[eosio::action]]
        void modifypostit( uint64_t id, uint32_t expired_time ) {
            require_auth(_self);

            postits _postits(get_self(), get_self().value);

            auto itr = _postits.find( id );
            
            if( itr != _postits.end() ) {
                _postits.modify( itr, get_self(), [&](auto &row) {
                    row.expired_time = expired_time;
                });
            }
            print("Poist it id ", id, " is deleted!");
        }


        [[eosio::action]]
        void setup( uint32_t maxmessages, uint32_t promo, uint32_t promoamount) {
            require_auth(_self);

            parameters _parameters(get_self(), get_self().value);

            auto itr = _parameters.find(1);
            
            if( itr != _parameters.end() ) {
                _parameters.modify( itr, get_self(), [&](auto &row) {
                    row.maxmessages = maxmessages;
                    row.promo = promo;
                    row.promoamount = promoamount;
                });
            } else {
                _parameters.emplace( get_self(), [&](auto &row){
                    row.id = 1;
                    row.maxmessages = maxmessages;
                    row.promo = promo;
                    row.promoamount = promoamount;
                    row.recordnumber = 0;
                });
            }

            print("Setup maxmessage ", maxmessages, " Promo until ", promo, " Promo amount ", promoamount, " VEX");
        }

   
        [[eosio::on_notify("vex.token::transfer")]]
        void upsert(name from, name to, eosio::asset quantity, std::string memo) {
    
            if (to != get_self() || from == get_self())
            {
                print("What are you doing?");
                return;
            }


            if ( to == get_self()) {

                // Check if sender already reach max messages
                check(get_msg_counter(from) <= get_param_maxmessages(), "You reach max messages allowed!");
                check(quantity.amount >= 10000, "Minimum transfer is 1 VEX");
                check(quantity.symbol == vex_symbol, "We only accept VEX.");
                check(memo.length() > 0, "Write your message in memo.");

                postits _postits(get_self(), get_self().value);

                //auto itr = _messages.end();

                _postits.emplace( get_self(), [&](auto &row){
                    row.id = get_param_recordnumber();
                    row.sender = from;
                    row.quantity.amount = quantity.amount;
                    row.quantity.symbol = quantity.symbol;
                    row.memo = memo;
                    row.created_time = now();
                    row.expired_time = now() + ( oneday * (quantity.amount / 10000)) ;
                });

                // increase message counter for the sender
                inc_msg_counter(from);
                // increase recordnumber
                inc_param_recordnumber();

                // if promo, return back the VEX to sender
                if ( get_param_promo() > now() ) {
                    
                    asset quantity_promo;
                    /*
                    if ( quantity.amount > (get_param_promoamount() * 10000) ) {
                        quantity_promo = asset( quantity.amount - (get_param_promoamount() * 10000), quantity.symbol ); 
                    } else {
                        quantity_promo = asset( quantity.amount, quantity.symbol ); 
                    }
                    */

                    quantity_promo = asset ( get_param_promoamount() * 10000, quantity.symbol );

                    action(
                        permission_level{ _self, "active"_n },
                        "vex.token"_n, "transfer"_n,
                        std::make_tuple(_self, from, quantity_promo, std::string("Promo Mr.TowalTowel. Free 1 VEX = 24 hours!"))
                    ).send();
                }
            }
            
       };
    

};