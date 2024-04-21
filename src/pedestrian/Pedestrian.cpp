/* Be advised: Turn all relative path into absolute path for all path in this file before running 
All new code requiring file access must use absolute path */

// Path format for json.hpp: ../../lib/nlohmann/json.hpp

#include "Pedestrian.h"
#include "../../lib/nlohmann/json.hpp"

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
    ifstream file("data/hospital.txt");
    string line;
    if (file.is_open()) {
        getline(file, line);
        for (int i = 2; i < 11; i++) {
            vector<Point> edges;
            Point mid1, mid2, p1, p2, p3, p4;
            double width;
            string ward_name;
            if (!getline(file, line)) {
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
        }
        file.close();
        cout << "Ward generation successful" << endl;
    }
    else {
        cout << "File not found" << endl;
    }
    return wards;
}

vector<Event> generateEvents() {
    vector<Event> events;
	ifstream file("data/event_distribution.txt");
	string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            Event event;
            double value;
            istringstream iss(line);
            vector<double> intensity;
            while (iss >> value) {
				intensity.push_back(value);
			}
            event.setIntensity(intensity);
            event.setTime(0);
            events.push_back(event);
        }
		file.close();
        cout << "Event generation successful" << endl;
	}
    else {
		cout << "File not found" << endl;
	}
	return events;
}

void generatePedestrians() {
    ifstream file("data/input.json");
    json inputData = json::parse(file);

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

    vector<Ward> wards = generateWard();
    Ward wardA;
    wardA.setWardName("A");
    wards.push_back(wardA);
    
    vector<Event> allEvents = generateEvents();
    vector<int> allTimeDistances;
    ifstream f2("data/time_distances_distribution.txt"); 
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
                    Personnel personnel;
                    vector<Event> events;
                    personnel.setID(ID);
                    Ward temp;

                    do {
                        personnel.setAge(ages[randomInt(0, ages.size() - 1)]);
                    } while (personnel.getAge() < 23 || personnel.getAge() > 61);

                    personnel.setPersonality(open);
                    personnel.setVelocity(ID <= 66 ? NoDisabilityNoOvertaking_velocity : NoDisabilityOvertaking_velocity);


                    personnel.setWardCount(3);
                    temp = wards[randomInt(0, wards.size() - 1)];
                    personnel.setStart(temp);
                    personnel.setEnd(temp);

                    for (int k = 0; k < 20; k++) {
                        Event event = allEvents[randomInt(0, allEvents.size() - 1)];
                        event.setTime(allTimeDistances[randomInt(0, allTimeDistances.size() - 1)]);
                        events.push_back(event);
                    }
                    personnel.setEvents(events);

                    pedestrians.push_back(personnel);
                    break;
                }
                case 1: {
                    Visitor visitor;
                    vector<Event> events;
                    visitor.setID(ID);

                    visitor.setAge(ages[randomInt(0, ages.size() - 1)]);
                    visitor.setPersonality(ID <= 150 ? open : (visitor.getAge() < 11 ? open : neurotic));
                    visitor.setVelocity(ID <= 168 ? (ID <= 117 ? NoDisabilityOvertaking_velocity : Crutches_velocity) : Sticks_velocity);
                    visitor.setWalkability(ID <= 168 ? (ID <= 117 ? Walkability::noDisability : Walkability::crutches) : Walkability::sticks);

                    visitor.setWardCount(1);
                    visitor.setStart(wards.back());
                    visitor.setEnd(wards.back());

                    for (int k = 0; k < 20; k++) {
                        Event event = allEvents[randomInt(0, allEvents.size() - 1)];
                        event.setTime(allTimeDistances[randomInt(0, allTimeDistances.size() - 1)]);
                        events.push_back(event);
                    }
                    visitor.setEvents(events);

                    pedestrians.push_back(visitor);
                    break;
                }
                case 2: {
				    Patient patient; 
				    vector<Event> events;
				    patient.setID(ID);

				    patient.setAge(ages[randomInt(0, ages.size() - 1)]); 
                    patient.setPersonality(patient.getAge() < 11 ? open : neurotic);  
                    patient.setVelocity(ID <= 285 ? Wheelchair_velocity : Blind_velocity); 
                    patient.setWalkability(ID <= 285 ? Walkability::wheelchair : Walkability::blind);
                    
                    patient.setWardCount(3);
                    patient.setStart(wards.back());
                    patient.setEnd(wards.back());

                    for (int k = 0; k < 20; k++) {
					    Event event = allEvents[randomInt(0, allEvents.size() - 1)];
					    event.setTime(allTimeDistances[randomInt(0, allTimeDistances.size() - 1)]);
					    events.push_back(event);
				    }
				    patient.setEvents(events);

				    pedestrians.push_back(patient);
				    break;
                }
            }
        }
    }

    ofstream outf("data/pedestrian.txt", ios::trunc);
    if (!outf.is_open()) {
        cout << "File creation failed" << endl;
        return;
    }
    for (int i = 0; i < pedestrians.size(); i++) {
        json j;
        j["ID"] = pedestrians[i].getID();
        j["age"] = pedestrians[i].getAge();
        j["wardCount"] = pedestrians[i].getWardCount();
        j["velocity"] = pedestrians[i].getVelocity();
        j["start"] = pedestrians[i].getStart().getWardName();
        j["end"] = pedestrians[i].getEnd().getWardName();

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








