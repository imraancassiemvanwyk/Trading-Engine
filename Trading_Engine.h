#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include <vector> 
#include <algorithm>
#include <fstream>
#include <sstream>


    struct Order {
        bool cancelled = false;
        int idNumber;
        bool buyOrSell;
        int shares;
        int limit;
        double entryTime;
        double eventTime;
    };

    class Engine
    {
    public:
        std::vector<std::pair<int, std::list<Order>>*> selltable;
        std::vector<std::pair<int, std::list<Order>>*> buytable;
        int lowestSell;
        int highestBuy;
        int upper_limit;
        int lower_limit;
        int tick_rate;
        int message_num = 0;
        bool Trading_halt = false;
        std::vector<std::vector<std::string>> data;
        std::vector<std::vector<std::string>> order_on_halt;

        void Initialise();

        void Add_order(Order ord);

        void Cancel_Order(Order ord);

        bool exists(Order ord);

        bool Exexute_order(Order ord, int shares);

        void partial_cancel(Order ord, int shares); 

        void Order_match(Order ord);

        void read_CSV(std::string filename);

        void Process_messages();

        void Message_handler(std::vector<std::string> message);
    };

