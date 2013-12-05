#ifndef SENSOR_ACCELEROMETTER_HPP
#define SENSOR_ACCELEROMETER_HPP

#include "sensor.hpp"

#define ACCELERO_WRITE_MIN_PERIOD (200 * 1000000LL) // 200ms to ns

class AccelerometerSensor : public Sensor {

public:
    AccelerometerSensor(void);
    virtual ~AccelerometerSensor(void);

private:
    AccelerometerSensor(const AccelerometerSensor &);
    AccelerometerSensor operator=(const AccelerometerSensor &);

public:
    virtual void generateEvent(sensors_event_t *data, t_sensor_data rawData);
private:
    // Timestamp to avoid properties write on every event received
    int64_t lastPropertiesWriteTimestamp;
};

#endif
