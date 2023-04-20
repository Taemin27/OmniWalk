#include <PS2MouseHandler.h>
#define MOUSE_DATA 5
#define MOUSE_CLOCK 19

PS2MouseHandler mouse(MOUSE_CLOCK, MOUSE_DATA, PS2_MOUSE_REMOTE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Serial Started");

  delay(1000);
  if(mouse.initialise() != 0){
    // mouse error
    Serial.println("Mouse Initialization Failed");
    for(;;) {

    }
  };

  Serial.println("Mouse Initialized");

  mouse.set_scaling_2_1();
  mouse.set_resolution(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  mouse.get_data();
  Serial.print(mouse.x_movement());
  Serial.print(", ");
  Serial.println(mouse.y_movement());
  delay(50);
}
