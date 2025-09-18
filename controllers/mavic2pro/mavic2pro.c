#include <webots/robot.h>
#include <webots/camera.h>
#include <webots/gps.h>
#include <webots/gyro.h>
#include <webots/inertial_unit.h>
#include <webots/keyboard.h>
#include <webots/led.h>
#include <webots/motor.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SIGN(x) ((x) > 0) - ((x) < 0)
#define CLAMP(value, low, high) ((value) < (low) ? (low) : ((value) > (high) ? (high) : (value)))

int main(int argc, char **argv) {
  wb_robot_init();
  int timestep = (int)wb_robot_get_basic_time_step();

  // Enable camera ONCE here (NOT inside loop)
  WbDeviceTag camera = wb_robot_get_device("camera");
  wb_camera_enable(camera, timestep);

  const char *image_path = "C:\\Users\\houssem\\Desktop\\camera_frame.jpg";
  int frame_counter = 0;

  // Enable other devices
  WbDeviceTag imu = wb_robot_get_device("inertial unit");
  wb_inertial_unit_enable(imu, timestep);
  WbDeviceTag gps = wb_robot_get_device("gps");
  wb_gps_enable(gps, timestep);
  WbDeviceTag gyro = wb_robot_get_device("gyro");
  wb_gyro_enable(gyro, timestep);
  WbDeviceTag camera_roll_motor = wb_robot_get_device("camera roll");
  WbDeviceTag camera_pitch_motor = wb_robot_get_device("camera pitch");

  WbDeviceTag motors[4] = {
    wb_robot_get_device("front left propeller"),
    wb_robot_get_device("front right propeller"),
    wb_robot_get_device("rear left propeller"),
    wb_robot_get_device("rear right propeller")
  };
  for (int i = 0; i < 4; ++i) {
    wb_motor_set_position(motors[i], INFINITY);
    wb_motor_set_velocity(motors[i], 1.0);
  }

  const double k_vertical_thrust = 68.5;
  const double k_vertical_offset = 0.6;
  const double k_vertical_p = 3.0;
  const double k_roll_p = 50.0;
  const double k_pitch_p = 30.0;
  double target_altitude = 1.0;

  while (wb_robot_step(timestep) != -1) {
    frame_counter++;

    const double roll = wb_inertial_unit_get_roll_pitch_yaw(imu)[0];
    const double pitch = wb_inertial_unit_get_roll_pitch_yaw(imu)[1];
    const double altitude = wb_gps_get_values(gps)[2];
    const double roll_velocity = wb_gyro_get_values(gyro)[0];
    const double pitch_velocity = wb_gyro_get_values(gyro)[1];

    // Save image every 20 steps (~0.5s) AFTER camera is initialized
    if (frame_counter > 5 && frame_counter % 20 == 0) {
      if (wb_camera_save_image(camera, image_path, 100)) {
        printf("Saved camera image successfully to %s\n", image_path);
      } else {
        printf("Failed to save camera image.\n");
      }
    }

    // Stabilize the camera
    wb_motor_set_position(camera_roll_motor, -0.115 * roll_velocity);
    wb_motor_set_position(camera_pitch_motor, -0.1 * pitch_velocity);

    // Compute stabilization inputs
    const double roll_input = k_roll_p * CLAMP(roll, -1.0, 1.0) + roll_velocity;
    const double pitch_input = k_pitch_p * CLAMP(pitch, -1.0, 1.0) + pitch_velocity;
    const double clamped_alt_diff = CLAMP(target_altitude - altitude + k_vertical_offset, -1.0, 1.0);
    const double vertical_input = k_vertical_p * pow(clamped_alt_diff, 3.0);

    const double motor_inputs[4] = {
      k_vertical_thrust + vertical_input - roll_input + pitch_input,
      k_vertical_thrust + vertical_input + roll_input + pitch_input,
      k_vertical_thrust + vertical_input - roll_input - pitch_input,
      k_vertical_thrust + vertical_input + roll_input - pitch_input
    };

    wb_motor_set_velocity(motors[0], motor_inputs[0]);
    wb_motor_set_velocity(motors[1], -motor_inputs[1]);
    wb_motor_set_velocity(motors[2], -motor_inputs[2]);
    wb_motor_set_velocity(motors[3], motor_inputs[3]);
  }

  wb_robot_cleanup();
  return EXIT_SUCCESS;
}
