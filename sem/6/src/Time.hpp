#ifndef TIME_HPP
#define TIME_HPP

#include <iomanip>
#include <iostream>
#include <stdexcept>

struct Time {
    Time() noexcept : Time(0, 0, 0) {
    }

    Time(int hours, int minutes, int seconds) noexcept
        : hours(hours), minutes(minutes), seconds(seconds) {
        renormalize();
    }

    int get_hours() const noexcept {
        return hours;
    }

    int get_minutes() const noexcept {
        return minutes;
    }

    int get_seconds() const noexcept {
        return seconds;
    }

    void set_hours(int hours) noexcept {
        this->hours = hours;
        renormalize();
    };

    void set_minutes(int minutes) noexcept {
        this->minutes = minutes;
        renormalize();
    };

    void set_seconds(int seconds) noexcept {
        this->seconds = seconds;
        renormalize();
    }

    void operator+=(const Time &other) noexcept {
        seconds += other.seconds;
        minutes += other.minutes;
        hours += other.hours;
        renormalize();
    }

    void operator+=(int seconds) noexcept {
        this->seconds += seconds;
        renormalize();
    }

    Time operator+(const Time &other) const noexcept {
        return Time(seconds + other.seconds, minutes + other.minutes,
                    hours + other.hours);
    }

    Time operator+(int seconds) const noexcept {
        return Time(this->seconds + seconds, minutes, hours);
    }

    void operator-=(const Time &other) noexcept {
        seconds -= other.seconds;
        minutes -= other.minutes;
        hours -= other.hours;
        renormalize();
    }

    void operator-=(int seconds) noexcept {
        this->seconds -= seconds;
        renormalize();
    }

    Time operator-(const Time &other) const noexcept {
        return Time(seconds - other.seconds, minutes - other.minutes,
                    hours - other.hours);
    }

    Time operator-(int seconds) const noexcept {
        return Time(this->seconds - seconds, minutes, hours);
    }

    bool operator==(const Time &other) const noexcept {
        return seconds == other.seconds && minutes == other.minutes &&
               hours == other.hours;
    }

    long to_seconds() const noexcept {
        return hours * 3600 + minutes * 60 + seconds;
    }

    void break_everything() const {
        throw std::runtime_error("AAAAAAAA");
    }

  private:
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    void renormalize() noexcept {
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