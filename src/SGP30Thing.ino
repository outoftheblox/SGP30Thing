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

    String sensorTopic = "things/" + thing.clientId() + "/sgp30/airquality";
    thing.addSensor(sensorTopic, 60000, getValue);
}


void getValue(Value &value)
{
    if (sgp30.IAQmeasure())
    {
        String line = "CO2:" + String(sgp30.eCO2) + ",TVOC:" + String(sgp30.TVOC);
        value = line;
        Serial.println(line);
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
