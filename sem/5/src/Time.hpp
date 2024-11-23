#ifndef TIME_HPP
#define TIME_HPP

#include <iomanip>
#include <iostream>

struct Time {
    Time() : Time(0, 0, 0) {
    }

    Time(int hours, int minutes, int seconds)
        : hours(hours), minutes(minutes), seconds(seconds) {
        renormalize();
    }

    int get_hours() const {
        return hours;
    }

    int get_minutes() const {
        return minutes;
    }

    int get_seconds() const {
        return seconds;
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

    void operator+=(int seconds) {
        this->seconds += seconds;
        renormalize();
    }

    Time operator+(const Time &other) const {
        return Time(seconds + other.seconds, minutes + other.minutes,
                    hours + other.hours);
    }

    Time operator+(int seconds) const {
        return Time(this->seconds + seconds, minutes, hours);
    }

    void operator-=(const Time &other) {
        seconds -= other.seconds;
        minutes -= other.minutes;
        hours -= other.hours;
        renormalize();
    }

    void operator-=(int seconds) {
        this->seconds -= seconds;
        renormalize();
    }

    Time operator-(const Time &other) const {
        return Time(seconds - other.seconds, minutes - other.minutes,
                    hours - other.hours);
    }

    Time operator-(int seconds) const {
        return Time(this->seconds - seconds, minutes, hours);
    }

    bool operator==(const Time &other) const {
        return seconds == other.seconds && minutes == other.minutes &&
               hours == other.hours;
    }

    long to_seconds() const {
        return hours * 3600 + minutes * 60 + seconds;
    }

  private:
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

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

static std::ostream &operator<<(std::ostream &output, const Time &time) {
    output << std::setfill('0') << std::setw(2) << time.get_hours();
    output << ":" << std::setw(2) << time.get_minutes();
    output << ":" << std::setw(2) << time.get_seconds();
    return output;
}

#endif