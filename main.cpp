#include "main.h"

//use the button on screen or x on the controller to start the test

#define expander_port 22 //22 for built in adi ports
#define adi_port 1 //A-H corresponds to 1-8
#define length 49 //LED count of the strip

bool rainbow = false;
int rotate_number = 0;
lv_obj_t *home;
lv_obj_t *home_img2;
LV_IMG_DECLARE(VexLogoTop);
LV_IMG_DECLARE(VexLogoBottom);
sylib::Addrled strip(expander_port, adi_port, length); // Create addrled objects for light strips

static lv_res_t btn_click_action(lv_obj_t *btn) {
  int id = lv_obj_get_free_num(btn);

  printf("LVGL button %d was released\n", id);
  // The button is released. Make something here //

  if (id == 1)
  {
    printf("Displaying rainbow on light strips\n");
    strip.gradient(0xFF0000, 0xFF0005, 0, 0, false, true);
    rotate_number = 0;
    rainbow = true;
  }

  return LV_RES_OK; // Return OK if the button is not deleted
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  // Set up sylib
  sylib::initialize();

  // Create a home page
  home = lv_page_create(lv_scr_act(), NULL);
  lv_obj_set_size(home, 510, 270);
  lv_obj_align(home, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_page_set_sb_mode(home, LV_SB_MODE_HIDE);

  lv_obj_t *home_img1 = lv_img_create(home, NULL);
  home_img2 = lv_img_create(home, NULL);

  // Add content to home
  lv_img_set_src(home_img1, &VexLogoBottom);
  lv_obj_align(home_img1, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  lv_img_set_src(home_img2, &VexLogoTop);
  lv_obj_align(home_img2, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

  // Create a normal button
  lv_obj_t *home_btn1 = lv_btn_create(home, NULL);
  lv_cont_set_fit(home_btn1, true, true); // Enable resizing horizontally and vertically
  lv_obj_align(home_btn1, home, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
  lv_obj_set_free_num(home_btn1, 1); // Set a unique number for the button
  lv_btn_set_action(home_btn1, LV_BTN_ACTION_CLICK, btn_click_action);

  // Add a label to button
  lv_obj_t *home_label = lv_label_create(home_btn1, NULL);
  lv_label_set_text(home_label, "Test");
  lv_obj_set_pos(home_btn1, 10, 10); // Set the position of the button

  pros::Task background_task{[]{
    int shake_start = pros::millis();
    while (true) {
      // Shake logo
      if (!lv_obj_get_hidden(home)) {
        if ((pros::millis() - shake_start) == 1500) {
          lv_obj_set_pos(home_img2, 3, 3);
        } else if ((pros::millis() - shake_start) == 1700) {
          lv_obj_set_pos(home_img2, -3, -3);
        } else if ((pros::millis() - shake_start) == 1900) {
          lv_obj_set_pos(home_img2, 0, 0);
        } else if ((pros::millis() - shake_start) == 3400) {
          lv_obj_set_pos(home_img2, 3, -3);
        } else if ((pros::millis() - shake_start) == 3600) {
          lv_obj_set_pos(home_img2, -3, 3);
        } else if ((pros::millis() - shake_start) > 3800) { // Greater than so shake_start resets even if home page is hidden when time is 3800
          shake_start = pros::millis();
          lv_obj_set_pos(home_img2, 0, 0);
        }
      }
      pros::delay(10);
    }
  }};
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
  pros::Controller master(CONTROLLER_MASTER);

  while (true)
  {
    // Start rainbow
    if (master.get_digital_new_press(DIGITAL_X)) {
      strip.gradient(0xFF0000, 0xFF0005, 0, 0, false, true);
      rotate_number = 0;
      rainbow = true;
    }

    // Rotate rainbow every loop
    if (rainbow) {
      if (rotate_number < (2 * length)) {
        strip.rotate(1, false);
        rotate_number++;
      } else {
        if (master.get_digital(DIGITAL_X)) {
          rotate_number = 0;
        } else {
          strip.clear();
          rainbow = false;
        }
      }
    }
    pros::delay(20);
  }
}