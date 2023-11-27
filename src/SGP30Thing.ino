#include <Thing.h>
#include <BlinkPattern.h>
#include <Adafruit_SGP30.h>
#include <Wire.h>
#include <ArduinoJson.h>

using namespace ootb;

Thing thing;
Adafruit_SGP30 sgp30;

BlinkPattern led(BUILTIN_LED);

BlinkPattern::Pattern<2> start{{1, 9}, 25};
BlinkPattern::Pattern<2> normal{{1, 39}, 25};
BlinkPattern::Pattern<0> disable{{}, 1000};

void setup()
{
    Serial.begin(230400);
    Serial.println("Client:" + thing.clientId());

    if (!sgp30.begin())
    {
        Serial.println("Sensor not found");
        while (1)
            ;
    }
    Serial.print("Found SGP30 serial #");
    Serial.print(sgp30.serialnumber[0], HEX);
    Serial.print(sgp30.serialnumber[1], HEX);
    Serial.println(sgp30.serialnumber[2], HEX);

    thing.begin();

    thing.onStateChange([](const String& msg){
        Serial.println(msg);
    });

    String sensorTopic = "things/" + thing.clientId() + "/sgp30/values";
    thing.addSensor(sensorTopic, 10000, getValue);
}


void getValue(Value &value)
{
    if (sgp30.IAQmeasure())
    {
        const int capacity = JSON_OBJECT_SIZE(3);
        StaticJsonBuffer<capacity> jb;
        JsonObject& doc = jb.createObject();
        //DynamicJsonDocument doc(1024);
        doc["eCO2"] = sgp30.eCO2;
        doc["TVOC"] = sgp30.TVOC;
        String output;
        doc.printTo(output);
        value = output;
        Serial.println(output);
        //serializeJson(doc, Serial);
    }
    else
    {
        String line = "error";
        value = line;
        Serial.println(line);
    }
}

void loop()
{
    thing.handle();
}
