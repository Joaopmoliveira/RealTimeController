#include "sensors_utilities.h"

void gps_readings_thread(hardtime::Sync &gps_syncer) {
  gps reading;
  const gps_layout gps_data;
  while (gps_syncer) {
    auto msg = message.load();
    if (!msg.gps_reading_reading &&
        msg.gps_reading_counter == gps_data.multiplicity) {
      msg.gps_reading_reading = false;
      msg.gps_reading_writing = true;
      message.store(msg);
      copy_from_gps_reading_to_memory(
          shared_memory->get_shared_memory_address(), reading);
      msg.gps_reading_reading = false;
      msg.gps_reading_writing = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void image_reading_thread(hardtime::Sync &image_syncer) {
  grayscale_image image_reading;
  const grayscale_image_layout image_reading_data;
  while (image_syncer) {
    auto msg = message.load();
    if (!msg.grayscale_image_1_reading &&
        msg.grayscale_image_1_counter == image_reading_data.multiplicity) {
      msg.grayscale_image_1_reading = false;
      msg.grayscale_image_1_writing = true;
      message.store(msg);
      copy_from_gps_reading_to_memory(
          shared_memory->get_shared_memory_address(), reading);
      msg.grayscale_image_1_reading = false;
      msg.grayscale_image_1_writing = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int read(){
    hardtime::Sensor sensor;
    hardtime::Sync gps_syncer;
    hardtime::Sync grayscale_syncer;

    sensor.init_watchdog_communication();

    std::thread gps_sensor{[&]() {
      gps_readings_thread(gps_syncer);
    }};

    std::thread image_sensor{[&]() {
      image_reading_thread(grayscale_syncer);
    }};

    while (sensor) {
        sensor.loop_tick();
    }
    std::raise(SIGINT);
    gps_sensor.join();
    image_sensor.join();
}

int main() {
  try {
    read();
  } catch (...) {
    std::cout << "failure was detected in either communication or shared "
                 "memory operation\n";
    return 1;
  }
  return 0;
}