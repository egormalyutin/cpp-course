#include <iomanip>
#include <iostream>

struct Time;
std::ostream &operator<<(std::ostream &output, const Time &time);

struct Time {
    Time() : Time(0, 0, 0) {
    }

    Time(int hours, int minutes, int seconds)
        : hours(hours), minutes(minutes), seconds(seconds) {
        renormalize();
        times++;
        std::cout << "time " << (*this) << " created, times count: " << times
                  << std::endl;
    }

    Time(const Time &time) {
        times++;
        std::cout << "time " << (*this) << " copied, times count: " << times
                  << std::endl;
    }

    int get_hours() const {
        return hours;
    }

    int get_minutes() const {
        return minutes;
    }

    int get_seconds() const {
        return minutes;
    }

    void set_hours(int hours) {
        this->hours = hours;
        renormalize();
    };

    void set_minutes(int minutes) {
        this->minutes = minutes;
        renormalize();
    };

    void set_seconds(int seconds) {
        this->seconds = seconds;
        renormalize();
    }

    void operator+=(const Time &other) {
        seconds += other.seconds;
        minutes += other.minutes;
        hours += other.hours;
        renormalize();
    }

    Time operator+(const Time &other) const {
        return Time(seconds + other.seconds, minutes + other.minutes,
                    hours + other.hours);
    }

    void operator-=(const Time &other) {
        seconds -= other.seconds;
        minutes -= other.minutes;
        hours -= other.hours;
        renormalize();
    }

    Time operator-(const Time &other) const {
        return Time(seconds - other.seconds, minutes - other.minutes,
                    hours - other.hours);
    }

    bool operator==(const Time &other) const {
        return seconds == other.seconds && minutes == other.minutes &&
               hours == other.hours;
    }

    ~Time() {
        times--;
        std::cout << "time " << (*this) << " destroyed, times count: " << times
                  << std::endl;
    }

  private:
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    static int times;

    void renormalize() {
        if (seconds < 0) {
            minutes += (seconds - 59) / 60;
            seconds = (60 + seconds % 60) % 60;
        } else if (seconds >= 60) {
            minutes += seconds / 60;
            seconds %= 60;
        }

        if (minutes < 0) {
            hours += (minutes - 59) / 60;
            minutes = (60 + minutes % 60) % 60;
        } else if (minutes >= 60) {
            hours += minutes / 60;
            minutes %= 60;
        }

        hours = (24 + hours % 24) % 24;
    }
};

int Time::times = 0;

std::ostream &operator<<(std::ostream &output, const Time &time) {
    output << std::setfill('0') << std::setw(2) << time.get_hours();
    output << ":" << std::setw(2) << time.get_minutes();
    output << ":" << std::setw(2) << time.get_seconds();
    return output;
}

int main() {
    Time t;
    t.set_hours(1);
    Time t2 = t;
    t2.set_hours(2);
    std::cout << t2 << std::endl;
    {
        t2.set_hours(12);
        std::cout << t2 << std::endl;
        Time t2(22, 22, 22);
        std::cout << t2 << std::endl;
    }
    std::cout << t2 << std::endl;
    Time t3(3, 3, 3);
    t3 = t;
    t3.set_hours(3);
    std::cout << t3 << std::endl;
}