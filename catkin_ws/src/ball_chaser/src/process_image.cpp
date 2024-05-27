#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the service and pass the requested linear and angular velocities
    if (!client.call(srv))
        ROS_ERROR("Failed to call service");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    bool ball_image = false;
    int image_column = 0;
    int white_pixel = 255;

    for (int i = 0; i < img.height * img.step; i++) {
        if (img.data[i] - white_pixel == 0) {
            ball_image = true;
            //Identify image column location
            image_column = i % img.step;

            break;
        }
    }

    if(ball_image)
    {
        // White ball located at left side of the image
        if((image_column >= 0) && image_column <= (img.step / 3))
        {
            drive_robot(0.0, 0.5);
        }
        // White ball located at middle of the image
        else if((image_column > (img.step / 3)) && (image_column < ((2 * img.step) / 3)))
        {
            drive_robot(0.5, 0.0);
        }
        // White ball located at right side side of the image
        else
        {
            drive_robot(0.0, -0.5);
        }
    }
    else
    {
        // Stop the robot
        drive_robot(0.0, 0.0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}