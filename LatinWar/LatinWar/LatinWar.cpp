// LatinWar.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <thread>
#include <vector> 
#include <chrono>
#include <mutex>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

//Country class
class Country {

    //The map of the country
    std::vector <std::vector<std::pair<std::string, int>>> Map;

    //Counters and values of country
    int totalCostCounter = 0;
    int reloadTime = 0;
    int costOfBombardment = 0;
    int numOfBattery = 0;
    int totalShotCost = 0;
    int damage = 0;
    std::string nameOfCountry;

    //This method computes total damage and damage to current cell 
    int shot(int x, int y) {
        damage += Map.at(x).at(y).second;
        int i = Map.at(x).at(y).second;
        Map.at(x).at(y).second = 0;
        return i;
    }

    //Method for thread. Computes the location of bombardment, checks if the war is ended
    void threadArttilery(Country& opponent, int shotCost, int& shotCounter, bool& end, int reload, int& totalShotCost, int battareyNum, std::string name, std::mutex& door, int random) {
        //Previously created value to create totaly random seed; 
        srand(random);

        int total = opponent.getTotalCostOfCountry();
        while ((total >= totalShotCost) && (opponent.getTotalCostOfCountry() > opponent.getDamage()) && !end) {
            
            int x = rand() % opponent.getSize();
            int y = rand() % opponent.getSize();
            shotCounter++;
            totalShotCost += shotCost;
            int cost = opponent.shot(x, y);
            while (true)
            {
                //mutex for making cout looks good
                if (door.try_lock())
                {
                    std::cout << "NEWS! Battarey number " << battareyNum << " of " << name << " made artillery bombardment to " << opponent.getNameOfPoint(x, y)
                        << "(" << x << "," << y << ")" << "\n Economic damage is:" << cost <<"\n" << "\n";
                    break;
                }                
            }
            //mutex for making cout looks good
            door.unlock();
            sleep_for(seconds(reload));
        }
        //Ends the war after check in while cycle
        end = true;
    }

public:
    //Method creates bataries, counts random value for seed, counts reload time 
    void startWar(bool& end, Country& opponent, int& shotCounter, std::mutex& door) {
        for (int i = 0; i < numOfBattery; i++)
        {
            int random = rand() % 10000;
            int time = reloadTime + rand() % (2);
            std::thread battarey(&Country::threadArttilery, this, std::ref(opponent), costOfBombardment, std::ref(shotCounter), std::ref(end),
                time, std::ref(totalShotCost), i, nameOfCountry, std::ref(door), random);
            battarey.detach();
            sleep_for(microseconds(100));
        }
    }

    //Default contructor
    Country()
    {

    }

    //Main constructor
    Country(size_t size, int time, int cost, int num, std::string name) {
        Map.resize(size);

        //Filling the map with empty spaces
        for (size_t i = 0; i < Map.size(); i++)
        {
            Map.at(i).resize(size);
            for (size_t t = 0; t < size; t++)
            {
                Map.at(i).at(t).first = "Empty space";
                Map.at(i).at(t).second = 0;
            }
        }
        reloadTime = time;
        costOfBombardment = cost;
        numOfBattery = num;
        nameOfCountry = name;
    }

    //Setter for creating unusual objects on map
    void set(std::pair<std::pair<int, int>, std::string> arg) {

        int value = 0;

        if (arg.second == "City")
        {
            value = 1000;
        }
        else if (arg.second == "Village")
        {
            value = 100;
        }
        else if (arg.second == "Millitary_base") {
            value = 500;
        }
        else
        {
            value = 250;
        }
        totalCostCounter = totalCostCounter + value;
        Map.at(arg.first.first).at(arg.first.second).first = arg.second;
        Map.at(arg.first.first).at(arg.first.second).second = value;
    }

    //Different properties
    int getTotalShotCost() {
        return totalShotCost;
    }

    int getSize() {
        return Map.size();
    }

    std::string getNameOfPoint(int x, int y) {
        return Map.at(x).at(y).first;
    }

    int getCostOfPoint(int x, int y) {
        return Map.at(x).at(y).second;
    }

    int getDamage() {
        return damage;
    }

    int getTotalCostOfCountry() {
        return totalCostCounter;
    }
    std::string getName() {
        return nameOfCountry;
    }
};


//reads file, checks if the file is true, creates th Country object

Country create(int i) {
    int reloadTime;
    int shotPrice;
    int battareyNum;
    int countrySize;
    std::string cmd = "";
    std::string path = "";

    int counter = 0;
    if (i == 0)
    {
        cmd = "Anchuria";
        path = "first.txt";
    }
    else
    {
        cmd = "Tarranteria";
        path = "second.txt";

    }

    try
    {
        
        std::fstream in(path, std::ios::in);
        in >> countrySize >> reloadTime >> shotPrice >> battareyNum;

        //Checking if the values in file is true;
        if (countrySize > 50 || countrySize < 4)
        {
            countrySize = 10;
        }
        if (shotPrice < 5 || shotPrice > 1000)
        {
            shotPrice = 50;
        }
        if (reloadTime > 200 || reloadTime < 2)
        {
            reloadTime = 5;
        }
        if (battareyNum < 2 || battareyNum > 5)
        {
            battareyNum = 4;
        }
        
        std::cout << "\nThe size of " << cmd << " is " << countrySize << ". It has " << battareyNum << " batteries, each reloads approximately " <<
            reloadTime << " sec. and costs " << shotPrice << "$ for each bombardment. \n" << "Here is the list of importent objects:\n";
        Country out = Country(countrySize, reloadTime, shotPrice, battareyNum, cmd);
        cmd = "";
        //Reading the file and creating unusual objects on map
        while (in >> cmd)
        {
            int x;
            int y;
            in >> x >> y;
            std::pair<std::pair<int, int>, std::string> arg;
            arg.first.first = x;
            arg.first.second = y;
            arg.second = cmd;
            if (x >= countrySize || x < 0 || y >= countrySize || y < 0)
            {
                continue;
            }
            counter++;
            out.set(arg);
            std::cout << cmd << " " << x << " " << y << " " << "\n";
        }
        // Creates one object, if there are no sutable object in file
        if (counter == 0)
        {
            std::pair<std::pair<int, int>, std::string> arg;
            int x = rand() % countrySize;
            int y = rand() % countrySize;
            arg.first.first = x;
            arg.first.second = y;
            arg.second = "City";
            out.set(arg);
            std::cout << "City" << " " << x << " " << y << " " << "\n";
        }
        return out;
    }
    catch (const std::exception&)
    {
        std::cout << "Wrong File! \n \n";
    }

    //Branch for creating Country object if the file is completely wrong
    countrySize = 10;
    shotPrice = 50;
    reloadTime = 5;
    battareyNum = 4;
    Country out = Country(countrySize, reloadTime, shotPrice, battareyNum, cmd);
    std::pair<std::pair<int, int>, std::string> arg;

    for (size_t i = 0; i < 3; i++)
    {
        int x = rand() % countrySize;
        int y = rand() % countrySize;
        arg.first.first = x;
        arg.first.second = y;
        arg.second = "City";
        out.set(arg);
        std::cout << "City" << " " << x << " " << y << " " << "\n";
    }
    
    return out;
}

//Method for tread. Waits 5 vinutes. Then ends the war, if it was not ended before
void waiter(bool &end, bool &timeover) {
    sleep_for(minutes(5));
    if (end == false())
    {
        timeover = true;
        end = true;
        std::cout << "THE END! USA and United Nations are coming! The war is ended by their pressure";
    }
}

int main()
{
    //Some important flags
    bool end = false;
    bool timeOver = false;
    //MUTEX WOW
    std::mutex door;

    //Creating Countries? reading files.
    std::vector<Country> countries;
    countries.resize(2);
    for (size_t i = 0; i < countries.size(); i++)
    {
        countries.at(i) = create(i);
    }

    int firstCounter = 0;
    int secondCounter = 0;
    std::cout << "\n\n";

    //Starting the war

    countries.at(0).startWar(std::ref(end), std::ref(countries.at(1)), std::ref(firstCounter), std::ref(door));
    countries.at(1).startWar(std::ref(end), std::ref(countries.at(0)), std::ref(secondCounter), std::ref(door));
    std::thread usWaiter(waiter, std::ref(end), std::ref(timeOver));
    usWaiter.detach();

    //Logging the war (using Mutex, of course)
    while (!end)
    {
        while (true)
        {
            if (door.try_lock())
            {
                std::cout << "~~~ " << countries.at(0).getName() << " made" << firstCounter << " bombardments, total damage is " << countries.at(0).getDamage() <<
                    " (max is " << countries.at(0).getTotalCostOfCountry() << ")" << "\n Total money spent on war: " << countries.at(0).getTotalShotCost()<< "\n"<< "\n" <<
                    "~~~ " << countries.at(0).getName() << " made" << secondCounter << " bombardments, total damage is " << countries.at(1).getDamage() <<
                    " (max is " << countries.at(1).getTotalCostOfCountry() << ")" << "\n Total money spent on war: " << countries.at(1).getTotalShotCost() << "\n" << "\n";
                door.unlock();
                if (end)
                {
                    break;
                }
                sleep_for(seconds(5));
                continue;
            }   
        }
    }

    //Logging the results of the war.
    if (!timeOver)
    {
        if (countries.at(0).getDamage() >= countries.at(0).getTotalCostOfCountry())
        {
            std::cout << "THE END! The war ended. " << countries.at(0).getName() << " is completely destroyed!";
        }
        else if (countries.at(1).getDamage() >= countries.at(1).getTotalCostOfCountry())
        {
            std::cout << "THE END! The war ended. " << countries.at(1).getName() << " is completely destroyed!";
        }
        else if (countries.at(0).getTotalShotCost() >= countries.at(1).getTotalCostOfCountry())
        {
            std::cout << "THE END! The war ended. It was too expensive for " << countries.at(0).getName();
        }
        else if (countries.at(0).getTotalShotCost() >= countries.at(1).getTotalCostOfCountry())
        {
            std::cout << "THE END! The war ended. It was too expensive for " << countries.at(1).getName();
        }
        else
        {
            std::cout << "THE END! The war stopped. No one knows why.";
        }
    }

    do
    {
        std::cout << '\n' << "Press a key to exit...";
    } while (std::cin.get() != '\n');

}
