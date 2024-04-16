#include "Pedestrian.h"
#include "lib/nlohmann/json.hpp"
#include "utility/Utility.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <random>

using namespace std;

using json = nlohmann::json;

double calcX(double coor, double width, bool left) {
    return left ? coor - width / 2 : coor + width / 2;
}

double randomDouble(double min, double max) {
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> dis(min, max);
	return dis(gen);
}

int randomInt(int min, int max) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(min, max);
	return dis(gen);
}   

vector<Ward> generateWard() {
    vector<Ward> wards;
    ifstream f("data/hospital.txt");
    string line;
    if (f.is_open()) {
        getline(f, line);
        for (int i = 2; i < 11; i++) {
            vector<Point> edges;
            Point mid1, mid2, p1, p2, p3, p4;
            double width;
            string ward_name;
            if (!getline(f, line)) {
                break;
            }
            istringstream iss(line);
            iss >> mid1.x >> mid1.y;
            iss >> mid2.x >> mid2.y;
            iss >> width;
            iss >> ward_name;

            p1.x = calcX(mid1.x, width, true);
            p1.y = mid1.y;
            edges.push_back(p1);

            p2.x = calcX(mid1.x, width, false);
            p2.y = mid1.y;
            edges.push_back(p2);

            p3.x = calcX(mid2.x, width, false);
            p3.y = mid2.y;
            edges.push_back(p3);

            p4.x = calcX(mid2.x, width, true);
            p4.y = mid2.y;
            edges.push_back(p4);

            wards.push_back(Ward(ward_name, mid1, mid2, edges));
            cout << "Current wards count: " << wards.size() << endl;
        }
        f.close();
    }
    else {
        cout << "File not found" << endl;
    }
    return wards;
}

vector<Event> eventGen() {
    vector<Event> events;
	ifstream f("data/event_distribution.txt");
	string line;

    if (f.is_open()) {
        while (getline(f, line)) {
            Event e;
            double value;
            istringstream iss(line);
            vector<double> intensity;
            while (iss >> value) {
				intensity.push_back(value);
			}
            cout << "Event creation successful" << endl;
            e.setIntensity(intensity);
            e.setTime(0);
            events.push_back(e);
        }
		f.close();
	}
    else {
		cout << "File not found" << endl;
	}
	return events;
}

void pedestrianGen() {
    json inputData;
    inputData = Utility::readInputData("data/input.json");
    int ID = 0;

    double deviation = randomDouble(1 - (double)inputData["experimentalDeviation"]["value"] / 100, 1 + (double)inputData["experimentalDeviation"]["value"] / 100);
    vector<double> ages;
    ifstream f1("data/age_distribution.txt");
    string line;
    while (getline(f1, line)) {
        ages.push_back(stod(line));
    }
    f1.close();

    // Distribution per category: 66 51 51 66 51 15
    // Distribution per role (Personnel, Visitor, Patient): 100, 113, 87  
    int pedestrianCountPerCat[] = { 100, 113, 87 };
    vector<Pedestrian> pedestrians;
    
    vector<Event> allEvents = eventGen();
    vector<int> allTimeDistances;
    ifstream f2("data/age_distribution.txt"); 
    string line; 
    while (getline(f2, line)) {
        allTimeDistances.push_back(stoi(line));
    }
    f2.close();

    Personality open, neurotic;
    open.setLambda(double(inputData["personalityDistribution"]["distribution"]["open"]["lambda"]));
    open.setPositiveEmotionThreshold(double(inputData["personalityDistribution"]["distribution"]["open"]["positiveEmotionThreshold"]));
    open.setNegativeEmotionThreshold(double(inputData["personalityDistribution"]["distribution"]["open"]["negativeEmotionThreshold"]));
    neurotic.setLambda(double(inputData["personalityDistribution"]["distribution"]["neurotic"]["lambda"]));
    neurotic.setPositiveEmotionThreshold(double(inputData["personalityDistribution"]["distribution"]["neurotic"]["positiveEmotionThreshold"]));
    neurotic.setNegativeEmotionThreshold(double(inputData["personalityDistribution"]["distribution"]["neurotic"]["negativeEmotionThreshold"]));

    double NoDisabilityNoOvertaking_velocity = double(inputData["walkability"]["distribution"]["noDisabilityNoOvertaking"]["velocity"]) * deviation;
    double NoDisabilityOvertaking_velocity = double(inputData["walkability"]["distribution"]["noDisabilityOvertaking"]["velocity"]) * deviation;
    double Crutches_velocity = double(inputData["walkability"]["distribution"]["crutches"]["velocity"]) * deviation;
    double Sticks_velocity = double(inputData["walkability"]["distribution"]["sticks"]["velocity"]) * deviation;
    double Wheelchair_velocity = double(inputData["walkability"]["distribution"]["wheelchair"]["velocity"]) * deviation;
    double Blind_velocity = double(inputData["walkability"]["distribution"]["blind"]["velocity"]) * deviation;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < pedestrianCountPerCat[i]; j++) {
            ID++;

            switch (i) {
                case 0: {
                    Personnel p;
                    vector<Event> events;
                    p.setID(ID);

                    int age;
                    while (age <= 23 || age >= 61) {
                        age = ages[randomInt(0, ages.size() - 1)];
                    }
                    p.setAge(age);

                    p.setPersonality(open);
                    p.setVelocity(ID <= 66 ? NoDisabilityNoOvertaking_velocity : NoDisabilityOvertaking_velocity);
                    p.setWardCount(3);

                    for (int k = 0; k < 20; k++) {
                        Event e = allEvents[randomInt(0, allEvents.size() - 1)];
                        e.setTime(allTimeDistances[randomInt(0, allTimeDistances.size() - 1)]);
                        events.push_back();
                    }
                    p.setEvents(events);

                    pedestrians.push_back(p);
                    break;
                }
                case 1: {
                    Visitor v;
                    vector<Event> events;
                    v.setID(ID);

                    v.setAge(ages[randomInt(0, ages.size() - 1)]);
                    v.setPersonality(ID <= 150 ? open : (v.getAge() < 11 ? open : neurotic));
                    v.setVelocity(ID <= 168 ? (ID <= 117 ? NoDisabilityOvertaking_velocity : Crutches_velocity) : Sticks_velocity);
                    v.setWardCount(1); 

                    for (int k = 0; k < 20; k++) {
                        Event e = allEvents[randomInt(0, allEvents.size() - 1)];
                        e.setTime(allTimeDistances[randomInt(0, allTimeDistances.size() - 1)]);
                        events.push_back();
                    }
                    v.setEvents(events);

                    pedestrians.push_back(v);
                    break;
                }
                case 2: {
				    Patient pt; 
				    vector<Event> events;
				    pt.setID(ID); 

				    pt.setAge(ages[randomInt(0, ages.size() - 1)]); 
                    pt.setPersonality(pt.getAge() < 11 ? open : neurotic);  
                    pt.setVelocity(ID <= 51 ? Wheelchair_velocity : Blind_velocity); 
                    pt.setWardCount(3);

                    for (int k = 0; k < 20; k++) {
					    Event e = allEvents[randomInt(0, allEvents.size() - 1)];
					    e.setTime(allTimeDistances[randomInt(0, allTimeDistances.size() - 1)]);
					    events.push_back();
				    }
				    pt.setEvents(events); 

				    pedestrians.push_back(pt);
				    break;      
                }
            }
        }
    }

    ofstream outf("data/pedestrian.txt", ios::app);
    if (!outf.is_open()) {
        cout << "File creation failed" << endl;
        return;
    }
    for (int i = 0; i < pedestrians.size(); i++) {
        json j;
        j["ID"] = pedestrians[i].getID();
        j["age"] = pedestrians[i].getAge();
        j["velocity"] = pedestrians[i].getVelocity();
        
        j["personality"]["name"] = pedestrians[i].getPersonality().getLambda() == 1 ? "open" : "neurotic";
        j["personality"]["lambda"] = pedestrians[i].getPersonality().getLambda(); 
        j["personality"]["positiveEmotionThreshold"] = pedestrians[i].getPersonality().getPositiveEmotionThreshold();
        j["personality"]["negativeEmotionThreshold"] = pedestrians[i].getPersonality().getNegativeEmotionThreshold(); 

        j["emotion"]["pleasure"] = pedestrians[i].getEmotion().getPleasure();
        j["emotion"]["surprise"] = pedestrians[i].getEmotion().getSurprise();
        j["emotion"]["anger"] = pedestrians[i].getEmotion().getAnger(); 
        j["emotion"]["fear"] = pedestrians[i].getEmotion().getFear(); 
        j["emotion"]["hate"] = pedestrians[i].getEmotion().getHate(); 
        j["emotion"]["sad"] = pedestrians[i].getEmotion().getSad();

        j["events"] = pedestrians[i].getEvents();

        outf << j << endl;
	}
    outf.close();

    cout << "Pedestrian generation successful" << endl;
    return;
}

int main() {
	pedestrianGen();
	return 0;
}




