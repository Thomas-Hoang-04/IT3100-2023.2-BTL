/* Be advised: Turn all relative path into absolute path for all path in this file before running 
All new code requiring file access must use absolute path */

// Path format for json.hpp: ../../lib/nlohmann/json.hpp

#include "Pedestrian.h"
#include "lib/nlohmann/json.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <random>
#include <matplotlibcpp.h> /* Need to install the third party library named matplotibcpp*/

namespace plt = matplotlibcpp;

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

                    int age;
                    while (age <= 23 || age >= 61) {
                        age = ages[randomInt(0, ages.size() - 1)];
                    }
                    personnel.setAge(age);

                    personnel.setPersonality(open);
                    personnel.setVelocity(ID <= 66 ? NoDisabilityNoOvertaking_velocity : NoDisabilityOvertaking_velocity);
                    personnel.setWardCount(3);

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
                    visitor.setWardCount(1); 

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
                    patient.setVelocity(ID <= 51 ? Wheelchair_velocity : Blind_velocity); 
                    patient.setWardCount(3);

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

vector<vector<double>> eventImpact(Pedestrian pedestrian, int timeHorizon) {
    vector<vector<double>> events = pedestrian.getEvents();
    double lambda = pedestrian.getPersonality().getLambda();
    vector<int> times(19);
    for (int i = 0; i < 19; ++i) {
        times[i] = randomInt(4, 10);
    }

    vector<vector<double>> allEmotions = {
        {pedestrian.getEmotion().getPleasure()},
        {pedestrian.getEmotion().getSurprise()},
        {pedestrian.getEmotion().getAnger()},
        {pedestrian.getEmotion().getFear()},
        {pedestrian.getEmotion().getHate()},
        {pedestrian.getEmotion().getSad()}
    };

    vector<int> when;
    int lastTime = 0;
    int index = 0;
    vector<double> temp(6, 0.0);
    double sum = 0;
    string positiveOrNegative;
    
    for (int i = 0; i < 6; ++i) {
        allEmotions[i].push_back(events[i][0] + allEmotions[i][0]*exp(-lambda) + allEmotions[i][0]);
    }

    for (int i = 2; i < 20; ++i) {
        for (int j = 0; j < 6; ++j) {
            temp[j] = allEmotions[j][i - 1];
        }

        if (i - lastTime == times[index]) {
            index++;
            lastTime = i;
            for (int j = 0; j < 6; ++j) {
                sum += events[j][index];
            }
            positiveOrNegative = (sum > 0) ? "(+)" : "(-)";
            when.push_back(i);
        }

        for (int j = 0; j < 6; ++j) {
            allEmotions[j].push_back(temp[j] + allEmotions[j][i - 1]*exp(-lambda) + events[j][index]);
        }
    }

    vector<string> emotions = {"pleasure", "surprise", "anger", "fear", "hate", "sad"};
    for (int i = 0; i < 6; ++i) {
        plt::plot(allEmotions[i], {{"label", "Way " + emotions[i]}});
    }

    plt::title("The number of events happening to this individual: " + to_string(index + 1) + " " + positiveOrNegative);
    plt::xlabel("Time: ");
    plt::ylabel("Value: ");
    plt::legend();

    double positiveEmotionThreshold = 0.6;
    double negativeEmotionThreshold = -0.4;

    plt::hlines(positiveEmotionThreshold, 0, 16, {{"colors", "black"}, {"linestyles", "dashed"}});
    plt::hlines(negativeEmotionThreshold, 0, 16, {{"colors", "black"}, {"linestyles", "dashed"}});
    plt::text(8, 0.7, "Positive Threshold", {{"color", "black"}, {"ha", "center"}, {"va", "bottom"}});
    plt::text(8, -0.7, "Negative Threshold", {{"color", "black"}, {"ha", "center"}, {"va", "bottom"}});

    double minY = *min_element(allEmotions.begin(), allEmotions.end());
    double maxY = *max_element(allEmotions.begin(), allEmotions.end());

    for (auto num : when) {
        plt::vlines(num, minY, maxY, {{"colors", "black"}, {"linestyles", "dashed"}});
    }

    plt::save("figure.pdf");
    return 0;

}





