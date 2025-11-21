#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "geometry_msgs/msg/pose2_d.hpp"
#include "std_msgs/msg/float32.hpp"
#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"
#include "irobot_create_msgs/msg/audio_note_vector.hpp"

using std::placeholders::_1;

class Pose_nav : public rclcpp::Node
{
public:
    Pose_nav() : Node("Pose_nav")
    {
        client_ptr_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(
            this, "navigate_to_pose");

        audio_pub_ = this->create_publisher<irobot_create_msgs::msg::AudioNoteVector>(
            "/cmd_audio", 10);

        timer_ = this->create_wall_timer(
            std::chrono::seconds(2),
            std::bind(&Pose_nav::start_two_goals, this));

        RCLCPP_INFO(this->get_logger(), "System Ready");
    }

    void start_two_goals()
    {
        timer_->cancel();

        current_goal_id = 1;
        second_goal_pending = true;

        RCLCPP_INFO(this->get_logger(), "Starting goal sequence...");

        send_goal(23.88, -7.93, 0); // FIRST GOAL
    }

    // ----------------------------------------------------------------------
    // SEND GOAL (SAVE LAST GOAL COORDINATES)
    // ----------------------------------------------------------------------
    void send_goal(double x, double y, double theta)
    {
        if (!this->client_ptr_->wait_for_action_server())
        {
            RCLCPP_ERROR(this->get_logger(), "Action server not available");
            return;
        }

        // Save last goal for retry
        last_goal_x = x;
        last_goal_y = y;
        last_goal_theta = theta;

        auto goal_msg = nav2_msgs::action::NavigateToPose::Goal();
        goal_msg.pose.pose.position.x = x;
        goal_msg.pose.pose.position.y = y;
        goal_msg.pose.pose.orientation.z = theta;
        goal_msg.pose.header.frame_id = "map";

        auto send_goal_options =
            rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SendGoalOptions();

        send_goal_options.goal_response_callback =
            std::bind(&Pose_nav::goal_response_callback, this, _1);

        send_goal_options.result_callback =
            std::bind(&Pose_nav::get_result_callback, this, _1);

        client_ptr_->async_send_goal(goal_msg, send_goal_options);

        RCLCPP_INFO(this->get_logger(),
                    "Sending goal: x=%.2f, y=%.2f, θ=%.2f deg",
                    x, y, theta);
    }

private:
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr client_ptr_;
    rclcpp::Publisher<irobot_create_msgs::msg::AudioNoteVector>::SharedPtr audio_pub_;

    int current_goal_id = 0;
    bool second_goal_pending = false;

    // Save last goal for retry
    double last_goal_x, last_goal_y, last_goal_theta;

    rclcpp::TimerBase::SharedPtr timer_;

    // ----------------------------------------------------------------------
    // BEEP FUNCTION
    // ----------------------------------------------------------------------
    void play_beep()
    {
        irobot_create_msgs::msg::AudioNoteVector beep_msg;
        irobot_create_msgs::msg::AudioNote note;

        note.frequency = 412;
        note.max_runtime.sec = 1;
        note.max_runtime.nanosec = 0;

        beep_msg.notes.push_back(note);
        audio_pub_->publish(beep_msg);

        RCLCPP_INFO(this->get_logger(), "Beep!");
    }
    void play2beeps(){
    	
        irobot_create_msgs::msg::AudioNoteVector msg;
        msg.append = false;

        irobot_create_msgs::msg::AudioNote n1, n2;
        n1.frequency = 412;
        n1.max_runtime.sec = 1;
        n1.max_runtime.nanosec = 0;

        n2.frequency = 412;
        n2.max_runtime.sec = 1;
        n2.max_runtime.nanosec = 0;

        msg.notes.push_back(n1);
        msg.notes.push_back(n2);

        audio_pub_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Beep Beep!");
    }
    

    // ----------------------------------------------------------------------
    // GOAL CALLBACKS
    // ----------------------------------------------------------------------
    void goal_response_callback(
        const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr &goal_handle)
    {
        if (!goal_handle)
            RCLCPP_ERROR(this->get_logger(), "Goal was rejected");
        else
            RCLCPP_INFO(this->get_logger(), "Goal accepted");
    }

    void get_result_callback(
        const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::WrappedResult &result)
    {
        // -----------------------------
        // AUTO RESEND IF FAILED
        // -----------------------------
        if (result.code == rclcpp_action::ResultCode::ABORTED ||
            result.code == rclcpp_action::ResultCode::CANCELED)
        {
            RCLCPP_ERROR(this->get_logger(),
                         "Goal aborted/canceled → resending goal...");

            send_goal(last_goal_x, last_goal_y, last_goal_theta);
            return;
        }

        if (result.code != rclcpp_action::ResultCode::SUCCEEDED)
        {
            RCLCPP_ERROR(this->get_logger(), "Goal failed");
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Goal succeeded");

        // -----------------------------
        // SUCCESS LOGIC
        // -----------------------------
        if (current_goal_id == 1)
        {
            play_beep();
            RCLCPP_INFO(this->get_logger(), "Finished Goal 1 (1 beep)");

            if (second_goal_pending)
            {
                second_goal_pending = false;
                current_goal_id = 2;

                send_goal(44.07, -34.50, 0); // SECOND GOAL
            }
        }
        else if (current_goal_id == 2)
        {
            
            play2beeps();	
            RCLCPP_INFO(this->get_logger(), "Finished Goal 2 (2 beeps)");
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Pose_nav>());
    rclcpp::shutdown();
    return 0;
}

