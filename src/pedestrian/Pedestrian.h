#ifndef PEDESTRIAN_H
#define PEDESTRIAN_H

#include <vector>
#include <string>


struct Point {
    double x, y;
};

class Ward {
private:
    std::string wardName;
    Point entrance, exit;
    std::vector<Point> wardCoordinates;

public:
    Ward(std::string wardName, Point entrance, Point exit, std::vector<Point> wardCoordinates) : wardName(wardName), entrance(entrance), exit(exit), wardCoordinates(wardCoordinates) {}

    std::string getWardName() const { return wardName; }
    const Point& getEntrance() const { return entrance; }
    const Point& getExit() const { return exit; }
    std::vector<Point> getWardCoordinates() const { return wardCoordinates; }

    void setWardName(std::string wardName) { this -> wardName = wardName; }
    void setEntrance(Point entrance) { this -> entrance = entrance; }
    void setExit(Point exit) { this -> exit = exit; }
    void setWardCoordinates(std::vector<Point> wardCoordinates) { this -> wardCoordinates = wardCoordinates; }
};

class Event {
private:
    double time;
    std::vector<double> intensity;
public:
    double getTime() const { return time; }
    std::vector<double> getIntensity() const { return intensity; }

    void setTime(double time) { this -> time = time; }
    void setIntensity(std::vector<double> intensity) { this -> intensity = intensity; }
};

class Emotion {
private:
    double pleasure;
    double surprise;
    double anger;
    double fear;
    double hate;
    double sad;

public:
    Emotion() : pleasure(0.75), surprise(0.5), anger(-0.2), fear(-0.2), hate(-0.4), sad(-0.4) {}

    double getPleasure() const { return pleasure; }
    double getSurprise() const { return surprise; }
    double getAnger() const { return anger; }
    double getFear() const { return fear; }
    double getHate() const { return hate; }
    double getSad() const { return sad; }
};

class Personality {
private:
    double lambda;
    double positiveEmotionThreshold;
    double negativeEmotionThreshold;

public:
    double getLambda() const { return lambda; }
    double getPositiveEmotionThreshold() const { return positiveEmotionThreshold; }
    double getNegativeEmotionThreshold() const { return negativeEmotionThreshold; }

    void setLambda(double lambda) { this -> lambda = lambda; }
    void setPositiveEmotionThreshold(double positiveEmotionThreshold) { this -> positiveEmotionThreshold = positiveEmotionThreshold; }
    void setNegativeEmotionThreshold(double negativeEmotionThreshold) { this -> negativeEmotionThreshold = negativeEmotionThreshold; }
};

enum class Walkability {
    noDisability,
    crutches,
    sticks,
    wheelchairs,
    blind
};

class AGVEvent : Event {};

class Pedestrian {
private:
    int ID;
    Ward start, end;
    std::vector<Ward> journey;
    double velocity;
    Personality personality;
    Emotion emotion;
    vector<Event> events;
    double walkingTime;
    double distance;
    double age;
    AGVEvent impactOfAGV;
    Point tempPoints;
public:
    int getID() const { return ID; }
    double getVelocity() const { return velocity; }
    Ward getStart() const { return start; }
    Ward getEnd() const { return end; }
    std::vector<Ward> getJourney() const { return journey; }
    Personality getPersonality() const { return personality; }
    Emotion getEmotion() const { return emotion; }
    vector<vector<double>> getEvents() { 
        vector<vector<double>> eventPerEmotion(6, vector<double>(20));
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 20; j++) {
                eventPerEmotion[i][j] = events[j].getIntensity()[i];
            }
        }
        return eventPerEmotion;
    }
    double getWalkingTime() const { return walkingTime; }
    double getDistance() const { return distance; }
    double getAge() const { return age; }
    AGVEvent getImpactOfAGV() const { return impactOfAGV; }
    Point getTempPoints() const { return tempPoints; }

    void setID(int ID) { this -> ID = ID; } 
    void setStart(Ward start) { this -> start = start; }
    void setEnd(Ward end) { this -> end = end; }    
    void setJourney(std::vector<Ward> journey) { this -> journey = journey; }
    void setVelocity(double velocity) { this -> velocity = velocity; }
    void setPersonality(Personality personality) { this -> personality = personality; }
    void setEvents(vector<Event> events) { this -> events = events; }
};

class Patient : public Pedestrian {
    Walkability walkability;
};

class Visitor : public Pedestrian {
    Walkability walkability;
};

class Personnel : public Pedestrian {
    Walkability walkability;
};

std::vector<Ward> generateWard();

#endif