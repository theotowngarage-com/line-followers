MyRenderer my_renderer;
MenuSystem ms(my_renderer);
bool buttonState = true;

Menu sm_data_streaming("Data streaming");
MenuItem mm_stram_raw_sensor("Stream sensor data", &handleStreamCommand);
MenuItem mm_stram_eq_sensor("Stream equalized sensor data", &handleStreamCommand);
MenuItem mm_stram_statistics("Stream statistics", &handleStreamCommand);
MenuItem mm_stram_heading("Stream heading", &handleStreamCommand);
Menu sm_config("Configuration");
MenuItem sm_config_save("Save configuration", &handleSaveConfig);
MenuItem sm_config_cal_contrast("Calibrate contrast", &handleCalibrateSensor);
MenuItem sm_config_cal_sensor("Calibrate sensor/next step", &handleCalibrateSensor);
NumericMenuItem sm_config_p("P", &handleUpdateConfiguration, 0.01, 0.01, 0.2, 0.005, formatFloat3Decimals);
NumericMenuItem sm_config_i("I", &handleUpdateConfiguration, 0.0, 0.0001, 0.002, 0.0001, formatFloat4Decimals);
NumericMenuItem sm_config_min_speed("Min. speed", &handleUpdateConfiguration, 0.4, 0.2, 0.9, 0.1);
NumericMenuItem sm_config_max_speed("Max. speed", &handleUpdateConfiguration, 0.7, 0.2, 1.0, 0.1);
NumericMenuItem sm_config_stop_contrast("Contrast limit", &handleUpdateConfiguration, 100, 0, 1000, 50);
NumericMenuItem sm_config_min_angle("Angle for min speed", &handleUpdateConfiguration, 0.3, 0.1, 0.5, 0.02);
Menu sm_test("Test");
MenuItem sm_test_stop("Stop motors", &handleDrive);
MenuItem sm_test_drive("Start drive", &handleDrive);
NumericMenuItem sm_test_speed("Speed", &handleDrive, 0.0, 0.0, 1.0, 0.1);
NumericMenuItem sm_test_direction("Direction", &handleDrive, 0.0, -1.0, 1.0, 0.1);
MenuItem mm_memory("Memory info", &handleMemoryInfo);

void setupMenu() {
  ms.get_root_menu().add_menu(&sm_data_streaming);
  sm_data_streaming.add_item(&mm_stram_raw_sensor);
  sm_data_streaming.add_item(&mm_stram_eq_sensor);
  sm_data_streaming.add_item(&mm_stram_statistics);
  sm_data_streaming.add_item(&mm_stram_heading);
  ms.get_root_menu().add_menu(&sm_config);
  sm_config.add_item(&sm_config_save);
  sm_config.add_item(&sm_config_cal_sensor);
  sm_config.add_item(&sm_config_cal_contrast);
  sm_config.add_item(&sm_config_p);
  sm_config.add_item(&sm_config_i);
  sm_config.add_item(&sm_config_min_speed);
  sm_config.add_item(&sm_config_max_speed);
  sm_config.add_item(&sm_config_stop_contrast);
  sm_config.add_item(&sm_config_min_angle);
  ms.get_root_menu().add_menu(&sm_test);
  sm_test.add_item(&sm_test_stop);
  sm_test.add_item(&sm_test_drive);
  sm_test.add_item(&sm_test_speed);
  sm_test.add_item(&sm_test_direction);
  ms.get_root_menu().add_item(&mm_memory);

  // update configuration menu items
  sm_config_p.set_value(config.lineFollowPID.p);
  sm_config_i.set_value(config.lineFollowPID.i);
  sm_config_min_speed.set_value(config.minSpeed);
  sm_config_max_speed.set_value(config.maxSpeed);
  sm_config_stop_contrast.set_value(config.minimumContrast);
  sm_config_min_angle.set_value(config.minSpeedAngle);

  displayHelp();
  ms.display();
}

void handleMemoryInfo(MenuComponent* p_menu_component) {
  Serial.print(F("Free memory="));
  Serial.println(freeMemory());
}

void handleDrive(MenuComponent* p_menu_component) {
  if(p_menu_component == &sm_test_drive) {
    driveFsm.trigger(DRIVE);
  } else if(p_menu_component == &sm_test_stop) {
    driveFsm.trigger(STOP);
  }
  setSpeed(sm_test_speed.get_value());
  setDirection(sm_test_direction.get_value());
  // DEBUG
  Serial.print(F("Test drive, speed="));
  Serial.print(sm_test_speed.get_value());
  Serial.print(F(", direction="));
  Serial.println(sm_test_direction.get_value());
}

void handleCalibrateSensor(MenuComponent* p_menu_component) {
  if(p_menu_component == &sm_config_cal_contrast) {
    calibrationFsm.trigger(CALIBRATION_CONTRAST);
  } else if(p_menu_component == &sm_config_cal_sensor) {
    calibrationFsm.trigger(CALIBRATION_SENSOR_NEXT);
  }
}

void handleUpdateConfiguration(MenuComponent* p_menu_component) {
  config.lineFollowPID.p = sm_config_p.get_value();
  config.lineFollowPID.i = sm_config_i.get_value();
  config.minSpeed = sm_config_min_speed.get_value();
  config.maxSpeed = sm_config_max_speed.get_value();
  config.minimumContrast = sm_config_stop_contrast.get_value();
  config.minSpeedAngle = sm_config_min_angle.get_value();
}

void handleSaveConfig(MenuComponent* p_menu_component) {
  writeConfigToFlash();
  Serial.println(F("Saved!"));
}

void handleStreamCommand(MenuComponent* p_menu_component) {
  if(p_menu_component == &mm_stram_raw_sensor) {
    streamingFsm.trigger(RAW_SENSOR);
  } else if(p_menu_component == &mm_stram_eq_sensor) {
    streamingFsm.trigger(EQ_SENSOR);
  } else if(p_menu_component == &mm_stram_statistics) {
    streamingFsm.trigger(SENSOR_STATISTICS);
  } else if(p_menu_component == &mm_stram_heading) {
    streamingFsm.trigger(SENSOR_HEADING);
  } else {
    streamingFsm.trigger(NONE);
  }
}

void displayHelp() {
    Serial.println(F("***************"));
    Serial.println(F("w: go to previus item (up)"));
    Serial.println(F("s: go to next item (down)"));
    Serial.println(F("a: go back"));
    Serial.println(F("d: activate item"));
    Serial.println(F("?, h: print this help"));
    Serial.println(F("** extra options **"));
    Serial.println(F("x: stop data streaming"));
    Serial.println(F("Button D2: follow line"));
    Serial.println(F("***************"));
}

void handleMenu() {
    char inChar;
    if ((inChar = Serial.read()) > 0) {
//        Serial.println(F("\033c"));
        switch (inChar) {
            case 'w': // Previus item
                ms.prev();
                ms.display();
                Serial.println();
                break;
            case 's': // Next item
                ms.next();
                ms.display();
                Serial.println();
                break;
            case 'a': // Back presed
                ms.back();
                ms.display();
                Serial.println();
                break;
            case 'd': // Select presed
                ms.select();
                ms.display();
                Serial.println();
                break;
            case '?':
            case 'h': // Display help
                displayHelp();
                ms.display();
                Serial.println();
                break;
            case 'x':
                handleStreamCommand(nullptr);
                ms.display();
                Serial.println();
                break;       
            default:
                break;
        }
    }
    handleButtons();
}

void handleButtons() {
  if(buttonPressed()) {
    driveFsm.trigger(FOLLOW_LINE);
  }
}

bool buttonPressed() {
  bool b = digitalRead(BUTTON_PIN);
  if(b != buttonState) {
    buttonState = b;
    if(buttonState == false) {
      return true;
    }
  }
  return false;
}

const String formatFloat3Decimals(const float value) {
  return String(value, 3);
}

const String formatFloat4Decimals(const float value) {
  return String(value, 4);
}
