#include "Trading_Engine.h"


void Engine::Initialise()
{
    int buckets = (upper_limit - lower_limit) / tick_rate;
    selltable.resize(buckets);
    buytable.resize(buckets); for (size_t i = 0; i < selltable.size(); i++)
    {
        buytable[i] = new std::pair<int, std::list<Order>>; 
        selltable[i] = new std::pair<int, std::list<Order>>;
    }
}

void Engine::Add_order(Order ord)
{

    if (ord.buyOrSell)
    {
        buytable[((ord.limit - lower_limit) / tick_rate) - 1]->second.push_back(ord);
        buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first += ord.shares;
        if (ord.limit > highestBuy)highestBuy = ord.limit;
    }
    else
    {
        selltable[((ord.limit - lower_limit) / tick_rate) - 1]->second.push_back(ord);
        selltable[((ord.limit - lower_limit) / tick_rate) - 1]->first += ord.shares;
        if (ord.limit < lowestSell)lowestSell = ord.limit;
    }

}

void Engine::Cancel_Order(Order ord)
{
    Order temp = ord;
    for (auto& i : buytable[((ord.limit - lower_limit) / tick_rate) - 1]->second)
    {
        if (i.idNumber == ord.idNumber)
        {
            i.cancelled = true;
        }
    }
    if(ord.buyOrSell) buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= ord.shares;
    else selltable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= ord.shares;
}

bool Engine::exists(Order ord)
{
    if (ord.buyOrSell)
    {
        for (auto i : buytable[((ord.limit - lower_limit) / tick_rate) - 1]->second)
        {
            if (i.idNumber == ord.idNumber)return true;
        }
    }
    else
    {
        for (auto i : selltable[((ord.limit - lower_limit) / tick_rate) - 1]->second)
        {
            if (i.idNumber == ord.idNumber)return true;
        }
    }
    
    return false;
}


bool Engine::Exexute_order(Order ord, int shares)
{
    if (shares > ord.shares) std::cout << "invalid share amount";
    if (ord.cancelled)
    {
        if (ord.buyOrSell)
        {
            buytable[((ord.limit - lower_limit) / tick_rate) - 1]->second.pop_front();
        }
        else
        {
            selltable[((ord.limit - lower_limit) / tick_rate) - 1]->second.pop_front();
        }
        return false;
    }
    else
    {
        if (ord.buyOrSell)
        {
            buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= shares;
            if (buytable[((ord.limit - lower_limit) / tick_rate) - 1]->second.front().shares == shares)
            {
                buytable[((ord.limit - lower_limit) / tick_rate) - 1]->second.pop_front();
                buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= shares;
                int count = 0;
                if ((buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first == 0) ||
                    (buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first == highestBuy))
                {
                    while (buytable[(((ord.limit - lower_limit) / tick_rate) - 1) - count]->first == 0)
                        count++;
                    highestBuy = buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first;
                }
            }
        }
        else
        {
            selltable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= shares;
            if (selltable[((ord.limit - lower_limit) / tick_rate) - 1]->second.front().shares == shares)
            {
                selltable[((ord.limit - lower_limit) / tick_rate) - 1]->second.pop_front();
                selltable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= shares;
                int count = 0;
                if ((selltable[((ord.limit - lower_limit) / tick_rate) - 1]->first == 0) ||
                    (selltable[((ord.limit - lower_limit) / tick_rate) - 1]->first == lowestSell))
                {
                    while (selltable[(((ord.limit - lower_limit) / tick_rate) - 1) + count]->first == 0)
                        count++;
                    lowestSell = selltable[(((ord.limit - lower_limit) / tick_rate) - 1) + count]->first;
                }
            }
        }
        return true;
    }
}

void Engine::partial_cancel(Order ord, int shares)
{
    if (shares > ord.shares)
    {
        std::cerr << "invalid share amount";
    }

    if (ord.shares == shares)
    {
        Cancel_Order(ord);
    }
    for (auto& i: buytable[((ord.limit - lower_limit) / tick_rate) - 1]->second)
    {
        if (i.idNumber == ord.idNumber)
        {
            i.shares -= shares;
        }
    }
    if (ord.buyOrSell) { 
        buytable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= shares;
    }
    else {
        selltable[((ord.limit - lower_limit) / tick_rate) - 1]->first -= shares;
    }
    
}

void Engine::Order_match(Order ord)
{

    if ((ord.limit < lowestSell) && ord.buyOrSell) {
        Add_order(ord);
    }
    else if ((ord.limit > highestBuy) && (!ord.buyOrSell)) {
        Add_order(ord);
    }
    else
    {
        if (ord.buyOrSell)
        {
            int count = (lowestSell / tick_rate) - 1;
            while ((ord.shares != 0) || !(ord.limit < lowestSell))
            {
                Order x;
                x = selltable[((ord.limit - lower_limit) / tick_rate) - 1]->second.front();
                if (ord.shares > x.shares)
                {
                    if (selltable[count]->first == 0)count--;
                    if (Exexute_order(x, x.shares))
                    {
                        ord.shares -= x.shares;
                    }

                }
                else
                {
                    if (selltable[count]->first == 0)count--;
                    if (Exexute_order(x, ord.shares))
                    {
                        ord.shares = 0;
                    }
                }
                delete& x;

            }
            if (ord.shares != 0)
            {
                Add_order(ord);
            }
        }
        else
        {
            int count = (highestBuy / tick_rate) - 1;
            while ((ord.shares != 0) || !(ord.limit < highestBuy))
            {
                Order x;
                x = buytable[count]->second.front();
                if (ord.shares > x.shares)
                {
                    if (buytable[count]->first == 0)count--;
                    if (Exexute_order(x, x.shares))
                    {
                        ord.shares -= x.shares;
                    }

                }
                else
                {
                    if (buytable[count]->first == 0)count--;
                    if (Exexute_order(x, ord.shares))
                    {
                        ord.shares = 0;
                    }
                }
                delete& x;
            }
            if (ord.shares != 0)
            {
                Add_order(ord);
            }
        }

    }
}

void Engine::read_CSV(std::string filename)
{
    std::vector<std::string> row;
    std::string line, word;
    std::fstream file(filename, std::ios::in);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            row.clear();

            std::stringstream str(line);
            int i = 0;
            while (getline(str, word, ',')) {
                row.push_back(word);
            }
            data.push_back(row);
        }
    }
    else {
        std::cerr << "Could not open the file\n";
    }
}

void Engine::Process_messages()
{
    for (size_t i = 0; i < data.size(); i++)
    {
        Message_handler(data[i]);
    }
}

void Engine::Message_handler(std::vector<std::string> message)
{
    Order ord;
    ord.entryTime = std::stod(message[0]);
    ord.idNumber = std::stoi(message[2]);
    ord.shares = std::stoi(message[3]);
    ord.limit = std::stoi(message[4]);
    if (std::stoi(message[5]) == 1)
    {
        ord.buyOrSell = true;
    }
    else if (std::stoi(message[5]) == -1)
    {
        ord.buyOrSell = false;
    }

    switch (std::stoi(message[1]))
    {
    case 1:
        Add_order(ord);
    case 2:
        partial_cancel(ord, ord.shares);
    case 3:
        Cancel_Order(ord);
    case 4:
        if (!Trading_halt)
        {
            Order_match(ord);
        }
        else
        {
            order_on_halt.push_back(message);
        }
    case 5:
        if (!Trading_halt)
        {
            Order_match(ord);
        }
        else
        {
            order_on_halt.push_back(message);
        }
    case 7:
        if (message[4] == "-1")Trading_halt = true;
        if (message[4] == "1")
        {
            Trading_halt = false;
            for(int i =0;i<order_on_halt.size();i++)
            {
                Message_handler(order_on_halt[i]);
            }
        }
    }
}
