#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/gpio.h>

int main(int argc, char *argv[ ])
{ int fd0 = open("/dev/gpiochip0",O_RDWR); // open the file descriptor
  struct gpiochip_info cinfo;
  ioctl(fd0,GPIO_GET_CHIPINFO_IOCTL,&cinfo); // get the chip information
  fprintf(stdout, "GPIO chip 0: %s, \"%s\", %u lines\n", cinfo.name, cinfo.label, cinfo.lines);

  // 3.1: blink Green and Yellow LEDs
  struct gpiohandle_request req_GY; // Green and Yellow
  struct gpiohandle_data data_GY;   // for data bit
  req_GY.lines = 2;                 // 2 pins in this handler
  req_GY.lineoffsets[0] = 4;        // pin 4 - Green LED
  req_GY.lineoffsets[1] = 17;       // pin 17 - Yellow LED
  req_GY.flags = GPIOHANDLE_REQUEST_OUTPUT; // set them to be output
  data_GY.values[0] = 1;            // set initial value of Green LED to High (ON)
  data_GY.values[1] = 0;            // set initial value of Yellow LED to Low (OFF)
  ioctl(fd0, GPIO_GET_LINEHANDLE_IOCTL, &req_GY); // now get the line handler req_GY

  for (int i = 0; i < 5; ++i){
    ioctl(req_GY.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_GY); // output data bits
    usleep(1000000);                // sleep for 1 second
    data_GY.values[0] = !data_GY.values[0]; // toggle
    data_GY.values[1] = !data_GY.values[1];
  } //for
  close(req_GY.fd);                 // release line

  // 3.2: blink Red LED whenever the Push Button is pressed (event-driven)
  struct gpiohandle_request req_R;  // Red LED
  struct gpiohandle_data data_R;
  memset(&req_R, 0, sizeof(req_R));
  req_R.lines = 1;
  req_R.lineoffsets[0] = 27;        // pin 27 - Red LED
  req_R.flags = GPIOHANDLE_REQUEST_OUTPUT;
  if (ioctl(fd0, GPIO_GET_LINEHANDLE_IOCTL, &req_R) < 0) perror("request red");

  struct gpioevent_request req_pb;  // Push Button event
  struct gpioevent_data event;
  memset(&req_pb, 0, sizeof(req_pb));
  req_pb.lineoffset = 11;           // pin 11 - Push Button
  req_pb.handleflags = GPIOHANDLE_REQUEST_INPUT;
  req_pb.eventflags = GPIOEVENT_REQUEST_RISING_EDGE; // using Rising Edge to trigger event
  strcpy(req_pb.consumer_label, "Push Button Event");
  if (ioctl(fd0, GPIO_GET_LINEEVENT_IOCTL, &req_pb) < 0) perror("request button event");

  printf("Waiting for button...\n");
  while (read(req_pb.fd, &event, sizeof(event)) == sizeof(event)) { // blocks until a press
    printf("Button pressed @ %" PRIu64 " ns\n", event.timestamp);
    data_R.values[0] = 1;           // Red on
    ioctl(req_R.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_R);
    usleep(200000);
    data_R.values[0] = 0;           // Red off
    ioctl(req_R.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_R);
    usleep(200000);                 // stay off so queued presses show as separate blinks
  }

  close(req_R.fd);
  close(req_pb.fd);
  close(fd0);                       // close the file
  exit(EXIT_SUCCESS);
}//main
